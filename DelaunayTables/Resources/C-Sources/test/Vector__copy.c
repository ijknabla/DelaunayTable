
#include "DelaunayTable.Container.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#define N 1024


static Vector* ULongVector__new(
    const size_t size
) {
    return Vector__new(size, sizeof(unsigned long));
}

static Vector* ULongVector__copy(
    const Vector* const this
) {
    return Vector__copy(
        this,
        sizeof(unsigned long)
    );
}

static void ULongVector__delete(
    Vector* const this
) {
    Vector__delete(this);
}

static int ULongVector__append(
    Vector* const this,
    const unsigned long l
) {
    return Vector__append(
        this,
        (Sequence__element) &l,
        sizeof(unsigned long)
    );
}

static inline unsigned long* ULongVector__elements(
    const Vector* const this
) {
    return Vector__elements(this, unsigned long);
}


int main(int argc, char** argv) {

    Vector* vector;
    assert( (vector = ULongVector__new(0)) != NULL );

    for (unsigned long element = 0; element < (unsigned long) N ; element++) {
        assert ( ULongVector__append(vector, element) == 0 );
    }

    Vector* copied_vector;
    assert( (copied_vector = ULongVector__copy(vector)) != NULL );

    assert( (vector->size       ) == (size_t) N );
    assert( (copied_vector->size) == (size_t) N );

    for (size_t i = 0 ; i < (vector->size) ; i++) {
        assert( ULongVector__elements(vector)[i] == ULongVector__elements(copied_vector)[i] );
    }

    ULongVector__delete(copied_vector);
    ULongVector__delete(vector);

    return EXIT_SUCCESS;
}
