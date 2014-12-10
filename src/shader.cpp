/**
 * NOTE: Tighten this. Will probably lead to problems.
 * Possible problems from not reference counting
 */

#include "shader.h"

#include <fstream>
#include <sstream>

namespace kms {

Shader::Shader( const char *shaderCode, GLenum shaderType) :
    _object(0)
{
    _object = glCreateShader( shaderType);
    if( !_object) {
        CPP_OUTPUTDEBUG( "SHADER FAIL: glCreateShader")
    }

    glShaderSource(_object, 1, (const GLchar**)&shaderCode, NULL);
    glCompileShader(_object);

    GLint status;
    glGetShaderiv(_object, GL_COMPILE_STATUS, &status);
    if( status == GL_FALSE) {
        CPP_OUTPUTDEBUG( "SHADER FAIL: Compiling")
    }

    std::cerr << shaderCode << std::endl;
}


Shader::Shader( std::string shaderCode, GLenum shaderType) :
                                Shader::Shader( shaderCode.c_str(), shaderType)
{}

Shader::Shader( const Shader& rhs) : _object(rhs._object) { }

Shader::~Shader() {
    glDeleteShader(_object);
    _object = 0;
}

GLuint Shader::object() const {
    return _object;
}

Shader& Shader::operator=( const Shader& rhs) {
    glDeleteShader(_object);
    _object = rhs._object;
    return *this;
}

Shader Shader::loadShaderFile(const char *filename, GLenum shaderType)
{
    std::ifstream file;
    file.open( filename, std::ios::in | std::ios::binary);
    if( !file.is_open()) {
        perror( "SHADER FAIL: Opening shader file\n");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    Shader shader(buffer.str(), shaderType);
    return shader;
}


} // namespace kms
