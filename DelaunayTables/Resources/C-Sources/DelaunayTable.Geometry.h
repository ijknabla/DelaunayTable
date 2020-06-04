
#pragma once

#include "DelaunayTable.Common.h"

#include <math.h>
#include <stdbool.h>


typedef void* Points;

typedef double* Points__get_coordinates (const Points, size_t index);


/// relative tolerance for `double__compare`
static const double rel_tol = 1.0e-9;

/// absolute tolerance for `double__compare`
static const double abs_tol = 0.0;


static inline double double__abs(
    const double x
) {
    return fabs(x);
}

static inline double double__max(
    const double x,
    const double y
) {
    return (x>y) ? x : y;
}

static inline int double__compare(
    const double x,
    const double y
) {
    const double diff = x - y;
    const double relative_eps = double__max(
        abs_tol,
        rel_tol * double__max(double__abs(x), double__abs(y))
    );

    if (diff < -relative_eps) return -1;
    if (diff > +relative_eps) return +1;
    return 0;
}


/// # Number of vertices
static inline size_t nVerticesInFace   (const size_t nDim) {return nDim  ;}
static inline size_t nVerticesInPolygon(const size_t nDim) {return nDim+1;}


/// # Geometry functions
extern int divisionRatioFromPolygonVertices(
    const size_t nDim,
    const double* const* polygon,  // double[nDim+1][nDim]
    const double*        point,    // double[nDim]
          double*        ratio     // double[nDim+1]
);

extern int insideCircumsphereOfPolygon(
    const size_t nDim,
    const double* const* polygon,  // double[nDim+1][nDim]
    const double*        point,    // double[nDim]
    bool* inside
);


/// # LAPACK external routines
typedef int    LAPACK_INTEGER;
typedef double LAPACK_DOUBLE_PRECISION;
typedef char   LAPACK_CHARACTER;


extern void dgetrf_(
    LAPACK_INTEGER*          M,
    LAPACK_INTEGER*          N,
    LAPACK_DOUBLE_PRECISION* A,
    LAPACK_INTEGER*          LDA,
    LAPACK_INTEGER*          IPIV,
    LAPACK_INTEGER*          INFO
);

extern void dgetri_(
    LAPACK_INTEGER*          N,
    LAPACK_DOUBLE_PRECISION* A,
    LAPACK_INTEGER*          LDA,
    LAPACK_INTEGER*          IPIV,
    LAPACK_DOUBLE_PRECISION* WORK,
    LAPACK_INTEGER*          LWORK,
    LAPACK_INTEGER*          INFO
);

extern void dgemv_(
    LAPACK_CHARACTER*        TRANS,
    LAPACK_INTEGER*          M,
    LAPACK_INTEGER*          N,
    LAPACK_DOUBLE_PRECISION* ALPHA,
    LAPACK_DOUBLE_PRECISION* A,
    LAPACK_INTEGER*          LDA,
    LAPACK_DOUBLE_PRECISION* X,
    LAPACK_INTEGER*          INCX,
    LAPACK_DOUBLE_PRECISION* BETA,
    LAPACK_DOUBLE_PRECISION* Y,
    LAPACK_INTEGER*          INCY
);
