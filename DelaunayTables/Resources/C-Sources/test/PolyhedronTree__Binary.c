
#include "DelaunayTable.PolyhedronTree.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

/// nDim must be 1
#define nDim      (1)

static const size_t nChildren = 2;  // If nChildren is 2, binary tree
static const size_t nDepth    = 10;

static void make_tree(
    PolyhedronTreeVector* const this,
    PolyhedronTree* const parent,
    const size_t depth,
    const size_t index
) {
    PolyhedronTree* polyhedron;
    assert ( (polyhedron = PolyhedronTree__new(nDim)) != NULL );

    /// nDim must be 1
    polyhedron->vertices[0] = depth;
    polyhedron->vertices[1] = index;

    assert ( PolyhedronTreeVector__append(this, polyhedron) == 0 );

    if (parent) {
        assert( PolyhedronTree__append_child(parent, polyhedron) == 0 );
    }

    if (depth == 0) return;

    for (size_t i = 0 ; i < nChildren ; i++) {
        make_tree(
            this,
            polyhedron,
            depth-1,
            nChildren * index + i
        );
    }
}

static void assert_polyhedron(
    const PolyhedronTree* const polyhedron
) {
    const size_t depth = polyhedron->vertices[0];
    const size_t index = polyhedron->vertices[1];

    if (depth == 0) {
        assert( PolyhedronTree__nChildren(polyhedron) == 0 );
    } else {
        assert( PolyhedronTree__nChildren(polyhedron) == nChildren );
        for (size_t i = 0 ; i < nChildren ; i++) {
            assert( PolyhedronTree__children(polyhedron)[i]->vertices[0] == depth-1            );
            assert( PolyhedronTree__children(polyhedron)[i]->vertices[1] == nChildren*index + i);
        }
    }
}

static inline size_t nPolyhedrons() {
    size_t result = 1;
    for (size_t i = 0 ; i < nDepth ; i++) {
        result = result * nChildren + 1;
    }
    return result;
}


int main(int argc, char** argv) {
    PolyhedronTreeVector* polyhedrons;
    assert( (polyhedrons = PolyhedronTreeVector__new(0)) != NULL );

    make_tree(polyhedrons, NULL, nDepth, 0);

    assert( polyhedrons->size == nPolyhedrons() );

    for (size_t i = 0 ; i < (polyhedrons->size) ; i++) {
        assert_polyhedron(
            PolyhedronTreeVector__elements(polyhedrons)[i]
        );
    }

    PolyhedronTreeVector__delete_elements(polyhedrons);
    PolyhedronTreeVector__delete(polyhedrons);

    return EXIT_SUCCESS;
}
