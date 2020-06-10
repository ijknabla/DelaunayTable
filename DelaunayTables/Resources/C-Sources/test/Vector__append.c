
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
        (ConstObject) &l,
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
        assert( (vector->size) == (size_t) element );
        assert( (vector->size) <= (vector->capacity) );

        assert ( ULongVector__append(vector, element) == 0 );

        for (size_t index = 0 ; index < element+1 ; index++) {
            assert( ULongVector__elements(vector)[index] == (unsigned long) index );
        }
    }

    assert( (vector->size) == (size_t) N );
    assert( (vector->size) <= (vector->capacity) );

    ULongVector__delete(vector);

    return EXIT_SUCCESS;
}
