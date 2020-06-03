
#include "DelaunayTable.PolygonTree.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

/// nDim must be 1
#define nDim      (1)

static const size_t nChildren = 2;  // If nChildren is 2, binary tree
static const size_t nDepth    = 10;

static void make_tree(
    PolygonTreeVector* const this,
    PolygonTree* const parent,
    const size_t depth,
    const size_t index
) {
    PolygonTree* polygon;
    assert ( (polygon = PolygonTree__new(nDim)) != NULL );

    /// nDim must be 1
    polygon->vertices[0] = depth;
    polygon->vertices[1] = index;

    assert ( PolygonTreeVector__append(this, polygon) == 0 );

    if (parent) {
        assert( PolygonTree__append_child(parent, polygon) == 0 );
    }

    if (depth == 0) return;

    for (size_t i = 0 ; i < nChildren ; i++) {
        make_tree(
            this,
            polygon,
            depth-1,
            nChildren * index + i
        );
    }
}

static void assert_polygon(
    const PolygonTree* const polygon
) {
    const size_t depth = polygon->vertices[0];
    const size_t index = polygon->vertices[1];

    if (depth == 0) {
        assert( PolygonTree__nChildren(polygon) == 0 );
    } else {
        assert( PolygonTree__nChildren(polygon) == nChildren );
        for (size_t i = 0 ; i < nChildren ; i++) {
            assert( PolygonTree__children(polygon)[i]->vertices[0] == depth-1            );
            assert( PolygonTree__children(polygon)[i]->vertices[1] == nChildren*index + i);
        }
    }
}

static inline size_t nPolygons() {
    size_t result = 1;
    for (size_t i = 0 ; i < nDepth ; i++) {
        result = result * nChildren + 1;
    }
    return result;
}


int main(int argc, char** argv) {
    PolygonTreeVector* polygons;
    assert( (polygons = PolygonTreeVector__new(0)) != NULL );

    make_tree(polygons, NULL, nDepth, 0);

    assert( polygons->size == nPolygons() );

    for (size_t i = 0 ; i < (polygons->size) ; i++) {
        assert_polygon(
            PolygonTreeVector__elements(polygons)[i]
        );
    }

    PolygonTreeVector__delete_elements(polygons);
    PolygonTreeVector__delete(polygons);

    return EXIT_SUCCESS;
}
