
#include <ModelicaUtilities.h>

#include "DelaunayTable.Error.h"

#include "DelaunayTable.Geometry.c"
#include "DelaunayTable.Container.c"
#include "DelaunayTable.ResourceStack.c"
#include "DelaunayTable.IndexVector.c"
#include "DelaunayTable.PolygonTree.c"
#include "DelaunayTable.Neighbor.c"
#include "DelaunayTable.IO.c"
#include "DelaunayTable.c"


static DelaunayTable* ExternalDelaunayTable__constructor(
    const modelica_integer nPoints,
    const modelica_integer nIn,
    const modelica_integer nOut,
    const modelica_real*   table,
    const modelica_integer verbosity
) {
    if (!(0 < nPoints)) {
        ModelicaFormatError(
            "nPoints must be positive. got %d\n"
            "at %s:%d",
            nPoints,
            __FILE__, __LINE__
        );
    }

    if (!(0 < nIn)) {
        ModelicaFormatError(
            "nIn must be positive. got %d\n"
            "at %s:%d",
            nIn,
            __FILE__, __LINE__
        );
    }

    if (!(0 < nOut)) {
        ModelicaFormatError(
            "nOut must be positive. got %d\n"
            "at %s:%d",
            nOut,
            __FILE__, __LINE__
        );
    }

    ResourceStack resources = ResourceStack__new();

    const size_t nTable = nPoints * (nIn + nOut);

    double* const buffer = ResourceStack__ensure_delete_on_error(
        resources,
        MALLOC(nTable * sizeof(double)),
        FREE
    );

    for (size_t i = 0 ; i < nTable ; i++) {
        buffer[i] = (double) table[i];
    }

    DelaunayTable* this = DelaunayTable__from_buffer(
        nPoints,
        nIn,
        nOut,
        buffer,
        verbosity,
        resources
    );

    ResourceStack__delete(resources);
    return this;
}

static void ExternalDelaunayTable__destructor(
    DelaunayTable* const this
) {
    if (this) {
        FREE((void*) this->table);
        DelaunayTable__delete(this);
    }
}

static void ExternalDelaunayTable__get_value(
    DelaunayTable* const this,
    const modelica_integer nIn,
    const modelica_integer nOut,
    const modelica_real*   u,
          modelica_real*   y,
    const modelica_integer verbosity
) {
    if (!(0 < nIn)) {
        ModelicaFormatError(
            "nIn must be positive. got %d\n"
            "at %s:%d",
            nIn,
            __FILE__, __LINE__
        );
    }

    if (!(0 < nOut)) {
        ModelicaFormatError(
            "nOut must be positive. got %d\n"
            "at %s:%d",
            nOut,
            __FILE__, __LINE__
        );
    }

    int status = SUCCESS;

    status = DelaunayTable__get_value(
        this,
        nIn,
        nOut,
        u,
        y
    );
    if (status) {
        ModelicaFormatError(
            "Error at ExternalDelaunayTable__get_value("
            "(DelaunayTable*) %p, %d, %d, (modelica_real*) %p, (modelica_real*) %p, %d)\n"
            "at %s:%d",
            this, nIn, nOut, u, y, verbosity,
            __FILE__, __LINE__
        );
    }
}
