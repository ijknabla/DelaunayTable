
#include "DelaunayTable.ResourceStack.h"

#include <stdbool.h>


static ResourcesAndDeleters* ResourcesAndDeleters__new(
    const size_t size
) {
    return Vector__new(size, sizeof(ResourceAndDeleter));
}

static void ResourcesAndDeleters__delete(
    ResourcesAndDeleters* const this
) {
    Vector__delete(this);
}

static int ResourcesAndDeleters__append(
    ResourcesAndDeleters* const this,
    const ResourceAndDeleter* const element
) {
    return Vector__append(
        this,
        (Sequence__element) element,
        sizeof(ResourceAndDeleter)
    );
}

static inline ResourceAndDeleter* ResourcesAndDeleters__elements(
    const ResourcesAndDeleters* const this
) {
    return Vector__elements(this, ResourceAndDeleter);
}

static bool ResourcesAndDeleters__pop(
    ResourcesAndDeleters* const this,
    ResourceAndDeleter* const element
) {
    if (this->size == 0) {
        element->resource = NULL;
        element->deleter  = NULL;
        return false;
    }

    ResourceAndDeleter* lastElement = ResourcesAndDeleters__elements(this) + (this->size - 1);

    element->resource = lastElement->resource;
    element->deleter  = lastElement->deleter;

    (this->size)--;

    return true;
}


// ResourceStack methods
ResourceStack ResourceStack__new(
) {
    ResourceStack this = (struct ResourceStack__TAG*) MALLOC(
        sizeof(struct ResourceStack__TAG)
    );
    if (!this) {
        goto error;
    }

    if (!(this->delete_finally = ResourcesAndDeleters__new(0))) {
        goto error;
    }
    if (!(this->delete_on_error = ResourcesAndDeleters__new(0))) {
        goto error;
    }

    return this;

error:

    if (this) {
        if (this->delete_finally) {
            ResourcesAndDeleters__delete(this->delete_finally);
        }
        if (this->delete_on_error) {
            ResourcesAndDeleters__delete(this->delete_on_error);
        }
        FREE(this);
    }

    Runtime__send_error(
        "FatalError :: failed to allocate new ResourceStack\n"
        "at %s:%d",
        __FILE__, __LINE__
    );
}

extern void ResourceStack__delete(
    const ResourceStack this
) {
    ResourceAndDeleter resourceAndDeleter;
    while (ResourcesAndDeleters__pop(this->delete_finally, &resourceAndDeleter)) {
        if (resourceAndDeleter.deleter && resourceAndDeleter.resource) {
            resourceAndDeleter.deleter(resourceAndDeleter.resource);
        }
    }
    ResourcesAndDeleters__delete(this->delete_finally);

    ResourcesAndDeleters__delete(this->delete_on_error);

    FREE(this);
}


void ResourceStack__enter(
    const ResourceStack this
) {
    const ResourceAndDeleter resourceAndDeleter = {};
    int status = ResourcesAndDeleters__append(this->delete_finally, &resourceAndDeleter);
    if (status) {
        ResourceStack__raise_error(this);

        Runtime__send_error(
            "FatalError :: failed to append new frame to ResourceStack\n"
            "at %s:%d",
            __FILE__, __LINE__
        );
    }
}

void ResourceStack__exit(
    const ResourceStack this
) {
    ResourceAndDeleter resourceAndDeleter;
    while (ResourcesAndDeleters__pop(this->delete_finally, &resourceAndDeleter)) {
        if (!resourceAndDeleter.resource && !resourceAndDeleter.deleter) {
            break;
        }
        if (resourceAndDeleter.deleter && resourceAndDeleter.resource) {
            resourceAndDeleter.deleter(resourceAndDeleter.resource);
        }
    }
}

extern void ResourceStack__raise_error(
    const ResourceStack this
) {
    ResourceAndDeleter resourceAndDeleter;
    while (ResourcesAndDeleters__pop(this->delete_finally, &resourceAndDeleter)) {
        if (resourceAndDeleter.deleter && resourceAndDeleter.resource) {
            resourceAndDeleter.deleter(resourceAndDeleter.resource);
        }
    }
    ResourcesAndDeleters__delete(this->delete_finally);

    while (ResourcesAndDeleters__pop(this->delete_on_error, &resourceAndDeleter)) {
        if (resourceAndDeleter.deleter && resourceAndDeleter.resource) {
            resourceAndDeleter.deleter(resourceAndDeleter.resource);
        }
    }
    ResourcesAndDeleters__delete(this->delete_on_error);

    FREE(this);
}
