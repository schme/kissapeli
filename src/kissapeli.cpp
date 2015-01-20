
#include "kissapeli.h"

static Game *game = NULL;
static MemoryStack *memory = NULL;


void getRect( Rect *in, Pad *p) {
    (*in)[0] = p->position + p->velocity;
    (*in)[1] = p->position + p->dimensions + p->velocity;
}

void getRect( Rect *in, Ball *b) {
    (*in)[0] = b->position - b->radius/2 + b->velocity;
    (*in)[1] = b->position + b->radius/2 + b->velocity;
}

void getRect( Rect *in, World *w) {
    (*in)[0] = glm::vec2( 0.f, 0.f);
    (*in)[1] = w->dimensions;
}


bool32 aabbCollision( Rect *A, Rect *B ) {
    return ((*A)[0].x < (*B)[1].x &&
            (*A)[1].x > (*B)[0].x &&
            (*A)[0].y < (*B)[1].y &&
            (*A)[1].y > (*B)[0].y );
}


void worldCollision( Pad *p, Rect *World) {

    glm::vec2 wDim= game->board.dimensions;

    if ((*World)[0].x < 0) {
        p->position.x = 0;
        p->velocity.x = 0;
    }
    if ((*World)[1].x > wDim.x) {
        p->position.x = wDim.x - p->dimensions.x;
        p->velocity.x = 0;
    }
    if ((*World)[0].y < 0) {
        p->position.y = 0;
        p->velocity.y = 0;
    }
    if ((*World)[1].y > wDim.y) {
        p->position.y = wDim.y - p->dimensions.y;
        p->velocity.y = 0;
    }
}


void worldCollision( Ball *b, Rect *World) {

    glm::vec2 wDim= game->board.dimensions;

    if ((*World)[0].x < 0) {
        b->position.x = b->radius / 2.f;
        b->velocity.x = -(b->velocity.x);
    }
    if ((*World)[1].x > wDim.x) {
        b->position.x = wDim.x - b->radius / 2.f;
        b->velocity.x = -(b->velocity.x);
    }
    if ((*World)[0].y < 0) {
        b->position.y = b->radius / 2.f;
        b->velocity.y = -(b->velocity.y);
    }
    if ((*World)[1].y > wDim.y) {
        b->position.y = wDim.y - b->radius / 2.f;
        b->velocity.y = -(b->velocity.y);
    }
}


void applyVelocities() {

    game->player1.position += game->player1.velocity;
    game->player2.position += game->player2.velocity;
    game->ball.position += game->ball.velocity;

}

    
void ballRectCollision( Rect *rectBall, Rect *rectPad) {

    if( aabbCollision( rectBall, rectPad)) {

        Ball *ball = &game->ball;
        bool32 rightSide = ball->position.x > game->board.dimensions.x/2.f;

        float padMiddlePos = (((*rectPad)[1].y - (*rectPad)[0].y) / 2.f) + (*rectPad)[0].y;
        float english = ballEnglishMax * std::abs( ball->position.y - padMiddlePos ) / ((*rectPad)[1].y - padMiddlePos);

        int velSign = ball->velocity.y < 0 ? -1 : 1;
        ball->velocity.y = velSign * (ballYSpeed + english);

        //WIN_OUTPUTDEBUG_F( "\nenglish ", english);
        //WIN_OUTPUTDEBUG_F( "padMiddlePos ", padMiddlePos);
        //WIN_OUTPUTDEBUG_F( "ball->position.y ", ball->position.y);
        //WIN_OUTPUTDEBUG_F( "ball->velocity.y ", ball->velocity.y);

        if (ball->position.y >= (*rectPad)[1].y || 
            ball->position.y <= (*rectPad)[0].y ) {

            ball->velocity.y = -ball->velocity.y;
        }

        ball->velocity.x = ( rightSide) ? -std::abs(ball->velocity.x) :
                                            std::abs( ball->velocity.x);
    }
}

void collisions() {

    Pad *player1 = &game->player1;
    Pad *player2 = &game->player2;
    Ball *ball = &game->ball;

    Rect rectPlayer1;
    Rect rectPlayer2;
    Rect rectBall;

    getRect( &rectPlayer1, player1);
    getRect( &rectPlayer2, player2);
    getRect( &rectBall, ball);

    assert( rectPlayer1 && "rectPlayer1 is null");
    assert( rectPlayer2 && "rectPlayer2 is null");
    assert( rectBall && "rectBall is null");

    worldCollision( player1, &rectPlayer1);
    worldCollision( player2, &rectPlayer2);
    worldCollision( ball, &rectBall);

    /** collisionHandling for my balls */
    ballRectCollision( &rectBall, &rectPlayer1);
    ballRectCollision( &rectBall, &rectPlayer2);

}


void runSimulation() {
    collisions();
    applyVelocities();
}

/**
 * Order: background, player1 , player2 
 */
void gameVertices() {

    char* index = (char*)vertexBuffer;
    
    float vb[] = { 0.f, 0.f,
                   game->board.dimensions.x, 0.f,
                   game->board.dimensions.x, game->board.dimensions.y,
                   0.f, game->board.dimensions.y };
    memcpy( index, vb, sizeof( vb));
    index += sizeof( vb);

    PlayerArray *qa = (PlayerArray*)index;

    Pad *p = &game->player1;
    (*qa)[0] = p->position.x;
    (*qa)[1] = p->position.y;

    (*qa)[2] = p->position.x + p->dimensions.x;
    (*qa)[3] = p->position.y;

    (*qa)[4] = p->position.x + p->dimensions.x; 
    (*qa)[5] = p->position.y + p->dimensions.y;

    (*qa)[6] = p->position.x;
    (*qa)[7] = p->position.y + p->dimensions.y;

    p = &game->player2;
    (*qa)[8] = p->position.x; 
    (*qa)[9] = p->position.y;

    (*qa)[10] =p->position.x + p->dimensions.x; 
    (*qa)[11] =p->position.y;

    (*qa)[12] =p->position.x + p->dimensions.x; 
    (*qa)[13] =p->position.y + p->dimensions.y;

    (*qa)[14] =p->position.x; 
    (*qa)[15] =p->position.y + p->dimensions.y;

    index += sizeof( PlayerArray);

    Ball *b = &game->ball;
    float vball[] = { b->position.x - b->radius/2, b->position.y - b->radius/2,
                      b->position.x + b->radius/2, b->position.y - b->radius/2,
                      b->position.x + b->radius/2, b->position.y + b->radius/2,
                      b->position.x - b->radius/2, b->position.y + b->radius/2 };

    memcpy( index, vball, sizeof(vball));

    index += sizeof( vball);


    //TODO: This is ugly pls do something
    float colors[] = { 
        bgShaderColor.r, bgShaderColor.g, bgShaderColor.b, bgShaderColor.a,
        bgShaderColor.r, bgShaderColor.g, bgShaderColor.b, bgShaderColor.a,
        bgShaderColor.r, bgShaderColor.g, bgShaderColor.b, bgShaderColor.a,
        bgShaderColor.r, bgShaderColor.g, bgShaderColor.b, bgShaderColor.a,

        p1ShaderColor.r, p1ShaderColor.g, p1ShaderColor.b, p1ShaderColor.a,
        p1ShaderColor.r, p1ShaderColor.g, p1ShaderColor.b, p1ShaderColor.a,
        p1ShaderColor.r, p1ShaderColor.g, p1ShaderColor.b, p1ShaderColor.a,
        p1ShaderColor.r, p1ShaderColor.g, p1ShaderColor.b, p1ShaderColor.a,

        p2ShaderColor.r, p2ShaderColor.g, p2ShaderColor.b, p2ShaderColor.a,
        p2ShaderColor.r, p2ShaderColor.g, p2ShaderColor.b, p2ShaderColor.a,
        p2ShaderColor.r, p2ShaderColor.g, p2ShaderColor.b, p2ShaderColor.a,
        p2ShaderColor.r, p2ShaderColor.g, p2ShaderColor.b, p2ShaderColor.a,

        bShaderColor.r, bShaderColor.g, bShaderColor.b, bShaderColor.a,
        bShaderColor.r, bShaderColor.g, bShaderColor.b, bShaderColor.a,
        bShaderColor.r, bShaderColor.g, bShaderColor.b, bShaderColor.a,
        bShaderColor.r, bShaderColor.g, bShaderColor.b, bShaderColor.a,
    };

    memcpy( index, colors, sizeof(colors));

}



void initGame() {

    srand((unsigned)time(NULL));
    int random = rand() / RAND_MAX;

    World board = { glm::vec2( boardWidth, boardHeight ) };

    Ball ball = { glm::vec2( boardWidth/2.f, boardHeight/2.f),
                  glm::vec2( (random < RAND_MAX/2) ? -ballXSpeed : ballXSpeed,
                             (random < RAND_MAX/2) ? -ballYSpeed : ballYSpeed),
                  ballRadius
    };
    Pad player1 = { glm::vec2( padPadding, (float)boardHeight/2.f - padInitHeight/2.f),
                    glm::vec2(padInitWidth, padInitHeight) 
    };
    Pad player2 = { glm::vec2(  boardWidth - padPadding - padInitWidth, 
                                (float)boardHeight/2.f - padInitHeight/2.f),
                    glm::vec2(padInitWidth, padInitHeight) 
    };

    game->player1 = player1;
    game->player2 = player2;
    game->ball = ball;
    game->board = board;
}

void gameRender(uint64 frame) {
    gameVertices();
    draw( frame);
}

void gameRender() {
}

void gameInit( MemoryStack* ms) {
    memory = ms;

    // TODO: Move this to MemoryAllocator for this?
    game = (Game*)popMemoryStack( memory, sizeof( Game));
    vertexBuffer = popMemoryStack( memory, vertexBufferSize);
    initGame();
    initRender( vertexBuffer, boardWidth, boardHeight);
};

void changeInputStates( GameInput *state, GameInput input) {

    if( input.KEY_W ) {
        (*state).KEY_W = !(*state).KEY_W;
    } 
    if( input.KEY_S) {
        (*state).KEY_S = !(*state).KEY_S;
    } 
    if( input.KEY_A) {
        (*state).KEY_A = !(*state).KEY_A;
    } 
    if( input.KEY_D) {
        (*state).KEY_D = !(*state).KEY_D;
    } 
    if( input.KEY_UP) {
        (*state).KEY_UP = !(*state).KEY_UP;
    } 
    if( input.KEY_DOWN) {
        (*state).KEY_DOWN = !(*state).KEY_DOWN;
    } 
    if( input.KEY_LEFT) {
        (*state).KEY_LEFT = !(*state).KEY_LEFT;
    } 
    if( input.KEY_RIGHT) {
        (*state).KEY_RIGHT = !(*state).KEY_RIGHT;
    }

}


void HandleInput( GameInput input) {

    static GameInput state;

    /** Change state */
    changeInputStates( &state, input);

    /** Player1 **/
    if( state.KEY_W) {
        game->player1.velocity.y = padVelocityMod;
    } else if( state.KEY_S) {
        game->player1.velocity.y = -padVelocityMod;
    } else {
        game->player1.velocity.y = 0;
    }


    //if( state.KEY_A) {
        //game->player1.velocity.x = -padVelocityMod;
    //} else if( state.KEY_D) {
        //game->player1.velocity.x = padVelocityMod;
    //} else {
        //game->player1.velocity.x = 0.f;
    //}


    /** Player2 **/
    if( state.KEY_UP) {
        game->player2.velocity.y = padVelocityMod;
    } else if( state.KEY_DOWN) {
        game->player2.velocity.y = -padVelocityMod;
    } else {
        game->player2.velocity.y = 0.f;
    }

    //if( state.KEY_LEFT) {
        //game->player2.velocity.x = -padVelocityMod;
    //} else if( state.KEY_RIGHT) {
        //game->player2.velocity.x = padVelocityMod;
    //} else {
        //game->player2.velocity.x = 0.f;
    //}

};
 

void gameUpdate(GameInput input) {


    // game stuff
    HandleInput(input);
    runSimulation();

    // render
    gameRender( input.frame);
}


void resizeCallback( int w, int h) {
    if( game) {
        game->board.dimensions.x = (float)w;
        game->board.dimensions.y = (float)h;
    }
    resize(w, h);
}


