
#pragma once

#include "DelaunayTable.ResourceStack.h"
#include "DelaunayTable.Geometry.h"
#include "DelaunayTable.Container.h"
#include "DelaunayTable.Neighbor.h"


/// # PolygonTree & PolygonTreeVector
typedef Vector PolygonTreeVector;

typedef struct PolygonTree__TAG {
    size_t* vertices;
    PolygonTreeVector* children;
} PolygonTree;

/// ## PolygonTree methods
extern PolygonTree* PolygonTree__new(
    const size_t nDim
);

extern void PolygonTree__delete(
    PolygonTree* this
);

static inline size_t PolygonTree__nChildren(
    const PolygonTree* this
);

static inline PolygonTree** PolygonTree__children(
    const PolygonTree* this
);

extern int PolygonTree__append_child(
    PolygonTree* this,
    PolygonTree* child
);

extern int PolygonTree__calculate_divisionRatio(
    const size_t nDim,
    const PolygonTree* this,
    const double* coordinates,
    const Points points,
    Points__get_coordinates* get_coordinates,
    double* divisionRatio
);

extern int PolygonTree__find(
    const size_t nDim,
    PolygonTree* rootPolygon,
    const double* coordinates,
    const Points points,
    Points__get_coordinates* get_coordinates,
    PolygonTree** foundPolygon,
    double* divisionRatio
);

extern int PolygonTree__get_around(
    const size_t nDim,
    const PolygonTree* polygon,
    const IndexVector* overlapVertices,
    const NeighborPairMap* neighborPairMap,
    PolygonTreeVector* aroundPolygons
);


/// ## PolygonTreeVector methods
extern PolygonTreeVector* PolygonTreeVector__new(
    const size_t size
);

extern void PolygonTreeVector__delete(
    PolygonTreeVector* this
);

extern void PolygonTreeVector__delete_elements(
    PolygonTreeVector* this
);

static inline PolygonTree** PolygonTreeVector__elements(
    const PolygonTreeVector* this
);

extern int PolygonTreeVector__append(
    PolygonTreeVector* this,
    PolygonTree* polygon
);

extern void PolygonTreeVector__divide_at_point(
    const size_t nDim,
    PolygonTreeVector* this,
    const size_t pointToDivide,
    const Points points,
    Points__get_coordinates* get_coordinates,
    PolygonTree* rootPolygon,
    NeighborPairMap* neighborPairMap,
    const enum Verbosity verbosity,
    ResourceStack resources
);


/// Declarations of static inline functions
static inline size_t PolygonTree__nChildren(
    const PolygonTree* const this
) {
    return this->children->size;
}

static inline PolygonTree** PolygonTree__children(
    const PolygonTree* const this
) {
    return PolygonTreeVector__elements(this->children);
}

static inline PolygonTree** PolygonTreeVector__elements(
    const PolygonTreeVector* this
) {
    return Vector__elements(this, PolygonTree*);
}
