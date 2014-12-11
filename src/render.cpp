#include "render.h"

#include <iostream>
#include <fstream>
#include <sstream>

HDC DeviceContext = {};

static GLuint shaderProgram = 0;
static GLuint vbo = 0;
static GLuint vao = 0;

void initVertexBuffer();


int initRender( HDC dc ) {

    DeviceContext = dc;

    std::vector<GLint> shaders;
    shaders.push_back(kms::loadShader( SHADERPATH("min.vert"), GL_VERTEX_SHADER));
    shaders.push_back(kms::loadShader( SHADERPATH("min.frag"), GL_FRAGMENT_SHADER));

    shaderProgram = kms::createProgram( shaders);

    if( !shaderProgram) {
        return 0;
    }

    initVertexBuffer();
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    return 1;
}


const float triangleVertice[] = {
        0.75f, 0.75f, 0.0f, 1.0f,
        0.75f, -0.75f, 0.0f, 1.0f,
        -0.75f, -0.75f, 0.0f, 1.0f,
        -0.75f, 0.75f, 0.0f, 1.0f,
};


void initVertexBuffer() {
    
    glGenBuffers(1, &vbo);

    glBindBuffer( GL_ARRAY_BUFFER, vbo);
    glBufferData( GL_ARRAY_BUFFER, sizeof( triangleVertice), triangleVertice, GL_STATIC_DRAW); 
    glBindBuffer( GL_ARRAY_BUFFER, 0);

}


void draw(Color clearColor) {

    glClearColor( clearColor[0], clearColor[1], clearColor[2], 0.f);
    glClear( GL_COLOR_BUFFER_BIT);

    glUseProgram( shaderProgram);

    glBindBuffer( GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays( GL_QUADS, 0, 4);
    glUseProgram(0);

    SwapBuffers( DeviceContext);
}


void resize( int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
};
