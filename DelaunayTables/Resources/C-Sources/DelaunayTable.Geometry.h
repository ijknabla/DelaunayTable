
#pragma once

#include "DelaunayTable.Common.h"

#include <math.h>


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
