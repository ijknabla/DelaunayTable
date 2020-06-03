
#pragma once

#include "DelaunayTable.Container.h"

#include <stdbool.h>
#include <stddef.h>


/// # Utility function
extern void sort__size_t__Array(
    size_t* sizes,
    const size_t nSizes
);


/// # IndexVector
typedef Vector IndexVector;

/// ## IndexVector methods
extern IndexVector* IndexVector__new(
    const size_t capacity
);

extern IndexVector* IndexVector__copy(
    const IndexVector* this
);

extern void IndexVector__delete(
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

static inline void IndexVector__sort(
    IndexVector* const this
) {
    sort__size_t__Array(
        IndexVector__elements(this),
        this->size
    );
}
