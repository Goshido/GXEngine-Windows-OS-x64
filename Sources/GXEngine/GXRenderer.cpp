// version 1.17

#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXEngineSettings.h>
#include <GXEngine/GXInput.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXPhysXFrontend.h>
#include <GXEngine/GXResource.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXTime.h>


#define WINDOW_OPENGL_CLASS     L"GX_RENDERER_CLASS"
#define DEFAULT_WINDOW_NAME     L"GXEngine"

#define DEFAULT_COLOR_BITS      32
#define DEFAULT_DEPTH_BITS      24
#define DEFAULT_STENCIL_BITS    8
#define CLEAR_DEPTH_VALUE       1.0f

//--------------------------------------------------------------------------------------------

GXRendererResolutions::GXRendererResolutions ():
    total ( 0u ),
    wxh ( nullptr )
{
    // NOTHING
}

GXRendererResolutions::~GXRendererResolutions ()
{
    Cleanup ();
}

GXVoid GXRendererResolutions::Cleanup ()
{
    GXSafeFree ( wxh );
}

GXUShort GXRendererResolutions::GetTotalResolutions ()
{
    return total;
}

GXVoid GXRendererResolutions::GetResolution ( GXUShort i, GXUShort &width, GXUShort &height )
{
    width = wxh[ i << 1 ];
    height = wxh[ ( i << 1 ) + 1 ];
}

//--------------------------------------------------------------------------------------------

GXGame*         GXRenderer::game = nullptr;

GXThread*       GXRenderer::thread = nullptr;
GXBool          GXRenderer::loopFlag = GX_TRUE;

HWND            GXRenderer::hwnd = (HWND)INVALID_HANDLE_VALUE;
HINSTANCE       GXRenderer::hinst = (HINSTANCE)INVALID_HANDLE_VALUE;
HGLRC           GXRenderer::hglRC = (HGLRC)INVALID_HANDLE_VALUE;
HDC             GXRenderer::hDC = (HDC)INVALID_HANDLE_VALUE;

GXBool          GXRenderer::isFullScreen;
GXBool          GXRenderer::isSettingsChanged;

GXInt           GXRenderer::width;
GXInt           GXRenderer::height;
GXInt           GXRenderer::vsync;

GXWChar*        GXRenderer::title = nullptr;

GXUInt          GXRenderer::currentFPS = 0u;
GXDouble        GXRenderer::accumulator = 0.0;
GXDouble        GXRenderer::lastTime = 0.0;
GXUShort        GXRenderer::fpsCounter = 0u;
GXDouble        GXRenderer::fpsTimer = 0.0;

GXBool          GXRenderer::isRenderableObjectInited = GX_FALSE;

GXRenderer*     GXRenderer::instance = nullptr;


GXRenderer& GXCALL GXRenderer::GetInstance ()
{
    if ( !instance )
        instance = new GXRenderer ();

    return *instance;
}

GXRenderer::~GXRenderer ()
{
    if ( isFullScreen )
    {
        if ( ChangeDisplaySettingsW ( nullptr, 0u ) != DISP_CHANGE_SUCCESSFUL )
        {
            GXWarningBox ( L"Не удалось вернуть графические настройки по умолчанию" );
            GXLogW ( L"GXRenderer::~GXRenderer::Error - Не удалось вернуть графические настройки по умолчанию\n" );
        }
    }

    GXSafeFree ( title );
    delete thread;

    instance = nullptr;
}

GXVoid GXRenderer::Start ( GXGame &gameObject )
{
    game = &gameObject;
    thread->Start ();
}

GXBool GXRenderer::Shutdown ()
{
    loopFlag = GX_FALSE;
    thread->Join ();

    return GX_TRUE;
}

GXVoid GXRenderer::SetFullscreen ( GXBool enabled )
{
    if ( isFullScreen == enabled ) return;

    isFullScreen = enabled;

    LONG exStyle = 0;
    LONG style = 0;

    if ( isFullScreen )
    {
        DEVMODEW dm;
        memset ( &dm, 0, sizeof ( dm ) );
        dm.dmSize = sizeof ( dm );
        dm.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;

        dm.dmPelsWidth = static_cast<DWORD> ( width );
        dm.dmPelsHeight = static_cast<DWORD> ( height );

        if ( ChangeDisplaySettingsW ( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
            MessageBoxW ( 0, L"Не удалось поменять оконный режим", L"Проблемка", MB_ICONEXCLAMATION );

        exStyle = WS_EX_APPWINDOW;
        style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
    }
    else
    {
        ChangeDisplaySettingsW ( nullptr, 0 );

        exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
    }

    SetWindowLongW ( hwnd, GWL_EXSTYLE, exStyle );
    SetWindowLongW ( hwnd, GWL_STYLE, style );

    if ( isFullScreen )
    {
        SetWindowPos ( hwnd, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW );
        return;
    }

    RECT windowRect;
    RECT clientRect;

    GetWindowRect ( hwnd, &windowRect );
    GetClientRect ( hwnd, &clientRect );

    GXInt deltaHeight = height - ( clientRect.bottom - clientRect.top );
    GXInt deltaWidth = width - ( clientRect.right - clientRect.left );

    SetWindowPos ( hwnd, HWND_TOP, 0, 0, ( windowRect.right - windowRect.left ) + deltaWidth, ( windowRect.bottom  - windowRect.top ) + deltaHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW );
}

GXVoid GXRenderer::SetVSync ( GXBool enabled )
{
    GXInt t = enabled ? 1 : 0;

    if ( t == vsync ) return;

    vsync = t;
    wglSwapIntervalEXT ( vsync );
}

GXVoid GXRenderer::SetResolution ( GXInt frameWidth, GXInt frameHeight )
{
    if ( width == frameWidth && height == frameHeight ) return;

    width = frameWidth;
    height = frameHeight;

    ReSizeScene ( width, height );

    if ( isFullScreen )
    {
        DEVMODE dm;
        memset ( &dm, 0, sizeof ( dm ) );
        dm.dmSize = sizeof ( dm );
        dm.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;

        dm.dmPelsWidth = static_cast<DWORD> ( width );
        dm.dmPelsHeight = static_cast<DWORD> ( height );

        if ( ChangeDisplaySettings ( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
            MessageBoxW ( 0, L"Не удалось поменять оконный режим", L"Проблемка", MB_ICONEXCLAMATION );

        SetWindowPos ( hwnd, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW );

        return;
    }

    RECT windowRect;
    RECT clientRect;

    GetWindowRect ( hwnd, &windowRect );
    GetClientRect ( hwnd, &clientRect );

    GXInt deltaHeight = height - ( clientRect.bottom - clientRect.top );
    GXInt deltaWidth = width - ( clientRect.right - clientRect.left );

    SetWindowPos ( hwnd, HWND_TOP, 0, 0, ( windowRect.right - windowRect.left ) + deltaWidth, ( windowRect.bottom  - windowRect.top ) + deltaHeight, SWP_SHOWWINDOW );
}

GXVoid GXRenderer::SetWindowName ( const GXWChar* name )
{
    GXSafeFree ( title );
    GXWcsclone ( &title, name );

    if ( hwnd == static_cast<HWND> ( INVALID_HANDLE_VALUE ) ) return;

    SetWindowTextW ( hwnd, title );
}

GXVoid GXRenderer::Show () const
{
    if ( hwnd == static_cast<HWND> ( INVALID_HANDLE_VALUE ) ) return;

    ShowWindow ( hwnd, SW_SHOW );
}

GXVoid GXRenderer::Hide () const
{
    if ( hwnd == static_cast<HWND> ( INVALID_HANDLE_VALUE ) ) return;

    ShowWindow ( hwnd, SW_HIDE );
}

GXBool GXRenderer::IsVisible () const
{
    return IsWindowVisible ( hwnd ) == TRUE;
}

GXUInt GXRenderer::GetCurrentFPS () const
{
    return currentFPS;
}

GXVoid GXRenderer::GetSupportedResolutions ( GXRendererResolutions &out ) const
{
    DEVMODEW dm;
    dm.dmSize = sizeof ( dm );

    GXUShort rawTotal;
    for ( rawTotal = 0; EnumDisplaySettingsW ( 0, rawTotal, &dm ); ++rawTotal );

    GXRendererResolutions temp;
    temp.wxh = static_cast<GXUShort*> ( malloc ( ( rawTotal << 1 ) * sizeof ( GXUShort ) ) );

    DWORD lastWidth = 0;
    DWORD lastHeight = 0;

    out.total = 0u;

    for ( GXUShort i = 0u; i < rawTotal; ++i )
    {
        EnumDisplaySettingsW ( nullptr, i, &dm );

        GXBool hit = GX_FALSE;

        for ( GXUShort j = 0u; j < out.total; ++j )
        {
            if ( ( lastWidth == dm.dmPelsWidth && lastHeight == dm.dmPelsHeight ) || ( dm.dmPelsWidth == temp.wxh[ j << 1 ] && dm.dmPelsHeight == temp.wxh[ ( j << 1 ) + 1 ] ) )
            {
                hit = GX_TRUE;
                break;
            }
        }

        if ( hit ) continue;
        
        lastWidth = dm.dmPelsWidth;
        lastHeight = dm.dmPelsHeight;

        temp.wxh[ out.total << 1 ] = static_cast<GXUShort> ( lastWidth );
        temp.wxh[ ( out.total << 1 ) + 1 ] = static_cast<GXUShort> ( lastHeight );

        ++out.total;
    }

    GXUInt size = ( out.total << 1 ) * sizeof ( GXUShort );
    out.wxh = static_cast<GXUShort*> ( malloc ( size ) );
    memcpy ( out.wxh, temp.wxh, size );

    temp.Cleanup ();

    qsort ( out.wxh, out.total, 2 * sizeof ( GXUShort ), &GXRenderer::GXResolutionCompare );
}

GXInt GXRenderer::GetWidth () const
{
    return width;
}

GXInt GXRenderer::GetHeight () const
{
    return height;
}

GXBool GXCALL GXRenderer::UpdateRendererSettings ()
{
    isSettingsChanged = GX_TRUE;
    return GX_TRUE;
}

GXVoid GXCALL GXRenderer::ReSizeScene ( GXInt frameWidth, GXInt frameHeight )
{
    if ( frameHeight == 0 )
        frameHeight = 1;

    width = frameWidth;
    height = frameHeight;

    game->OnResize ( width, height );
}

GXInt GXRenderer::GXResolutionCompare ( const GXVoid* a, const GXVoid* b )
{
    const GXUShort* itemA = static_cast<const GXUShort*> ( a );
    const GXUShort* itemB = static_cast<const GXUShort*> ( b );

    GXUInt keyA = 10000u * itemA[ 0 ] + itemA[ 1 ];
    GXUInt keyB = 10000u * itemB[ 0 ] + itemB[ 1 ];

    if ( keyA < keyB )
        return -1;

    if ( keyA > keyB )
        return 1;

    return 0;
}

GXRenderer::GXRenderer ()
{
    isRenderableObjectInited = GX_FALSE;
    hinst = GetModuleHandle ( nullptr );

    isFullScreen = !gx_EngineSettings.windowed;

    width = gx_EngineSettings.rendererWidth;
    height = gx_EngineSettings.rendererHeight;

    vsync = ( gx_EngineSettings.vSync ) ? 1 : 0;

    currentFPS = 0u;
    lastTime = GXGetProcessorTicks ();
    accumulator = 0.0;
    fpsCounter = 0u;
    fpsTimer = 0.0;

    isSettingsChanged = GX_FALSE;
    loopFlag = GX_TRUE;
    thread = new GXThread ( &RenderLoop, nullptr );

    SetWindowName ( DEFAULT_WINDOW_NAME );
}

GXUPointer GXTHREADCALL GXRenderer::RenderLoop ( GXVoid* /*args*/, GXThread &threadObject )
{
    if ( !MakeWindow () ) return 0u;

    while ( loopFlag )
    {
        if ( !isRenderableObjectInited )
            InitRenderableObjects ();

        MSG msg;

        while ( PeekMessage ( &msg, 0, 0u, 0u, PM_REMOVE ) ) 
        {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );
        }

        if ( isSettingsChanged )
        {
            Destroy ();
            MakeWindow ();
            isSettingsChanged = GX_FALSE;
        }
        else
        {
            if ( isRenderableObjectInited )
            {
                DrawScene ();
                SwapBuffers ( hDC );
            }
        }
        
        threadObject.Switch();
    }

    Destroy ();
    return 0u;
}

GXVoid GXCALL GXRenderer::InitOpenGL ()
{
    glClearDepth ( CLEAR_DEPTH_VALUE );
    glEnable ( GL_DEPTH_TEST );
    glDepthFunc ( GL_LEQUAL );
    glEnable ( GL_CULL_FACE );
    glEnable ( GL_TEXTURE_CUBE_MAP_SEAMLESS );
    glCullFace ( GL_FRONT );

    GXLogW ( L"GXRenderer::InitOpenGL::Info - Аппаратные возможности:\n--------Текстурирование:\n" );

    GLint value[2];
    glGetIntegerv ( GL_MAX_TEXTURE_SIZE, value );
    GXLogW ( L"Максимальный размер 2D текстуры: %u (минимум стандарта - 1024)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_ARRAY_TEXTURE_LAYERS, value );
    GXLogW ( L"Максимальное количество слоёв для текстурных массивов: %u (минимум стандарта - 256)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_CUBE_MAP_TEXTURE_SIZE, value );
    GXLogW ( L"Максимальный размер cube map текстуры: %u (минимум стандарта - 1024)\n", value[ 0 ] );

    GXLogW ( L"\n--------Буфер кадра:\n" );

    glGetIntegerv ( GL_MAX_FRAMEBUFFER_WIDTH, value );
    glGetIntegerv ( GL_MAX_FRAMEBUFFER_HEIGHT, value + 1 );
    GXLogW ( L"Максимальное разрешение буфера кадра: %u х %u (минимум стандарта - 16384 х 16384)\n", value[ 0 ], value[ 1 ] );

    glGetIntegerv ( GL_MAX_DRAW_BUFFERS, value );
    GXLogW ( L"Максимальное количество выходных буферов, доступных фрагментному шейдеру: %u (минимум стандарта - 8)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_VIEWPORT_DIMS, value );
    GXLogW ( L"Максимальное разрешение viewport'а: %u x %u\n", value[ 0 ], value[ 1 ] );

    GXLogW ( L"\n--------Шейдерная программа:\n" );

    // There is not GL_MAX_VARYING_VECTORS in OpenGL 3.3 core profile standart. But GL_MAX_VARYING_COMPONENTS did not work at all (Error 0x500 - Invalid Enum)!
    // Tested on NVIDIA GeForce 770 GTX (Driver version 384.94)
    glGetIntegerv ( GL_MAX_VARYING_VECTORS, value );
    GXLogW ( L"Максимальное количество векторов-интерполяторов, доступных шейдерной программе: %u (минимум стандарта - 15)\n", value[ 0 ] );

    GXLogW ( L"\n--------Вершинный шейдер:\n" );

    glGetIntegerv ( GL_MAX_VERTEX_ATTRIBS, value );
    GXLogW ( L"Максимальное количество входных атрибутов-векторов, доступных вершинному шейдеру: %u (минимум стандарта - 16)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, value );
    GXLogW ( L"Максимальное количество текстур, доступных вершинному шейдеру: %u (минимум стандарта - 16)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_VERTEX_UNIFORM_VECTORS, value );
    GXLogW ( L"Максимальное количество юниформ-векторов, доступных вершинному шейдеру: %u (минимум стандарта - 256)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_VERTEX_UNIFORM_COMPONENTS, value );
    GXLogW ( L"Максимальное количество юниформ-компонент, доступных вершинному шейдеру: %u (минимум стандарта - 1024)\n", value[ 0 ] );

    GXLogW ( L"\n--------Геометрический шейдер:\n" );

    glGetIntegerv ( GL_MAX_GEOMETRY_INPUT_COMPONENTS, value );
    GXLogW ( L"Максимальное количество входных атрибутов-компонент, доступных геометрическому шейдеру: %u (минимум стандарта - 64)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, value );
    GXLogW ( L"Максимальное количество текстур, доступных геометрическому шейдеру: %u (минимум стандарта - 16)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, value );
    GXLogW ( L"Максимальное количество выходных атрибутов-компонент, доступных геометрическому шейдеру: %u (минимум стандарта - 128)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, value );
    GXLogW ( L"Максимальное количество юниформ-компонент, доступных геометрическому шейдеру: %u (минимум стандарта - 1024)\n", value[ 0 ] );

    GXLogW ( L"\n--------Фрагментный шейдер:\n" );

    glGetIntegerv ( GL_MAX_FRAGMENT_INPUT_COMPONENTS, value );
    GXLogW ( L"Максимальное количество входных атрибутов-компонент, доступных фрагментному шейдеру: %u (минимум стандарта - 128)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_TEXTURE_IMAGE_UNITS, value );
    GXLogW ( L"Максимальное количество текстур, доступных фрагментному шейдеру: %u (минимум стандарта - 16)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_FRAGMENT_UNIFORM_VECTORS, value );
    GXLogW ( L"Максимальное количество юниформ-векторов, доступных фрагментному шейдеру: %u (минимум стандарта - 256)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, value );
    GXLogW ( L"Максимальное количество юниформ-компонент, доступных фрагментному шейдеру: %u (минимум стандарта - 1024)\n", value[ 0 ] );

    GXCheckOpenGLError ();
}

GXVoid GXCALL GXRenderer::DrawScene ()
{
    GXGetPhysXInstance ()->DoSimulate ();

    GXDouble newtime = GXGetProcessorTicks ();
    GXDouble delta = ( newtime - lastTime ) / GXGetProcessorTicksPerSecond ();
    lastTime = newtime;
    
    accumulator += delta;
    fpsTimer += delta;

    if ( fpsTimer >= 1.0 )
    {
        currentFPS = fpsCounter;
        fpsTimer = 0.0;
        fpsCounter = 0u;
    }

    if ( accumulator <= 0.001 ) return;

    ++fpsCounter;

    GXFloat update = static_cast<GXFloat> ( accumulator );
    accumulator = 0.0;
    game->OnFrame ( update );
}

GXVoid GXCALL GXRenderer::Destroy ()
{
    DeleteRenderableObjects ();

    GXShaderProgram::DestroyPrecompiledShaderProgramSubsystem ();

    if ( isFullScreen )
    {
        ChangeDisplaySettingsW ( nullptr, 0 );
        while ( ShowCursor ( TRUE ) <= 0 );
    }

    if ( hglRC )
    {
        if ( !wglMakeCurrent ( 0, 0 ) )
        {
            GXWarningBox ( L"Освобождение контекстов устройства и редеринга провалено" );
            GXLogW ( L"GXRenderer::Destroy::Error - Освобождение контекстов устройства и редеринга провалено\n" );
        }

        if ( !wglDeleteContext ( hglRC ) )
        {
            GXWarningBox ( L"Удаление контекста редеринга провалено" );
            GXLogW ( L"GXRenderer::Destroy::Error - Удаление контекста редеринга провалено\n" );
        }

        hglRC = static_cast<HGLRC> ( INVALID_HANDLE_VALUE );
    }

    if ( hDC && !DeleteDC ( hDC ) )
    {
        GXWarningBox ( L"Освобождение контекста устройства провалено" );
        GXLogW ( L"GXRenderer::Destroy::Error - Освобождение контекста устройства провалено\n" );
        hDC = static_cast<HDC> ( INVALID_HANDLE_VALUE );
    }

    if ( hwnd && !DestroyWindow ( hwnd ) )
    {
        GXWarningBox ( L"Освобождение HWND провалено" );
        GXLogW ( L"GXRenderer::Destroy::Error - Освобождение HWND провалено\n" );
        hwnd = static_cast<HWND> ( INVALID_HANDLE_VALUE );
    }

    if ( UnregisterClassW ( WINDOW_OPENGL_CLASS, hinst ) ) return;

    GXWarningBox ( L"Снятие регистрации класса окна провалено" );
    GXLogW ( L"GXRenderer::Destroy::Error - Снятие регистрации класса окна провалено\n" );
    hinst = static_cast<HINSTANCE> ( INVALID_HANDLE_VALUE );
}

GXBool GXCALL GXRenderer::MakeWindow ()
{
    WNDCLASSW wc;
    DWORD dwExStyle;
    DWORD dwStyle;

    memset ( &wc, 0, sizeof ( wc ) );
    wc.hInstance = hinst;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    wc.lpszClassName = WINDOW_OPENGL_CLASS;
    wc.lpfnWndProc = &GXInput::InputProc;
    wc.hCursor = LoadCursor ( 0, IDC_ARROW );
    wc.hIcon = LoadIcon ( hinst, MAKEINTRESOURCE ( GX_RID_EXE_MAINICON ) );

    if ( !RegisterClassW ( &wc ) )
    {
        GXWarningBox ( L"Не удалось зарегистрировать класс окна" );
        GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось зарегистрировать класс окна\n" );
        return GX_FALSE;
    }

    DEVMODEW dm;
    memset ( &dm, 0, sizeof ( dm ) );
    dm.dmSize = sizeof ( dm );
    dm.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH | DM_BITSPERPEL;

    if ( isFullScreen )
    {
        dm.dmBitsPerPel = DEFAULT_COLOR_BITS;
        dm.dmPelsHeight = static_cast<DWORD> ( height );
        dm.dmPelsWidth = static_cast<DWORD> ( width );

        if ( ChangeDisplaySettingsW ( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
        {
            if ( MessageBoxW ( 0, L"Не удалось установить полноэкранный режим. Использовать обычный режим?", L"Проблемка", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES )
                isFullScreen = FALSE;
            else
                return GX_FALSE;
        }
    }

    if ( isFullScreen )
    {
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
    }
    else
    {
        if ( ChangeDisplaySettingsW ( nullptr, 0 ) != DISP_CHANGE_SUCCESSFUL )
        {
            GXWarningBox ( L"Не удалось вернуть графические настройки по умолчанию" );
            GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось вернуть графические настройки по умолчанию\n" );

            return GX_FALSE;
        }
        else
        {
            dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
            dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_CAPTION;
        }
    }

    hwnd = CreateWindowExW ( dwExStyle, WINDOW_OPENGL_CLASS, title, dwStyle, 0, 0, width, height, 0, 0, hinst, 0 );

    if ( !hwnd )
    {
        GXWarningBox ( L"При создании окна произошла ошибка" );
        GXLogW ( L"GXRenderer::MakeWindow::Error - При создании окна произошла ошибка\n" );
        Destroy ();
        return GX_FALSE;
    }

    RECT windowRect;
    RECT clientRect;

    GetWindowRect ( hwnd, &windowRect );
    GetClientRect ( hwnd, &clientRect );

    GXInt deltaHeight = height - ( clientRect.bottom - clientRect.top );
    GXInt deltaWidth = width - ( clientRect.right - clientRect.left );

    SetWindowPos ( hwnd, HWND_TOP, 0, 0, ( windowRect.right - windowRect.left ) + deltaWidth, ( windowRect.bottom  - windowRect.top ) + deltaHeight, SWP_HIDEWINDOW );

    static  PIXELFORMATDESCRIPTOR pfd;
    memset ( &pfd, 0, sizeof ( pfd ) );
    pfd.nSize = sizeof ( PIXELFORMATDESCRIPTOR );
    pfd.nVersion = 1;
    pfd.cColorBits = DEFAULT_COLOR_BITS;
    pfd.cDepthBits = DEFAULT_DEPTH_BITS;
    pfd.cStencilBits = DEFAULT_STENCIL_BITS;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |    PFD_DOUBLEBUFFER; 
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.iLayerType = PFD_MAIN_PLANE;

    hDC = GetDC ( hwnd );

    if ( !hDC )
    {
        GXWarningBox ( L"Не удалось получить hDC" );
        GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось получить hDC\n" );
        Destroy ();

        return GX_FALSE;
    }

    GXInt pixelFomat = ChoosePixelFormat ( hDC, &pfd );

    if ( pixelFomat == 0 )
    {
        GXWarningBox ( L"Не удалось найти подходящий формат пикселей" );
        GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось найти подходящий формат пикселей\n" );
        Destroy ();

        return GX_FALSE;
    }

    if ( !SetPixelFormat ( hDC, pixelFomat, &pfd ) )
    {
        GXWarningBox ( L"Не удалось установить формат пикселей" );
        GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось установить формат пикселей\n" );
        Destroy ();

        return GX_FALSE;
    }

    hglRC = wglCreateContext ( hDC );

    if ( hglRC == nullptr )
    {
        GXWarningBox ( L"Не удалось создать контекст рендеринга" );
        GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось создать контекст рендеринга\n" );
        Destroy ();

        return GX_FALSE;
    }

    if ( !wglMakeCurrent ( hDC, hglRC ) )
    {
        GXWarningBox ( L"Не удалось установить контекст рендеринга" );
        GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось установить контекст рендеринга\n" );
        Destroy ();

        return GX_FALSE;
    }

    GXOpenGLInit ();

    wglMakeCurrent ( nullptr, nullptr );
    wglDeleteContext ( hglRC );

    if ( !wglCreateContextAttribsARB )
    {
        GXWarningBox ( L"wglCreateContextAttribsARB провален" );
        GXLogW ( L"GXRenderer::MakeWindow::Error - wglCreateContextAttribsARB провален\n" );

        return GX_FALSE;
    }

    GXInt attribs [] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB,      3,
        WGL_CONTEXT_MINOR_VERSION_ARB,      3,
        WGL_CONTEXT_FLAGS_ARB,              WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,       WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    hglRC = wglCreateContextAttribsARB ( hDC, nullptr, attribs );

    if ( !hglRC || !wglMakeCurrent ( hDC, hglRC ) )
    {
        GXWarningBox ( L"Создание контекста OpenGL версии 3.3 провалено" );
        GXLogW ( L"GXRenderer::MakeWindow::Error - Создание контекста OpenGL версии 3.3 провалено\n" );

        return GX_FALSE;
    }

    GXShaderProgram::InitPrecompiledShaderProgramSubsystem ();

    SetForegroundWindow ( hwnd );
    SetFocus ( hwnd );
    ReSizeScene ( width, height );
    InitOpenGL ();

    wglSwapIntervalEXT ( vsync );

    gx_ui_Scale = GetDeviceCaps ( hDC, LOGPIXELSX ) / 2.54f;
    InitRenderableObjects ();

    DrawScene ();

    ShowCursor ( 0 );

    return GX_TRUE;
}

GXVoid GXCALL GXRenderer::InitRenderableObjects ()
{
    game->OnInit ();
    isRenderableObjectInited = GX_TRUE;
}

GXVoid GXCALL GXRenderer::DeleteRenderableObjects ()
{
    game->OnDestroy ();
    isRenderableObjectInited = GX_FALSE;
}
