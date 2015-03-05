CC=cl
CFLAGS=-nologo -FC -Zi -EHsc -Oi -Od -WX -W4 -wd4201 -wd4189 -wd4100 -wd4127
LDFLAGS=
SRCF= src
SOURCES= $(SRCF)\win_kissapeli.cpp $(SRCF)\kissapeli.cpp $(SRCF)\render.cpp $(SRCF)\memorystack.cpp $(SRCF)\ks_glutils.cpp $(SRCF)\win_audio.cpp
DEFINES= -DGLEW_STATIC -DBUILD_INTERNAL -DENABLE_CONSOLE -DLOGLEVEL=1 -DVSYNC=0
INCLUDES= /I C:\dxsdk\Include
LIBS=user32.lib gdi32.lib opengl32.lib glu32.lib glew32s.lib ole32.lib winmm.lib

windows:
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(DEFINES) $(SOURCES) $(LIBS) -Fekissapeli

clean:
	del *.obj *.exe
