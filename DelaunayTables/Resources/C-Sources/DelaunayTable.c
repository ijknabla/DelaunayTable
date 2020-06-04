
#include "DelaunayTable.h"


/// ## static function declarations
static const double* DelaunayTable__get_coordinates(
    const DelaunayTable* this,
    const size_t iPoint
);

static int DelaunayTable__extend_table(
    DelaunayTable* this
);

static int DelaunayTable__delaunay_divide(
    DelaunayTable* this
);


/// ## DelaunayTable methods
int DelaunayTable__from_buffer(
    DelaunayTable** const reference,
    const size_t nPoints,
    const size_t nIn,
    const size_t nOut,
    const double* const buffer
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
        this
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
    for (size_t iPoint = 0 ; iPoint < nVerticesInPolygon(nDim) ; iPoint++) {
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

    return SUCCESS;
}

static int DelaunayTable__delaunay_divide(
    DelaunayTable* this
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
        status = PolygonTreeVector__divide_at_point(
            nDim,
            this->polygonTreeVector,
            pointToDivide,
            this,
            (Points__get_coordinates*) DelaunayTable__get_coordinates,
            bigPolygon,
            this->neighborPairMap
        );
        if (status) {
            goto finally;
        }
    }

finally:

    if (face) {IndexVector__delete(face);}

    return status;
}
