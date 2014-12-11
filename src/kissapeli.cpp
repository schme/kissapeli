
#include "kissapeli.h"

static bool32 gameInitialised = false;
static Game *game = NULL;

static Color clearColor = {};

static float playIndex1 = 0.f;
static float playIndex2 = 0.f;



void initGame(MemoryStack *memory) {

    // TODO: Move this to MemoryAllocator for this?
    game = (Game*)popMemoryStack( memory, sizeof( Game));

    Board board = { boardWidth, boardHeight };
    Ball ball = { glm::vec2( boardWidth/2, boardHeight/2),
                  glm::vec2( 0, 0),
                  50 };

    Pad player1 = { glm::vec2( padPadding, boardHeight/2 + padInitHeight/2),
                    padInitWidth, padInitHeight };
    Pad player2 = { glm::vec2( boardWidth - padPadding, boardHeight/2 + padInitHeight/2),
                    padInitWidth, padInitHeight };

    game->player1 = player1;
    game->player2 = player2;
    game->ball = ball;
    game->board = board;

    gameInitialised = true;
}


void gameRender( HDC DeviceContext) {

    draw( clearColor);
}

 
void gameUpdate( MemoryStack* memory, HDC DeviceContext) {

    if( !gameInitialised) {
        initRender( DeviceContext);
        initGame( memory);
    }

    // game stuff
    playIndex1 += .001f;
    playIndex2 += .002f;

    clearColor[0] = sin( clearColor[0] + playIndex2);
    clearColor[1] = sin( clearColor[1] + playIndex1);
    clearColor[2] = sin( clearColor[2] + playIndex1 + playIndex2);

    // render
    gameRender( DeviceContext);
}


void resizeCallback( int w, int h) {
    resize(w, h);
}

