
#include "DelaunayTable.Neighbor.h"

#include <string.h>


/// # NeighborPair static functions
static Neighbor* NeighborPair__copy(
    const Neighbor this[2]
) {
    Neighbor* copied = (Neighbor*) MALLOC(sizeof(Neighbor[2]));
    if (!copied) {return NULL;}

    memcpy(copied, this, sizeof(Neighbor[2]));

    return copied;
}

static void NeighborPair__delete(
    Neighbor this[2]
) {
    FREE(this);
}


/// # NeighborPairMap methods
NeighborPairMap* NeighborPairMap__new(
) {
    return HashMap__new();
}

void NeighborPairMap__delete(
    NeighborPairMap* this
) {
    HashMap__delete(
        this,
        (Object__delete) IndexVector__delete,
        (Object__delete) NeighborPair__delete
    );
}

bool NeighborPairMap__get(
    const NeighborPairMap* const this,
    const IndexVector* const face,
    Neighbor** const neighborPair
) {
    return HashMap__get(
        this,
        face,
        (Object*) neighborPair,
        (Object__hash)  IndexVector__hash,
        (Object__equal) IndexVector__equal
    );
}

int NeighborPairMap__set(
    NeighborPairMap* const this,
    const IndexVector* face,
    const Neighbor neighborPair[2]
) {
    return HashMap__set(
        this,
        face,
        neighborPair,
        (Object__copy)   IndexVector__copy,
        (Object__delete) IndexVector__delete,
        (Object__hash)   IndexVector__hash,
        (Object__equal)  IndexVector__equal,
        (Object__copy)   NeighborPair__copy,
        (Object__delete) NeighborPair__delete
    );
}

bool NeighborPairMap__remove(
    NeighborPairMap* const this,
    const IndexVector* const face
) {
    return HashMap__remove(
        this,
        face,
        (Object__delete) IndexVector__delete,
        (Object__hash)   IndexVector__hash,
        (Object__equal)  IndexVector__equal,
        (Object__delete) NeighborPair__delete
    );
}

int NeighborPairMap__update_by_opposite(
    NeighborPairMap* this,
    const IndexVector* face,
    const size_t opposite_old,
    const size_t opposite_new,
    struct PolygonTree__TAG* polygon_new
) {
    Neighbor* neighborPair;

    if (!NeighborPairMap__get(this, face, &neighborPair)) {
        return FAILURE;
    }

    for (size_t i = 0 ; i < 2 ; i++) {
        if (
            neighborPair[i].polygon
            && neighborPair[i].opposite == opposite_old
        ) {
            neighborPair[i].polygon  = polygon_new;
            neighborPair[i].opposite = opposite_new;
            return SUCCESS;
        }
    }

    return FAILURE;
}
