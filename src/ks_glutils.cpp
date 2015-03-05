#include "ks_glutils.h"

#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#define PRINT_SHADER_ERROR( shader )\
do {\
        GLint logSize = 0;\
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize);\
        GLchar *errorMessage = (GLchar*)malloc( (uint32)logSize );\
        glGetShaderInfoLog( shader, logSize, NULL, errorMessage);\
        std::cout << errorMessage << std::endl;\
        free(errorMessage);\
} while(0)

#define PRINT_PROGRAM_ERROR( program)\
do {\
        GLint logSize = 0;\
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);\
        char* errorMessage = (char*)malloc( (uint32)logSize );\
        glGetProgramInfoLog( program, logSize, NULL, errorMessage);\
        std::cout << errorMessage << std::endl;\
        free(errorMessage);\
} while(0)


inline std::string 
fileToString( const char *filename) {

    std::ifstream file( filename);
    if( !file.is_open()) {
        assert( 0 && "FAIL: Opening file ");
        std::string msg("FAIL: Opening file: ");
        msg += filename; msg += "\n";
        std::cout << msg.c_str() << std::endl;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

GLint 
loadShader( const char *filename, GLenum shaderType) {

    GLint shader = 0;

    std::string shaderStr = fileToString( filename);
    assert(!shaderStr.empty());

    shader = glCreateShader( shaderType);

    glShaderSource( shader, 1, (const GLchar**)&shaderStr, NULL);
    glCompileShader( shader);
    
    GLint status;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status);
    if( status == GL_FALSE) {

#if ENABLE_CONSOLE
        PRINT_SHADER_ERROR( shader);
#endif
        assert(!"Shader didn't compile!");
    } else {

#if ENABLE_CONSOLE
        printf("Shader compilation succeeded\n");
#endif
    }
    return shader;
}


GLint 
createProgram(const char *vertex, const char *fragment) {

    GLint program = glCreateProgram();

    GLint vertexObject = loadShader( vertex, GL_VERTEX_SHADER);
    GLint fragmentObject = loadShader( fragment, GL_FRAGMENT_SHADER);

    glAttachShader( program, vertexObject);
    glAttachShader( program, fragmentObject);
    glLinkProgram( program);

    glDetachShader( program, vertexObject);
    glDetachShader( program, fragmentObject);

    GLint status;
    glGetProgramiv( program, GL_LINK_STATUS, &status);
    if( status == GL_FALSE) {
        PRINT_PROGRAM_ERROR( program);
        assert(0 && "Shader linking failed!\n");
        glDeleteProgram( program );
        program = 0;
    }
    return program;
}


GLint 
createProgram(const char *vertex, const char *fragment, const char *geometry) {

    GLint program = glCreateProgram();

    GLint vertexObject = loadShader( vertex, GL_VERTEX_SHADER);
    GLint fragmentObject = loadShader( fragment, GL_FRAGMENT_SHADER);
    GLint geometryObject = loadShader( geometry, GL_GEOMETRY_SHADER);

    glAttachShader( program, vertexObject);
    glAttachShader( program, fragmentObject);
    glAttachShader( program, geometryObject);
    glLinkProgram( program);

    glDetachShader( program, vertexObject);
    glDetachShader( program, fragmentObject);
    glDetachShader( program, geometryObject);

    GLint status;
    glGetProgramiv( program, GL_LINK_STATUS, &status);
    if( status == GL_FALSE) {
        PRINT_PROGRAM_ERROR( program);
        assert(0 && "Shader linking failed!\n");
        glDeleteProgram( program );
        program = 0;
    }
    return program;
}

