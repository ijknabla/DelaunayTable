
#pragma once

#include "DelaunayTable.Container.h"


typedef struct {
    Object         resource;
    Object__delete deleter;
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


extern Object ResourceStack__ensure_delete_finally__impl__(
    ResourceStack  this,
    Object         resource,
    Object__delete deleter,
    const char* resource_expr,
    const char* file,
    const int   line
);

extern Object ResourceStack__ensure_delete_on_error__impl__(
    ResourceStack  this,
    Object         resource,
    Object__delete deleter,
    const char* resource_expr,
    const char* file,
    const int   line
);

#define ResourceStack__ensure_delete_finally(resources, expr, deleter) ( \
    ResourceStack__ensure_delete_finally__impl__( \
        (resources),                \
        (Object)         (expr),    \
        (Object__delete) (deleter), \
        #expr,                      \
        __FILE__,                   \
        __LINE__                    \
    )                               \
)

#define ResourceStack__ensure_delete_on_error(resources, expr, deleter) ( \
    ResourceStack__ensure_delete_on_error__impl__( \
        (resources),                \
        (Object)         (expr),    \
        (Object__delete) (deleter), \
        #expr,                      \
        __FILE__,                   \
        __LINE__                    \
    )                               \
)


extern void ResourceStack__raise_error(
    const ResourceStack resources
);
