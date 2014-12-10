#include "render.h"

HDC DeviceContext = {};

GLuint shaderProgram = 0;

void initRender( HDC dc ) {
    DeviceContext = dc;

    std::vector<kms::Shader> shaders;
    shaders.push_back( kms::Shader::loadShaderFile( SHADERPATH("min.vert"), GL_VERTEX_SHADER));
    shaders.push_back( kms::Shader::loadShaderFile( SHADERPATH("min.frag"), GL_FRAGMENT_SHADER));

    shaderProgram = glCreateProgram();
    for (int i = 0; i < shaders.size(); ++i) {
        glAttachShader( shaderProgram, shaders[i].object());
    }
    glLinkProgram( shaderProgram);
    for (int i = 0; i < shaders.size(); ++i)
    {
        glDetachShader( shaderProgram, shaders[i].object());
    }

    GLint status;
    glGetProgramiv( shaderProgram, GL_LINK_STATUS, &status);
    if( status == GL_FALSE) {
        std::string msg( "Program linking failure: " );

        GLint infoLogLength;
        glGetProgramiv( shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength );
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog( shaderProgram, infoLogLength, NULL, strInfoLog );
        msg += strInfoLog;
        delete[] strInfoLog;
        CPP_OUTPUTDEBUG( msg )

        glDeleteProgram( shaderProgram ); shaderProgram = 0;
    }
}


void gl_Draw(Color clearColor) {

    glClearColor( clearColor[0], clearColor[1], clearColor[2], 0.f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_QUADS);
    glNormal3f( 0.0F, 0.0F, 1.0F);
    glVertex3f( 0.5F, 0.5F, 0.5F); glVertex3f(-0.5F, 0.5F, 0.5F);
    glVertex3f(-0.5F,-0.5F, 0.5F); glVertex3f( 0.5F,-0.5F, 0.5F);

    glNormal3f( 0.0F, 0.0F,-1.0F);
    glVertex3f(-0.5F,-0.5F,-0.5F); glVertex3f(-0.5F, 0.5F,-0.5F);
    glVertex3f( 0.5F, 0.5F,-0.5F); glVertex3f( 0.5F,-0.5F,-0.5F);

    glNormal3f( 0.0F, 1.0F, 0.0F);
    glVertex3f( 0.5F, 0.5F, 0.5F); glVertex3f( 0.5F, 0.5F,-0.5F);
    glVertex3f(-0.5F, 0.5F,-0.5F); glVertex3f(-0.5F, 0.5F, 0.5F);

    glNormal3f( 0.0F,-1.0F, 0.0F);
    glVertex3f(-0.5F,-0.5F,-0.5F); glVertex3f( 0.5F,-0.5F,-0.5F);
    glVertex3f( 0.5F,-0.5F, 0.5F); glVertex3f(-0.5F,-0.5F, 0.5F);

    glNormal3f( 1.0F, 0.0F, 0.0F);
    glVertex3f( 0.5F, 0.5F, 0.5F); glVertex3f( 0.5F,-0.5F, 0.5F);
    glVertex3f( 0.5F,-0.5F,-0.5F); glVertex3f( 0.5F, 0.5F,-0.5F);

    glNormal3f(-1.0F, 0.0F, 0.0F);
    glVertex3f(-0.5F,-0.5F,-0.5F); glVertex3f(-0.5F,-0.5F, 0.5F);
    glVertex3f(-0.5F, 0.5F, 0.5F); glVertex3f(-0.5F, 0.5F,-0.5F);
    glEnd();

    SwapBuffers( DeviceContext);
}
