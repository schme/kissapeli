#ifndef KMS_H_
#define KMS_H_

#include <stdio.h>

#define ASSETPATH  "assets/"
#define SHADERPATH( s_name)  ASSETPATH "shaders/" s_name

#define DEBUG_STRING_SIZE 256

#define C_OUTPUTDEBUG( msg)\
    printf( msg );

#define CPP_OUTPUTDEBUG( msg )\
    std::cout << msg << std::endl;

#define WIN_OUTPUTDEBUG( msg )\
    char buffer[DEBUG_STRING_SIZE];\
    snprintf( buffer, DEBUG_STRING_SIZE, "%s", msg);\
    OutputDebugStringA( msg );


#endif // KMS_H_
