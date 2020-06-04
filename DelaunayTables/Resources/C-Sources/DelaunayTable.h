
#pragma once

#include "DelaunayTable.PolygonTree.h"

#include <stddef.h>


/// # DelaunayTable
typedef struct{
    size_t nPoints;
    size_t nIn;
    size_t nOut;
    double* table;
    double* table_extended;
    PolygonTreeVector* polygonTreeVector;
    NeighborPairMap*   neighborPairMap;
} DelaunayTable;


/// ## DelaunayTable methods
extern int DelaunayTable__from_buffer(
    DelaunayTable** reference,
    const size_t nPoints,
    const size_t nIn,
    const size_t nOut,
    double* buffer
);

extern void DelaunayTable__delete(
    DelaunayTable* this
);

extern int DelaunayTable__get_value(
    DelaunayTable** this,
    size_t nIn,
    size_t nOut,
    const double* u,
          double* y
);
