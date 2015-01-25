#ifndef KMS_GLUTILS_H_
#define KMS_GLUTILS_H_

#include <GL/glew.h>
#include <GL/wglew.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "kms.h"

#define PRINT_SHADER_ERROR( shader ) \
        GLint logSize = 0;\
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize);\
        GLchar *errorMessage = (GLchar*)malloc( (uint32)logSize );\
        glGetShaderInfoLog( shader, logSize, NULL, errorMessage);\
        printf("%s\n", errorMessage)

std::string fileToString( const char *filename);
GLint loadShader( const char *filename, GLenum shaderType);
GLint createProgram( std::vector<GLint> &shaders);

#endif // KMS_GLUTILS_H_
