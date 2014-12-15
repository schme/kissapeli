#include "kms_glutils.h"

namespace kms {


std::string fileToString( const char *filename) {
    std::ifstream file( filename);
    if( !file.is_open()) {
        assert( 0 && "FAIL: Opening file ");
        std::string msg("FAIL: Opening file: ");
        msg += filename; msg += "\n";
        OUTPUTDEBUG( msg.c_str() );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

GLint loadShader( const char *filename, GLenum shaderType) {

    std::string shader = fileToString( filename);

    GLuint shaderObject = glCreateShader( shaderType);
    glShaderSource( shaderObject, 1, (const GLchar**)&shader, NULL);
    glCompileShader( shaderObject);
    
    GLint status;
    glGetShaderiv( shaderObject, GL_COMPILE_STATUS, &status);
    if( status == GL_FALSE) {
        assert(0 && "Shader didn't compile!");
    }

    return shaderObject;
}

GLint createProgram( std::vector<GLint> &shaders) {

    GLint shaderProgram = glCreateProgram();
    for (int i = 0; i < shaders.size(); ++i)
    {
        glAttachShader( shaderProgram, shaders[i]);
    }
    glLinkProgram( shaderProgram);
    for (int i = 0; i < shaders.size(); ++i)
    {
        glDetachShader( shaderProgram, shaders[i]);
    }

    GLint status;
    glGetProgramiv( shaderProgram, GL_LINK_STATUS, &status);
    if( status == GL_FALSE) {
        assert(0 && "Shader linking failed!\n");
        std::string msg( "Program linking failure: " );

        GLint infoLogLength;
        glGetProgramiv( shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength );
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog( shaderProgram, infoLogLength, NULL, strInfoLog );
        msg += strInfoLog;
        delete[] strInfoLog;
        OUTPUTDEBUG( msg.c_str() );

        glDeleteProgram( shaderProgram ); shaderProgram = 0;
    }
    return shaderProgram;
}



} // namespace kms
