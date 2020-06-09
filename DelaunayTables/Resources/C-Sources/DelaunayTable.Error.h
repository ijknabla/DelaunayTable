
#pragma once

#include "DelaunayTable.ResourceStack.h"


static const char* ErrorFormat =
"Error :: %s\n"
"at %s:%d";


#define raise_Error(resources, message) (                           \
    ResourceStack__raise_error((resources)),                        \
    Runtime__send_error(ErrorFormat, (message), __FILE__, __LINE__) \
)
