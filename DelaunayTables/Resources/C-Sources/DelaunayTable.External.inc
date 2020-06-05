
#include <ModelicaUtilities.h>

#include "DelaunayTable.Geometry.c"
#include "DelaunayTable.Container.c"
#include "DelaunayTable.IndexVector.c"
#include "DelaunayTable.PolygonTree.c"
#include "DelaunayTable.Neighbor.c"
#include "DelaunayTable.IO.c"
#include "DelaunayTable.c"


static DelaunayTable* ExternalDelaunayTable__constructor(
    const modelica_integer nPoints,
    const modelica_integer nIn,
    const modelica_integer nOut,
    const modelica_real* table
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

    int status = SUCCESS;

    const size_t nTable = nPoints * (nIn + nOut);

    double* const buffer = (double*) MALLOC(nTable * sizeof(double));
    if (!buffer) {
        ModelicaFormatError(
            "Error at ExternalDelaunayTable__constructor(%d, %d, %d, (modelica_real*) %p)\n"
            "at %s:%d",
            nPoints, nIn, nOut, table,
            __FILE__, __LINE__
        );
    }

    for (size_t i = 0 ; i < nTable ; i++) {
        buffer[i] = (double) table[i];
    }

    DelaunayTable* this;

    status = DelaunayTable__from_buffer(
        &this,
        nPoints,
        nIn,
        nOut,
        buffer,
        1  // Verbosity is quiet
    );
    if (status) {
        FREE(buffer);
        ModelicaFormatError(
            "Error at ExternalDelaunayTable__constructor(%d, %d, %d, (modelica_real*) %p)\n"
            "at %s:%d",
            nPoints, nIn, nOut, table,
            __FILE__, __LINE__
        );
    }

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
    const modelica_real* u,
          modelica_real* y
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
            "Error at ExternalDelaunayTable__get_value((DelaunayTable*) %p, %d, %d, (modelica_real*) %p, (modelica_real*) %p)\n"
            "at %s:%d",
            this, nIn, nOut, u, y,
            __FILE__, __LINE__
        );
    }
}
