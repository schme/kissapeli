#ifndef KISSAPELI_H_
#define KISSAPELI_H_
#include <cmath>    // for floating point abs()
#include <stdlib.h> // rand()
#include <time.h>

//TODO(kasper): Isolate the audio layer
#include "win_audio.h"

#include "render.h"
#include "types.h"
#include "memorystack.h"

const real32 goalResetTime = 1000; //in ms
const real32 timeStep = 16.6f;

const real32 padPadding = 128;
const real32 padInitWidth = 12;
const real32 padInitHeight = 32; 
const real32 ballXSpeed = 5;
const real32 ballYSpeed = 3;
const real32 ballEnglishMax = 4;

static real32 ballRadius = 12;
static real32 padVelocityMod = 10;

static glm::vec4 p1ObjectColor = glm::vec4( 1.f, 0.f, 0.f, 1.f);
static glm::vec4 p2ObjectColor = glm::vec4( 0.f, 1.f, 0.f, 1.f);
static glm::vec4 bObjectColor = glm::vec4( 0.f, 0.f, 1.f, 1.f);
static glm::vec4 bgObjectColor = glm::vec4( .1f, .1f, .1f, 1.f);

static real32 boardWidth = 1024;
static real32 boardHeight = 512;

/** Rect[0] left bottom, Rect[1] right top*/
typedef glm::vec2 Rect[2];

struct Pad {
    glm::vec2 position;
    glm::vec2 dimensions;
    glm::vec2 velocity;
};

struct Ball {
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
};

struct Board {
    glm::vec2 dimensions;
};

struct Game {
    Pad player1;
    Pad player2;
    Ball ball;
    Board board;
};

struct GameInput {
    bool32 KEY_W;
    bool32 KEY_A;
    bool32 KEY_S;
    bool32 KEY_D;
    bool32 KEY_UP;
    bool32 KEY_LEFT;
    bool32 KEY_DOWN;
    bool32 KEY_RIGHT;
    real64 deltaTime;
    uint64 frame;
};

void gameRender();
void gameUpdate( GameInput);
void resizeCallback( int w, int h);
void gameInit( MemoryStack *, AudioEngine *);

#endif // KISSAPELI_H_
