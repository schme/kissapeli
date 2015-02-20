
#include "kissapeli.h"

enum {
    BACKGROUND, PLAYER1, PLAYER2, BALL
};

static Game *game;
static MemoryStack *memory;
static GameStatus gameStatus = Playing;
static AudioEngine *audioEngine;

static uint32 p1lives = 9;
static uint32 p2lives = 9;


void getRect( Rect *in, Pad *p) {
    (*in)[0] = p->position + p->velocity;
    (*in)[1] = p->position + p->dimensions + p->velocity;
}

void getRect( Rect *in, Ball *b) {
    (*in)[0] = b->position - b->radius/2 + b->velocity;
    (*in)[1] = b->position + b->radius/2 + b->velocity;
}

void getRect( Rect *in, Board *w) {
    (*in)[0] = glm::vec2( 0.f, 0.f);
    (*in)[1] = w->dimensions;
}

bool32 aabbCollision( Rect *A, Rect *B ) {
    return ((*A)[0].x < (*B)[1].x &&
            (*A)[1].x > (*B)[0].x &&
            (*A)[0].y < (*B)[1].y &&
            (*A)[1].y > (*B)[0].y );
}

/** * Order: background, player1 , player2, ball
 * TODO: Tidy this mofo uuuup! Clean your room! Do the dishes!
 */
void gameVertices() {

    char* index = (char*)vertexBuffer;
    
    float boardVertices[] = { 0.f, 0.f,
                   game->board.dimensions.x, 0.f,
                   game->board.dimensions.x, game->board.dimensions.y,
                   0.f, game->board.dimensions.y };
    memcpy( index, boardVertices, sizeof( boardVertices));
    index += sizeof( boardVertices);


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

    (*qa)[10] = p->position.x + p->dimensions.x; 
    (*qa)[11] = p->position.y;

    (*qa)[12] = p->position.x + p->dimensions.x; 
    (*qa)[13] = p->position.y + p->dimensions.y;

    (*qa)[14] = p->position.x; 
    (*qa)[15] = p->position.y + p->dimensions.y;

    index += sizeof( PlayerArray);

    Ball *b = &game->ball;
    float ballVertices[] = { b->position.x - b->radius/2, b->position.y - b->radius/2,
                      b->position.x + b->radius/2, b->position.y - b->radius/2,
                      b->position.x + b->radius/2, b->position.y + b->radius/2,
                      b->position.x - b->radius/2, b->position.y + b->radius/2 };

    memcpy( index, ballVertices, sizeof(ballVertices));

    index += sizeof( ballVertices);

    //TODO: is ugly pls do something
    float colors[] = { 
        backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a,
        backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a,
        backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a,
        backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a,

        player1Color.r, player1Color.g, player1Color.b, player1Color.a,
        player1Color.r, player1Color.g, player1Color.b, player1Color.a,
        player1Color.r, player1Color.g, player1Color.b, player1Color.a,
        player1Color.r, player1Color.g, player1Color.b, player1Color.a,

        player2Color.r, player2Color.g, player2Color.b, player2Color.a,
        player2Color.r, player2Color.g, player2Color.b, player2Color.a,
        player2Color.r, player2Color.g, player2Color.b, player2Color.a,
        player2Color.r, player2Color.g, player2Color.b, player2Color.a,

        ballColor.r, ballColor.g, ballColor.b, ballColor.a,
        ballColor.r, ballColor.g, ballColor.b, ballColor.a,
        ballColor.r, ballColor.g, ballColor.b, ballColor.a,
        ballColor.r, ballColor.g, ballColor.b, ballColor.a,
    };

    memcpy( index, colors, sizeof(colors));
}


void worldCollision( Pad *p, Rect *Board) {

    glm::vec2 wDim = game->board.dimensions;

    if ((*Board)[0].x < 0) {
        p->position.x = 0;
        p->velocity.x = 0;
    }
    if ((*Board)[1].x > wDim.x) {
        p->position.x = wDim.x - p->dimensions.x;
        p->velocity.x = 0;
    }
    if ((*Board)[0].y < 0) {
        p->position.y = 0;
        p->velocity.y = 0;
    }
    if ((*Board)[1].y > wDim.y) {
        p->position.y = wDim.y - p->dimensions.y;
        p->velocity.y = 0;
    }
}


void worldCollision( Ball *b, Rect *Board) {

    bool32 collidedGoal = false;
    bool32 collidedEdge = false;

    glm::vec2 wDim= game->board.dimensions;

    if ((*Board)[0].x < 0) {
        --p1lives;
        gameStatus = Resetting;
        collidedGoal = true;
    } else if ((*Board)[1].x > wDim.x) {
        --p2lives;
        gameStatus = Resetting;
        collidedGoal = true;
    } else if ((*Board)[0].y < 0) {
        b->position.y = b->radius / 2.f;
        b->velocity.y = -(b->velocity.y);
        collidedEdge = true;
    } else if ((*Board)[1].y > wDim.y) {
        b->position.y = wDim.y - b->radius / 2.f;
        b->velocity.y = -(b->velocity.y);
        collidedEdge = true;
    }

    if( collidedGoal) {
        audioEngine->playAudio(3);
        printf("p1lives: %d\tp2lives: %d\n", p1lives, p2lives);
    } else if( collidedEdge) {
        audioEngine->playAudio(2);
    }
}


void applyVelocities(real64 ddeltaTime) {

    real32 deltaTime = (real32)ddeltaTime;
    game->player1.position += game->player1.velocity * (deltaTime / timeStep);
    game->player2.position += game->player2.velocity * (deltaTime / timeStep);

    if( gameStatus == Playing) {
        game->ball.position += game->ball.velocity * (deltaTime / timeStep);
    }
}

    
void ballRectCollision( Rect *rectBall, Rect *rectPad) {

    if( aabbCollision( rectBall, rectPad)) {

        Ball *ball = &game->ball;
        bool32 rightSide = ball->position.x > game->board.dimensions.x/2.f;

        /** already collided and heading the other way, ignore */
        if( (rightSide && ball->velocity.x < 0) ||
            (!rightSide && ball->velocity.x > 0)) {
            return;
        }

        float padMiddlePos = (((*rectPad)[1].y - (*rectPad)[0].y) / 2.f) + (*rectPad)[0].y;
        float english = ballEnglishMax * std::abs( ball->position.y - padMiddlePos ) / ((*rectPad)[1].y - padMiddlePos);

        int velSign = ball->velocity.y < 0 ? -1 : 1;
        ball->velocity.y = velSign * (ballYSpeed + english);
        ball->velocity.x = ( rightSide) ? -std::abs(ball->velocity.x) :
                                            std::abs( ball->velocity.x);

        //play sound
        audioEngine->playAudio(1);
    }
}

void collisions(real64 deltaTime) {

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

    /** collisionHandling for my ballz */
    ballRectCollision( &rectBall, &rectPlayer1);
    ballRectCollision( &rectBall, &rectPlayer2);
}


void runSimulation(real64 deltaTime) {
    
    collisions(deltaTime);
    applyVelocities(deltaTime);
}


void allocateGameMemory() {

    game = (Game*)popMemoryStack( memory, sizeof( Game));
    vertexBuffer = popMemoryStack( memory, vertexBufferSize);
}

/**
 * TODO(Kasper): Why is the board wrong width before resize?
 */
Ball getNewBall() {
    int32 random = rand();

    Ball ball = { glm::vec2( game->board.dimensions.x/2.f, 
                             game->board.dimensions.y/2.f),
                  glm::vec2( (random < RAND_MAX/2) ? -ballXSpeed : ballXSpeed,
                             (random % 10 < 5) ? -ballYSpeed : ballYSpeed),
                  ballRadius
    };

    return ball;
}


void initGame() {

    srand((uint32)time(NULL));
    Board board = { glm::vec2( boardWidth, boardHeight ) };

    Pad player1 = { glm::vec2( padPadding, (real32)boardHeight/2.f - padInitHeight/2.f),
                    glm::vec2(padInitWidth, padInitHeight) 
    };
    Pad player2 = { glm::vec2(  boardWidth - padPadding - padInitWidth, 
                                boardHeight/2.f - padInitHeight/2.f),
                    glm::vec2(padInitWidth, padInitHeight) 
    };

    game->player1 = player1;
    game->player2 = player2;
    game->board = board;
    game->ball = getNewBall();

}


void initAudio() {
    audioEngine->loadAudio( "assets/audio/song.wav", 1, 0);
    audioEngine->loadAudio( "assets/audio/beepA.wav", 0, 1);
    audioEngine->loadAudio( "assets/audio/beepF.wav", 0, 2);
    audioEngine->loadAudio( "assets/audio/wouu.wav", 0, 3);

#if 0
    audioEngine->playAudio(0);
#endif
}

void gameRender(uint64 frame, real64 deltaTime) {

    GameState state = {
        deltaTime, gameStatus, p1lives, p2lives
    };
    gameVertices();
    draw( frame, state);
}


void gameInit( MemoryStack* ms, AudioEngine* ae) {
    memory = ms;
    audioEngine = ae;

    allocateGameMemory();
    initAudio();
    initGame();
    initRender( vertexBuffer, boardWidth, boardHeight);
};

void changeInputState( GameInput *state, GameInput input) {

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

    changeInputState( &state, input);

    /** Player1 **/
    if( state.KEY_W) {
        game->player1.velocity.y = padVelocityMod;

    } else if( state.KEY_S) {
        game->player1.velocity.y = -padVelocityMod;

    } else {
        game->player1.velocity.y = 0;
    }

    /** Player2 **/
    if( state.KEY_UP) {
        game->player2.velocity.y = padVelocityMod;

    } else if( state.KEY_DOWN) {
        game->player2.velocity.y = -padVelocityMod;

    } else {
        game->player2.velocity.y = 0.f;
    }

};
 

void gameUpdate(GameInput input) {

    static real64 timer;
    GameStatus lastStatus = gameStatus;

    HandleInput(input);
    runSimulation(input.deltaTime);

    switch (gameStatus) { 
        case Resetting: {

            if( lastStatus == Playing ) {
                game->ball = getNewBall();
                timer = 0.0;

            } else if( timer >= goalResetTime) {
                gameStatus = Playing;

            } else {
                timer += input.deltaTime;
            }
            
        } break;
        default: break;
    }

    gameRender( input.frame, input.deltaTime);
}


void resizeCallback( int w, int h) {
    resize(w, h);
}
