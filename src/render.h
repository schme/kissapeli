#ifndef RENDER_H_
#define RENDER_H_

#include <vector>

#include "GL/glew.h"
#include "GL/wglew.h"

#include "shader.h"
#include "kms.h"


typedef float Color[3];

void gl_Draw(Color );
void initRender( HDC);


#endif // RENDER_H_
