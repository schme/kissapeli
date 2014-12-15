#ifndef KISSAPELI_H_
#define KISSAPELI_H_
#include <math.h>

#include "types.h"
#include "render.h"
#include "memorystack.h"

const static float padPadding = 32;
const static float padInitWidth = 32;
const static float padInitHeight = 96; 

static int boardWidth = 1000;
static int boardHeight = 600;
static float ballRadius = 32;
static float padVelocityMod = 10;


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
};

void gameRender();
void gameUpdate( GameInput);
void resizeCallback( int w, int h);
void gameInit( MemoryStack *);

#endif // KISSAPELI_H_
