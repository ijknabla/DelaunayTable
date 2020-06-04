
#include "DelaunayTable.h"


/// ## static function declarations
static int DelaunayTable__extend_table(
    DelaunayTable* this
);

/// ## DelaunayTable methods
int DelaunayTable__from_buffer(
    DelaunayTable** const reference,
    const size_t nPoints,
    const size_t nIn,
    const size_t nOut,
    double* const buffer
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
static double* DelaunayTable__get_coordinates(
    const DelaunayTable* const this,
    const size_t iPoint
) {
    if (tablePointBegin(this) <= iPoint && iPoint < tablePointEnd(this)) {
        return (this->table) + (this->nIn + this->nOut) * (iPoint - tablePointBegin(this));
    } else if (extendedPointBegin(this) <= iPoint && iPoint < extendedPointEnd(this)
    ) {
        return (this->table_extended) + (this->nIn) * (iPoint - extendedPointBegin(this));
    } else {
        return NULL;
    }
}

static int DelaunayTable__extend_table(
    DelaunayTable* this
) {
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

        for (size_t i = 0 ; i < (this->nIn) ; i++) {
            maxAbs = double__max(maxAbs, double__abs(coords[i]));
        }
    }

    // Assign extended table data
    for (size_t iPoint = 0 ; iPoint < nVerticesInPolygon(this->nIn) ; iPoint++) {
        double* const coords = DelaunayTable__get_coordinates(
            this, extendedPointBegin(this)+iPoint
        );

        for (size_t i = 0 ; i < (this->nIn) ; i++) {
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
