#ifndef WIN_KISSAPELI_H_
#define WIN_KISSAPELI_H_

#include <windows.h>

#include "GL/glew.h"
#include "GL/wglew.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>

//TODO: global for now
static HDC DeviceContext;

#include "kissapeli.h"
#include "types.h"


const static int windowWidth = boardWidth;
const static int windowHeight = boardHeight;


// Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'


struct Win_WindowDimensions 
{
    int width;
    int height;
};


#endif // WIN_KISSAPELI_H_
