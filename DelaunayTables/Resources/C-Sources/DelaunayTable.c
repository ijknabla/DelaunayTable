
#include "DelaunayTable.h"

#include <stdbool.h>


/// ## static function declarations
static const double* DelaunayTable__get_coordinates(
    const DelaunayTable* this,
    const size_t iPoint
);

static int DelaunayTable__extend_table(
    DelaunayTable* this
);

static int DelaunayTable__delaunay_divide(
    DelaunayTable* this,
    const enum Verbosity verbosity
);

static int ensure_polygon_on_table(
    const DelaunayTable* this,
    const double* coordinates,
    PolygonTree** polygon,
    double* divisionRatio
);


/// ## DelaunayTable methods
int DelaunayTable__from_buffer(
    DelaunayTable** const reference,
    const size_t nPoints,
    const size_t nIn,
    const size_t nOut,
    const double* const buffer,
    const enum Verbosity verbosity
) {
    int status = SUCCESS;

    *reference = NULL;

    DelaunayTable* this = (DelaunayTable*) MALLOC(sizeof(DelaunayTable));
    if (!this) {
        status = FAILURE; goto finally;
    }

    this->nPoints = nPoints;
    this->nIn     = nIn;
    this->nOut    = nOut;
    this->table   = buffer;

    // Resources
    this->table_extended    = NULL;
    this->polygonTreeVector = NULL;
    this->neighborPairMap   = NULL;

    this->table_extended = (double*) MALLOC(
        nVerticesInPolygon(nIn) * nIn * sizeof(double)
    );
    if (!(this->table_extended)) {
        status = FAILURE; goto finally;
    }

    this->polygonTreeVector = PolygonTreeVector__new(0);
    if (!(this->polygonTreeVector)) {
        status = FAILURE; goto finally;
    }

    this->neighborPairMap = NeighborPairMap__new();
    if (!(this->neighborPairMap)) {
        status = FAILURE; goto finally;
    }

    status = DelaunayTable__extend_table(
        this
    );
    if (status) {
        goto finally;
    }

    status = DelaunayTable__delaunay_divide(
        this,
        verbosity
    );
    if (status) {
        goto finally;
    }

    *reference = this;

finally:

    if (status) {
        if (this) {
            if (this->table_extended) {
                FREE(this->table_extended);
            }
            if (this->polygonTreeVector) {
                PolygonTreeVector__delete_elements(this->polygonTreeVector);
                PolygonTreeVector__delete         (this->polygonTreeVector);
            }
            if (this->neighborPairMap) {
                NeighborPairMap__delete(this->neighborPairMap);
            }
            FREE(this);
        }
    }

    return status;
}


void DelaunayTable__delete(
    DelaunayTable* const this
) {
    FREE(this->table_extended);
    PolygonTreeVector__delete_elements(this->polygonTreeVector);
    PolygonTreeVector__delete         (this->polygonTreeVector);
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

    double* divisionRatio = (double*) MALLOC(nVerticesInPolygon(nDim) * sizeof(double));
    if (!divisionRatio) {
        status = FAILURE; goto finally;
    }

    PolygonTree* polygon;

    status = PolygonTree__find(
        nDim,
        PolygonTreeVector__elements(this->polygonTreeVector)[0],
        u,
        this,
        (Points__get_coordinates*) DelaunayTable__get_coordinates,
        &polygon,
        divisionRatio
    );
    if (status) {
        goto finally;
    }

    if (!polygon) {
        status = FAILURE; goto finally;
    }

    status = ensure_polygon_on_table(
        this,
        u,
        &polygon,
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
    for (size_t iVertex = 0 ; iVertex < nVerticesInPolygon(nDim) ; iVertex++) {
        const double* coords = DelaunayTable__get_coordinates(
            this,
            polygon->vertices[iVertex]
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

static int DelaunayTable__extend_table(
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
    const double big_coordinate = (double) nDim * 2 * maxAbs;
    for (size_t iPoint = 0 ; iPoint < nVerticesInPolygon(nDim) ; iPoint++) {
        double* const coords = (this->table_extended) + iPoint * nDim;

        for (size_t i = 0 ; i < nDim ; i++) {
            if (iPoint == 0) {
                coords[i] = -big_coordinate;
            } else if (iPoint == (i+1)) {
                coords[i] = +big_coordinate;
            } else {
                coords[i] = 0.0;
            }
        }
    }

    return SUCCESS;
}

static int DelaunayTable__delaunay_divide(
    DelaunayTable* this,
    const enum Verbosity verbosity
) {
    // Assertion polygonTreeVector must be empty
    if ( (this->polygonTreeVector->size) != 0 ) {
        return FAILURE;
    }

    int status = SUCCESS;

    const size_t nDim = this->nIn;

    IndexVector* face = IndexVector__new(nVerticesInFace(nDim));
    if (!face) {
        status = FAILURE; goto finally;
    }

    // Setup bigPolygon as root of polygonTree
    PolygonTree* const bigPolygon = PolygonTree__new(nDim);
    if (!bigPolygon) {status = FAILURE; goto finally;}

    status = PolygonTreeVector__append(this->polygonTreeVector, bigPolygon);
    if (status) {
        PolygonTree__delete(bigPolygon);
        goto finally;
    }

    for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
        bigPolygon->vertices[i] = extendedPointBegin(this) + i;
    }

    for (size_t iEx = 0 ; iEx < nVerticesInPolygon(nDim) ; iEx++) {
        // Set vertices of face
        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (i < iEx) {
                IndexVector__elements(face)[i] = bigPolygon->vertices[i+0];
            } else {
                IndexVector__elements(face)[i] = bigPolygon->vertices[i+1];
            }
        }

        // Set to neighborPairMap
        Neighbor neighborPair[2] = {
            {bigPolygon->vertices[iEx], bigPolygon},
            {-1                       , NULL      }
        };

        status = NeighborPairMap__set(
            this->neighborPairMap,
            face,
            neighborPair
        );
        if (status) {
            goto finally;
        }
    }

    for (
        size_t pointToDivide = tablePointBegin(this);
        pointToDivide < tablePointEnd(this);
        pointToDivide++
    ) {
        if (verbosity >= Verbosity__debug) {
            Runtime__send_message(
                "Divide polygon tree (contains %6lu polygons) by point [%3lu]",
                this->polygonTreeVector->size,
                pointToDivide+1
            );
        }

        status = PolygonTreeVector__divide_at_point(
            nDim,
            this->polygonTreeVector,
            pointToDivide,
            this,
            (Points__get_coordinates*) DelaunayTable__get_coordinates,
            bigPolygon,
            this->neighborPairMap,
            verbosity
        );
        if (status) {
            goto finally;
        }
    }

finally:

    if (face) {IndexVector__delete(face);}

    return status;
}

static inline bool polygon_on_table(
    const DelaunayTable* const this,
    const PolygonTree* const polygon
) {
    const size_t nDim = this->nIn;

    for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
        const bool vertexOnTable = (
            polygon->vertices[i] >= tablePointBegin(this) &&
            polygon->vertices[i] <  tablePointEnd(this)
        );
        if (!vertexOnTable) {return false;}
    }

    return true;
}

int ensure_polygon_on_table(
    const DelaunayTable* const this,
    const double* const coordinates,
    PolygonTree** const polygon,
    double* const divisionRatio
) {
    const size_t nDim = this->nIn;
    PolygonTree* const previousPolygon = *polygon;

    // Early return
    // [1] if all vertices on table -> success (do nothing)
    if (polygon_on_table(this, previousPolygon)) {
        return SUCCESS;
    }
    // [2] else if coordinates not on face -> failure
    if (!divisionRatio__on_face(nDim, divisionRatio)) {
        return FAILURE;
    }

    int status = SUCCESS;

    IndexVector*       overlapVertices = NULL;
    PolygonTreeVector* aroundPolygons  = NULL;

    overlapVertices = IndexVector__new(0);
    if (!overlapVertices) {
        status = FAILURE; goto finally;
    }

    aroundPolygons = PolygonTreeVector__new(0);
    if (!aroundPolygons) {
        status = FAILURE; goto finally;
    }

    for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
        if (double__compare(divisionRatio[i], 0.0) != 0) {
            status = IndexVector__append(
                overlapVertices,
                previousPolygon->vertices[i]
            );
            if (status) {
                goto finally;
            }
        }
    }

    status = PolygonTree__get_around(
        nDim,
        previousPolygon,
        overlapVertices,
        this->neighborPairMap,
        aroundPolygons
    );
    if (status) {
        goto finally;
    }

    // return first polygon on table
    for (size_t i = 0 ; i < (aroundPolygons->size) ; i++) {
        PolygonTree* const candidate
            = PolygonTreeVector__elements(aroundPolygons)[i];

        if (candidate == previousPolygon)      {continue;}
        if (!polygon_on_table(this, candidate)) {continue;}

        status = PolygonTree__calculate_divisionRatio(
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

        *polygon = candidate;
        goto finally;
    }

    // polygon on table not found -> failure
    *polygon = NULL;
    status = FAILURE;

finally:

    if (overlapVertices) {IndexVector__delete(overlapVertices);}
    if (aroundPolygons)  {PolygonTreeVector__delete(aroundPolygons);}

    return status;
}