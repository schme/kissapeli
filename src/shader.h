#ifndef SHADER_H_
#define SHADER_H_

#include <GL/glew.h>
#include <iostream>
#include <string>

#include "kms.h"

namespace kms {


class Shader {
    GLuint _object;
public:
    static Shader loadShaderFile( const char *filename, GLenum shaderType);
    Shader( const char *shaderCode, GLenum shaderType);
    Shader( std::string shaderCode, GLenum shaderType);
    GLuint object() const;

    Shader(const Shader& rhs);
    Shader& operator = (const Shader& rhs);
    ~Shader();
};


} // namespace kms

#endif // SHADER_H_
