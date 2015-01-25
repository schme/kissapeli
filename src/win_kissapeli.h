#ifndef WIN_KISSAPELI_H_
#define WIN_KISSAPELI_H_

#include <windows.h>

#include "GL/glew.h"
#include "GL/wglew.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>


#include "kissapeli.h"
#include "types.h"


const static int32 windowWidth = (int32)boardWidth;
const static int32 windowHeight = (int32)boardHeight;


struct Win_WindowDimensions 
{
    int32 width;
    int32 height;
};


#endif // WIN_KISSAPELI_H_
