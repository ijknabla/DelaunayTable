
#include "DelaunayTable.IO.h"

#include <stddef.h>
#include <stdio.h>

#define DEFAULT_BUFFER_SIZE 256


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
    const char* fileName;
    FILE* fp;
    char* buffer;
    size_t capacity;
    size_t lineNO;
} LineIterator;

static void LineIterator__initialize(
    LineIterator* const this,
    const char* const fileName
) {
    if (!(this->fp = fopen(fileName, "r"))) {raise_FileOpenError(fileName);}
    if (!(this->buffer = MALLOC(DEFAULT_BUFFER_SIZE))) {raise_MemoryError;}

    this->fileName = fileName;
    this->capacity = DEFAULT_BUFFER_SIZE;
    this->lineNO = 0;
}

static void LineIterator__finalize(
    LineIterator* const this
) {
    if (this->fp) {
        if (fclose(this->fp)) {raise_FileCloseError(this->fileName);}
    }
    if (this->buffer) {FREE(this->buffer);}
}

double* readModelicaStandardTxtTableFormatV1(
    const char* const tableName,
    const char* const fileName,
    size_t* const nRow,
    size_t* const nCol,
    const enum Verbosity verbosity
) {
    LineIterator lineIterator = {NULL};
    LineIterator__initialize(
        &lineIterator,
        fileName
    );

    if (verbosity > Verbosity__quiet) {
        ModelicaFormatMessage("Successfully opened file \"%s\"\n", fileName);
    }

    LineIterator__finalize(&lineIterator);
    if (verbosity > Verbosity__quiet) {
        ModelicaFormatMessage("Successfully closed file \"%s\"\n", fileName);
    }


    return NULL;
}
