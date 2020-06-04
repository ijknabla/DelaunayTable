
#include "DelaunayTable.Geometry.c"
#include "DelaunayTable.Container.c"
#include "DelaunayTable.IndexVector.c"
#include "DelaunayTable.PolygonTree.c"
#include "DelaunayTable.Neighbor.c"
#include "DelaunayTable.IO.c"

static DelaunayTable* ExternalDelaunayTable__constructor(
    const modelica_integer nPoints,
    const modelica_integer nIn,
    const modelica_integer nOut,
    const modelica_real* table
) {
    int status = SUCCESS;

    DelaunayTable* this;

    status = DelaunayTable__from_buffer(
        &this,
        nPoints,
        nIn,
        nOut,
        NULL
    );

    return this;
}

static void ExternalDelaunayTable__destructor(
    DelaunayTable* const this
) {
    if (this) {
        DelaunayTable__delete(this);
    }
}
