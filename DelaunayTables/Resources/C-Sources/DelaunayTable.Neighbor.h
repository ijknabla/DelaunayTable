
#pragma once

#include "DelaunayTable.IndexVector.h"
#include "DelaunayTable.PolygonTree.h"

#include <stddef.h>


typedef struct {
    size_t opposite;
    PolygonTree* polygon;
} Neighbor;
