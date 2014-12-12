
#include "kissapeli.h"

static Game *game = NULL;
static MemoryStack *memory = NULL;

static float playIndex1 = 0.f;
static float playIndex2 = 0.f;


/**
 * Order: player1 first, player2 second
 */
void padPositionVertices() {
    PlayerArray *qa = (PlayerArray*)vertexBuffer;

    Pad *p = &game->player1;
    (*qa)[0] = (float)p->position.x;
    (*qa)[1] = (float)p->position.y;

    (*qa)[2] = (float)p->position.x + p->dimensions.x;
    (*qa)[3] = (float)p->position.y;

    (*qa)[4] = (float)p->position.x + p->dimensions.x; 
    (*qa)[5] = (float)p->position.y + p->dimensions.y;

    (*qa)[6] = (float)p->position.x;
    (*qa)[7] = (float)p->position.y + p->dimensions.y;

    p = &game->player2;
    (*qa)[8] = (float)p->position.x; 
    (*qa)[9] = (float)p->position.y;

    (*qa)[10] = (float) p->position.x + p->dimensions.x; 
    (*qa)[11] = (float)p->position.y;

    (*qa)[12] = (float)p->position.x + p->dimensions.x; 
    (*qa)[13] = (float)p->position.y + p->dimensions.y;

    (*qa)[14] = (float)p->position.x; 
    (*qa)[15] = (float)p->position.y + p->dimensions.y;
}


PlayerColor* padsToColor() {
    return NULL;
}



void initGame() {

    World board = { glm::vec4( ),
                    glm::vec2( boardWidth, boardHeight )
    };
    Ball ball = { glm::vec4(1.f,1.f,1.f,1.f),
                  glm::vec2( boardWidth/2, boardHeight/2),
                  glm::vec2( 0, 0),
                  50 
    };
    Pad player1 = { glm::vec4(), 
                    glm::vec2( padPadding, (float)boardHeight/2.f + padInitHeight/2.f),
                    glm::vec2(padInitWidth, padInitHeight) 
    };
    Pad player2 = { glm::vec4(),
                    glm::vec2(  boardWidth - padPadding, 
                                (float)boardHeight/2.f + padInitHeight/2.f),
                    glm::vec2(padInitWidth, padInitHeight) 
    };

    game->player1 = player1;
    game->player2 = player2;
    game->ball = ball;
    game->board = board;
}

void gameRender() {
    draw( game->board.color);
}


void gameInit( MemoryStack* ms, HDC DeviceContext) {
    memory = ms;

    // TODO: Move this to MemoryAllocator for this?
    game = (Game*)popMemoryStack( memory, sizeof( Game));
    vertexBuffer = popMemoryStack( memory, vertexBufferSize);
    initGame();
    initRender( vertexBuffer, DeviceContext, boardWidth, boardHeight);
};

void HandleInput( GameInput input) {

    static GameInput down;


    if( input.KEY_W ) {
        game->player1.position.y += 20;
    }
    if( input.KEY_A) {
        game->player1.position.x -= 20;
    }
    if( input.KEY_S) {
        game->player1.position.y -= 20;
    }
    if( input.KEY_D) {
        game->player1.position.x += 20;
    }
    if( input.KEY_UP) {
        game->player2.position.y += 20;
    }
    if( input.KEY_LEFT) {
        game->player2.position.x -= 20;
    }
    if( input.KEY_DOWN) {
        game->player2.position.y -= 20;
    }
    if( input.KEY_RIGHT) {
        game->player2.position.x += 20;

    }

    down = input;

};
 

void gameUpdate(GameInput input) {


    // game stuff
    playIndex1 += .00001f;
    playIndex2 += .00002f;

    game->board.color =  glm::vec4( 
                             sin( game->board.color.x + playIndex1),
                             sin( game->board.color.y + playIndex2),
                             sin( game->board.color.z + playIndex2 + playIndex1),
                             0.f
                         );
    HandleInput(input);

    padPositionVertices();
    // render
    gameRender();
}


void resizeCallback( int w, int h) {
    resize(w, h);
}


