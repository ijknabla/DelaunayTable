
#include "DelaunayTable.IO.h"

#include <stddef.h>
#include <stdio.h>


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

    size_t nRow, nCol;
    double* table = readModelicaStandardTxtTableFormatV1(
        tableName,
        fileName,
        &nRow,
        &nCol
    );
    return NULL;
}

void DelaunayTable__close(
    DelaunayTable* this
) {
}

double* readModelicaStandardTxtTableFormatV1(
    const char* const tableName,
    const char* const fileName,
    size_t* const nRow,
    size_t* const nCol
) {
    FILE* fp = fopen(fileName, "r");
    if (!fp) {raise_FileOpenError(fileName);}

    if (fclose(fp)) {raise_FileCloseError(fileName);}

    return NULL;
}
