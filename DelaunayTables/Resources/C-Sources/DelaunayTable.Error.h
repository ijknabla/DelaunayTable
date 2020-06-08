
#pragma once

#include "DelaunayTable.ResourceStack.h"


static const char* FatalErrorFormat =
"FatalError :: %s\n"
"at %s:%d";

static const char* MemoryAllocationErrorFormat =
"MemoryAllocationError\n"
"at %s:%d";


#define raise_FatalError(resources, message) (                         \
    ResourceStack__raise_error((resources)),                           \
    Runtime__send_error(FatalErrorFormat, message, __FILE__, __LINE__) \
)

#define raise_MemoryAllocationError(resources) (                          \
    ResourceStack__raise_error((resources)),                              \
    Runtime__send_error(MemoryAllocationErrorFormat, __FILE__, __LINE__) \
)
