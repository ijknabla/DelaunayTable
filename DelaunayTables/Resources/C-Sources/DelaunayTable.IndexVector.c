
#include "DelaunayTable.IndexVector.h"

#include <stdlib.h>


/// # Utility function
static int compare__size_t(const void* a, const void* b) {
    const size_t sa = *(size_t*) a;
    const size_t sb = *(size_t*) b;
    if      (sa < sb) { return -1;}
    else if (sa > sb) { return +1;}
    else              { return  0;}
}

void sort__size_t__Array(
    size_t* const sizes,
    const size_t nSizes
) {
    qsort(sizes, nSizes, sizeof(size_t), compare__size_t);
}


/// ## IndexVector methods
IndexVector* IndexVector__new(
    const size_t size
) {
    return Vector__new(size, sizeof(size_t));
}

IndexVector* IndexVector__copy(
    const IndexVector* const this
) {
    return Vector__copy(this, sizeof(size_t));
}

extern void IndexVector__delete(
    IndexVector* const this
) {
    Vector__delete(this);
}

int IndexVector__append(
    IndexVector* const this,
    const size_t index
) {
    return Vector__append(
        this,
        (ConstObject) &index,
        sizeof(size_t)
    );
}

void IndexVector__sort(
    IndexVector* const this
) {
    Vector__sort(
        this,
        compare__size_t,
        sizeof(size_t)
    );
}

bool IndexVector__equality(
    const IndexVector* const v0,
    const IndexVector* const v1
) {
    if (v0->size != v1->size) {
        return false;
    }

    for (size_t i = 0 ; i < (v0->size) ; i++) {
        if (IndexVector__elements(v0)[i] != IndexVector__elements(v1)[i]) {
            return false;
        }
    }

    return true;
}

size_t IndexVector__hash(
    const IndexVector* const this
) {
    size_t hash = 0;
    for (size_t i = 0 ; i < (this->size) ; i++) {
        // 11, 13 is prime number
        hash += 11 * hash + 13 * IndexVector__elements(this)[i];
    }
    return hash;
}
