
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


extern Resource ResourceStack__ensure_delete_finally__impl__(
    ResourceStack      this,
    Resource           resource,
    Resource__deleter* deleter,
    const char* resource_expr,
    const char* file,
    const int   line
);

extern Resource ResourceStack__ensure_delete_on_error__impl__(
    ResourceStack      this,
    Resource           resource,
    Resource__deleter* deleter,
    const char* resource_expr,
    const char* file,
    const int   line
);

#define ResourceStack__ensure_delete_finally2(resources, expr, deleter) ( \
    ResourceStack__ensure_delete_finally__impl__( \
        (resources),                    \
        (Resource)           (expr),    \
        (Resource__deleter*) (deleter), \
        #expr,                          \
        __FILE__,                       \
        __LINE__                        \
    )                                   \
)

#define ResourceStack__ensure_delete_on_error2(resources, expr, deleter) ( \
    ResourceStack__ensure_delete_on_error__impl__( \
        (resources),                    \
        (Resource)           (expr),    \
        (Resource__deleter*) (deleter), \
        #expr,                          \
        __FILE__,                       \
        __LINE__                        \
    )                                   \
)


extern void ResourceStack__raise_error(
    const ResourceStack resources
);
