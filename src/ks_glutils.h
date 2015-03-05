#ifndef KMS_GLUTILS_H_
#define KMS_GLUTILS_H_

#include "types.h"

#include <GL/glew.h>
#include <GL/wglew.h>

#define SHADERPATH(shader) "shaders/" shader

GLint createProgram( const char *vertex, const char *fragment);
GLint createProgram( const char *vertex, const char *fragment, const char *geometry);

#endif // KMS_GLUTILS_H_
