
#pragma once

#include "DelaunayTable.PolygonTree.h"

#include <stddef.h>


/// # DelaunayTable
typedef struct{
    size_t nPoints;
    size_t nIn;
    size_t nOut;
    const double* table;
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
    const double* buffer
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

/// ## DelaunayTable properties
static inline size_t tablePointSize     (const DelaunayTable* const this) {return this->nPoints;}
static inline size_t extendedPointSize  (const DelaunayTable* const this) {return nVerticesInPolygon(this->nIn);}
static inline size_t allPointSize       (const DelaunayTable* const this) {return tablePointSize(this) + extendedPointSize(this);}

static inline size_t tablePointBegin    (const DelaunayTable* const this) {return 0;}
static inline size_t extendedPointBegin (const DelaunayTable* const this) {return tablePointSize(this);}
static inline size_t allPointBegin      (const DelaunayTable* const this) {return 0;}

static inline size_t tablePointEnd      (const DelaunayTable* const this) {return tablePointSize(this);}
static inline size_t extendedPointEnd   (const DelaunayTable* const this) {return allPointSize(this);}
static inline size_t allPointEnd        (const DelaunayTable* const this) {return allPointSize(this);}
