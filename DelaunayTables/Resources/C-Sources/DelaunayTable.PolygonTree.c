
#include "DelaunayTable.PolygonTree.h"

#include "DelaunayTable.IndexVector.h"

#include <stdbool.h>


/// # FaceVector
typedef Vector FaceVector;

static FaceVector* FaceVector__new(
    const size_t size
) {
    return Vector__new(
        size,
        sizeof(IndexVector*)
    );
}

static inline IndexVector** FaceVector__elements(
    const FaceVector* const this
) {
    return Vector__elements(this, IndexVector*);
}

static void FaceVector__delete(
    FaceVector* const this
) {
    for (size_t i = 0 ; i < (this->size) ; i++) {
        IndexVector__delete(FaceVector__elements(this)[i]);
    }

    Vector__delete(this);
}

static int FaceVector__append(
    FaceVector* const this,
    IndexVector* const face
) {
    IndexVector* copied = IndexVector__copy(face);
    if (!copied) {
        return FAILURE;
    }

    return Vector__append(
        this,
        &copied,
        sizeof(IndexVector*)
    );
}


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

static int PolygonTreeVector__divide_polygon_inside(
    const size_t nDim,
    PolygonTreeVector* this,
    PolygonTree* const polygonToDivide,
    const size_t pointToDivide,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    NeighborPairMap* const neighborPairMap,
    FaceVector* const faceVector
) {
    int status = SUCCESS;

    const size_t previousPolygonVectorSize = this->size;

    IndexVector* face = IndexVector__new(nVerticesInFace(nDim));
    if (!face) {
        status = FAILURE; goto finally;
    }

    /**
     * Add new polygons.
     * Each new polygon has `nDim+1` vertices.
     * - One vertex is `pointToDivide`.
     * - `nDim` vertices are selected from the `nDim+1` vertices in `polygonToDivide`.
     */
    for (size_t iEx = 0 ; iEx < nVerticesInPolygon(nDim) ; iEx++) {
        // Alloc polygon & append PolygonTreeVector
        PolygonTree* const polygon = PolygonTree__new(nDim);
        if (!polygon) {
            status = FAILURE; goto finally;
        }

        status = PolygonTreeVector__append(
            this, polygon
        );
        if (status) {
            PolygonTree__delete(polygon);
            goto finally;
        }

        status = PolygonTree__append_child(
            polygonToDivide,
            polygon
        );
        if (status) {
            goto finally;
        }

        // Set vertices of polygon
        for (size_t i = 0 ; i < (nVerticesInPolygon(nDim)-1) ; i++) {
            if (i < iEx) {
                polygon->vertices[i] = polygonToDivide->vertices[i+0];
            } else {
                polygon->vertices[i] = polygonToDivide->vertices[i+1];
            }
        }
        polygon->vertices[nVerticesInPolygon(nDim)-1] = pointToDivide;

        sort__size_t__Array(polygon->vertices, nVerticesInPolygon(nDim));
    }

    PolygonTree** const newPolygons = PolygonTreeVector__elements(this) + previousPolygonVectorSize;

    /**
     * Add new faces inside `polygonToDivide`
     * Each new face has `nDim` vertices.
     * - One vertex is `pointToDivide`.
     * - `nDim-1` vertices are selected from the `nDim+1` vertices in `polygonToDivide`.
     */
    for (size_t iEx_a = 0       ; iEx_a < nVerticesInPolygon(nDim) ; iEx_a++)
    for (size_t iEx_b = iEx_a+1 ; iEx_b < nVerticesInPolygon(nDim) ; iEx_b++)
    {
        // Set vertices of face
        for (size_t i = 0 ; i < (nVerticesInFace(nDim)-1) ; i++) {
            if (i+0 < iEx_a) {
                IndexVector__elements(face)[i] = polygonToDivide->vertices[i+0];
            } else if (i+1 < iEx_b) {
                IndexVector__elements(face)[i] = polygonToDivide->vertices[i+1];
            } else {
                IndexVector__elements(face)[i] = polygonToDivide->vertices[i+2];
            }
        }
        IndexVector__elements(face)[nVerticesInFace(nDim)-1] = pointToDivide;

        sort__size_t__Array(IndexVector__elements(face), nVerticesInFace(nDim));

        // Set to neighborPairMap
        Neighbor neighborPair[2] = {
            {polygonToDivide->vertices[iEx_a], newPolygons[iEx_b]},
            {polygonToDivide->vertices[iEx_b], newPolygons[iEx_a]}
        };

        status = NeighborPairMap__set(
            neighborPairMap,
            face,
            neighborPair
        );
        if (status) {goto finally;}
    }

    /**
     * Update faces outside `polygonToDivide`
     * Each face has `nDim` vertices.
     * - `nDim` vertices are selected from the `nDim+1` vertices in `polygonToDivide`.
     */
    for (size_t iEx = 0 ; iEx < nVerticesInPolygon(nDim) ; iEx++) {
        // Set vertices of face
        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (i < iEx) {
                IndexVector__elements(face)[i] = polygonToDivide->vertices[i+0];
            } else {
                IndexVector__elements(face)[i] = polygonToDivide->vertices[i+1];
            }
        }

        // Update neighborPairMap
        status = NeighborPairMap__update_by_opposite(
            neighborPairMap,
            face,
            polygonToDivide->vertices[iEx],  // opposite_old
            pointToDivide,                   // opposite_new
            newPolygons[iEx]                 // polygon_new
        );
        if (status) {
            goto finally;
        }

        status = FaceVector__append(
            faceVector,
            face
        );
        if (status) {
            goto finally;
        }
    }

finally:

    if (face) {IndexVector__delete(face);}

    return status;
}

static int Face__is_valid(
    const IndexVector* const face,
    const NeighborPairMap* const neighborPairMap,
    const Points points,
    Points__get_coordinates* get_coordinates,
    bool* const validFace
) {
    int status = SUCCESS;

    const size_t nDim = face->size;

    *validFace = false;

    const double** shape = NULL;

    Neighbor* neighborPair;
    if (!NeighborPairMap__get(neighborPairMap, face, &neighborPair)) {
        status = FAILURE; goto finally;
    }
    if (!neighborPair[0].polygon || !neighborPair[1].polygon) {
        *validFace = true;
        goto finally;
    }

    shape = (const double**) MALLOC(
        nVerticesInPolygon(nDim) * sizeof(double*)
    );
    if (!shape) {
        status = FAILURE; goto finally;
    }

    const PolygonTree* const polygon = neighborPair[0].polygon;

    for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
        shape[i] = get_coordinates(points, polygon->vertices[i]);
    }

    const double* point = get_coordinates(points, neighborPair[1].opposite);

    bool insideCircumsphere;

    status = insideCircumsphereOfPolygon(
        nDim,
        shape,
        point,
        &insideCircumsphere
    );
    if (status) {
        goto finally;
    }

    *validFace = !insideCircumsphere;

finally:

    if (shape) {FREE(shape);}

    return status;
}

static int PolygonTreeVector__flip_face(
    const size_t nDim,
    PolygonTreeVector* const this,
    const IndexVector* faceToFlip,
    const size_t pointToDivide,
    const Points points,
    Points__get_coordinates* get_coordinates,
    NeighborPairMap* const neighborPairMap,
    FaceVector* faceVector
) {
    int status = SUCCESS;

    const size_t previousPolygonVectorSize = this->size;

    IndexVector* face = NULL;

    // Early return (check face is valid)
    bool validFace;

    status = Face__is_valid(
        faceToFlip,
        neighborPairMap,
        points,
        get_coordinates,
        &validFace
    );
    if (status)    {goto finally;}
    if (validFace) {goto finally;}

    face = IndexVector__new(nVerticesInFace(nDim));
    if (!face) {
        status = FAILURE; goto finally;
    }

    Neighbor* neighborPairToFlip = NULL;

    if (!NeighborPairMap__get(neighborPairMap, faceToFlip, &neighborPairToFlip)) {
        status = FAILURE; goto finally;
    }

    if (
        !neighborPairToFlip[0].polygon ||
        !neighborPairToFlip[1].polygon
    ) {
        status = FAILURE; goto finally;
    }

    /**
     * Add new polygons.
     * Each new polygon has `nDim+1` vertices.
     * - One vertex is `opposite0`.
     * - One vertex in `opposite1`.
     * - `nDim-1` vertices are selected from the `nDim` vertices in `faceToSplit`.
     */
    for (size_t iEx = 0 ; iEx < nVerticesInFace(nDim) ; iEx++) {
        // Alloc polygon & append PolygonTreeVector
        PolygonTree* const polygon = PolygonTree__new(nDim);
        if (!polygon) {
            status = FAILURE; goto finally;
        }

        status = PolygonTreeVector__append(
            this, polygon
        );
        if (status) {
            PolygonTree__delete(polygon);
            goto finally;
        }

        for (size_t i = 0 ; i < 2 ; i++) {
            status = PolygonTree__append_child(
                neighborPairToFlip[i].polygon,
                polygon
            );
            if (status) {
                goto finally;
            }
        }

        // Set vertices of polygon
        for (size_t i = 0 ; i < (nVerticesInFace(nDim)-1) ; i++) {
            if (i < iEx) {
                polygon->vertices[i] = IndexVector__elements(faceToFlip)[i+0];
            } else {
                polygon->vertices[i] = IndexVector__elements(faceToFlip)[i+1];
            }
        }
        polygon->vertices[nVerticesInPolygon(nDim)-2] = neighborPairToFlip[0].opposite;
        polygon->vertices[nVerticesInPolygon(nDim)-1] = neighborPairToFlip[1].opposite;

        sort__size_t__Array(polygon->vertices, nVerticesInPolygon(nDim));
    }

    PolygonTree** const newPolygons = PolygonTreeVector__elements(this) + previousPolygonVectorSize;

    /**
     * Add new faces inside `neighborPairToFlip`
     * Each new face has `nDim` vertices.
     * - Two vertex are `opposite` of `neighborPairToFlip`.
     * - `nDim-2` vertices are selected from the `nDim` vertices in `faceToFlip`.
     */
    for (size_t iEx_a = 0       ; iEx_a < nVerticesInFace(nDim) ; iEx_a++)
    for (size_t iEx_b = iEx_a+1 ; iEx_b < nVerticesInFace(nDim) ; iEx_b++) {
        // Set vertices of face
        for (size_t i = 0 ; i < nVerticesInFace(nDim)-2 ; i++) {
            if (i+0 < iEx_a) {
                IndexVector__elements(face)[i] = IndexVector__elements(faceToFlip)[i+0];
            } else if (i+1 < iEx_b) {
                IndexVector__elements(face)[i] = IndexVector__elements(faceToFlip)[i+1];
            } else {
                IndexVector__elements(face)[i] = IndexVector__elements(faceToFlip)[i+2];
            }
        }
        IndexVector__elements(face)[nVerticesInFace(nDim)-2] = neighborPairToFlip[0].opposite;
        IndexVector__elements(face)[nVerticesInFace(nDim)-1] = neighborPairToFlip[1].opposite;

        sort__size_t__Array(IndexVector__elements(face), nVerticesInFace(nDim));

        // Set to neighborPairMap
        Neighbor neighborPair[2] = {
            {IndexVector__elements(faceToFlip)[iEx_a], newPolygons[iEx_b]},
            {IndexVector__elements(faceToFlip)[iEx_b], newPolygons[iEx_a]}
        };

        status = NeighborPairMap__set(neighborPairMap, face, neighborPair);
        if (status) {goto finally;}
    }

    /**
     * Update faces outside `neighborPairToFlip`
     * Each new face has `nDim` vertices.
     * - One vertex is selected from `opposite` of `neighborPairToFlip`.
     * - `nDim-1` vertices are selected from the `nDim` vertices in `faceToFlip`.
     */
    for (size_t iNeighbor = 0 ; iNeighbor < 2               ; iNeighbor++) {
        const bool appendToFaceVector
            = neighborPairToFlip[iNeighbor].opposite != pointToDivide;

        for (size_t iEx = 0       ; iEx < nVerticesInFace(nDim) ; iEx++) {
            // Set vertices of face
            for (size_t i = 0 ; i < (nVerticesInFace(nDim)-1) ; i++) {
                if (i < iEx) {
                    IndexVector__elements(face)[i] = IndexVector__elements(faceToFlip)[i+0];
                } else {
                    IndexVector__elements(face)[i] = IndexVector__elements(faceToFlip)[i+1];
                }
            }
            IndexVector__elements(face)[nVerticesInFace(nDim)-1]
                = neighborPairToFlip[iNeighbor].opposite;

            sort__size_t__Array(IndexVector__elements(face), nVerticesInFace(nDim));

            // Update neighborPairMap
            status = NeighborPairMap__update_by_opposite(
                neighborPairMap,
                face,
                IndexVector__elements(faceToFlip)[iEx],
                neighborPairToFlip[1-iNeighbor].opposite,
                newPolygons[iEx]
            );
            if (status) {goto finally;}

            if (appendToFaceVector) {
                status = FaceVector__append(faceVector, face);
                if (status) {
                    goto finally;
                }
            }
        }
    }

    // Remove faceToFlip
    if (!NeighborPairMap__remove(neighborPairMap, faceToFlip)) {
        status = FAILURE; goto finally;
    }

finally:

    if (face) {IndexVector__delete(face);}

    return status;
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

    FaceVector* faceVector    = NULL;
    double*     divisionRatio = NULL;

    faceVector = FaceVector__new(0);
    if (!faceVector) {
        status = FAILURE; goto finally;
    }

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

    if (!divisionRatio__on_face(nDim, divisionRatio)) {
        status = PolygonTreeVector__divide_polygon_inside(
            nDim,
            this,
            polygonToDivide,
            pointToDivide,
            points,
            get_coordinates,
            neighborPairMap,
            faceVector
        );
        if (status) {goto finally;}
    }

    for (size_t i = 0 ; i < (faceVector->size) ; i++) {
        status = PolygonTreeVector__flip_face(
            nDim,
            this,
            FaceVector__elements(faceVector)[i],
            pointToDivide,
            points,
            get_coordinates,
            neighborPairMap,
            faceVector
        );
        if (status) {
            goto finally;
        }
    }

finally:

    if (faceVector)    {FaceVector__delete(faceVector);}
    if (divisionRatio) {FREE(divisionRatio);}

    return status;
}
