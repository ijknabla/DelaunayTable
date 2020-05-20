
#pragma once

#include <stdlib.h>
#include <stdnoreturn.h>


#define MALLOC  (malloc)
#define CALLOC  (calloc)
#define REALLOC (realloc)
#define FREE    (free)

#define SUCCESS ( 0)
#define FAILURE (-1)

typedef struct{} DelaunayTable;

/// # ModelicaUtilities.h
extern           void ModelicaFormatMessage(const char *string, ...);
extern _Noreturn void ModelicaFormatError(const char *string, ...);

/// # macro for error
#define MemoryError_format    "Memory allocation error at %s:%d\n"
#define FileOpenError_format  "Can't open file \"%s\" at %s:%d\n"
#define FileCloseError_format "Can't close file \"%s\" at %s:%d\n"
#define FileReadError_format  "Can't read line %zu from file \"%s\" at %s:%d\n"

#define raise_MemoryError                     (ModelicaFormatError(MemoryError_format, __FILE__, __LINE__))
#define raise_FileOpenError(fileName)         (ModelicaFormatError(FileOpenError_format, (char*) fileName, __FILE__, __LINE__))
#define raise_FileCloseError(fileName)        (ModelicaFormatError(FileCloseError_format, (char*) fileName, __FILE__, __LINE__))
#define raise_FileReadError(fileName, lineNO) (ModelicaFormatError(FileReadError_format, (size_t) lineNO, (char*) fileName, __FILE__, __LINE__))

enum Verbosity {
    Verbosity__quiet = 1,
    Verbosity__info,
    Verbosity__debug,
    Verbosity__detail
};
