#ifndef RENDER_H_
#define RENDER_H_

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "types.h"
#include "kms.h"
#include "kms_glutils.h"

#define ASSETPATH  "assets/"
#define SHADERPATH(shader)  "shaders/" shader

const static int vertexBufferSize = 1024;
static void *vertexBuffer = NULL;

typedef glm::vec4 Color;
typedef float PlayerArray[16];
typedef float GameRectangle[8];

enum GameStatus {
    Playing, Paused, Resetting, Over
};

struct GameState {
    GameStatus state;
    uint32 p1lives;
    uint32 p2lives;
};

int initRender(void* vertexBuffer, real32 width, real32 height);
void resize(int w, int h);
void draw(uint64 frame, GameState state);

#endif // RENDER_H_
