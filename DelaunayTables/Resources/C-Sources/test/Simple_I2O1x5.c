
#include "DelaunayTable.h"

#include <stddef.h>
#include <assert.h>


#define u2y(u1, u2) ((u1) * 1.0 + (u2) * 2.0)

#define nIn     (2)
#define nOut    (1)
#define nPoints (5)

static const size_t N = 64+1;

static const double x_min = -1.0;
static const double x_max = +1.0;

static inline double range(
    const size_t i,
    const size_t N
) {
    double r = (double) i / (double) (N-1);
    return (1-r) * x_min + r * x_max;
}

static const double table[] = {
    -1, -1, u2y(-1, -1),
    -1, +1, u2y(-1, +1),
    +1, -1, u2y(+1, -1),
    +1, +1, u2y(+1, +1),
     0,  0, u2y( 0,  0)
};

int main(int argc, char** argv) {

    DelaunayTable* delaunayTable;

    assert( DelaunayTable__from_buffer(&delaunayTable, nPoints, nIn, nOut, table) == 0 );
    assert( delaunayTable != NULL );

    for (size_t ix = 0 ; ix < N ; ix++)
    for (size_t iy = 0 ; iy < N ; iy++) {
        const double u[nIn] = {range(ix, N), range(iy, N)};
        double y[nOut];

        assert( DelaunayTable__get_value(delaunayTable, nIn, nOut, u, y) == 0 );
        assert( double__compare(y[0], u2y(u[0], u[1])) == 0 );
    }

    DelaunayTable__delete(delaunayTable);

    return EXIT_SUCCESS;
}
