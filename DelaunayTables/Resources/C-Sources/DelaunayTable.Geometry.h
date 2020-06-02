
#pragma once

#include "DelaunayTable.Common.h"

#include <math.h>


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
