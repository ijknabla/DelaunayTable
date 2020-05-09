
#include "DelaunayTable.Container.h"

#include <stdlib.h>
#include <assert.h>

#define N 1024


int main(int argc, char** argv) {

    List list = List__INITIALIZER;

    assert( List__empty(&list) );

    for (size_t i = 0 ; i < N ; i++) {
        size_t* element;
        if (!(element = malloc(sizeof(size_t)))) {
            return EXIT_FAILURE;
        }

        *element = i;

        assert( List__size(&list) == i );

        if (List__append(&list, element)) {
            return EXIT_FAILURE;
        }
    }

    assert( List__size(&list) == N );

    {
        size_t index; ListNode* node;
        for (
            (index = 0, node = list.first);
            node;
            (index++  , node = node->next)
        ) {
            size_t* const element = node->element;
            assert(index == *element);
        }
    }

    List__clear(&list, free);

    assert( List__empty(&list) );

}
