
#include "DelaunayTable.PolygonTree.h"


/// ## PolygonTree methods
PolygonTree* PolygonTree__new(
    const size_t nDim
) {
    PolygonTree* const this = (PolygonTree*) MALLOC(sizeof(PolygonTree));
    if (!this) {goto error;}

    this->vertices = NULL;
    this->children = NULL;

    this->vertices = (size_t*) CALLOC(nVerticesInPolygon(nDim), sizeof(size_t));
    if (!(this->vertices)) {goto error;}

    this->children = PolygonTreeVector__new();
    if (!(this->children)) {goto error;}

    return this;

error:

    if (this) {
        if (this->vertices) FREE(this->vertices);
        if (this->children) PolygonTreeVector__delete(this->children);
        FREE(this);
    }

    return NULL;
}

void PolygonTree__delete(
    PolygonTree* const this
) {
    FREE(this->vertices);
    PolygonTreeVector__delete(this->children);
    FREE(this);
}

int PolygonTree__append_child(
    PolygonTree* const this,
    PolygonTree* const child
) {
    return PolygonTreeVector__append(this->children, child);
}


/// ## PolygonTreeVector methods
PolygonTreeVector* PolygonTreeVector__new(
) {
    return (PolygonTreeVector*) Vector__new(
        2,
        sizeof(PolygonTree*)
    );
}

void PolygonTreeVector__delete(
    PolygonTreeVector* const this
) {
    Vector__delete(this);
}

void PolygonTreeVector__delete_elements(
    PolygonTreeVector* const this
) {
    for (size_t i = 0 ; i < (this->size) ; i++) {
        PolygonTree__delete(PolygonTreeVector__elements(this)[i]);
    }
}

extern int PolygonTreeVector__append(
    PolygonTreeVector* const this,
    PolygonTree* polygon
) {
    return Vector__append(
        this,
        &polygon,
        sizeof(PolygonTree*)
    );
}
