
#include "DelaunayTable.IO.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
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

static bool LineIterator__next(
    LineIterator* const this
) {
    size_t readed = 0;

    while (true) {
        fgets(
            (this->buffer)  +readed,
            (this->capacity)-readed,
            this->fp);

        if (feof(this->fp)) {
            return false;
        }

        if (ferror(this->fp)) {
            clearerr(this->fp);
            raise_FileReadError(this->fileName, (this->lineNO)+1);
        }

        if (strchr((this->buffer)+readed, '\n')) {
            break;
        } else {
            readed  = (this->capacity)-1;
            this->capacity *= 2;
            if (!(this->buffer = REALLOC(this->buffer, this->capacity))) {raise_MemoryError;}
        }
    }

    (this->lineNO)++;

    return true;
}

static void removeCRLF(char* buffer) {
    const char* ci; char* co;
    for(ci = co = buffer ; *ci ; ci++) {
        if (*ci != '\r' && *ci != '\n') {
            *co = *ci;
            *co++;
        }
    }
    *co = '\0';
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

    while (LineIterator__next(&lineIterator)) {
        removeCRLF(lineIterator.buffer);
        ModelicaFormatMessage("\"%s\"\n", lineIterator.buffer);
    }

    LineIterator__finalize(&lineIterator);
    if (verbosity > Verbosity__quiet) {
        ModelicaFormatMessage("Successfully closed file \"%s\"\n", fileName);
    }


    return NULL;
}
