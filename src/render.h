#ifndef RENDER_H_
#define RENDER_H_

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "kms.h"
#include "kms_glutils.h"

const static int vertexBufferSize = 1024;
static void *vertexBuffer = NULL;

typedef glm::vec4 Color;
typedef float PlayerColor[16];
typedef float PlayerArray[16];

int initRender(void* vertexBuffer, HDC DC, int width, int height);
void resize(int w, int h);
void draw(Color clearColor);


#endif // RENDER_H_
