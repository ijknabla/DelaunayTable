
#include "DelaunayTable.Geometry.h"

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


#define nCase (2)
#define nDim  (4)

static const size_t N = 8+1;

static const double x_min = -10.0;
static const double x_max = +10.0;

static inline double range(
    const size_t i,
    const size_t N
) {
    double r = (double) i / (double) (N-1);
    return (1-r) * x_min + r * x_max;
}

const double coordinates[nCase][nDim+1][nDim] = {
    {  // case1
        {-1, -1, -1, -1},
        {+1,  0,  0,  0},
        { 0, +1,  0,  0},
        { 0,  0, +1,  0},
        { 0,  0,  0, +1}
    },
    {  // case2
        { 0,  1,  2,  1},
        { 3,  0,  4,  4},
        { 5,  5,  0,  6},
        { 8,  7,  7,  0},
        { 0,  0,  9,  9}
    }
};


int main(int argc, char** argv) {

    for (size_t iCase = 0 ; iCase < nCase ; iCase++) {

        const double* polygon[nDim+1];
        for (size_t i = 0 ; i < (nDim+1) ; i++) {
            polygon[i] = (const double*) &coordinates[iCase][i];
        }

        for (size_t iw = 0 ; iw < N ; iw++)
        for (size_t ix = 0 ; ix < N ; ix++)
        for (size_t iy = 0 ; iy < N ; iy++)
        for (size_t iz = 0 ; iz < N ; iz++) {

            const double point[nDim] = {
                range(iw, N),
                range(ix, N),
                range(iy, N),
                range(iz, N)
            };

            double ratio[nDim+1] = {};

            assert( divisionRatioFromPolygonVertices(nDim, polygon, point, ratio) == 0 );

            double point_ref[nDim];

            for (size_t i = 0 ; i < nDim ; i++) {
                point_ref[i] = 0.0;
                for (size_t j = 0 ; j < (nDim+1) ; j++) {
                    point_ref[i] += polygon[j][i] * ratio[j];
                }
            }

            for (size_t i = 0 ; i < nDim ; i++) {
                assert( double__compare(point[i], point_ref[i]) == 0 );
            }
        }
    }

    return EXIT_SUCCESS;
}
