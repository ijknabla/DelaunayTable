
#include "DelaunayTable.IO.h"

#include <stddef.h>
#include <stdio.h>


DelaunayTable* DelaunayTable__open(
    const char* const tableName,
    const char* const fileName,
    const enum Verbosity verbosity
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
        &nCol,
        verbosity
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
    size_t* const nCol,
    const enum Verbosity verbosity
) {
    FILE* fp = fopen(fileName, "r");
    if (!fp) {raise_FileOpenError(fileName);}

    if (verbosity > Verbosity__quiet) {
        ModelicaFormatMessage("Successfully opened file \"%s\"\n", fileName);
    }

    if (fclose(fp)) {raise_FileCloseError(fileName);}

    return NULL;
}
