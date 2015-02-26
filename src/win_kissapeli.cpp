
#include "win_kissapeli.h"
#include "win_audio.h"

#include <wingdi.h>

#if ENABLE_CONSOLE
#include <io.h>
#include <fcntl.h>
#endif

const uint64 memoryStackSize = 4*1024;
static AudioEngine audioEngine;

static HDC DeviceContext;
static HGLRC RenderingContext;
static WINDOWPLACEMENT windowPlacement = { sizeof( WINDOWPLACEMENT)};
static bool32 globalCursor = true;
static bool32 globalPlaying;
static int64 perfCountFrequency;
static uint64 frame;


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


void
Win_ToggleFullscreen( HWND hwnd)
{
    DWORD dwStyle = GetWindowLong( hwnd, GWL_STYLE);
    if( dwStyle & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = { sizeof(mi) };
        if( GetWindowPlacement( hwnd, &windowPlacement) &&
            GetMonitorInfo( MonitorFromWindow( hwnd,
                    MONITOR_DEFAULTTOPRIMARY), &mi)) 
            {
            SetWindowLong( hwnd, GWL_STYLE,
                    dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos( hwnd, HWND_TOP,
                   mi.rcMonitor.left, mi.rcMonitor.top,
                   mi.rcMonitor.right - mi.rcMonitor.left,
                   mi.rcMonitor.bottom - mi.rcMonitor.top,
                   SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
     } else {
       SetWindowLong(hwnd, GWL_STYLE,
                     dwStyle | WS_OVERLAPPEDWINDOW);
       SetWindowPlacement(hwnd, &windowPlacement);
       SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                    SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
     }
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
        0,                              /* main layer */
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

    int OpenGLVersion[2];
    glGetIntegerv( GL_MAJOR_VERSION, &OpenGLVersion[0]);
    glGetIntegerv( GL_MINOR_VERSION, &OpenGLVersion[1]);

    // OpenGL Extensions

#if VSYNC
    if (wglewIsSupported( "WGL_EXT_swap_control")) {
        wglSwapIntervalEXT(1);
    }
#endif

    return 1;
}



inline LARGE_INTEGER
Win_GetWallClock() {
    LARGE_INTEGER Result;
    QueryPerformanceCounter( &Result);
    return Result;
}


inline real32
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
            //gameRender();
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
        case WM_SETCURSOR: {
            if( globalCursor) {
                result = DefWindowProcA( Window, Message, wParam, lParam);
            } else {
                SetCursor(0);
            }
        } break;
        case WM_ACTIVATEAPP: {

            OutputDebugStringA( "WM_ACTIVATEAPP\n");
        } break;
        case WM_CLOSE: {
            
            OutputDebugStringA("WM_CLOSE\n");
            globalPlaying = false;
            wglMakeCurrent( NULL, NULL);
            wglDeleteContext( RenderingContext);
            ReleaseDC( Window, DeviceContext); 
        } break;
        case WM_DESTROY: {

            OutputDebugStringA("WM_DESTROY\n");
            globalPlaying = false;
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
                            //OutputDebugStringA("W \n");
                        } break;
                        case 'A': {
                            input->KEY_A = 1;
                            //OutputDebugStringA("A \n");
                        } break;
                        case 'S': {
                            input->KEY_S = 1;
                            //OutputDebugStringA("S \n");
                        } break;
                        case 'D': {
                            input->KEY_D = 1;
                            //OutputDebugStringA("D \n");
                        } break;
                        case 'Q': {

                            //OutputDebugStringA("Q \n");
                        } break;
                        case 'E': {

                            //OutputDebugStringA("E \n");
                        } break;
                        case VK_UP: {

                            input->KEY_UP = 1;
                            //OutputDebugStringA("UP \n");
                        } break;
                        case VK_LEFT: {

                            input->KEY_LEFT = 1;
                            //OutputDebugStringA("LEFT \n");
                        } break;
                        case VK_DOWN: {

                            input->KEY_DOWN = 1;
                            //OutputDebugStringA("DOWN \n");
                        } break;
                        case VK_RIGHT: {

                            input->KEY_RIGHT = 1;
                            //OutputDebugStringA("RIGHT \n");
                        } break;
                        case VK_SPACE: {
                            
                            OutputDebugStringA("SPACE \n");
                        } break;
                        case VK_ESCAPE: {

                            globalPlaying = false;
                        } break;
                    } // VKCode
                } //wasDown != isDown

                if( isDown) {
                    bool32 AltKeyWasDown = (Message.lParam & (1 << 29));
                    if(( VKCode == VK_F4) && AltKeyWasDown) {
                        globalPlaying = false;
                    }
                    if(( VKCode == VK_RETURN) && AltKeyWasDown) {
                        if( Message.hwnd) {
                            Win_ToggleFullscreen( Message.hwnd);
                        }
                    }
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

#if ENABLE_CONSOLE
    if(!AllocConsole()) {
        assert(!"AllocConsole() failed");
    }

    int32 winStdOutFileDesc = _open_osfhandle((intptr_t)GetStdHandle( STD_OUTPUT_HANDLE), _O_TEXT);
    FILE *fpout = _fdopen( winStdOutFileDesc, "w");
    *stdout = *fpout;
    setvbuf( stdout, NULL, _IONBF, 0);

#endif

    // Timing
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency( &PerfCountFrequencyResult);
    perfCountFrequency = PerfCountFrequencyResult.QuadPart;

    bool32 sleepIsGranular = (timeBeginPeriod( 1) == TIMERR_NOERROR);
    
#define GameUpdateHz 60
    real32 targetSecondsPerFrame = 1.0f / (real32)GameUpdateHz; 

    // WindowClass
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win_WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor(0, IDC_CROSS);
    //WindowClass.hIcon;
    WindowClass.lpszClassName = "KissapeliWindowClass";


    const char *audioTestFile = "assets/audio/song.wav";

    /* Has to be called before AudioEngine init */
    CoInitializeEx( NULL, COINIT_MULTITHREADED);

    if(!audioEngine.init()) {
        OutputDebugStringA( "FAILED: AudioEngine.Init()\n");
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

    SetWindowText( Window, "Kissapeli NextGen");

    // Timing
    LARGE_INTEGER LastCounter = Win_GetWallClock();
    uint64 LastCycleCount = __rdtsc();
    real64 msPerFrame = 0.0;

    MemoryStack gameMemory = {};
    gameMemory.stackSize = memoryStackSize;
    gameMemory.memoryPool = VirtualAlloc( 0, gameMemory.stackSize,
                            MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    gameMemory.top = gameMemory.memoryPool;
    gameMemory.isInitialized = true;
    // TODO: Not like this you idiot.
    assert( gameMemory.memoryPool && " MemoryPool allocation failed!\n");

    gameInit( &gameMemory, &audioEngine);

    /**
     * MainLoop
     */
    while( globalPlaying) {

        GameInput input = {};
        Win_HandleMessages(&input);
        input.frame = frame;
        input.deltaTime = msPerFrame;

        gameUpdate(input);

        // Timing
        LARGE_INTEGER workCounter = Win_GetWallClock();
        real32 workSecondElapsed = Win_GetSecondsElapsed( LastCounter, workCounter);
        real32 secondsElapsedForFrame = workSecondElapsed;


        if( secondsElapsedForFrame < targetSecondsPerFrame )
        {
            if( sleepIsGranular)
            {
                DWORD SleepMS = (DWORD)(1000.0f * (targetSecondsPerFrame -
                                                    secondsElapsedForFrame));
#if !VSYNC
                if( SleepMS > 0) {
                    Sleep(SleepMS);
                }
#endif
            }

            real32 testSecondsElapsedForFrame = Win_GetSecondsElapsed( LastCounter, Win_GetWallClock());
        } else {
            OutputDebugStringA( "MISSED FRAME\n"); 
        }

        
        LARGE_INTEGER EndCounter = Win_GetWallClock();
        msPerFrame = 1000.0f*Win_GetSecondsElapsed( LastCounter, EndCounter);
        LastCounter = EndCounter;

        SwapBuffers( DeviceContext);

        uint64 endCycleCount = __rdtsc();
        uint64 cyclesElapsed = endCycleCount - LastCycleCount;
        LastCycleCount = endCycleCount;

        int64 counterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;

        ++frame;

#if BUILD_INTERNAL
        real64 FPS = (double)perfCountFrequency / (double)counterElapsed;
        real64 MCPF = ((double)cyclesElapsed / (1000.0f * 1000.0f));

        char timeStrBuffer[256];

        _snprintf_s( timeStrBuffer, sizeof( timeStrBuffer), "%.02fms/f, %.02fmc/f, frame %llu \n", msPerFrame, MCPF, frame);

        OutputDebugStringA( timeStrBuffer);
        if( frame % 20 == 0) {
            SetWindowText( Window, timeStrBuffer);
        }
#endif
#if LOGLEVEL > 1 && ENABLE_CONSOLE
        printf( "frame: %llu\n", frame);
#endif

        
    }

    return 0;
}
