#include "render.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


enum {
    BACKGROUND, PLAYER1, PLAYER2, BALL
};

static GLuint vao = 0;
static GLuint shaderProgram[2] = {0, 0};
static GLuint vbo[2] = {0, 0};  // 0 for objects, 1 for scoreboard

static GLuint score_ebo;
static GLuint score_tex[10];
static GLint score_pos_attrib;
static GLint score_col_attrib;
static GLint score_tex_attrib;

static real32 aspectRatio = 0;
static real32 screenWidth = 0;
static real32 screenHeight = 0;
static real32 boardWidth = 0;
static real32 boardHeight = 0;

// 2 floats * 4 corners * ( board, 2 pads, ball )
static uint32 vertexDataSize = 32 * sizeof(float);

GLint screenSizeUnif[2];
GLint boardSizeUnif[2];
GLint frameUnif;
GLint deltaTimeUnif;
GLint objectUnif;

GLint p1score_sampler;
GLint p2score_sampler;
GLint scoreUnif;

static glm::vec2 s1bl = glm::vec2( 430.0f, 420.0f);
static glm::vec2 s2bl = glm::vec2( 530.0f, 420.0f);
const real32 scrSz = 64.0f;

static GLfloat scoreBoard[] = {

//  Position                                Color                   Texcoords
     s1bl.x, s1bl.y,                        0.4f, 0.2f, 0.4f,       0.0f, 1.0f, //Bottom-left
     s1bl.x + scrSz, s1bl.y,                0.4f, 0.2f, 0.4f,       1.0f, 1.0f, //Bottom-right
     s1bl.x + scrSz,  s1bl.y + scrSz,       0.2f, 0.0f, 0.2f,       1.0f, 0.0f, //Top-right
     s1bl.x,  s1bl.y + scrSz,               0.2f, 0.0f, 0.2f,       0.0f, 0.0f, //Top-left

//  Position                                Color                   Texcoords
     s2bl.x, s2bl.y,                        0.4f, 0.2f, 0.4f,       0.0f, 1.0f, //Bottom-left
     s2bl.x + scrSz, s2bl.y,                0.4f, 0.2f, 0.4f,       1.0f, 1.0f, //Bottom-right
     s2bl.x + scrSz,  s2bl.y + scrSz,       0.2f, 0.0f, 0.2f,       1.0f, 0.0f, //Top-right
     s2bl.x,  s2bl.y + scrSz,               0.2f, 0.0f, 0.2f,       0.0f, 0.0f  //Top-left
};

static GLuint elements[] = {
    0, 1, 2,
    2, 3, 0,
    4, 5, 6,
    6, 7, 4
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

    glBindBuffer( GL_ARRAY_BUFFER, 0);
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
}


int initRender( void* vertBuf, real32 width, real32 height ) {

    vertexBuffer = vertBuf;
    boardWidth = screenWidth = width;
    boardHeight = screenHeight = height;

    initVertexBuffer();

    shaderProgram[0] = createProgram( SHADERPATH("kp_shader.vert"), SHADERPATH("kp_shader.frag"));
    assert(shaderProgram[0]);

    glUseProgram( shaderProgram[0]);

    screenSizeUnif[0] = glGetUniformLocation( shaderProgram[0], "screenSize");
    boardSizeUnif[0] = glGetUniformLocation( shaderProgram[0], "boardSize");
    frameUnif = glGetUniformLocation( shaderProgram[0], "frame");
    objectUnif = glGetUniformLocation( shaderProgram[0], "object");
    deltaTimeUnif = glGetUniformLocation( shaderProgram[0], "deltaTime");

    glUniform2f( screenSizeUnif[0], screenWidth, screenHeight);
    glUniform2f( boardSizeUnif[0], boardWidth, boardHeight);


    shaderProgram[1] = createProgram( SHADERPATH("score.vert"), SHADERPATH("score.frag"));
    assert(shaderProgram[1]);


    screenSizeUnif[1] = glGetUniformLocation( shaderProgram[1], "screenSize");
    boardSizeUnif[1] = glGetUniformLocation( shaderProgram[1], "boardSize");
    p1score_sampler = glGetUniformLocation( shaderProgram[1], "p1score");
    p2score_sampler = glGetUniformLocation( shaderProgram[1], "p2score");
    scoreUnif = glGetUniformLocation( shaderProgram[1], "score");

    score_pos_attrib = glGetAttribLocation( shaderProgram[1], "position");
    score_col_attrib = glGetAttribLocation( shaderProgram[1], "color");
    score_tex_attrib = glGetAttribLocation( shaderProgram[1], "texcoord");

    glUseProgram( shaderProgram[1]);
    glUniform2f( screenSizeUnif[1], screenWidth, screenHeight);
    glUniform2f( boardSizeUnif[1], boardWidth, boardHeight);
    glUseProgram(0);

    glGenTextures(10, score_tex);

    int32 x,y,n;
    uint8 *image = NULL;
    const char *extension = ".bmp";

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

    glEnable( GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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


    /*****************
     *  Score Board  *
     *****************/

    glUseProgram(shaderProgram[1]);

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, score_ebo);
    glBindBuffer( GL_ARRAY_BUFFER, vbo[1]);

    glEnableVertexAttribArray(score_pos_attrib);
    glVertexAttribPointer(score_pos_attrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(score_col_attrib);
    glVertexAttribPointer(score_col_attrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    glEnableVertexAttribArray(score_tex_attrib);
    glVertexAttribPointer(score_tex_attrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

    glUniform1i( p1score_sampler, gameState.p1lives);
    glUniform1i( p2score_sampler, gameState.p2lives);

    glUniform1i( scoreUnif, 1);
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUniform1i( scoreUnif, 2);
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));

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
