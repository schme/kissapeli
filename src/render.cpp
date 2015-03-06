#include "render.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


enum {
    BACKGROUND, PLAYER1, PLAYER2, BALL
};

static GLuint vao = 0;
static GLuint shaderProgram[2] = {0, 0};
static GLuint vbo[2] = {0, 0};  // 0 for objects, 1 for

static GLuint score_ebo;
static GLuint score_tex[10];
static GLuint score_pos_attrib;
static GLuint score_col_attrib;
static GLuint score_tex_attrib;

static real32 aspectRatio = 0;
static real32 screenWidth = 0;
static real32 screenHeight = 0;
static real32 boardWidth = 0;
static real32 boardHeight = 0;

// 2 floats * 4 corners * ( board, 2 pads, ball )
static uint32 vertexDataSize = 32 * sizeof(float);

GLuint screenSizeUnif[2];
GLuint boardSizeUnif;
GLuint frameUnif;
GLuint deltaTimeUnif;
GLuint objectUnif;

GLuint p1score_sampler;
GLuint p2score_sampler;

static float scoreBoard[] = {
//  Position      Color             Texcoords
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
};

static GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
};


void initVertexBuffer() {

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(2, vbo);

    glBindBuffer( GL_ARRAY_BUFFER, vbo[0]);
    glBufferData( GL_ARRAY_BUFFER, vertexBufferSize, vertexBuffer, GL_STATIC_DRAW);

    glBindBuffer( GL_ARRAY_BUFFER, vbo[1]);
    glBufferData( GL_ARRAY_BUFFER, sizeof( scoreBoard), scoreBoard, GL_STATIC_DRAW);

    glGenBuffers(1, &score_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, score_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    score_pos_attrib = glGetAttribLocation( shaderProgram[1], "position");
    glEnableVertexAttribArray(score_pos_attrib);
    glVertexAttribPointer(score_pos_attrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

    score_col_attrib = glGetAttribLocation(shaderProgram[1], "color");
    glEnableVertexAttribArray(score_col_attrib);
    glVertexAttribPointer(score_col_attrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    score_tex_attrib = glGetAttribLocation(shaderProgram[1], "texcoord");
    glEnableVertexAttribArray(score_tex_attrib);
    glVertexAttribPointer(score_tex_attrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

    glBindBuffer( GL_ARRAY_BUFFER, 0);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
}


int initRender( void* vertBuf, real32 width, real32 height ) {

    vertexBuffer = vertBuf;
    boardWidth = width;
    boardHeight = height;

    shaderProgram[0] = createProgram( SHADERPATH("kp_shader.vert"), SHADERPATH("kp_shader.frag"));
    assert(shaderProgram[0]);

    shaderProgram[1] = createProgram( SHADERPATH("score.vert"), SHADERPATH("score.frag"));
    assert(shaderProgram[1]);

    screenSizeUnif[0] = glGetUniformLocation( shaderProgram[0], "screenSize");
    boardSizeUnif = glGetUniformLocation( shaderProgram[0], "boardSize");
    frameUnif = glGetUniformLocation( shaderProgram[0], "frame");
    objectUnif = glGetUniformLocation( shaderProgram[0], "object");
    deltaTimeUnif = glGetUniformLocation( shaderProgram[0], "deltaTime");

    screenSizeUnif[1] = glGetUniformLocation( shaderProgram[1], "screenSize");

    p1score_sampler = glGetUniformLocation( shaderProgram[1], "p1score");
    p2score_sampler = glGetUniformLocation( shaderProgram[1], "p2score");

    glUseProgram( shaderProgram[0]);
    glUniform2f( screenSizeUnif[0], screenWidth, screenHeight);
    glUniform2f( boardSizeUnif, boardWidth, boardHeight);

    glUseProgram( shaderProgram[1]);
    glUniform2f( screenSizeUnif[1], screenWidth, screenHeight);

    glUseProgram(0);


    initVertexBuffer();
    glGenTextures(10, score_tex);

    int32 x,y,n;
    uint8 *image = NULL;
    const char *extension = ".bmp";
#if 1
    for (int i = 0; i < 10; ++i)
    {
        glActiveTexture( GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, score_tex[i]);
        uint8 img_file[64];
        _snprintf_s( (char*)img_file, 64, 63, IMAGEPATH "%d%s", i, extension);
        image = stbi_load( (char*)img_file, &x, &y, &n, 4);
        assert(image && "Reading texture file failed");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA,
                        GL_UNSIGNED_BYTE, image);
        stbi_image_free( image);

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
#else
    glActiveTexture( GL_TEXTURE0);
    uint8 img_file[64];
    _snprintf_s( (char*)img_file, 64, 63, IMAGEPATH "%d%s", 9, extension);
    image = stbi_load( (char*)img_file, &x, &y, &n, 4);
    assert(image && "Reading texture file failed");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, image);
    stbi_image_free( image);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#endif

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

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);

    return 1;
}


void draw(uint64 frame, GameState gameState) {

    glClear( GL_COLOR_BUFFER_BIT );
#if 0
    glUseProgram( shaderProgram[0]);

    glUniform1i( frameUnif, (uint32)frame);
    glUniform1f( deltaTimeUnif, (real32)gameState.deltaTime);

    glBindBuffer( GL_ARRAY_BUFFER, vbo[0]);
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

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
#endif

    glUseProgram(shaderProgram[1]);

    if( gameState.p1lives <= 9) {
        //glUniform1i( p1score_sampler, gameState.p1lives);
        //
        glUniform1i( p1score_sampler, 9);
    } else {
        glUniform1i( p1score_sampler, 0);
    }

    if( gameState.p2lives <= 9) {
        //glUniform1i( p2score_sampler, gameState.p2lives);
        glUniform1i( p2score_sampler, 9);
    } else {
        glUniform1i( p2score_sampler, 0);
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, score_ebo);
    glBindBuffer( GL_ARRAY_BUFFER, vbo[1]);

    glEnableVertexAttribArray( score_pos_attrib);
    glEnableVertexAttribArray( score_col_attrib);
    glEnableVertexAttribArray( score_tex_attrib);

    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUseProgram(0);

    glDisableVertexAttribArray(score_tex_attrib);
    glDisableVertexAttribArray(score_col_attrib);
    glDisableVertexAttribArray(score_pos_attrib);

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer( GL_ARRAY_BUFFER, 0);
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

    glUseProgram( shaderProgram[0]);
    glUniform2f( screenSizeUnif[0], screenWidth, screenHeight);
    glUseProgram( shaderProgram[1]);
    glUniform2f( screenSizeUnif[1], screenWidth, screenHeight);
    glUseProgram(0);
};
