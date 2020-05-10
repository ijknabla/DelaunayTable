
#include "DelaunayTable.IO.h"

#include <stddef.h>


DelaunayTable* DelaunayTable__open(
    const char* tableName,
    const char* fileName
) {
    ModelicaFormatMessage(
        "tableName = %s\n", tableName
    );
    ModelicaFormatMessage(
        "fileName = %s\n", fileName
    );
    return NULL;
}

extern void DelaunayTable__close(
    DelaunayTable* this
) {
}
