
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

static void TestHashMap__delete(
    TestHashMap* this
) {
    HashMap__delete(
        this,
        &size_t__delete,
        &size_t__delete
    );
}


int main(int argc, char** argv) {

    TestHashMap* map = HashMap__new();
    if (!map) {return EXIT_FAILURE;}

    for (size_t i = 0 ; i < N ; i+=2) {
        if (TestHashMap__set(map, i, i)) {
            return EXIT_FAILURE;
        }
    }

    TestHashMap__delete(map);

    return EXIT_SUCCESS;
}
