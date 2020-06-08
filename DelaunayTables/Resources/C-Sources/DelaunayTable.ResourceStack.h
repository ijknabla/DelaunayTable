
#pragma once

#include "DelaunayTable.Container.h"


/// # basic data type for Resource, deleter
typedef void* Resource;
typedef void  Resource__deleter(Resource);

typedef struct {
    Resource resource;
    Resource__deleter* deleter;
} ResourceAndDeleter;

typedef Vector ResourcesAndDeleters;

struct ResourceStack__TAG {
    ResourcesAndDeleters* delete_finally;
    ResourcesAndDeleters* delete_on_error;
};

typedef struct ResourceStack__TAG* ResourceStack;


/// methods for ResourceStack
extern ResourceStack ResourceStack__new(
);

extern void ResourceStack__delete(
    const ResourceStack resources
);


extern void ResourceStack__enter(
    const ResourceStack resources
);

extern void ResourceStack__exit(
    const ResourceStack resources
);


extern Resource ResourceStack__ensure_delete_finally(
    const ResourceStack resources,
    Resource__deleter* const deleter,
    const Resource resource
);

extern Resource ResourceStack__ensure_delete_on_error(
    const ResourceStack resources,
    Resource__deleter* const deleter,
    const Resource resource
);


extern void ResourceStack__raise_error(
    const ResourceStack resources
);
