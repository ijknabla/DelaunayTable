
#pragma once

#include "DelaunayTable.Container.h"

#include <stdbool.h>
#include <stddef.h>


/// # Utility functions
extern void sort__size_t__Array(
    size_t* sizes,
    const size_t nSizes
);

static inline bool contains__size_t__Array(
    const size_t        nSizes0,
    const size_t* const sizes0,
    const size_t        nSizes1,
    const size_t* const sizes1
) {
    size_t i1 = 0;
    for (
        size_t i0 = 0;
        (i0 < nSizes0 && i1 < nSizes1);
        i0++
    ) {
        if (sizes0[i0] == sizes1[i1]) {i1++;}
    }
    return i1 == nSizes1;
}


/// # IndexVector
typedef Vector IndexVector;

/// ## IndexVector methods
extern IndexVector* IndexVector__new(
    const size_t size
);

extern IndexVector* IndexVector__copy(
    const IndexVector* this
);

extern void IndexVector__delete(
    IndexVector* this
);

extern int IndexVector__append(
    IndexVector* this,
    const size_t index
);

extern void IndexVector__sort(
    IndexVector* this
);

extern bool IndexVector__equality(
    const IndexVector* v0,
    const IndexVector* v1
);

extern size_t IndexVector__hash(
    const IndexVector* this
);

static inline size_t* IndexVector__elements(
    const IndexVector* const this
) {
    return Vector__elements(this, size_t);
}
