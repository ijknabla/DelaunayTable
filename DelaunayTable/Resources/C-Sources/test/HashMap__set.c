
#include "DelaunayTable.Container.h"

#include <stdlib.h>
#include <assert.h>

#define N 1024


typedef HashMap TestHashMap;

#define size_t__delete free

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

    TestHashMap__delete(map);

    return EXIT_SUCCESS;
}
