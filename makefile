CC=cl
CFLAGS=-nologo -FC -Zi -EHsc -Oi -Od -WX -W4 -wd4201 -wd4189 -wd4100
LDFLAGS=
SRCF= src
SOURCES= $(SRCF)\win_kissapeli.cpp $(SRCF)\kissapeli.cpp $(SRCF)\render.cpp $(SRCF)\shader.cpp $(SRCF)\memorystack.cpp
DEFINES= -DGLEW_STATIC
INCLUDES= /I C:\dxsdk\Include
LIBS=user32.lib gdi32.lib opengl32.lib glu32.lib glew32s.lib ole32.lib

windows: 
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(DEFINES) $(SOURCES) $(LIBS) -Fekissapeli
