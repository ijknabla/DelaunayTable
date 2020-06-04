
#include "DelaunayTable.h"


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

    // Resources
    this->table             = NULL;
    this->table_extended    = NULL;
    this->polygonTreeVector = NULL;
    this->neighborPairMap   = NULL;

    const size_t nTable          = nPoints                 * (nIn+nOut);
    const size_t nTable_extended = nVerticesInPolygon(nIn) * nIn       ;

    this->table = (double*) MALLOC( nTable * sizeof(double) );
    if (!(this->table)) {
        status = FAILURE; goto finally;
    }

    this->table_extended = (double*) MALLOC( nTable_extended * sizeof(double) );
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

    *reference = this;

finally:

    if (status) {
        if (this) {
            if (this->table) {
                FREE(this->table);
            }
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
    FREE(this->table);
    FREE(this->table_extended);
    PolygonTreeVector__delete_elements(this->polygonTreeVector);
    PolygonTreeVector__delete         (this->polygonTreeVector);
    NeighborPairMap__delete(this->neighborPairMap);
    FREE(this);
}
