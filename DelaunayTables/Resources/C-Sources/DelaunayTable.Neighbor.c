
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
