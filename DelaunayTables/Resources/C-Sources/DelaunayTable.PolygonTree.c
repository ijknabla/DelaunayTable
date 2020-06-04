
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

    this->children = PolygonTreeVector__new(0);
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

int PolygonTree__find(
    const size_t nDim,
    PolygonTree* const root_polygon,
    const double* const coordinates,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    PolygonTree** const found_polygon,
    double* const divisionRatio
) {
    int status = SUCCESS;
    *found_polygon = NULL;

    const double** const shape = (const double**) MALLOC(
        nVerticesInPolygon(nDim) * sizeof(double*)
    );
    if (!shape) {status = FAILURE; goto finally;}

    for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
        shape[i] = get_coordinates(points, root_polygon->vertices[i]);
    }

    status = divisionRatioFromPolygonVertices(
        nDim,
        shape,
        coordinates,
        divisionRatio
    );
    if (status) {goto finally;}

    // if coordinates not in root_polygon: SUCCESS, result is NULL
    if (!divisionRatio__inside(nDim, divisionRatio)) {
        status = SUCCESS; goto finally;
    }

    // ** coordinates in root_polygon **

    // if root_polygon does not have children: SUCCESS, result is root_polygon
    if (PolygonTree__nChildren(root_polygon) == 0) {
        *found_polygon = root_polygon;
        status = SUCCESS; goto finally;
    }

    // ** root_polygon has children **

    // if any find(child, ...) success: SUCCESS, return first result
    for (size_t i = 0 ; i < PolygonTree__nChildren(root_polygon) ; i++) {
        status = PolygonTree__find(
            nDim,
            PolygonTree__children(root_polygon)[i],
            coordinates,
            points,
            get_coordinates,
            found_polygon,
            divisionRatio
        );
        if (status) {goto finally;}

        if (*found_polygon) {goto finally;}
    }

    // else: failure
    *found_polygon = NULL;
    status = FAILURE; goto finally;

finally:

    if (shape) {FREE(shape);}

    return status;
}


/// ## PolygonTreeVector methods
PolygonTreeVector* PolygonTreeVector__new(
    const size_t size
) {
    return Vector__new(size, sizeof(PolygonTree*));
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
