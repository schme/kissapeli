#ifndef RENDER_H_
#define RENDER_H_

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "kms.h"
#include "kms_glutils.h"

const static int vertexBufferSize = 1024;
static void *vertexBuffer = NULL;

typedef glm::vec4 Color;
typedef float PlayerArray[16];
typedef float GameRectangle[8];

int initRender(void* vertexBuffer, int width, int height);
void resize(int w, int h);
void draw();


#endif // RENDER_H_
