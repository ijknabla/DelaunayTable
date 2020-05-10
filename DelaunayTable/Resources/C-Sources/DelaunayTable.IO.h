
#pragma once

#include "DelaunayTable.h"


extern DelaunayTable* DelaunayTable__open(
    const char* tableName,
    const char* fileName
);

extern void DelaunayTable__close(
    DelaunayTable* this
);