
#include "DelaunayTable.Container.h"

#include <stdlib.h>
#include <assert.h>

#define N 1024


typedef HashMap TestHashMap;

// TestHashMap
size_t* size_t__copy(
    const size_t* size
) {
    size_t* const result = (size_t*) malloc(sizeof(size_t));
    if (!result) return NULL;

    *result = *size;
    return result;
}

#define size_t__delete free

size_t size_t__hash(
    const size_t* const size
) {
    return *size;
}

bool size_t__equality(
    const size_t* const size0,
    const size_t* const size1
) {
    return *size0 == *size1;
}

static void TestHashMap__clear(
    TestHashMap* this
) {
    HashMap__clear(
        this,
        &size_t__delete,
        &size_t__delete
    );
}

static void TestHashMap__delete(
    TestHashMap* this
) {
    HashMap__delete(
        this,
        &size_t__delete,
        &size_t__delete
    );
}

static bool TestHashMap__get(
    const TestHashMap* const this,
    const size_t key,
    size_t* const value
) {
    size_t* value_;
    bool result = HashMap__get(
        this,
        (Map__key) &key,
        (Map__value*) &value_,
        (Map__key__hash_function*) size_t__hash,
        (Map__key__equality_function*) size_t__equality
    );

    if (result) {
        *value = *value_;
    } else {
        *value = 0;
    }

    return result;
}

static int TestHashMap__set(
    TestHashMap* const this,
    const size_t key,
    const size_t value
) {
    return HashMap__set(
        this,
        (Map__key) &key,
        (Map__value) &value,
        (Map__key__copy_function*) size_t__copy,
        (Map__key__delete_function*) size_t__delete,
        (Map__key__hash_function*) size_t__hash,
        (Map__key__equality_function*) size_t__equality,
        (Map__value__copy_function*) size_t__copy,
        (Map__value__delete_function*) size_t__delete
    );
}

static bool TestHashMap__remove(
    TestHashMap* const this,
    const size_t key
) {
    return HashMap__remove(
        this,
        (Map__key) &key,
        (Map__key__delete_function*) size_t__delete,
        (Map__key__hash_function*) size_t__hash,
        (Map__key__equality_function*) size_t__equality,
        (Map__value__delete_function*) size_t__delete
    );
}


int main(int argc, char** argv) {

    TestHashMap* map = HashMap__new();
    if (!map) {return EXIT_FAILURE;}

    for (size_t i = 0 ; i*2 < N ; i++) {
        assert( (map->size) == i);

        if (TestHashMap__set(map, i*2, i*2)) {
            return EXIT_FAILURE;
        }

        for (size_t j = 0 ; j < N ; j++) {
            size_t value;

            if ( (j % 2 == 0) && (j <= i*2) ) {
                assert(  TestHashMap__get(map, j, &value) );
                assert(  value == j );
            } else {
                assert( !TestHashMap__get(map, j, &value) );
            }
        }
    }

    for (size_t i = 0 ; i*4 < N ; i++) {
        assert( (map->size) == (N/2 - i) );

        assert (  TestHashMap__remove(map, i*4) );
        assert ( !TestHashMap__remove(map, i*4) );

        for (size_t j = 0 ; j < N ; j++) {
            size_t value;

            if ( (j % 2 == 0) && !(j % 4 == 0 && j <= i*4) ) {
                assert(  TestHashMap__get(map, j, &value) );
                assert(  value == j );
            } else {
                assert( !TestHashMap__get(map, j, &value) );
            }
        }
    }

    TestHashMap__clear(map);

    assert( (map->size) == 0 );

    TestHashMap__delete(map);

    return EXIT_SUCCESS;
}
