
#pragma once

#include "DelaunayTable.h"


extern DelaunayTable* DelaunayTable__open(
    const char* tableName,
    const char* fileName,
    const enum Verbosity verbosity
);

extern void DelaunayTable__close(
    DelaunayTable* this
);

extern double* readModelicaStandardTxtTableFormatV1(
    const char* tableName,
    const char* fileName,
    size_t* nRow,
    size_t* nCol,
    const enum Verbosity verbosity
);
