
#pragma once

#include "DelaunayTable.Geometry.h"
#include "DelaunayTable.Container.h"


/// # PolygonTree & PolygonTreeVector
typedef Vector PolygonTreeVector;

typedef struct {
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


/// ## PolygonTreeVector methods
extern PolygonTreeVector* PolygonTreeVector__new(
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
