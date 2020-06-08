
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
