#ifndef KISSAPELI_H_
#define KISSAPELI_H_
#include <math.h>

#include "types.h"
#include "render.h"
#include "memorystack.h"


const static int boardWidth = 1280;
const static int boardHeight = 800;
const static float padPadding = 200;
const static float padInitWidth = 80;
const static float padInitHeight = 160; 

typedef glm::vec2 Square[4];
typedef glm::vec4 Color;
typedef Square PadVertices;


struct Pad {
    glm::vec4 color;
    glm::vec2 position;
    glm::vec2 dimensions;
};


struct Ball {
    glm::vec4 color;
    glm::vec2 position;
    glm::vec2 velocity;
    uint32 radius;
};


struct World {
    glm::vec4 color;
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
};

void gameRender();
void gameUpdate( GameInput);
void resizeCallback( int w, int h);
void gameInit( MemoryStack *, HDC);

#endif // KISSAPELI_H_
