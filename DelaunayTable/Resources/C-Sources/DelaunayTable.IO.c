
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

typedef struct {
    FILE* fp;
    char* buffer;
    size_t capacity;
    size_t lineNO;
} LineIterator;

static void LineIterator__delete(
    LineIterator* const this
) {
    if (this->buffer) {
        FREE(this->buffer);
    }
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

    LineIterator lineIterator = {fp, NULL, 0, 0};

    LineIterator__delete(&lineIterator);

    if (fclose(fp)) {raise_FileCloseError(fileName);}

    return NULL;
}
