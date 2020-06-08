
#include "DelaunayTable.PolygonTree.h"

#include "DelaunayTable.Error.h"
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

int PolygonTree__calculate_divisionRatio(
    const size_t nDim,
    const PolygonTree* const this,
    const double* const coordinates,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    double* const divisionRatio
) {
    int status = SUCCESS;

    const double** const shape = (const double**) MALLOC(
        nVerticesInPolygon(nDim) * sizeof(double*)
    );
    if (!shape) {status = FAILURE; goto finally;}

    for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
        shape[i] = get_coordinates(points, this->vertices[i]);
    }

    status = divisionRatioFromPolygonVertices(
        nDim,
        shape,
        coordinates,
        divisionRatio
    );
    if (status) {goto finally;}

finally:

    if (shape) {FREE(shape);}

    return status;
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

    status = PolygonTree__calculate_divisionRatio(
        nDim,
        rootPolygon,
        coordinates,
        points,
        get_coordinates,
        divisionRatio
    );
    if (status) {
        *foundPolygon = NULL;
        return status;
    }

    // if coordinates not in rootPolygon: SUCCESS, result is NULL
    if (!divisionRatio__inside(nDim, divisionRatio)) {
        *foundPolygon = NULL;
        return SUCCESS;
    }

    // ** coordinates in rootPolygon **

    // if rootPolygon does not have children: SUCCESS, result is rootPolygon
    if (PolygonTree__nChildren(rootPolygon) == 0) {
        *foundPolygon = rootPolygon;
        return SUCCESS;
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
        if (status) {
            return status;
        }

        if (*foundPolygon) return SUCCESS;
    }

    // else: failure
    *foundPolygon = NULL;
    return FAILURE;
}

int PolygonTree__get_around(
    const size_t nDim,
    const PolygonTree* const polygon,
    const IndexVector* const overlapVertices,
    const NeighborPairMap* const neighborPairMap,
    PolygonTreeVector* const aroundPolygons
) {
    // Early return
    for (size_t i = 0 ; i < (aroundPolygons->size) ; i++) {
        if (polygon == PolygonTreeVector__elements(aroundPolygons)[i]) {
            return SUCCESS;
        }
    }

    int status = SUCCESS;

    // resources
    IndexVector* face = IndexVector__new(nVerticesInFace(nDim));
    if (!face) {
        status = FAILURE; goto finally;
    }

    status = PolygonTreeVector__append(
        aroundPolygons,
        (PolygonTree*) polygon
    );
    if (status) {
        goto finally;
    }

    for (size_t iEx = 0 ; iEx < nVerticesInPolygon(nDim) ; iEx++) {
        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (i < iEx) {
                IndexVector__elements(face)[i] = polygon->vertices[i+0];
            } else {
                IndexVector__elements(face)[i] = polygon->vertices[i+1];
            }
        }

        if (!contains__size_t__Array(
            nVerticesInFace(nDim), IndexVector__elements(face),
            overlapVertices->size, IndexVector__elements(overlapVertices)
        )) {
            continue;
        }

        Neighbor* neighborPair;
        if (!NeighborPairMap__get(neighborPairMap, face, &neighborPair)) {
            status = FAILURE; goto finally;
        }

        for (size_t i = 0 ; i < 2 ; i++) {
            PolygonTree* candidate = neighborPair[i].polygon;
            if (candidate && candidate != polygon) {
                status = PolygonTree__get_around(
                    nDim,
                    candidate,
                    overlapVertices,
                    neighborPairMap,
                    aroundPolygons
                );
                if (status) {
                    goto finally;
                }
            }
        }
    }

finally:

    if (face) {IndexVector__delete(face);}

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
    FaceVector* const faceVector,
    const enum Verbosity verbosity
) {
    if (verbosity >= Verbosity__debug) {
        Runtime__send_message("- - Divide polygon by inside point");
    }

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

        if (verbosity >= Verbosity__debug) {
            char buffer[1024];

            sprintf(buffer, "- - - Append new polygon {");
            for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
                sprintf(
                    buffer+strlen(buffer), "%lu%s",
                    polygon->vertices[i]+1,
                    (i < (nVerticesInPolygon(nDim)-1)) ? ", " : "}"
                );
            }

            Runtime__send_message(buffer);
        }
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

static int PolygonTreeVector__divide_polygon_by_face(
    const size_t nDim,
    PolygonTreeVector* const this,
    PolygonTree* const polygonToDivide,
    const size_t pointToDivide,
    const double* divisionRatio,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    NeighborPairMap* const neighborPairMap,
    FaceVector* const faceVector,
    const enum Verbosity verbosity
) {
    if (verbosity >= Verbosity__debug) {
        Runtime__send_message("- - Divide polygon by point on face");
    }

    int status = SUCCESS;

    const size_t previousPolygonVectorSize = this->size;

    // Resources
    PolygonTreeVector* aroundPolygons  = NULL;
    IndexVector*       overlapVertices = NULL;
    IndexVector*       face            = NULL;
    bool*              faceChecked     = NULL;

    aroundPolygons = PolygonTreeVector__new(0);
    if (!aroundPolygons) {
        status = FAILURE; goto finally;
    }

    overlapVertices = IndexVector__new(0);
    if (!overlapVertices) {
        status = FAILURE; goto finally;
    }

    face = IndexVector__new(nVerticesInFace(nDim));
    if (!face) {
        status = FAILURE; goto finally;
    }

    // Get `overlapVertices`
    for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
        if (double__compare(divisionRatio[i], 0.0) != 0) {
            status = IndexVector__append(
                overlapVertices,
                polygonToDivide->vertices[i]
            );
            if (status) {
                goto finally;
            }
        }
    }

    // Get `aroundPolygons`
    status = PolygonTree__get_around(
        nDim,
        polygonToDivide,
        overlapVertices,
        neighborPairMap,
        aroundPolygons
    );
    if (status) {
        goto finally;
    }

    const size_t nAroundPolygons  = aroundPolygons->size;
    const size_t nOverlapVertices = overlapVertices->size;

    if (verbosity >= Verbosity__detail) {
        Runtime__send_message(
            "- - - Find %lu around polygons",
            nAroundPolygons
        );

        for (size_t iAround = 0 ; iAround < nAroundPolygons ; iAround++) {
            const PolygonTree* const aroundPolygon
                = PolygonTreeVector__elements(aroundPolygons)[iAround];

            char buffer[1024];
            sprintf(
                buffer, "- - - - aroundPolygon[%2lu] {",
                iAround+1
            );
            for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
                sprintf(
                    buffer+strlen(buffer), "%lu%s",
                    aroundPolygon->vertices[i]+1,
                    (i < (nVerticesInPolygon(nDim)-1)) ? ", " : "}"
                );
            }

            Runtime__send_message(buffer);
        }
    }

    /**
     * Add new polygons.
     * Repeat new polygons creation for all `aroundPolygon` in `aroundPolygons`.
     * - Each new polygon has `nDim+1` vertices.
     * - - One vertex is `pointToDivide`.
     * - - `nOverlapVertices-1`      vertices are selected from the vertices in `overlapVertices`.
     * - - `nDim+1-nOverlapVertices` vertices are selected from non-overlap vertices of `aroundPolygon`.
     */
    for (size_t iAround = 0 ; iAround < nAroundPolygons ; iAround++) {
        PolygonTree* const aroundPolygon
            = PolygonTreeVector__elements(aroundPolygons)[iAround];

        for (size_t iEx = 0 ; iEx < nOverlapVertices ; iEx++) {
            // Allocate polygon & append PolygonTrees Vector
            PolygonTree* polygon = PolygonTree__new(nDim);
            if (!polygon) {
                status = FAILURE; goto finally;
            }

            status = PolygonTreeVector__append(
                this,
                polygon
            );
            if (status) {
                PolygonTree__delete(polygon);
                goto finally;
            }

            status = PolygonTree__append_child(
                aroundPolygon,
                polygon
            );
            if (status) {
                goto finally;
            }

            // Set vertices of polygon
            const size_t vertexToExclude = IndexVector__elements(overlapVertices)[iEx];

            size_t offset = 0;
            for (size_t i = 0 ; i < (nVerticesInPolygon(nDim)-1) ; i++) {
                if (aroundPolygon->vertices[i+offset] == vertexToExclude) {
                    offset++;
                }
                polygon->vertices[i] = aroundPolygon->vertices[i+offset];;
            }
            polygon->vertices[nVerticesInPolygon(nDim)-1] = pointToDivide;

            sort__size_t__Array(polygon->vertices, nVerticesInPolygon(nDim));

            if (verbosity >= Verbosity__debug) {
                char buffer[1024];

                sprintf(buffer, "- - - Append new polygon {");
                for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
                    sprintf(
                        buffer+strlen(buffer), "%lu%s",
                        polygon->vertices[i]+1,
                        (i < (nVerticesInPolygon(nDim)-1)) ? ", " : "}"
                    );
                }

                Runtime__send_message(buffer);
            }
        }
    }

    PolygonTree** const newPolygons = PolygonTreeVector__elements(this) + previousPolygonVectorSize;

    /**
     * Add new faces inside polygon
     */
    const size_t nFaces = nAroundPolygons * nOverlapVertices * nVerticesInPolygon(nDim);
    faceChecked = (bool*) CALLOC(nFaces, sizeof(bool));
    if (!faceChecked) {
        status = FAILURE; goto finally;
    }

    for (size_t iFace_a = 0 ; iFace_a < nFaces ; iFace_a++) {
        if (faceChecked[iFace_a]) {continue;}

        size_t iPolygon_a = iFace_a / nVerticesInPolygon(nDim);
        size_t iEx_a      = iFace_a % nVerticesInPolygon(nDim);

        PolygonTree* polygon_a = newPolygons[iPolygon_a];
        size_t opposite_a = polygon_a->vertices[iEx_a];

        if (opposite_a == pointToDivide) {continue;}

        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (i < iEx_a) {
                IndexVector__elements(face)[i] = polygon_a->vertices[i+0];
            } else {
                IndexVector__elements(face)[i] = polygon_a->vertices[i+1];
            }
        };

        for (size_t iFace_b = iFace_a+1 ; iFace_b < nFaces ; iFace_b++) {
            size_t iPolygon_b = iFace_b / nVerticesInPolygon(nDim);
            size_t iEx_b      = iFace_b % nVerticesInPolygon(nDim);

            PolygonTree* polygon_b = newPolygons[iPolygon_b];
            size_t opposite_b = polygon_b->vertices[iEx_b];

            if (contains__size_t__Array(
                nVerticesInFace(nDim), IndexVector__elements(face),
                1                    , &opposite_b
            )) {continue;}

            if (!contains__size_t__Array(
                nVerticesInPolygon(nDim), polygon_b->vertices,
                nVerticesInFace(nDim)   , IndexVector__elements(face)
            )) {continue;}

            faceChecked[iFace_b] = true;

            Neighbor neighborPair[2] = {
                {opposite_a, polygon_a},
                {opposite_b, polygon_b}
            };

            status = NeighborPairMap__set(
                neighborPairMap, face, neighborPair
            );
            if (status) {goto finally;}
        }
    }

    /**
     * Update faces outside polygon
     * Repeat face update for all `aroundPolygon` in `aroundPolygons`.
     * - Each face has `nDim` vertices.
     * - - `nOverlapVertices-1`      vertices are selected from the vertices in `overlapVertices`.
     * - - `nDim+1-nOverlapVertices` vertices are selected from non-overlap vertices of `aroundPolygon`.
     */
    for (size_t iAround = 0 ; iAround < nAroundPolygons  ; iAround++)
    for (size_t iEx     = 0 ; iEx     < nOverlapVertices ; iEx++    ) {
        PolygonTree* const newPolygon = newPolygons[iAround * nOverlapVertices + iEx];
        const size_t vertexToUpdate = IndexVector__elements(overlapVertices)[iEx];

        // Set vertices of face
        size_t offset = 0;
        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (newPolygon->vertices[i+offset] == pointToDivide) {
                offset++;
            }
            IndexVector__elements(face)[i] = newPolygon->vertices[i+offset];
        }

        // Update neighbor
        status = NeighborPairMap__update_by_opposite(
            neighborPairMap,
            face,
            vertexToUpdate,
            pointToDivide,
            newPolygon
        );
        if (status) {
            goto finally;
        }
    }

finally:

    if (aroundPolygons)  {PolygonTreeVector__delete(aroundPolygons);}
    if (overlapVertices) {IndexVector__delete(overlapVertices);}
    if (face)            {IndexVector__delete(face);}
    if (faceChecked)     {FREE(faceChecked);}

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
    FaceVector* faceVector,
    const enum Verbosity verbosity
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

    if (verbosity >= Verbosity__debug) {
        char buffer[1024];

        sprintf(buffer, "- - Flip face {");
        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            sprintf(
                buffer+strlen(buffer), "%lu%s",
                IndexVector__elements(faceToFlip)[i]+1,
                (i < (nVerticesInFace(nDim)-1)) ? ", " : "}"
            );
        }

        const size_t oppositeVertex = (
            (pointToDivide == neighborPairToFlip[1].opposite)
            ? neighborPairToFlip[0].opposite
            : neighborPairToFlip[1].opposite
        );

        sprintf(
            buffer+strlen(buffer), " (opposite is %lu)",
            oppositeVertex+1
        );

        Runtime__send_message(buffer);
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

        if (verbosity >= Verbosity__debug) {
            char buffer[1024];

            sprintf(buffer, "- - - Append new polygon {");
            for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
                sprintf(
                    buffer+strlen(buffer), "%lu%s",
                    polygon->vertices[i]+1,
                    (i < (nVerticesInPolygon(nDim)-1)) ? ", " : "}"
                );
            }

            Runtime__send_message(buffer);
        }
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

void PolygonTreeVector__divide_at_point(
    const size_t nDim,
    PolygonTreeVector* const this,
    const size_t pointToDivide,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    PolygonTree* const rootPolygon,
    NeighborPairMap* const neighborPairMap,
    const enum Verbosity verbosity,
    ResourceStack resources
) {
    int status = SUCCESS;

    FaceVector* faceVector = ResourceStack__ensure_delete_finally(
        resources, (Resource__deleter*) FaceVector__delete,
        FaceVector__new(0)
    );
    if (!faceVector) {
        raise_Error(resources, "FaceVector__new(0) failed");
    }

    double* divisionRatio = ResourceStack__ensure_delete_finally(
        resources, FREE,
        MALLOC(nVerticesInPolygon(nDim) * sizeof(double))
    );
    if (!divisionRatio) {
        raise_MemoryAllocationError(resources);
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
    if (status) {
        raise_Error(resources, "PolygonTree__find(...) failed");
    }

    if (!polygonToDivide) {
        raise_Error(resources, "can not find polygonToDivide");
    }

    if (verbosity >= Verbosity__debug) {
        char buffer[1024];

        sprintf(buffer, "- Find polygonToDivide {");
        for (size_t i = 0 ; i < nVerticesInPolygon(nDim) ; i++) {
            sprintf(
                buffer+strlen(buffer), "%lu%s",
                polygonToDivide->vertices[i]+1,
                (i < (nVerticesInPolygon(nDim)-1)) ? ", " : "}"
            );
        }

        sprintf(
            buffer+strlen(buffer), " contains %lu",
            pointToDivide+1
        );

        Runtime__send_message(buffer);
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
            faceVector,
            verbosity
        );
        if (status) {
            raise_Error(resources, "PolygonTreeVector__divide_polygon_inside(...) failed");
        }
    } else { // divisionRatio__on_face(...)
        status = PolygonTreeVector__divide_polygon_by_face(
            nDim,
            this,
            polygonToDivide,
            pointToDivide,
            divisionRatio,
            points,
            get_coordinates,
            neighborPairMap,
            faceVector,
            verbosity
        );
        if (status) {
            raise_Error(resources, "PolygonTreeVector__divide_polygon_by_face(...) failed");
        }
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
            faceVector,
            verbosity
        );
        if (status) {
            raise_Error(resources, "PolygonTreeVector__flip_face(...) failed");
        }
    }
}
