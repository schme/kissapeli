#ifndef KISSAPELI_H_
#define KISSAPELI_H_

#include <math.h>

#include <glm/vec2.hpp>

#include "types.h"
#include "render.h"
#include "memorystack.h"


const static uint32 boardWidth = 1280;
const static uint32 boardHeight = 800;
const static uint32 padPadding = 200;
const static uint32 padInitWidth = 80;
const static uint32 padInitHeight = 160; 


struct Pad {
    glm::vec2 position;
    uint32 width;
    uint32 height;
};


struct Ball {
    glm::vec2 position;
    glm::vec2 velocity;
    uint32 radius;
};


struct Board {
    uint32 width;
    uint32 height;
};


struct Game {
    Pad player1;
    Pad player2;
    Ball ball;
    Board board;
};


void gameRender( HDC);
void gameUpdate(MemoryStack*, HDC);

#endif // KISSAPELI_H_
