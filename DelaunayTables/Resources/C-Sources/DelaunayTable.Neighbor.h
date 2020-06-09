
#pragma once

#include "DelaunayTable.Container.h"
#include "DelaunayTable.IndexVector.h"

#include <stdbool.h>
#include <stddef.h>


struct PolyhedronTree__TAG;

typedef struct {
    size_t opposite;
    struct PolyhedronTree__TAG* polyhedron;
} Neighbor;


/// # NeighborPairMap
typedef HashMap NeighborPairMap;

/// # NeighborPairMap methods
extern NeighborPairMap* NeighborPairMap__new(
);

extern void NeighborPairMap__delete(
    NeighborPairMap* this
);

extern bool NeighborPairMap__get(
    const NeighborPairMap* this,
    const IndexVector* face,
    Neighbor** neighborPair
);

extern int NeighborPairMap__set(
    NeighborPairMap* this,
    const IndexVector* face,
    const Neighbor neighborPair[2]
);

extern bool NeighborPairMap__remove(
    NeighborPairMap* this,
    const IndexVector* face
);

extern int NeighborPairMap__update_by_opposite(
    NeighborPairMap* this,
    const IndexVector* face,
    const size_t opposite_old,
    const size_t opposite_new,
    struct PolyhedronTree__TAG* polyhedron_new
);
