
#include "DelaunayTable.h"

#include <stddef.h>
#include <assert.h>


#define u2y(u1, u2, u3) ((u1) * 1.0 + (u2) * 2.0 + (u3) * 3.0)

#define nIn     (3)
#define nOut    (1)
#define nPoints (8)

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
    -1, -1, -1, u2y(-1, -1, -1),
    -1, -1, +1, u2y(-1, -1, +1),
    -1, +1, -1, u2y(-1, +1, -1),
    -1, +1, +1, u2y(-1, +1, +1),
    +1, -1, -1, u2y(+1, -1, -1),
    +1, -1, +1, u2y(+1, -1, +1),
    +1, +1, -1, u2y(+1, +1, -1),
    +1, +1, +1, u2y(+1, +1, +1)
};

int main(int argc, char** argv) {

    DelaunayTable* delaunayTable;

    assert( DelaunayTable__from_buffer(&delaunayTable, nPoints, nIn, nOut, table, Verbosity__detail) == 0 );
    assert( delaunayTable != NULL );

    for (size_t ix = 0 ; ix < N ; ix++)
    for (size_t iy = 0 ; iy < N ; iy++)
    for (size_t iz = 0 ; iz < N ; iz++) {
        const double u[nIn] = {range(ix, N), range(iy, N), range(iz, N)};
        double y[nOut];

        assert( DelaunayTable__get_value(delaunayTable, nIn, nOut, u, y) == 0 );
        assert( double__compare(y[0], u2y(u[0], u[1], u[2])) == 0 );
    }

    DelaunayTable__delete(delaunayTable);

    return EXIT_SUCCESS;
}
