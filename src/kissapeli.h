#ifndef KISSAPELI_H_
#define KISSAPELI_H_
#include <cmath>    // for floating point abs()
#include <stdlib.h>
#include <time.h>

#include "types.h"
#include "render.h"
#include "memorystack.h"

const float padPadding = 128;
const float padInitWidth = 12;
const float padInitHeight = 32; 
const float ballXSpeed = 5;
const float ballYSpeed = 3;
const float ballEnglishMax = 4;

static int boardWidth = 1024;
static int boardHeight = 512;
static float ballRadius = 12;
static float padVelocityMod = 10;



static glm::vec4 p1ShaderColor = glm::vec4( 1.f, 0.f, 0.f, 1.f);
static glm::vec4 p2ShaderColor = glm::vec4( 0.f, 1.f, 0.f, 1.f);
static glm::vec4 bShaderColor = glm::vec4( 0.f, 0.f, 1.f, 1.f);
static glm::vec4 bgShaderColor = glm::vec4( .1f, .1f, .1f, 1.f);


/** Rect[0] left bottom, Rect[1] right top*/
typedef glm::vec2 Rect[2];

enum class Collision { Top, Left, Bottom, Right, No };

struct CollisionEvent {
    Rect collidable;
    Collision collision;
};


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

struct World {
    glm::vec2 dimensions;
};

struct Game {
    Pad player1;
    Pad player2;
    Ball ball;
    World board;
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
    bool32 end;
    uint64 frame;
};

void gameRender();
void gameUpdate( GameInput);
void resizeCallback( int w, int h);
void gameInit( MemoryStack *);

#endif // KISSAPELI_H_
