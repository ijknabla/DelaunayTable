
#include "DelaunayTable.h"

#include <assert.h>


#define u2y(u1, u2) ((u1) * 1.0 + (u2) * 2.0)

static const int nIn = 2;
static const int nOut = 1;
static const int nPoints = 4;

static const double table[] = {
    -1, -1, u2y(-1, -1),
    -1, +1, u2y(-1, +1),
    +1, -1, u2y(+1, -1),
    +1, +1, u2y(+1, +1)
};

int main(int argc, char** argv) {

    DelaunayTable* delaunayTable;

    assert( DelaunayTable__from_buffer(&delaunayTable, nPoints, nIn, nOut, table) == 0 );
    assert( delaunayTable != NULL );

    DelaunayTable__delete(delaunayTable);

    return EXIT_SUCCESS;
}
