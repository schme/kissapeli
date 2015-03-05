#ifndef RENDER_H_
#define RENDER_H_

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "kms.h"
#include "ks_glutils.h"

#define REALLY_CONCAT(a,b) a##b
#define CONCAT(a,b) REALLY_CONCAT(a,b)
#define ASSETPATH  "assets/"
#define IMAGEPATH   ASSETPATH "images/"
#define IMAGEPATH_i(image)   ASSETPATH "images/" image
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
    real64 deltaTime;
    GameStatus state;
    uint32 p1lives;
    uint32 p2lives;
};

int initRender(void* vertexBuffer, real32 width, real32 height);
void resize(int w, int h);
void draw(uint64 frame, GameState gameState);

#endif // RENDER_H_
