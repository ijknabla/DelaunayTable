
#pragma once

#include "DelaunayTable.Container.h"


/// # basic data type for Resource, deleter
typedef void* Resource;
typedef void  Resource__deleter(Resource);

typedef struct {
    Resource resource;
    Resource__deleter* deleter;
} ResourceAndDeleter;
