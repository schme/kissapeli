#ifndef KMS_H_
#define KMS_H_

#include <stdio.h>
#include <assert.h>

#define ASSETPATH  "assets/"
#define SHADERPATH( s_name)  ASSETPATH "shaders/" s_name

#define DEBUG_STRING_SIZE 256

#define CONCAT( first, second)\
    char msg[DEBUG_STRING_SIZE];\
    snprintf( msg, DEBUG_STRING_SIZE, "%s%s", first, second);\

#define OUTPUTDEBUG( msg)\
    fprintf( stderr, "%s", msg )

#define WIN_OUTPUTDEBUG_F( msg, val )\
{\
    char buffer[DEBUG_STRING_SIZE];\
    _snprintf_s( buffer, DEBUG_STRING_SIZE, "%s%f\n", msg, val);\
    OutputDebugStringA( buffer );\
}


#endif // KMS_H_
