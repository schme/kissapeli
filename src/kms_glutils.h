#ifndef KMS_GLUTILS_H_
#define KMS_GLUTILS_H_

#include <GL/glew.h>
#include <GL/wglew.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "kms.h"

namespace kms {

std::string fileToString( const char *filename);
GLint loadShader( const char *filename, GLenum shaderType);
GLint createProgram( std::vector<GLint> &shaders);


} // namespace kms

#endif // KMS_GLUTILS_H_
