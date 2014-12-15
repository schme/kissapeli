#include "render.h"

#include <iostream>
#include <fstream>
#include <sstream>

static GLuint shaderProgram = 0;
static GLuint vbo = 0;
static GLuint vao = 0;

static float screenWidth = 0;
static float screenHeight = 0;

GLuint screenSizeUnif;
GLuint timeUnif;

void initVertexBuffer() {
    
    glGenBuffers(1, &vbo);

    glBindBuffer( GL_ARRAY_BUFFER, vbo);
    glBufferData( GL_ARRAY_BUFFER, vertexBufferSize, vertexBuffer, GL_DYNAMIC_DRAW); 
    glBindBuffer( GL_ARRAY_BUFFER, 0);

}

int initRender( void* vertBuf, int width, int height ) {

    vertexBuffer = vertBuf;    
    screenWidth = (float)width;
    screenHeight = (float)height;

    std::vector<GLint> shaders;
    shaders.push_back(kms::loadShader( SHADERPATH("kp_shader.vert"), GL_VERTEX_SHADER));
    shaders.push_back(kms::loadShader( SHADERPATH("kp_shader.frag"), GL_FRAGMENT_SHADER));

    shaderProgram = kms::createProgram( shaders);
    if( !shaderProgram) {
        assert(0 && "No Shader Program!\n");
        return 0;
    }

    glUseProgram( shaderProgram);
    screenSizeUnif = glGetUniformLocation( shaderProgram, "screenSize"); 
    timeUnif = glGetUniformLocation( shaderProgram, "timeUnif"); 
    glUniform2f( screenSizeUnif, screenWidth, screenHeight);
    glUseProgram(0);

    glClearColor( 0.5f, 0.5f, 0.0f, 1.0f);
    initVertexBuffer();
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnable( GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    return 1;
}


void draw() {

    glClear( GL_COLOR_BUFFER_BIT);
    glUseProgram( shaderProgram);

    glBindBuffer( GL_ARRAY_BUFFER, vbo);
    glBufferSubData( GL_ARRAY_BUFFER, 0, vertexBufferSize, vertexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof( PlayerArray) + sizeof( GameRectangle)));

    // TODO: Change to triangles
    glDrawArrays( GL_QUADS, 0, 32);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glUseProgram(0);
}


void resize( int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    screenWidth = (float)w;
    screenHeight = (float)h;
    glUniform2f( screenSizeUnif,  screenWidth, screenHeight);
};
