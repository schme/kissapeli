
#include "win_kissapeli.h"

#include <wingdi.h>
#include <xaudio2.h>

const uint64 memoryStackSize = 1024 * 1024;


//TODO: global for now
static HDC DeviceContext;
static HGLRC RenderingContext;
static bool32 globalPlaying;
static int64 perfCountFrequency;


HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset);


void
Win_FreeMemory( void *Memory) {
    if( Memory) {
        VirtualFree( Memory, 0, MEM_RELEASE);
    }
}


Win_WindowDimensions
Win_GetWindowDimensions( HWND Window) {
    Win_WindowDimensions result;

    RECT ClientRect;
    GetClientRect( Window, &ClientRect);
    result.width = ClientRect.right - ClientRect.left;
    result.height = ClientRect.bottom - ClientRect.top;

    return result;
}


int
Win_CreateGLContext()
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                              /* version */
        PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,                             /* colordepth of the framebuffer */
        0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
        0,                              /* no alpha buffer */
        0,                              /* alpha bits (ignored) */
        0,                              /* no accumulation buffer */
        0, 0, 0, 0,                     /* accum bits (ignored) */
        24,                             /* depth buffer */
        8,                              /* no stencil buffer */
        0,                              /* no auxiliary buffers */
        PFD_MAIN_PLANE,                 /* main layer */
        0,                              /* reserved */
        0, 0, 0,                        /* no layer, visible, damage masks */
    };

    int pixelFormat = ChoosePixelFormat(DeviceContext, &pfd);
    if( !SetPixelFormat(DeviceContext, pixelFormat, &pfd)) {
        OutputDebugStringA( "FAILED: Setting pixelformat\n");
        return 0;
    }

    HGLRC tempContext = wglCreateContext( DeviceContext);
    wglMakeCurrent( DeviceContext, tempContext);

    if( glewInit() != GLEW_OK) {
        OutputDebugStringA( "FAILED: glewInit()\n");
        return 0;
    }

    int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 1,
            WGL_CONTEXT_FLAGS_ARB, 0,
            0  
    };

    if( wglewIsSupported( "WGL_ARB_create_context") == 1)
    {
        RenderingContext = wglCreateContextAttribsARB( DeviceContext, 0, attribs);
        wglMakeCurrent( NULL, NULL);
        wglDeleteContext( tempContext);
        wglMakeCurrent( DeviceContext, RenderingContext);
        OutputDebugStringA( "SUCCESS: GL 3.x context received\n");

    } else {
        RenderingContext = tempContext;
        OutputDebugStringA( "FAILED: GL 3.x context. Using GL 2.1 or earlier\n");
    }

    if( !RenderingContext) {
        OutputDebugStringA( "FAILED: Getting RenderingContext\n");
        return 0;
    }

    /*
    int OpenGLVersion[2];
    glGetIntegerv( GL_MAJOR_VERSION, &OpenGLVersion[0]);
    glGetIntegerv( GL_MINOR_VERSION, &OpenGLVersion[1]);
    */

    // OpenGL Extensions

    if (wglewIsSupported( "WGL_EXT_swap_control")) {
        wglSwapIntervalEXT(-1);
    }  else {
        wglSwapIntervalEXT(1);
    }

    return 1;
}



inline LARGE_INTEGER
Win_GetWallClock() {
    LARGE_INTEGER Result;
    QueryPerformanceCounter( &Result);
    return Result;
}


inline float
Win_GetSecondsElapsed( LARGE_INTEGER Start, LARGE_INTEGER End)
{
    float Result = ((float)(End.QuadPart - Start.QuadPart) /
                    (float)perfCountFrequency);
    return Result;
}



LRESULT CALLBACK 
Win_WindowProc(  HWND Window,
                  UINT Message,
                  WPARAM wParam,
                  LPARAM lParam )
{
    LRESULT result = 0;
    switch( Message) {

        case WM_PAINT: {
            PAINTSTRUCT paint;
            BeginPaint( Window, &paint);
            gameRender();
            EndPaint( Window, &paint);
        } break;
        case WM_SIZE: {
            Win_WindowDimensions dimensions = Win_GetWindowDimensions( Window);
            resizeCallback( dimensions.width, dimensions.height);
        } break;
        case WM_CREATE: {

            DeviceContext = GetDC(Window);
            if(Win_CreateGLContext()) {
                globalPlaying = true;
            }
        } break;
        case WM_ACTIVATEAPP: {

            OutputDebugStringA( "WM_ACTIVATEAPP\n");
        } break;
        case WM_CLOSE: {
            
            OutputDebugStringA("\nWM_CLOSE\n");
            globalPlaying = false;
            wglMakeCurrent( NULL, NULL);
            wglDeleteContext( RenderingContext);
            ReleaseDC( Window, DeviceContext); 
        } break;
        case WM_DESTROY: {

            OutputDebugStringA("\nWM_DESTROY\n");
            globalPlaying = false;
        } break;
        case WM_SYSKEYDOWN: 
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            assert (!"Got WM_Key message outside the mainloop\n");
        } break;

    default:
        result =  DefWindowProc(Window, Message, wParam, lParam);
    }
    return result;
}


void
Win_HandleMessages(GameInput *input) {

    MSG Message;
    while( PeekMessage( &Message, 0,0,0, PM_REMOVE)) {

        switch( Message.message ) {

            case WM_QUIT: {
                globalPlaying = false;
            } break;
                          
            case WM_SYSKEYUP:
            case WM_KEYUP:
            case WM_SYSKEYDOWN: 
            case WM_KEYDOWN: {

                uint32 VKCode = (uint32)Message.wParam;
                bool32 wasDown = ((Message.lParam & (1 << 30)) != 0);
                bool32 isDown = ((Message.lParam & (1 << 31)) == 0);
                if( wasDown != isDown) {

                    switch (VKCode) { 
                        case 'W': {
                            input->KEY_W = 1;
                            OutputDebugStringA("W ");
                        } break;
                        case 'A': {
                            input->KEY_A = 1;
                            OutputDebugStringA("A ");
                        } break;
                        case 'S': {
                            input->KEY_S = 1;
                            OutputDebugStringA("S ");
                        } break;
                        case 'D': {
                            input->KEY_D = 1;
                            OutputDebugStringA("D ");
                        } break;
                        case 'Q': {

                            OutputDebugStringA("Q ");
                        } break;
                        case 'E': {

                            OutputDebugStringA("E ");
                        } break;
                        case VK_UP: {

                            input->KEY_UP = 1;
                            OutputDebugStringA("UP ");
                        } break;
                        case VK_LEFT: {

                            input->KEY_LEFT = 1;
                            OutputDebugStringA("LEFT ");
                        } break;
                        case VK_DOWN: {

                            input->KEY_DOWN = 1;
                            OutputDebugStringA("DOWN ");
                        } break;
                        case VK_RIGHT: {

                            input->KEY_RIGHT = 1;
                            OutputDebugStringA("RIGHT ");
                        } break;
                        case VK_SPACE: {
                            
                            OutputDebugStringA("SPACE ");
                        } break;
                        case VK_ESCAPE: {

                            globalPlaying = false;
                        } break;
                    } // VKCode
                } //wasDown != isDown

                bool32 AltKeyWasDown = (Message.lParam & (1 << 29));
                if(( VKCode == VK_F4) && AltKeyWasDown) {
                    globalPlaying = false;
                }

            } break;
    
        default: {
            TranslateMessage( &Message);
            DispatchMessageA( &Message);
        } break;

        } // switch (Message)
    } // while( PeekMessage())
}


int 
CALLBACK WinMain(   HINSTANCE Instance, 
                    HINSTANCE PrevInstance,
                    LPSTR CmdLine, 
                    int ShowCommand )
{

    // Timing
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency( &PerfCountFrequencyResult);
    perfCountFrequency = PerfCountFrequencyResult.QuadPart;

    bool32 sleepIsGranular = (timeBeginPeriod( 1) == TIMERR_NOERROR);
    
#define MonitorRefreshRate 60
#define GameUpdateHz (MonitorRefreshRate / 2)
    float targetSecondsPerFrame = 1.0f / (float)GameUpdateHz; 

    // WindowClass
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win_WindowProc;
    WindowClass.hInstance = Instance;
    //WindowClass.hIcon;
    WindowClass.lpszClassName = "KissapeliWindowClass";

    /* XAudio2 */
    WAVEFORMATEXTENSIBLE wfx = {};
    XAUDIO2_BUFFER buffer = {};

    CoInitializeEx( NULL, COINIT_MULTITHREADED);

    IXAudio2 *XAudioInstance = NULL;
    if( FAILED( XAudio2Create( &XAudioInstance, 0 ))) {
        OutputDebugStringA( "FAILED: Creating XAudio2 instance\n");
        return 0;
    }
    IXAudio2MasteringVoice *AudioMasterVoice = NULL;
    if( FAILED( XAudioInstance->CreateMasteringVoice( &AudioMasterVoice))) {
        OutputDebugStringA( "FAILED: CreateMasteringVoice\n");
        return 0;
    }

    /* Open the audio file */
    HANDLE hFile = CreateFile(
        "assets\\audio\\song.wav",
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL );

    if( INVALID_HANDLE_VALUE == hFile ) {
        return HRESULT_FROM_WIN32( GetLastError());
    }
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) ) {
        return HRESULT_FROM_WIN32( GetLastError());
    }
    DWORD dwChunkSize;
    DWORD dwChunkPosition;

    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk( hFile,fourccRIFF,dwChunkSize, dwChunkPosition );
    DWORD filetype;
    ReadChunkData( hFile,&filetype,sizeof(DWORD),dwChunkPosition);
    if (filetype != fourccWAVE) {
        OutputDebugStringA( "FAILED: Audio file not recognized as WAVE\n");
        return S_FALSE;
    }

    FindChunk( hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData( hFile, &wfx, dwChunkSize, dwChunkPosition);

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition );
    BYTE *pDataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

    buffer.AudioBytes = dwChunkSize;        //size of the audio buffer in bytes
    buffer.pAudioData = pDataBuffer;        //buffer containing audio data
    buffer.LoopBegin = 0;
    buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    buffer.Flags = XAUDIO2_END_OF_STREAM;   // tell the source voice not to expect any data after this buffer


    
    IXAudio2SourceVoice *AudioSourceVoice = NULL;
    if( FAILED( XAudioInstance->CreateSourceVoice( &AudioSourceVoice, (WAVEFORMATEX*)&wfx))) {
        OutputDebugStringA( "FAILED: CreateSourceVoice\n");
        return 0;
    }
    if( FAILED( AudioSourceVoice->SubmitSourceBuffer( &buffer))) {
        OutputDebugStringA( "FAILED: SubmitSourceBuffer\n");
        return 0;
    }

    if( !RegisterClassA( &WindowClass)) {
        OutputDebugStringA( "FAILED: RegisterWindowClass\n");
        return 0;
    }

    HWND Window = 
            CreateWindowExA( 
                    0,
                    WindowClass.lpszClassName,
                    "Kissapeli NextGen",
                    WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT,
                    windowWidth, windowHeight,
                    NULL,   // PrevInstance
                    NULL,
                    Instance,
                    NULL );
    if( !Window) {
        OutputDebugStringA( "FAILED: Create Window\n");
        return 0;
    }

    /*// play awesome song
    if( FAILED( AudioSourceVoice->Start(0))) {
        OutputDebugStringA( "FAILED: AudioSourceVoice->Start\n");
        return 0;
    }*/


    // Timing
    LARGE_INTEGER LastCounter = Win_GetWallClock();
    uint64 LastCycleCount = __rdtsc();


    MemoryStack gameMemory = {};
    gameMemory.stackSize = memoryStackSize;
    gameMemory.memoryPool = VirtualAlloc( 0, gameMemory.stackSize,
                            MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    gameMemory.top = gameMemory.memoryPool;
    gameMemory.isInitialized = true;
    // TODO: Not like this you idiot.
    assert( gameMemory.memoryPool && " MemoryPool allocation failed!\n");

    gameInit( &gameMemory);

    /**
     * MainLoop
     */
    while( globalPlaying) {

        GameInput input = {};
        Win_HandleMessages(&input);
        gameUpdate(input);

        // Timing
        


        LARGE_INTEGER workCounter = Win_GetWallClock();
        float workSecondElapsed = Win_GetSecondsElapsed( LastCounter, workCounter);
        float secondsElapsedForFrame = workSecondElapsed;


        if( secondsElapsedForFrame < targetSecondsPerFrame )
        {
            if( sleepIsGranular)
            {
                DWORD SleepMS = (DWORD)(1000.0f * (targetSecondsPerFrame -
                                                    secondsElapsedForFrame));
                if( SleepMS > 0) {
                    Sleep(SleepMS);
                }
            }

            float testSecondsElapsedForFrame = Win_GetSecondsElapsed( LastCounter, Win_GetWallClock());
            if( testSecondsElapsedForFrame < targetSecondsPerFrame) {
                //OutputDebugStringA( "Missed sleep\n");
            }
        } else {
            // Logging
            OutputDebugStringA( "MISSED FRAME\n"); 
        }

        
        LARGE_INTEGER EndCounter = Win_GetWallClock();
        double MSPerFrame = 1000.0f*Win_GetSecondsElapsed( LastCounter, EndCounter);
        LastCounter = EndCounter;

        SwapBuffers( DeviceContext);


        uint64 endCycleCount = __rdtsc();
        uint64 cyclesElapsed = endCycleCount - LastCycleCount;
        LastCycleCount = endCycleCount;

        int64 counterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;


        double FPS = (double)perfCountFrequency / (double)counterElapsed;
        double MCPF = ((double)cyclesElapsed / (1000.0f * 1000.0f));

        char timeStrBuffer[256];
        _snprintf_s( timeStrBuffer, sizeof( timeStrBuffer), "%.02fms/f, %0.2ff/s, %.02fmc/f\n", MSPerFrame, FPS, MCPF);
        OutputDebugStringA( timeStrBuffer);

        
    }

    // Cleanup

    return 0;
}



HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
                hr = HRESULT_FROM_WIN32( GetLastError() );
            break;

        default:
            if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, NULL, FILE_CURRENT ) )
            return HRESULT_FROM_WIN32( GetLastError() );            
        }

        dwOffset += sizeof(DWORD) * 2;
        
        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }
        dwOffset += dwChunkDataSize;
        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }
    return S_OK;
    
}


HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferoffset, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );
    DWORD dwRead;
    if( 0 == ReadFile( hFile, buffer, buffersize, &dwRead, NULL ) )
        hr = HRESULT_FROM_WIN32( GetLastError() );
    return hr;
}

