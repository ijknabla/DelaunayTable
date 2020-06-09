
#include "DelaunayTable.h"

#include "DelaunayTable.Error.h"

#include <stdbool.h>


/// ## static function declarations
static const double* DelaunayTable__get_coordinates(
    const DelaunayTable* this,
    const size_t iPoint
);

static void DelaunayTable__extend_table(
    DelaunayTable* this
);

static void DelaunayTable__delaunay_divide(
    DelaunayTable* this,
    const enum Verbosity verbosity,
    ResourceStack resources
);

static int ensure_polyhedron_on_table(
    const DelaunayTable* this,
    const double* coordinates,
    PolyhedronTree** polyhedron,
    double* divisionRatio
);


/// ## DelaunayTable methods
DelaunayTable* DelaunayTable__from_buffer(
    const size_t nPoints,
    const size_t nIn,
    const size_t nOut,
    const double* const buffer,
    const enum Verbosity verbosity,
    ResourceStack resources
) {
    ResourceStack__enter(resources);

    DelaunayTable* this = ResourceStack__ensure_delete_on_error(
        resources, FREE,
        MALLOC(sizeof(DelaunayTable))
    );
    if (!this) {
        raise_MemoryAllocationError(resources);
    }

    this->nPoints = nPoints;
    this->nIn     = nIn;
    this->nOut    = nOut;
    this->table   = buffer;

    // Resources
    this->table_extended    = NULL;
    this->polyhedronTreeVector = NULL;
    this->neighborPairMap   = NULL;

    this->table_extended = ResourceStack__ensure_delete_on_error(
        resources, FREE,
        MALLOC(nVerticesInPolyhedron(nIn) * nIn * sizeof(double))
    );
    if (!(this->table_extended)) {
        raise_MemoryAllocationError(resources);
    }

    this->polyhedronTreeVector = ResourceStack__ensure_delete_on_error(
        resources, (Resource__deleter*) PolyhedronTreeVector__delete,
        PolyhedronTreeVector__new(0)
    );
    if (!(this->polyhedronTreeVector)) {
        raise_Error(resources, "PolyhedronTreeVector__new(0) failed");
    }

    this->neighborPairMap = ResourceStack__ensure_delete_on_error(
        resources, (Resource__deleter*) NeighborPairMap__delete,
        NeighborPairMap__new()
    );
    if (!(this->neighborPairMap)) {
        raise_Error(resources, "NeighborPairMap__new() failed");
    }

    DelaunayTable__extend_table(
        this
    );

    DelaunayTable__delaunay_divide(
        this,
        verbosity,
        resources
    );

    ResourceStack__exit(resources);
    return this;
}


void DelaunayTable__delete(
    DelaunayTable* const this
) {
    FREE(this->table_extended);
    PolyhedronTreeVector__delete_elements(this->polyhedronTreeVector);
    PolyhedronTreeVector__delete         (this->polyhedronTreeVector);
    NeighborPairMap__delete(this->neighborPairMap);
    FREE(this);
}

int DelaunayTable__get_value(
    DelaunayTable* const this,
    size_t nIn,
    size_t nOut,
    const double* u,
          double* y
) {
    // Assertion for nIn, nOut
    if (nIn != (this->nIn)) {
        return FAILURE;
    }
    if (nOut != (this->nOut)) {
        return FAILURE;
    }

    const size_t nDim = this->nIn;

    int status = SUCCESS;

    double* divisionRatio = (double*) MALLOC(nVerticesInPolyhedron(nDim) * sizeof(double));
    if (!divisionRatio) {
        status = FAILURE; goto finally;
    }

    PolyhedronTree* polyhedron;

    status = PolyhedronTree__find(
        nDim,
        PolyhedronTreeVector__elements(this->polyhedronTreeVector)[0],
        u,
        this,
        (Points__get_coordinates*) DelaunayTable__get_coordinates,
        &polyhedron,
        divisionRatio
    );
    if (status) {
        goto finally;
    }

    if (!polyhedron) {
        status = FAILURE; goto finally;
    }

    status = ensure_polyhedron_on_table(
        this,
        u,
        &polyhedron,
        divisionRatio
    );
    if (status) {
        goto finally;
    }

    /// # interpolate y[:]
    /// ## initialize y[:]
    for (size_t iOut = 0 ; iOut < nOut ; iOut++) {
        y[iOut] = 0.0;
    }
    /// ## linear interpolation by `divisionRatio`
    for (size_t iVertex = 0 ; iVertex < nVerticesInPolyhedron(nDim) ; iVertex++) {
        const double* coords = DelaunayTable__get_coordinates(
            this,
            polyhedron->vertices[iVertex]
        );
        for (size_t iOut = 0 ; iOut < nOut ; iOut++) {
            y[iOut] += divisionRatio[iVertex] * coords[this->nIn + iOut];
        }
    }

finally:

    if (divisionRatio) {FREE(divisionRatio);}

    return status;
}

/// static function implementations
static const double* DelaunayTable__get_coordinates(
    const DelaunayTable* const this,
    const size_t iPoint
) {
    if (tablePointBegin(this) <= iPoint && iPoint < tablePointEnd(this)) {
        return (this->table) + (this->nIn + this->nOut) * (iPoint - tablePointBegin(this));
    } else if (extendedPointBegin(this) <= iPoint && iPoint < extendedPointEnd(this)
    ) {
        return (const double*) (this->table_extended) + (this->nIn) * (iPoint - extendedPointBegin(this));
    } else {
        return NULL;
    }
}

static void DelaunayTable__extend_table(
    DelaunayTable* this
) {
    const size_t nDim = this->nIn;

    // Calcurate maxAbs
    double maxAbs = 1.0;  // minimum maxAbs is 1.0
    for (
        size_t iPoint = tablePointBegin(this);
        iPoint < tablePointEnd(this);
        iPoint++
    ) {
        const double* const coords = DelaunayTable__get_coordinates(
            this, iPoint
        );

        for (size_t i = 0 ; i < nDim ; i++) {
            maxAbs = double__max(maxAbs, double__abs(coords[i]));
        }
    }

    // Assign extended table data
    for (size_t iPoint = 0 ; iPoint < nVerticesInPolyhedron(nDim) ; iPoint++) {
        double* const coords = (this->table_extended) + iPoint * nDim;

        for (size_t i = 0 ; i < nDim ; i++) {
            if (iPoint == 0) {
                coords[i] = -4.0 * maxAbs;
            } else if (iPoint == (i+1)) {
                coords[i] = +4.0 * maxAbs;
            } else {
                coords[i] =  0.0;
            }
        }
    }
}

static void DelaunayTable__delaunay_divide(
    DelaunayTable* this,
    const enum Verbosity verbosity,
    ResourceStack resources
) {
    ResourceStack__enter(resources);

    int status = SUCCESS;

    // Assertion polyhedronTreeVector must be empty
    if ( (this->polyhedronTreeVector->size) != 0 ) {
        raise_Error(resources, "duplicate call of DelaunayTable__delaunay_divide()");
    }

    const size_t nDim = this->nIn;

    IndexVector* face = ResourceStack__ensure_delete_finally(
        resources, (Resource__deleter*) IndexVector__delete,
        IndexVector__new(nVerticesInFace(nDim))
    );
    if (!face) {
        raise_Error(resources, "IndexVector__new(nVerticesInFace(nDim)) failed");
    }

    // Setup bigPolyhedron as root of polyhedronTree
    PolyhedronTree* const bigPolyhedron = PolyhedronTree__new(nDim);
    if (!bigPolyhedron) {
        raise_Error(resources, "PolyhedronTree__new(nDim) failed");
    }

    status = PolyhedronTreeVector__append(this->polyhedronTreeVector, bigPolyhedron);
    if (status) {
        PolyhedronTree__delete(bigPolyhedron);
        raise_Error(resources, "failed to append bigPolyhedron to this->polyhedronTreeVector");
    }

    for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
        bigPolyhedron->vertices[i] = extendedPointBegin(this) + i;
    }

    for (size_t iEx = 0 ; iEx < nVerticesInPolyhedron(nDim) ; iEx++) {
        // Set vertices of face
        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (i < iEx) {
                IndexVector__elements(face)[i] = bigPolyhedron->vertices[i+0];
            } else {
                IndexVector__elements(face)[i] = bigPolyhedron->vertices[i+1];
            }
        }

        // Set to neighborPairMap
        Neighbor neighborPair[2] = {
            {bigPolyhedron->vertices[iEx], bigPolyhedron},
            {-1                       , NULL      }
        };

        status = NeighborPairMap__set(
            this->neighborPairMap,
            face,
            neighborPair
        );
        if (status) {
            raise_Error(
                resources,
                "failed to set face => neighborPair to this->neighborPairMap"
            );
        }
    }

    for (
        size_t pointToDivide = tablePointBegin(this);
        pointToDivide < tablePointEnd(this);
        pointToDivide++
    ) {
        if (verbosity >= Verbosity__debug) {
            Runtime__send_message(
                "Divide polyhedron tree (contains %6lu polyhedrons) by point [%3lu]",
                this->polyhedronTreeVector->size,
                pointToDivide+1
            );
        }

        PolyhedronTreeVector__divide_at_point(
            nDim,
            this->polyhedronTreeVector,
            pointToDivide,
            this,
            (Points__get_coordinates*) DelaunayTable__get_coordinates,
            bigPolyhedron,
            this->neighborPairMap,
            verbosity,
            resources
        );
    }

    ResourceStack__exit(resources);
}

static inline bool polyhedron_on_table(
    const DelaunayTable* const this,
    const PolyhedronTree* const polyhedron
) {
    const size_t nDim = this->nIn;

    for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
        const bool vertexOnTable = (
            polyhedron->vertices[i] >= tablePointBegin(this) &&
            polyhedron->vertices[i] <  tablePointEnd(this)
        );
        if (!vertexOnTable) {return false;}
    }

    return true;
}

int ensure_polyhedron_on_table(
    const DelaunayTable* const this,
    const double* const coordinates,
    PolyhedronTree** const polyhedron,
    double* const divisionRatio
) {
    const size_t nDim = this->nIn;
    PolyhedronTree* const previousPolyhedron = *polyhedron;

    // Early return
    // [1] if all vertices on table -> success (do nothing)
    if (polyhedron_on_table(this, previousPolyhedron)) {
        return SUCCESS;
    }
    // [2] else if coordinates not on face -> failure
    if (!divisionRatio__on_face(nDim, divisionRatio)) {
        return FAILURE;
    }

    int status = SUCCESS;

    IndexVector*       overlapVertices = NULL;
    PolyhedronTreeVector* aroundPolyhedrons  = NULL;

    overlapVertices = IndexVector__new(0);
    if (!overlapVertices) {
        status = FAILURE; goto finally;
    }

    aroundPolyhedrons = PolyhedronTreeVector__new(0);
    if (!aroundPolyhedrons) {
        status = FAILURE; goto finally;
    }

    for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
        if (double__compare(divisionRatio[i], 0.0) != 0) {
            status = IndexVector__append(
                overlapVertices,
                previousPolyhedron->vertices[i]
            );
            if (status) {
                goto finally;
            }
        }
    }

    status = PolyhedronTree__get_around(
        nDim,
        previousPolyhedron,
        overlapVertices,
        this->neighborPairMap,
        aroundPolyhedrons
    );
    if (status) {
        goto finally;
    }

    // return first polyhedron on table
    for (size_t i = 0 ; i < (aroundPolyhedrons->size) ; i++) {
        PolyhedronTree* const candidate
            = PolyhedronTreeVector__elements(aroundPolyhedrons)[i];

        if (candidate == previousPolyhedron)      {continue;}
        if (!polyhedron_on_table(this, candidate)) {continue;}

        status = PolyhedronTree__calculate_divisionRatio(
            nDim,
            candidate,
            coordinates,
            (Points) this,
            (Points__get_coordinates*) DelaunayTable__get_coordinates,
            divisionRatio
        );
        if (status) {
            goto finally;
        }

        *polyhedron = candidate;
        goto finally;
    }

    // polyhedron on table not found -> failure
    *polyhedron = NULL;
    status = FAILURE;

finally:

    if (overlapVertices) {IndexVector__delete(overlapVertices);}
    if (aroundPolyhedrons)  {PolyhedronTreeVector__delete(aroundPolyhedrons);}

    return status;
}
