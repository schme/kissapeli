#include "render.h"


enum {
    BACKGROUND, PLAYER1, PLAYER2, BALL
};

static GLuint shaderProgram = 0;
static GLuint vbo = 0;
static GLuint vao = 0;

static real32 aspectRatio = 0;
static real32 screenWidth = 0;
static real32 screenHeight = 0;
static real32 boardWidth = 0;
static real32 boardHeight = 0;

// 2 floats * 4 corners * ( board, 2 pads, ball )
static uint32 vertexDataSize = 32 * sizeof(float);

GLuint screenSizeUnif;
GLuint boardSizeUnif;
GLuint frameUnif;
GLuint deltaTimeUnif;
GLuint objectUnif;

float scoreBoard[] = {
//  Position      Color             Texcoords
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
};

void initVertexBuffer() {
    
    glGenBuffers(1, &vbo);

    glBindBuffer( GL_ARRAY_BUFFER, vbo);
    glBufferData( GL_ARRAY_BUFFER, vertexBufferSize, vertexBuffer, GL_STATIC_DRAW); 

    glBindBuffer( GL_ARRAY_BUFFER, 0);
}

int initRender( void* vertBuf, real32 width, real32 height ) {

    vertexBuffer = vertBuf;    
    boardWidth = width;
    boardHeight = height;

    std::vector<GLint> shaders;
    shaders.push_back(loadShader( SHADERPATH("kp_shader.vert"), GL_VERTEX_SHADER));
    shaders.push_back(loadShader( SHADERPATH("kp_shader.frag"), GL_FRAGMENT_SHADER));

    shaderProgram = createProgram( shaders);
    if( !shaderProgram) {
        assert(0 && "No Shader Program!\n");
        return 0;
    }

    glUseProgram( shaderProgram);

    screenSizeUnif = glGetUniformLocation( shaderProgram, "screenSize"); 
    boardSizeUnif = glGetUniformLocation( shaderProgram, "boardSize");
    frameUnif = glGetUniformLocation( shaderProgram, "frame"); 
    objectUnif = glGetUniformLocation( shaderProgram, "object");
    deltaTimeUnif = glGetUniformLocation( shaderProgram, "deltaTime");

    glUniform2f( screenSizeUnif, screenWidth, screenHeight);
    glUniform2f( boardSizeUnif, boardWidth, boardHeight);
    glUseProgram(0);

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);

    initVertexBuffer();
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    //glEnable( GL_BLEND);
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable( GL_DEPTH_TEST);
    //glDepthMask( GL_TRUE);
    //glDepthFunc( GL_LEQUAL);
    //glDepthRange( 1.0f, 0.0f);
    //glClearDepth(1.0f);

    glEnable( GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    return 1;
}


void draw(uint64 frame, GameState gameState) {

    glClear( GL_COLOR_BUFFER_BIT );
    glUseProgram( shaderProgram);

    glUniform1i( frameUnif, (uint32)frame);
    glUniform1f( deltaTimeUnif, (real32)gameState.deltaTime);

    glBindBuffer( GL_ARRAY_BUFFER, vbo);
    glBufferSubData( GL_ARRAY_BUFFER, 0, vertexBufferSize, vertexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataSize));

    glUniform1i( objectUnif, BACKGROUND);
    glDrawArrays( GL_QUADS, 0, 4);

    glUniform1i( objectUnif, PLAYER1);
    glDrawArrays( GL_QUADS, 4, 4);

    glUniform1i( objectUnif, PLAYER2);
    glDrawArrays( GL_QUADS, 8, 4);

    glUniform1i( objectUnif, BALL);
    glDrawArrays( GL_QUADS, 12, 4);

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glUseProgram(0);
}


void resize( int32 w, int32 h) {

    real32 aspectRatio = boardWidth / boardHeight;
    real32 windowAspectRatio = (real32)w / (real32)h;
    real32 paddingY = 0;
    real32 paddingX = 0;


    if( windowAspectRatio > aspectRatio ) {
        screenHeight = (real32)h;
        screenWidth = screenHeight * aspectRatio;
        paddingX = (w - screenWidth) / 2.0f;

    } else {
        screenWidth = (real32)w;
        screenHeight = screenWidth / aspectRatio;
        paddingY = (h - screenHeight) / 2.0f;
    }

    glViewport((GLint)paddingX, (GLint)paddingY, (GLsizei)screenWidth, (GLsizei)screenHeight);

    glUseProgram( shaderProgram);
    glUniform2f( screenSizeUnif, screenWidth, screenHeight);
    glUseProgram(0);
};
