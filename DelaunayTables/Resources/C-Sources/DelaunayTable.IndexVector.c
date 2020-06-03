
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
