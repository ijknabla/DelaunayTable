
#include "DelaunayTable.IndexVector.h"


/// ## IndexVector methods
IndexVector* IndexVector__new(
    const size_t capacity
) {
    return Vector__new(capacity, sizeof(size_t));
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
