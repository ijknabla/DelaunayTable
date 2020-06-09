
#include "DelaunayTable.PolyhedronTree.h"

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


/// ## PolyhedronTree methods
PolyhedronTree* PolyhedronTree__new(
    const size_t nDim
) {
    PolyhedronTree* const this = (PolyhedronTree*) MALLOC(sizeof(PolyhedronTree));
    if (!this) {goto error;}

    this->vertices = NULL;
    this->children = NULL;

    this->vertices = (size_t*) CALLOC(nVerticesInPolyhedron(nDim), sizeof(size_t));
    if (!(this->vertices)) {goto error;}

    this->children = PolyhedronTreeVector__new(0);
    if (!(this->children)) {goto error;}

    return this;

error:

    if (this) {
        if (this->vertices) FREE(this->vertices);
        if (this->children) PolyhedronTreeVector__delete(this->children);
        FREE(this);
    }

    return NULL;
}

void PolyhedronTree__delete(
    PolyhedronTree* const this
) {
    FREE(this->vertices);
    PolyhedronTreeVector__delete(this->children);
    FREE(this);
}

int PolyhedronTree__append_child(
    PolyhedronTree* const this,
    PolyhedronTree* const child
) {
    return PolyhedronTreeVector__append(this->children, child);
}

int PolyhedronTree__calculate_divisionRatio(
    const size_t nDim,
    const PolyhedronTree* const this,
    const double* const coordinates,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    double* const divisionRatio
) {
    int status = SUCCESS;

    const double** const shape = (const double**) MALLOC(
        nVerticesInPolyhedron(nDim) * sizeof(double*)
    );
    if (!shape) {status = FAILURE; goto finally;}

    for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
        shape[i] = get_coordinates(points, this->vertices[i]);
    }

    status = divisionRatioFromPolyhedronVertices(
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

int PolyhedronTree__find(
    const size_t nDim,
    PolyhedronTree* const rootPolyhedron,
    const double* const coordinates,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    PolyhedronTree** const foundPolyhedron,
    double* const divisionRatio
) {
    int status = SUCCESS;

    status = PolyhedronTree__calculate_divisionRatio(
        nDim,
        rootPolyhedron,
        coordinates,
        points,
        get_coordinates,
        divisionRatio
    );
    if (status) {
        *foundPolyhedron = NULL;
        return status;
    }

    // if coordinates not in rootPolyhedron: SUCCESS, result is NULL
    if (!divisionRatio__inside(nDim, divisionRatio)) {
        *foundPolyhedron = NULL;
        return SUCCESS;
    }

    // ** coordinates in rootPolyhedron **

    // if rootPolyhedron does not have children: SUCCESS, result is rootPolyhedron
    if (PolyhedronTree__nChildren(rootPolyhedron) == 0) {
        *foundPolyhedron = rootPolyhedron;
        return SUCCESS;
    }

    // ** rootPolyhedron has children **

    // if any find(child, ...) success: SUCCESS, return first result
    for (size_t i = 0 ; i < PolyhedronTree__nChildren(rootPolyhedron) ; i++) {
        status = PolyhedronTree__find(
            nDim,
            PolyhedronTree__children(rootPolyhedron)[i],
            coordinates,
            points,
            get_coordinates,
            foundPolyhedron,
            divisionRatio
        );
        if (status) {
            return status;
        }

        if (*foundPolyhedron) return SUCCESS;
    }

    // else: failure
    *foundPolyhedron = NULL;
    return FAILURE;
}

int PolyhedronTree__get_around(
    const size_t nDim,
    const PolyhedronTree* const polyhedron,
    const IndexVector* const overlapVertices,
    const NeighborPairMap* const neighborPairMap,
    PolyhedronTreeVector* const aroundPolyhedrons
) {
    // Early return
    for (size_t i = 0 ; i < (aroundPolyhedrons->size) ; i++) {
        if (polyhedron == PolyhedronTreeVector__elements(aroundPolyhedrons)[i]) {
            return SUCCESS;
        }
    }

    int status = SUCCESS;

    // resources
    IndexVector* face = IndexVector__new(nVerticesInFace(nDim));
    if (!face) {
        status = FAILURE; goto finally;
    }

    status = PolyhedronTreeVector__append(
        aroundPolyhedrons,
        (PolyhedronTree*) polyhedron
    );
    if (status) {
        goto finally;
    }

    for (size_t iEx = 0 ; iEx < nVerticesInPolyhedron(nDim) ; iEx++) {
        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (i < iEx) {
                IndexVector__elements(face)[i] = polyhedron->vertices[i+0];
            } else {
                IndexVector__elements(face)[i] = polyhedron->vertices[i+1];
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
            PolyhedronTree* candidate = neighborPair[i].polyhedron;
            if (candidate && candidate != polyhedron) {
                status = PolyhedronTree__get_around(
                    nDim,
                    candidate,
                    overlapVertices,
                    neighborPairMap,
                    aroundPolyhedrons
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


/// ## PolyhedronTreeVector methods
PolyhedronTreeVector* PolyhedronTreeVector__new(
    const size_t size
) {
    return Vector__new(size, sizeof(PolyhedronTree*));
}

void PolyhedronTreeVector__delete(
    PolyhedronTreeVector* const this
) {
    Vector__delete(this);
}

void PolyhedronTreeVector__delete_elements(
    PolyhedronTreeVector* const this
) {
    for (size_t i = 0 ; i < (this->size) ; i++) {
        PolyhedronTree__delete(PolyhedronTreeVector__elements(this)[i]);
    }
}

int PolyhedronTreeVector__append(
    PolyhedronTreeVector* const this,
    PolyhedronTree* polyhedron
) {
    return Vector__append(
        this,
        &polyhedron,
        sizeof(PolyhedronTree*)
    );
}

static int PolyhedronTreeVector__divide_polyhedron_inside(
    const size_t nDim,
    PolyhedronTreeVector* this,
    PolyhedronTree* const polyhedronToDivide,
    const size_t pointToDivide,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    NeighborPairMap* const neighborPairMap,
    FaceVector* const faceVector,
    const enum Verbosity verbosity
) {
    if (verbosity >= Verbosity__debug) {
        Runtime__send_message("- - Divide polyhedron by inside point");
    }

    int status = SUCCESS;

    const size_t previousPolyhedronVectorSize = this->size;

    IndexVector* face = IndexVector__new(nVerticesInFace(nDim));
    if (!face) {
        status = FAILURE; goto finally;
    }

    /**
     * Add new polyhedrons.
     * Each new polyhedron has `nDim+1` vertices.
     * - One vertex is `pointToDivide`.
     * - `nDim` vertices are selected from the `nDim+1` vertices in `polyhedronToDivide`.
     */
    for (size_t iEx = 0 ; iEx < nVerticesInPolyhedron(nDim) ; iEx++) {
        // Alloc polyhedron & append PolyhedronTreeVector
        PolyhedronTree* const polyhedron = PolyhedronTree__new(nDim);
        if (!polyhedron) {
            status = FAILURE; goto finally;
        }

        status = PolyhedronTreeVector__append(
            this, polyhedron
        );
        if (status) {
            PolyhedronTree__delete(polyhedron);
            goto finally;
        }

        status = PolyhedronTree__append_child(
            polyhedronToDivide,
            polyhedron
        );
        if (status) {
            goto finally;
        }

        // Set vertices of polyhedron
        for (size_t i = 0 ; i < (nVerticesInPolyhedron(nDim)-1) ; i++) {
            if (i < iEx) {
                polyhedron->vertices[i] = polyhedronToDivide->vertices[i+0];
            } else {
                polyhedron->vertices[i] = polyhedronToDivide->vertices[i+1];
            }
        }
        polyhedron->vertices[nVerticesInPolyhedron(nDim)-1] = pointToDivide;

        sort__size_t__Array(polyhedron->vertices, nVerticesInPolyhedron(nDim));

        if (verbosity >= Verbosity__debug) {
            char buffer[1024];

            sprintf(buffer, "- - - Append new polyhedron {");
            for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
                sprintf(
                    buffer+strlen(buffer), "%lu%s",
                    polyhedron->vertices[i]+1,
                    (i < (nVerticesInPolyhedron(nDim)-1)) ? ", " : "}"
                );
            }

            Runtime__send_message(buffer);
        }
    }

    PolyhedronTree** const newPolyhedrons = PolyhedronTreeVector__elements(this) + previousPolyhedronVectorSize;

    /**
     * Add new faces inside `polyhedronToDivide`
     * Each new face has `nDim` vertices.
     * - One vertex is `pointToDivide`.
     * - `nDim-1` vertices are selected from the `nDim+1` vertices in `polyhedronToDivide`.
     */
    for (size_t iEx_a = 0       ; iEx_a < nVerticesInPolyhedron(nDim) ; iEx_a++)
    for (size_t iEx_b = iEx_a+1 ; iEx_b < nVerticesInPolyhedron(nDim) ; iEx_b++)
    {
        // Set vertices of face
        for (size_t i = 0 ; i < (nVerticesInFace(nDim)-1) ; i++) {
            if (i+0 < iEx_a) {
                IndexVector__elements(face)[i] = polyhedronToDivide->vertices[i+0];
            } else if (i+1 < iEx_b) {
                IndexVector__elements(face)[i] = polyhedronToDivide->vertices[i+1];
            } else {
                IndexVector__elements(face)[i] = polyhedronToDivide->vertices[i+2];
            }
        }
        IndexVector__elements(face)[nVerticesInFace(nDim)-1] = pointToDivide;

        sort__size_t__Array(IndexVector__elements(face), nVerticesInFace(nDim));

        // Set to neighborPairMap
        Neighbor neighborPair[2] = {
            {polyhedronToDivide->vertices[iEx_a], newPolyhedrons[iEx_b]},
            {polyhedronToDivide->vertices[iEx_b], newPolyhedrons[iEx_a]}
        };

        status = NeighborPairMap__set(
            neighborPairMap,
            face,
            neighborPair
        );
        if (status) {goto finally;}
    }

    /**
     * Update faces outside `polyhedronToDivide`
     * Each face has `nDim` vertices.
     * - `nDim` vertices are selected from the `nDim+1` vertices in `polyhedronToDivide`.
     */
    for (size_t iEx = 0 ; iEx < nVerticesInPolyhedron(nDim) ; iEx++) {
        // Set vertices of face
        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (i < iEx) {
                IndexVector__elements(face)[i] = polyhedronToDivide->vertices[i+0];
            } else {
                IndexVector__elements(face)[i] = polyhedronToDivide->vertices[i+1];
            }
        }

        // Update neighborPairMap
        status = NeighborPairMap__update_by_opposite(
            neighborPairMap,
            face,
            polyhedronToDivide->vertices[iEx],  // opposite_old
            pointToDivide,                   // opposite_new
            newPolyhedrons[iEx]                 // polyhedron_new
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

static int PolyhedronTreeVector__divide_polyhedron_by_face(
    const size_t nDim,
    PolyhedronTreeVector* const this,
    PolyhedronTree* const polyhedronToDivide,
    const size_t pointToDivide,
    const double* divisionRatio,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    NeighborPairMap* const neighborPairMap,
    FaceVector* const faceVector,
    const enum Verbosity verbosity
) {
    if (verbosity >= Verbosity__debug) {
        Runtime__send_message("- - Divide polyhedron by point on face");
    }

    int status = SUCCESS;

    const size_t previousPolyhedronVectorSize = this->size;

    // Resources
    PolyhedronTreeVector* aroundPolyhedrons  = NULL;
    IndexVector*       overlapVertices = NULL;
    IndexVector*       face            = NULL;
    bool*              faceChecked     = NULL;

    aroundPolyhedrons = PolyhedronTreeVector__new(0);
    if (!aroundPolyhedrons) {
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
    for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
        if (double__compare(divisionRatio[i], 0.0) != 0) {
            status = IndexVector__append(
                overlapVertices,
                polyhedronToDivide->vertices[i]
            );
            if (status) {
                goto finally;
            }
        }
    }

    // Get `aroundPolyhedrons`
    status = PolyhedronTree__get_around(
        nDim,
        polyhedronToDivide,
        overlapVertices,
        neighborPairMap,
        aroundPolyhedrons
    );
    if (status) {
        goto finally;
    }

    const size_t nAroundPolyhedrons  = aroundPolyhedrons->size;
    const size_t nOverlapVertices = overlapVertices->size;

    if (verbosity >= Verbosity__detail) {
        Runtime__send_message(
            "- - - Find %lu around polyhedrons",
            nAroundPolyhedrons
        );

        for (size_t iAround = 0 ; iAround < nAroundPolyhedrons ; iAround++) {
            const PolyhedronTree* const aroundPolyhedron
                = PolyhedronTreeVector__elements(aroundPolyhedrons)[iAround];

            char buffer[1024];
            sprintf(
                buffer, "- - - - aroundPolyhedron[%2lu] {",
                iAround+1
            );
            for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
                sprintf(
                    buffer+strlen(buffer), "%lu%s",
                    aroundPolyhedron->vertices[i]+1,
                    (i < (nVerticesInPolyhedron(nDim)-1)) ? ", " : "}"
                );
            }

            Runtime__send_message(buffer);
        }
    }

    /**
     * Add new polyhedrons.
     * Repeat new polyhedrons creation for all `aroundPolyhedron` in `aroundPolyhedrons`.
     * - Each new polyhedron has `nDim+1` vertices.
     * - - One vertex is `pointToDivide`.
     * - - `nOverlapVertices-1`      vertices are selected from the vertices in `overlapVertices`.
     * - - `nDim+1-nOverlapVertices` vertices are selected from non-overlap vertices of `aroundPolyhedron`.
     */
    for (size_t iAround = 0 ; iAround < nAroundPolyhedrons ; iAround++) {
        PolyhedronTree* const aroundPolyhedron
            = PolyhedronTreeVector__elements(aroundPolyhedrons)[iAround];

        for (size_t iEx = 0 ; iEx < nOverlapVertices ; iEx++) {
            // Allocate polyhedron & append PolyhedronTrees Vector
            PolyhedronTree* polyhedron = PolyhedronTree__new(nDim);
            if (!polyhedron) {
                status = FAILURE; goto finally;
            }

            status = PolyhedronTreeVector__append(
                this,
                polyhedron
            );
            if (status) {
                PolyhedronTree__delete(polyhedron);
                goto finally;
            }

            status = PolyhedronTree__append_child(
                aroundPolyhedron,
                polyhedron
            );
            if (status) {
                goto finally;
            }

            // Set vertices of polyhedron
            const size_t vertexToExclude = IndexVector__elements(overlapVertices)[iEx];

            size_t offset = 0;
            for (size_t i = 0 ; i < (nVerticesInPolyhedron(nDim)-1) ; i++) {
                if (aroundPolyhedron->vertices[i+offset] == vertexToExclude) {
                    offset++;
                }
                polyhedron->vertices[i] = aroundPolyhedron->vertices[i+offset];;
            }
            polyhedron->vertices[nVerticesInPolyhedron(nDim)-1] = pointToDivide;

            sort__size_t__Array(polyhedron->vertices, nVerticesInPolyhedron(nDim));

            if (verbosity >= Verbosity__debug) {
                char buffer[1024];

                sprintf(buffer, "- - - Append new polyhedron {");
                for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
                    sprintf(
                        buffer+strlen(buffer), "%lu%s",
                        polyhedron->vertices[i]+1,
                        (i < (nVerticesInPolyhedron(nDim)-1)) ? ", " : "}"
                    );
                }

                Runtime__send_message(buffer);
            }
        }
    }

    PolyhedronTree** const newPolyhedrons = PolyhedronTreeVector__elements(this) + previousPolyhedronVectorSize;

    /**
     * Add new faces inside polyhedron
     */
    const size_t nFaces = nAroundPolyhedrons * nOverlapVertices * nVerticesInPolyhedron(nDim);
    faceChecked = (bool*) CALLOC(nFaces, sizeof(bool));
    if (!faceChecked) {
        status = FAILURE; goto finally;
    }

    for (size_t iFace_a = 0 ; iFace_a < nFaces ; iFace_a++) {
        if (faceChecked[iFace_a]) {continue;}

        size_t iPolyhedron_a = iFace_a / nVerticesInPolyhedron(nDim);
        size_t iEx_a      = iFace_a % nVerticesInPolyhedron(nDim);

        PolyhedronTree* polyhedron_a = newPolyhedrons[iPolyhedron_a];
        size_t opposite_a = polyhedron_a->vertices[iEx_a];

        if (opposite_a == pointToDivide) {continue;}

        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (i < iEx_a) {
                IndexVector__elements(face)[i] = polyhedron_a->vertices[i+0];
            } else {
                IndexVector__elements(face)[i] = polyhedron_a->vertices[i+1];
            }
        };

        for (size_t iFace_b = iFace_a+1 ; iFace_b < nFaces ; iFace_b++) {
            size_t iPolyhedron_b = iFace_b / nVerticesInPolyhedron(nDim);
            size_t iEx_b      = iFace_b % nVerticesInPolyhedron(nDim);

            PolyhedronTree* polyhedron_b = newPolyhedrons[iPolyhedron_b];
            size_t opposite_b = polyhedron_b->vertices[iEx_b];

            if (contains__size_t__Array(
                nVerticesInFace(nDim), IndexVector__elements(face),
                1                    , &opposite_b
            )) {continue;}

            if (!contains__size_t__Array(
                nVerticesInPolyhedron(nDim), polyhedron_b->vertices,
                nVerticesInFace(nDim)   , IndexVector__elements(face)
            )) {continue;}

            faceChecked[iFace_b] = true;

            Neighbor neighborPair[2] = {
                {opposite_a, polyhedron_a},
                {opposite_b, polyhedron_b}
            };

            status = NeighborPairMap__set(
                neighborPairMap, face, neighborPair
            );
            if (status) {goto finally;}
        }
    }

    /**
     * Update faces outside polyhedron
     * Repeat face update for all `aroundPolyhedron` in `aroundPolyhedrons`.
     * - Each face has `nDim` vertices.
     * - - `nOverlapVertices-1`      vertices are selected from the vertices in `overlapVertices`.
     * - - `nDim+1-nOverlapVertices` vertices are selected from non-overlap vertices of `aroundPolyhedron`.
     */
    for (size_t iAround = 0 ; iAround < nAroundPolyhedrons  ; iAround++)
    for (size_t iEx     = 0 ; iEx     < nOverlapVertices ; iEx++    ) {
        PolyhedronTree* const newPolyhedron = newPolyhedrons[iAround * nOverlapVertices + iEx];
        const size_t vertexToUpdate = IndexVector__elements(overlapVertices)[iEx];

        // Set vertices of face
        size_t offset = 0;
        for (size_t i = 0 ; i < nVerticesInFace(nDim) ; i++) {
            if (newPolyhedron->vertices[i+offset] == pointToDivide) {
                offset++;
            }
            IndexVector__elements(face)[i] = newPolyhedron->vertices[i+offset];
        }

        // Update neighbor
        status = NeighborPairMap__update_by_opposite(
            neighborPairMap,
            face,
            vertexToUpdate,
            pointToDivide,
            newPolyhedron
        );
        if (status) {
            goto finally;
        }
    }

finally:

    if (aroundPolyhedrons)  {PolyhedronTreeVector__delete(aroundPolyhedrons);}
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
    if (!neighborPair[0].polyhedron || !neighborPair[1].polyhedron) {
        *validFace = true;
        goto finally;
    }

    shape = (const double**) MALLOC(
        nVerticesInPolyhedron(nDim) * sizeof(double*)
    );
    if (!shape) {
        status = FAILURE; goto finally;
    }

    const PolyhedronTree* const polyhedron = neighborPair[0].polyhedron;

    for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
        shape[i] = get_coordinates(points, polyhedron->vertices[i]);
    }

    const double* point = get_coordinates(points, neighborPair[1].opposite);

    bool insideCircumsphere;

    status = insideCircumsphereOfPolyhedron(
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

static int PolyhedronTreeVector__flip_face(
    const size_t nDim,
    PolyhedronTreeVector* const this,
    const IndexVector* faceToFlip,
    const size_t pointToDivide,
    const Points points,
    Points__get_coordinates* get_coordinates,
    NeighborPairMap* const neighborPairMap,
    FaceVector* faceVector,
    const enum Verbosity verbosity
) {
    int status = SUCCESS;

    const size_t previousPolyhedronVectorSize = this->size;

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
     * Add new polyhedrons.
     * Each new polyhedron has `nDim+1` vertices.
     * - One vertex is `opposite0`.
     * - One vertex in `opposite1`.
     * - `nDim-1` vertices are selected from the `nDim` vertices in `faceToSplit`.
     */
    for (size_t iEx = 0 ; iEx < nVerticesInFace(nDim) ; iEx++) {
        // Alloc polyhedron & append PolyhedronTreeVector
        PolyhedronTree* const polyhedron = PolyhedronTree__new(nDim);
        if (!polyhedron) {
            status = FAILURE; goto finally;
        }

        status = PolyhedronTreeVector__append(
            this, polyhedron
        );
        if (status) {
            PolyhedronTree__delete(polyhedron);
            goto finally;
        }

        for (size_t i = 0 ; i < 2 ; i++) {
            status = PolyhedronTree__append_child(
                neighborPairToFlip[i].polyhedron,
                polyhedron
            );
            if (status) {
                goto finally;
            }
        }

        // Set vertices of polyhedron
        for (size_t i = 0 ; i < (nVerticesInFace(nDim)-1) ; i++) {
            if (i < iEx) {
                polyhedron->vertices[i] = IndexVector__elements(faceToFlip)[i+0];
            } else {
                polyhedron->vertices[i] = IndexVector__elements(faceToFlip)[i+1];
            }
        }
        polyhedron->vertices[nVerticesInPolyhedron(nDim)-2] = neighborPairToFlip[0].opposite;
        polyhedron->vertices[nVerticesInPolyhedron(nDim)-1] = neighborPairToFlip[1].opposite;

        sort__size_t__Array(polyhedron->vertices, nVerticesInPolyhedron(nDim));

        if (verbosity >= Verbosity__debug) {
            char buffer[1024];

            sprintf(buffer, "- - - Append new polyhedron {");
            for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
                sprintf(
                    buffer+strlen(buffer), "%lu%s",
                    polyhedron->vertices[i]+1,
                    (i < (nVerticesInPolyhedron(nDim)-1)) ? ", " : "}"
                );
            }

            Runtime__send_message(buffer);
        }
    }

    PolyhedronTree** const newPolyhedrons = PolyhedronTreeVector__elements(this) + previousPolyhedronVectorSize;

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
            {IndexVector__elements(faceToFlip)[iEx_a], newPolyhedrons[iEx_b]},
            {IndexVector__elements(faceToFlip)[iEx_b], newPolyhedrons[iEx_a]}
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
                newPolyhedrons[iEx]
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

void PolyhedronTreeVector__divide_at_point(
    const size_t nDim,
    PolyhedronTreeVector* const this,
    const size_t pointToDivide,
    const Points points,
    Points__get_coordinates* const get_coordinates,
    PolyhedronTree* const rootPolyhedron,
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
        MALLOC(nVerticesInPolyhedron(nDim) * sizeof(double))
    );
    if (!divisionRatio) {
        raise_MemoryAllocationError(resources);
    }

    const double* const coordinatesToDivide
        = get_coordinates(points, pointToDivide);

    PolyhedronTree* polyhedronToDivide;

    status = PolyhedronTree__find(
        nDim,
        rootPolyhedron,
        coordinatesToDivide,
        points,
        get_coordinates,
        &polyhedronToDivide,
        divisionRatio
    );
    if (status) {
        raise_Error(resources, "PolyhedronTree__find(...) failed");
    }

    if (!polyhedronToDivide) {
        raise_Error(resources, "can not find polyhedronToDivide");
    }

    if (verbosity >= Verbosity__debug) {
        char buffer[1024];

        sprintf(buffer, "- Find polyhedronToDivide {");
        for (size_t i = 0 ; i < nVerticesInPolyhedron(nDim) ; i++) {
            sprintf(
                buffer+strlen(buffer), "%lu%s",
                polyhedronToDivide->vertices[i]+1,
                (i < (nVerticesInPolyhedron(nDim)-1)) ? ", " : "}"
            );
        }

        sprintf(
            buffer+strlen(buffer), " contains %lu",
            pointToDivide+1
        );

        Runtime__send_message(buffer);
    }

    if (!divisionRatio__on_face(nDim, divisionRatio)) {
        status = PolyhedronTreeVector__divide_polyhedron_inside(
            nDim,
            this,
            polyhedronToDivide,
            pointToDivide,
            points,
            get_coordinates,
            neighborPairMap,
            faceVector,
            verbosity
        );
        if (status) {
            raise_Error(resources, "PolyhedronTreeVector__divide_polyhedron_inside(...) failed");
        }
    } else { // divisionRatio__on_face(...)
        status = PolyhedronTreeVector__divide_polyhedron_by_face(
            nDim,
            this,
            polyhedronToDivide,
            pointToDivide,
            divisionRatio,
            points,
            get_coordinates,
            neighborPairMap,
            faceVector,
            verbosity
        );
        if (status) {
            raise_Error(resources, "PolyhedronTreeVector__divide_polyhedron_by_face(...) failed");
        }
    }

    for (size_t i = 0 ; i < (faceVector->size) ; i++) {
        status = PolyhedronTreeVector__flip_face(
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
            raise_Error(resources, "PolyhedronTreeVector__flip_face(...) failed");
        }
    }
}
