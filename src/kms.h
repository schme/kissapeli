#ifndef KMS_H_
#define KMS_H_

#include <stdio.h>
#include <assert.h>

#include "types.h"

#define DEBUG_STRING_SIZE 256

#define OUTPUTDEBUG( msg)\
    fprintf( stderr, "%s", msg )

#define WIN_OUTPUTDEBUG_F( msg, val )\
{\
    char buffer[DEBUG_STRING_SIZE];\
    _snprintf_s( buffer, DEBUG_STRING_SIZE, "%s%f\n", msg, val);\
    OutputDebugStringA( buffer );\
}

#define WIN_OUTPUTDEBUG_I( msg, val )\
{\
    char buffer[DEBUG_STRING_SIZE];\
    _snprintf_s( buffer, DEBUG_STRING_SIZE, "%s%d\n", msg, val);\
    OutputDebugStringA( buffer );\
}
#endif // KMS_H_
