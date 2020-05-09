
#include "DelaunayTable.Container.h"

#include <stdlib.h>
#include <assert.h>

#define N 1024


int main(int argc, char** argv) {

    HashMap* map = HashMap__new();
    if (!map) {return EXIT_FAILURE;}

    HashMap__delete(
        map,
        NULL,
        NULL
    );

    return EXIT_SUCCESS;
}
