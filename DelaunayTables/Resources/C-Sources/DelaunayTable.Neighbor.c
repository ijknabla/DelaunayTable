
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
        (Map__key__delete_function*)   IndexVector__delete,
        (Map__value__delete_function*) NeighborPair__delete
    );
}

bool NeighborPairMap__get(
    const NeighborPairMap* const this,
    const IndexVector* const face,
    Neighbor** const neighborPair
) {
    return HashMap__get(
        this,
        (Map__key)    face,
        (Map__value*) neighborPair,
        (Map__key__hash_function*)     IndexVector__hash,
        (Map__key__equality_function*) IndexVector__equality
    );
}

int NeighborPairMap__set(
    NeighborPairMap* const this,
    const IndexVector* face,
    const Neighbor neighborPair[2]
) {
    return HashMap__set(
        this,
        (Map__key)   face,
        (Map__value) neighborPair,
        (Map__key__copy_function*)     IndexVector__copy,
        (Map__key__delete_function*)   IndexVector__delete,
        (Map__key__hash_function*)     IndexVector__hash,
        (Map__key__equality_function*) IndexVector__equality,
        (Map__value__copy_function*)   NeighborPair__copy,
        (Map__value__delete_function*) NeighborPair__delete
    );
}

bool NeighborPairMap__remove(
    NeighborPairMap* const this,
    const IndexVector* const face
) {
    return HashMap__remove(
        this,
        (Map__key) face,
        (Map__key__delete_function*)   IndexVector__delete,
        (Map__key__hash_function*)     IndexVector__hash,
        (Map__key__equality_function*) IndexVector__equality,
        (Map__value__delete_function*) NeighborPair__delete
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
