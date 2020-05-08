
#pragma once

typedef struct{} DelaunayTable;


extern DelaunayTable* DelaunayTable__open(
);

extern void DelaunayTable__close(
    DelaunayTable* this
);