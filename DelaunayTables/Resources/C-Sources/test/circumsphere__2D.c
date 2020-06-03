
#include "DelaunayTable.Geometry.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


#define nCase (1)
#define nDim  (2)

static const size_t N = 100+1;

static const double x_min = -6.0;
static const double x_max = +6.0;

static inline double range(
    const size_t i,
    const size_t N
) {
    const double r = (double) i / (double) (N-1);
    return (1-r) * x_min + r * x_max;
}

static inline double norm(
    const double* xs
) {
    double result = 0.0;
    for (size_t i = 0 ; i < nDim ; i++) {
        result += xs[i] * xs[i];
    }
    return result;
}

const double coordinates[nCase][nDim+1][nDim] = {
    {  // case1
        { 3,  4},
        { 4, -3},
        {-4,  3}
    },
};


int main(int argc, char** argv) {

    for (size_t iCase = 0 ; iCase < nCase ; iCase++) {

        const double* polygon[nDim+1];
        for (size_t i = 0 ; i < (nDim+1) ; i++) {
            polygon[i] = (const double*) &coordinates[iCase][i];
        }

        const double R0 = norm(polygon[0]);

        for (size_t i = 1 ; i < nDim ; i++) {
            assert( double__compare(norm(polygon[i]), R0) == 0 );
        }

        for (size_t ix = 0 ; ix < N ; ix++)
        for (size_t iy = 0 ; iy < N ; iy++) {

            const double point[nDim] = {
                range(ix, N),
                range(iy, N)
            };

            const double R1 = norm(point);

            bool inside;
            assert( insideCircumsphereOfPolygon(nDim, polygon, point, &inside) == 0 );

            if (double__compare(R0, R1) != 0) {
                assert( inside == (double__compare(R0, R1) > 0) );
            }
        }
    }

    return EXIT_SUCCESS;
}
