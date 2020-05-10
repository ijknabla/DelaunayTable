
#pragma once

#include <stdlib.h>
#include <stdnoreturn.h>


#define MALLOC (malloc)
#define CALLOC (calloc)
#define FREE   (free)

#define SUCCESS ( 0)
#define FAILURE (-1)

typedef struct{} DelaunayTable;

/// # ModelicaUtilities.h
extern           void ModelicaFormatMessage(const char *string, ...);
extern _Noreturn void ModelicaFormatError(const char *string, ...);
