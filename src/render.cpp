#include "render.h"

#include <iostream>
#include <fstream>
#include <sstream>

HDC DeviceContext = {};

static GLuint shaderProgram = 0;
static GLuint vbo = 0;
static GLuint vao = 0;
static float screenWidth = 0;
static float screenHeight = 0;

GLuint screenSizeUnif;

void initVertexBuffer() {
    
    glGenBuffers(1, &vbo);

    glBindBuffer( GL_ARRAY_BUFFER, vbo);
    glBufferData( GL_ARRAY_BUFFER, vertexBufferSize, vertexBuffer, GL_DYNAMIC_DRAW); 
    glBindBuffer( GL_ARRAY_BUFFER, 0);

}

int initRender( void* vertBuf, HDC dc, int width, int height ) {

    DeviceContext = dc;
    vertexBuffer = vertBuf;    
    screenWidth = (float)width;
    screenHeight = (float)height;

    std::vector<GLint> shaders;
    shaders.push_back(kms::loadShader( SHADERPATH("min.vert"), GL_VERTEX_SHADER));
    shaders.push_back(kms::loadShader( SHADERPATH("min.frag"), GL_FRAGMENT_SHADER));

    shaderProgram = kms::createProgram( shaders);
    if( !shaderProgram) {
        return 0;
    }

    glUseProgram( shaderProgram);
    screenSizeUnif = glGetUniformLocation( shaderProgram, "screenSize"); 
    glUniform2f( screenSizeUnif, screenWidth, screenHeight);
    glUseProgram(0);
    

    initVertexBuffer();
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnable( GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    return 1;
}


void draw(Color clearColor) {

    glClearColor( clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear( GL_COLOR_BUFFER_BIT);

    glUseProgram( shaderProgram);

    glBindBuffer( GL_ARRAY_BUFFER, vbo);
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( PlayerArray), vertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays( GL_QUADS, 0, 16);

    glDisableVertexAttribArray(0);
    glUseProgram(0);

    SwapBuffers( DeviceContext);
}


void resize( int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    screenWidth = (float)w;
    screenHeight = (float)h;
    glUniform2f( screenSizeUnif,  screenWidth, screenHeight);
};
