
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
    PolygonTree* const rootPolygon,
    const double* const coordinates,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    PolygonTree** const foundPolygon,
    double* const divisionRatio
) {
    int status = SUCCESS;
    *foundPolygon = NULL;

    const double** const shape = (const double**) MALLOC(
        nVerticesInPolygon(nDim) * sizeof(double*)
    );
    if (!shape) {status = FAILURE; goto finally;}

    for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
        shape[i] = get_coordinates(points, rootPolygon->vertices[i]);
    }

    status = divisionRatioFromPolygonVertices(
        nDim,
        shape,
        coordinates,
        divisionRatio
    );
    if (status) {goto finally;}

    // if coordinates not in rootPolygon: SUCCESS, result is NULL
    if (!divisionRatio__inside(nDim, divisionRatio)) {
        status = SUCCESS; goto finally;
    }

    // ** coordinates in rootPolygon **

    // if rootPolygon does not have children: SUCCESS, result is rootPolygon
    if (PolygonTree__nChildren(rootPolygon) == 0) {
        *foundPolygon = rootPolygon;
        status = SUCCESS; goto finally;
    }

    // ** rootPolygon has children **

    // if any find(child, ...) success: SUCCESS, return first result
    for (size_t i = 0 ; i < PolygonTree__nChildren(rootPolygon) ; i++) {
        status = PolygonTree__find(
            nDim,
            PolygonTree__children(rootPolygon)[i],
            coordinates,
            points,
            get_coordinates,
            foundPolygon,
            divisionRatio
        );
        if (status) {goto finally;}

        if (*foundPolygon) {goto finally;}
    }

    // else: failure
    *foundPolygon = NULL;
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

int PolygonTreeVector__append(
    PolygonTreeVector* const this,
    PolygonTree* polygon
) {
    return Vector__append(
        this,
        &polygon,
        sizeof(PolygonTree*)
    );
}

int PolygonTreeVector__divide_at_point(
    const size_t nDim,
    PolygonTreeVector* const this,
    const size_t pointToDivide,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    PolygonTree* const rootPolygon,
    NeighborPairMap* const neighborPairMap
) {
    int status = SUCCESS;

    double* divisionRatio = NULL;

    divisionRatio = (double*) MALLOC(
        nVerticesInPolygon(nDim) * sizeof(double)
    );
    if (!divisionRatio) {
        status = FAILURE; goto finally;
    }

    const double* const coordinatesToDivide
        = get_coordinates(points, pointToDivide);

    PolygonTree* polygonToDivide;

    status = PolygonTree__find(
        nDim,
        rootPolygon,
        coordinatesToDivide,
        points,
        get_coordinates,
        &polygonToDivide,
        divisionRatio
    );
    if (status) {goto finally;}

    if (!polygonToDivide) {
        status = FAILURE; goto finally;
    }

finally:

    if (divisionRatio) {FREE(divisionRatio);}

    return status;
}
