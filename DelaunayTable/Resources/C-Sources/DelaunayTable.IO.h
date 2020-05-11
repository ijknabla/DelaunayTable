
#pragma once

#include "DelaunayTable.h"


extern DelaunayTable* DelaunayTable__open(
    const char* tableName,
    const char* fileName
);

extern void DelaunayTable__close(
    DelaunayTable* this
);

extern int readModelicaStandardTxtTableFormatV1(
    const char* tableName,
    const char* fileName,
    size_t* nRow,
    size_t* nCol,
    double** table
);
