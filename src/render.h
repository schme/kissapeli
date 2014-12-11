#ifndef RENDER_H_
#define RENDER_H_


#include "kms.h"
#include "kms_glutils.h"

typedef float Color[3];

int initRender( HDC);
void resize(int w, int h);
void draw(Color );


#endif // RENDER_H_
