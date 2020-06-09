
#pragma once

#include "DelaunayTable.ResourceStack.h"
#include "DelaunayTable.Geometry.h"
#include "DelaunayTable.Container.h"
#include "DelaunayTable.Neighbor.h"


/// # PolyhedronTree & PolyhedronTreeVector
typedef Vector PolyhedronTreeVector;

typedef struct PolyhedronTree__TAG {
    size_t* vertices;
    PolyhedronTreeVector* children;
} PolyhedronTree;

/// ## PolyhedronTree methods
extern PolyhedronTree* PolyhedronTree__new(
    const size_t nDim
);

extern void PolyhedronTree__delete(
    PolyhedronTree* this
);

static inline size_t PolyhedronTree__nChildren(
    const PolyhedronTree* this
);

static inline PolyhedronTree** PolyhedronTree__children(
    const PolyhedronTree* this
);

extern int PolyhedronTree__append_child(
    PolyhedronTree* this,
    PolyhedronTree* child
);

extern int PolyhedronTree__calculate_divisionRatio(
    const size_t nDim,
    const PolyhedronTree* this,
    const double* coordinates,
    const Points points,
    Points__get_coordinates* get_coordinates,
    double* divisionRatio
);

extern int PolyhedronTree__find(
    const size_t nDim,
    PolyhedronTree* rootPolyhedron,
    const double* coordinates,
    const Points points,
    Points__get_coordinates* get_coordinates,
    PolyhedronTree** foundPolyhedron,
    double* divisionRatio
);

extern int PolyhedronTree__get_around(
    const size_t nDim,
    const PolyhedronTree* polyhedron,
    const IndexVector* overlapVertices,
    const NeighborPairMap* neighborPairMap,
    PolyhedronTreeVector* aroundPolyhedrons
);


/// ## PolyhedronTreeVector methods
extern PolyhedronTreeVector* PolyhedronTreeVector__new(
    const size_t size
);

extern void PolyhedronTreeVector__delete(
    PolyhedronTreeVector* this
);

extern void PolyhedronTreeVector__delete_elements(
    PolyhedronTreeVector* this
);

static inline PolyhedronTree** PolyhedronTreeVector__elements(
    const PolyhedronTreeVector* this
);

extern int PolyhedronTreeVector__append(
    PolyhedronTreeVector* this,
    PolyhedronTree* polyhedron
);

extern void PolyhedronTreeVector__divide_at_point(
    const size_t nDim,
    PolyhedronTreeVector* this,
    const size_t pointToDivide,
    const Points points,
    Points__get_coordinates* get_coordinates,
    PolyhedronTree* rootPolyhedron,
    NeighborPairMap* neighborPairMap,
    const enum Verbosity verbosity,
    ResourceStack resources
);


/// Declarations of static inline functions
static inline size_t PolyhedronTree__nChildren(
    const PolyhedronTree* const this
) {
    return this->children->size;
}

static inline PolyhedronTree** PolyhedronTree__children(
    const PolyhedronTree* const this
) {
    return PolyhedronTreeVector__elements(this->children);
}

static inline PolyhedronTree** PolyhedronTreeVector__elements(
    const PolyhedronTreeVector* this
) {
    return Vector__elements(this, PolyhedronTree*);
}
