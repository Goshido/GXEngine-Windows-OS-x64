// version 1.18

#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXDesktopInput.h>
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
    _total ( 0u ),
    _wxh ( nullptr )
{
    // NOTHING
}

GXRendererResolutions::~GXRendererResolutions ()
{
    Cleanup ();
}

GXVoid GXRendererResolutions::Cleanup ()
{
    GXSafeFree ( _wxh );
}

GXUShort GXRendererResolutions::GetTotalResolutions ()
{
    return _total;
}

GXVoid GXRendererResolutions::GetResolution ( GXUShort i, GXUShort &width, GXUShort &height )
{
    width = _wxh[ i << 1 ];
    height = _wxh[ ( i << 1 ) + 1 ];
}

//--------------------------------------------------------------------------------------------

GXGame*         GXRenderer::_game = nullptr;

GXThread*       GXRenderer::_thread = nullptr;
GXBool          GXRenderer::_loopFlag = GX_TRUE;

HWND            GXRenderer::_hwnd = (HWND)INVALID_HANDLE_VALUE;
HINSTANCE       GXRenderer::_hinst = (HINSTANCE)INVALID_HANDLE_VALUE;
HGLRC           GXRenderer::_hglRC = (HGLRC)INVALID_HANDLE_VALUE;
HDC             GXRenderer::_hDC = (HDC)INVALID_HANDLE_VALUE;

GXBool          GXRenderer::_isFullScreen;
GXBool          GXRenderer::_isSettingsChanged;

GXInt           GXRenderer::_width;
GXInt           GXRenderer::_height;
GXInt           GXRenderer::_vsync;

GXWChar*        GXRenderer::_title = nullptr;

GXUInt          GXRenderer::_currentFPS = 0u;
GXDouble        GXRenderer::_accumulator = 0.0;
GXDouble        GXRenderer::_lastTime = 0.0;
GXUShort        GXRenderer::_fpsCounter = 0u;
GXDouble        GXRenderer::_fpsTimer = 0.0;

GXBool          GXRenderer::_isRenderableObjectInited = GX_FALSE;

GXRenderer*     GXRenderer::_instance = nullptr;


GXRenderer& GXCALL GXRenderer::GetInstance ()
{
    if ( !_instance )
        _instance = new GXRenderer ();

    return *_instance;
}

GXRenderer::~GXRenderer ()
{
    if ( _isFullScreen )
    {
        if ( ChangeDisplaySettingsW ( nullptr, 0u ) != DISP_CHANGE_SUCCESSFUL )
        {
            GXWarningBox ( "Не удалось вернуть графические настройки по умолчанию" );
            GXLogA ( "GXRenderer::~GXRenderer::Error - Не удалось вернуть графические настройки по умолчанию\n" );
        }
    }

    GXSafeFree ( _title );
    delete _thread;

    _instance = nullptr;
}

GXVoid GXRenderer::Start ( GXGame &gameObject )
{
    _game = &gameObject;
    _thread->Start ();
}

GXBool GXRenderer::Shutdown ()
{
    _loopFlag = GX_FALSE;
    _thread->Join ();

    return GX_TRUE;
}

GXVoid GXRenderer::SetFullscreen ( GXBool enabled )
{
    if ( _isFullScreen == enabled ) return;

    _isFullScreen = enabled;

    LONG exStyle = 0;
    LONG style = 0;

    if ( _isFullScreen )
    {
        DEVMODEW dm;
        memset ( &dm, 0, sizeof ( dm ) );
        dm.dmSize = sizeof ( dm );
        dm.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;

        dm.dmPelsWidth = static_cast<DWORD> ( _width );
        dm.dmPelsHeight = static_cast<DWORD> ( _height );

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

    SetWindowLongW ( _hwnd, GWL_EXSTYLE, exStyle );
    SetWindowLongW ( _hwnd, GWL_STYLE, style );

    if ( _isFullScreen )
    {
        SetWindowPos ( _hwnd, HWND_TOP, 0, 0, _width, _height, SWP_FRAMECHANGED | SWP_SHOWWINDOW );
        return;
    }

    RECT windowRect;
    RECT clientRect;

    GetWindowRect ( _hwnd, &windowRect );
    GetClientRect ( _hwnd, &clientRect );

    GXInt deltaHeight = _height - ( clientRect.bottom - clientRect.top );
    GXInt deltaWidth = _width - ( clientRect.right - clientRect.left );

    SetWindowPos ( _hwnd, HWND_TOP, 0, 0, ( windowRect.right - windowRect.left ) + deltaWidth, ( windowRect.bottom  - windowRect.top ) + deltaHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW );
}

GXVoid GXRenderer::SetVSync ( GXBool enabled )
{
    GXInt t = enabled ? 1 : 0;

    if ( t == _vsync ) return;

    _vsync = t;
    wglSwapIntervalEXT ( _vsync );
}

GXVoid GXRenderer::SetResolution ( GXInt frameWidth, GXInt frameHeight )
{
    if ( _width == frameWidth && _height == frameHeight ) return;

    _width = frameWidth;
    _height = frameHeight;

    ReSizeScene ( _width, _height );

    if ( _isFullScreen )
    {
        DEVMODE dm;
        memset ( &dm, 0, sizeof ( dm ) );
        dm.dmSize = sizeof ( dm );
        dm.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;

        dm.dmPelsWidth = static_cast<DWORD> ( _width );
        dm.dmPelsHeight = static_cast<DWORD> ( _height );

        if ( ChangeDisplaySettings ( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
            MessageBoxW ( 0, L"Не удалось поменять оконный режим", L"Проблемка", MB_ICONEXCLAMATION );

        SetWindowPos ( _hwnd, HWND_TOP, 0, 0, _width, _height, SWP_FRAMECHANGED | SWP_SHOWWINDOW );

        return;
    }

    RECT windowRect;
    RECT clientRect;

    GetWindowRect ( _hwnd, &windowRect );
    GetClientRect ( _hwnd, &clientRect );

    GXInt deltaHeight = _height - ( clientRect.bottom - clientRect.top );
    GXInt deltaWidth = _width - ( clientRect.right - clientRect.left );

    SetWindowPos ( _hwnd, HWND_TOP, 0, 0, ( windowRect.right - windowRect.left ) + deltaWidth, ( windowRect.bottom  - windowRect.top ) + deltaHeight, SWP_SHOWWINDOW );
}

GXVoid GXRenderer::SetWindowName ( const GXWChar* name )
{
    GXSafeFree ( _title );
    GXWcsclone ( &_title, name );

    if ( _hwnd == static_cast<HWND> ( INVALID_HANDLE_VALUE ) ) return;

    SetWindowTextW ( _hwnd, _title );
}

GXVoid GXRenderer::Show () const
{
    if ( _hwnd == static_cast<HWND> ( INVALID_HANDLE_VALUE ) ) return;

    ShowWindow ( _hwnd, SW_SHOW );
}

GXVoid GXRenderer::Hide () const
{
    if ( _hwnd == static_cast<HWND> ( INVALID_HANDLE_VALUE ) ) return;

    ShowWindow ( _hwnd, SW_HIDE );
}

GXBool GXRenderer::IsVisible () const
{
    return IsWindowVisible ( _hwnd ) == TRUE;
}

GXUInt GXRenderer::GetCurrentFPS () const
{
    return _currentFPS;
}

GXVoid GXRenderer::GetSupportedResolutions ( GXRendererResolutions &out ) const
{
    DEVMODEW dm;
    dm.dmSize = sizeof ( dm );

    GXUShort rawTotal;
    for ( rawTotal = 0; EnumDisplaySettingsW ( 0, rawTotal, &dm ); ++rawTotal );

    GXRendererResolutions temp;
    temp._wxh = static_cast<GXUShort*> ( malloc ( ( rawTotal << 1 ) * sizeof ( GXUShort ) ) );

    DWORD lastWidth = 0;
    DWORD lastHeight = 0;

    out._total = 0u;

    for ( GXUShort i = 0u; i < rawTotal; ++i )
    {
        EnumDisplaySettingsW ( nullptr, i, &dm );

        GXBool hit = GX_FALSE;

        for ( GXUShort j = 0u; j < out._total; ++j )
        {
            if ( ( lastWidth == dm.dmPelsWidth && lastHeight == dm.dmPelsHeight ) || ( dm.dmPelsWidth == temp._wxh[ j << 1 ] && dm.dmPelsHeight == temp._wxh[ ( j << 1 ) + 1 ] ) )
            {
                hit = GX_TRUE;
                break;
            }
        }

        if ( hit ) continue;
        
        lastWidth = dm.dmPelsWidth;
        lastHeight = dm.dmPelsHeight;

        temp._wxh[ out._total << 1 ] = static_cast<GXUShort> ( lastWidth );
        temp._wxh[ ( out._total << 1 ) + 1 ] = static_cast<GXUShort> ( lastHeight );

        ++out._total;
    }

    GXUInt size = ( out._total << 1 ) * sizeof ( GXUShort );
    out._wxh = static_cast<GXUShort*> ( malloc ( size ) );
    memcpy ( out._wxh, temp._wxh, size );

    temp.Cleanup ();

    qsort ( out._wxh, out._total, 2 * sizeof ( GXUShort ), &GXRenderer::GXResolutionCompare );
}

GXInt GXRenderer::GetWidth () const
{
    return _width;
}

GXInt GXRenderer::GetHeight () const
{
    return _height;
}

GXBool GXCALL GXRenderer::UpdateRendererSettings ()
{
    _isSettingsChanged = GX_TRUE;
    return GX_TRUE;
}

GXVoid GXCALL GXRenderer::ReSizeScene ( GXInt frameWidth, GXInt frameHeight )
{
    if ( frameHeight == 0 )
        frameHeight = 1;

    _width = frameWidth;
    _height = frameHeight;

    _game->OnResize ( _width, _height );
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
    _isRenderableObjectInited = GX_FALSE;
    _hinst = GetModuleHandle ( nullptr );

    _isFullScreen = !gx_EngineSettings._windowed;

    _width = gx_EngineSettings._rendererWidth;
    _height = gx_EngineSettings._rendererHeight;

    _vsync = ( gx_EngineSettings._vSync ) ? 1 : 0;

    _currentFPS = 0u;
    _lastTime = GXGetProcessorTicks ();
    _accumulator = 0.0;
    _fpsCounter = 0u;
    _fpsTimer = 0.0;

    _isSettingsChanged = GX_FALSE;
    _loopFlag = GX_TRUE;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXThread" )
    _thread = new GXThread ( &RenderLoop, nullptr );

    SetWindowName ( DEFAULT_WINDOW_NAME );
}

GXUPointer GXTHREADCALL GXRenderer::RenderLoop ( GXVoid* /*args*/, GXThread &threadObject )
{
    if ( !MakeWindow () )
        return 0u;

    GXDesktopInput& desktopInput = GXDesktopInput::GetInstance ();

    while ( _loopFlag )
    {
        if ( !_isRenderableObjectInited )
            InitRenderableObjects ();

        MSG msg;

        while ( PeekMessage ( &msg, 0, 0u, 0u, PM_REMOVE ) ) 
        {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );

            desktopInput.ProcessOSMessage ( &msg );
        }

        if ( _isSettingsChanged )
        {
            Destroy ();
            MakeWindow ();
            _isSettingsChanged = GX_FALSE;
        }
        else
        {
            if ( _isRenderableObjectInited )
            {
                DrawScene ();
                SwapBuffers ( _hDC );
            }
        }

        threadObject.Switch ();
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

    GXLogA ( "GXRenderer::InitOpenGL::Info - Аппаратные возможности:\n--------Текстурирование:\n" );

    GLint value[ 2u ];
    glGetIntegerv ( GL_MAX_TEXTURE_SIZE, value );
    GXLogA ( "Максимальный размер 2D текстуры: %u (минимум стандарта - 1024)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_ARRAY_TEXTURE_LAYERS, value );
    GXLogA ( "Максимальное количество слоёв для текстурных массивов: %u (минимум стандарта - 256)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_CUBE_MAP_TEXTURE_SIZE, value );
    GXLogA ( "Максимальный размер cube map текстуры: %u (минимум стандарта - 1024)\n", value[ 0u ] );

    GXLogA ( "\n--------Буфер кадра:\n" );

    glGetIntegerv ( GL_MAX_FRAMEBUFFER_WIDTH, value );
    glGetIntegerv ( GL_MAX_FRAMEBUFFER_HEIGHT, value + 1 );
    GXLogA ( "Максимальное разрешение буфера кадра: %u х %u (минимум стандарта - 16384 х 16384)\n", value[ 0u ], value[ 1u ] );

    glGetIntegerv ( GL_MAX_DRAW_BUFFERS, value );
    GXLogA ( "Максимальное количество выходных буферов, доступных фрагментному шейдеру: %u (минимум стандарта - 8)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_VIEWPORT_DIMS, value );
    GXLogA ( "Максимальное разрешение viewport'а: %u x %u\n", value[ 0u ], value[ 1u ] );

    GXLogA ( "\n--------Шейдерная программа:\n" );

    // There is not GL_MAX_VARYING_VECTORS in OpenGL 3.3 core profile standart. But GL_MAX_VARYING_COMPONENTS did not work at all (Error 0x500 - Invalid Enum)!
    // Tested on NVIDIA GeForce 770 GTX (Driver version 384.94)
    glGetIntegerv ( GL_MAX_VARYING_VECTORS, value );
    GXLogA ( "Максимальное количество векторов-интерполяторов, доступных шейдерной программе: %u (минимум стандарта - 15)\n", value[ 0u ] );

    GXLogA ( "\n--------Вершинный шейдер:\n" );

    glGetIntegerv ( GL_MAX_VERTEX_ATTRIBS, value );
    GXLogA ( "Максимальное количество входных атрибутов-векторов, доступных вершинному шейдеру: %u (минимум стандарта - 16)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, value );
    GXLogA ( "Максимальное количество текстур, доступных вершинному шейдеру: %u (минимум стандарта - 16)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_VERTEX_UNIFORM_VECTORS, value );
    GXLogA ( "Максимальное количество юниформ-векторов, доступных вершинному шейдеру: %u (минимум стандарта - 256)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_VERTEX_UNIFORM_COMPONENTS, value );
    GXLogA ( "Максимальное количество юниформ-компонент, доступных вершинному шейдеру: %u (минимум стандарта - 1024)\n", value[ 0u ] );

    GXLogA ( "\n--------Геометрический шейдер:\n" );

    glGetIntegerv ( GL_MAX_GEOMETRY_INPUT_COMPONENTS, value );
    GXLogA ( "Максимальное количество входных атрибутов-компонент, доступных геометрическому шейдеру: %u (минимум стандарта - 64)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, value );
    GXLogA ( "Максимальное количество текстур, доступных геометрическому шейдеру: %u (минимум стандарта - 16)\n", value[ 0 ] );

    glGetIntegerv ( GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, value );
    GXLogA ( "Максимальное количество выходных атрибутов-компонент, доступных геометрическому шейдеру: %u (минимум стандарта - 128)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, value );
    GXLogA ( "Максимальное количество юниформ-компонент, доступных геометрическому шейдеру: %u (минимум стандарта - 1024)\n", value[ 0u ] );

    GXLogA ( "\n--------Фрагментный шейдер:\n" );

    glGetIntegerv ( GL_MAX_FRAGMENT_INPUT_COMPONENTS, value );
    GXLogA ( "Максимальное количество входных атрибутов-компонент, доступных фрагментному шейдеру: %u (минимум стандарта - 128)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_TEXTURE_IMAGE_UNITS, value );
    GXLogA ( "Максимальное количество текстур, доступных фрагментному шейдеру: %u (минимум стандарта - 16)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_FRAGMENT_UNIFORM_VECTORS, value );
    GXLogA ( "Максимальное количество юниформ-векторов, доступных фрагментному шейдеру: %u (минимум стандарта - 256)\n", value[ 0u ] );

    glGetIntegerv ( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, value );
    GXLogA ( "Максимальное количество юниформ-компонент, доступных фрагментному шейдеру: %u (минимум стандарта - 1024)\n", value[ 0u ] );

    GXCheckOpenGLError ();
}

GXVoid GXCALL GXRenderer::DrawScene ()
{
    GXGetPhysXInstance ()->DoSimulate ();

    GXDouble newtime = GXGetProcessorTicks ();
    GXDouble delta = ( newtime - _lastTime ) / GXGetProcessorTicksPerSecond ();
    _lastTime = newtime;
    
    _accumulator += delta;
    _fpsTimer += delta;

    if ( _fpsTimer >= 1.0 )
    {
        _currentFPS = _fpsCounter;
        _fpsTimer = 0.0;
        _fpsCounter = 0u;
    }

    if ( _accumulator <= 0.001 ) return;

    ++_fpsCounter;

    GXFloat update = static_cast<GXFloat> ( _accumulator );
    _accumulator = 0.0;
    _game->OnFrame ( update );
}

GXVoid GXCALL GXRenderer::Destroy ()
{
    DeleteRenderableObjects ();

    GXShaderProgram::DestroyPrecompiledShaderProgramSubsystem ();

    if ( _isFullScreen )
    {
        ChangeDisplaySettingsW ( nullptr, 0 );
        while ( ShowCursor ( TRUE ) <= 0 );
    }

    if ( _hglRC )
    {
        if ( !wglMakeCurrent ( 0, 0 ) )
        {
            GXWarningBox ( "Освобождение контекстов устройства и редеринга провалено" );
            GXLogA ( "GXRenderer::Destroy::Error - Освобождение контекстов устройства и редеринга провалено\n" );
        }

        if ( !wglDeleteContext ( _hglRC ) )
        {
            GXWarningBox ( "Удаление контекста редеринга провалено" );
            GXLogA ( "GXRenderer::Destroy::Error - Удаление контекста редеринга провалено\n" );
        }

        _hglRC = static_cast<HGLRC> ( INVALID_HANDLE_VALUE );
    }

    if ( _hDC && !DeleteDC ( _hDC ) )
    {
        GXWarningBox ( "Освобождение контекста устройства провалено" );
        GXLogA ( "GXRenderer::Destroy::Error - Освобождение контекста устройства провалено\n" );
        _hDC = static_cast<HDC> ( INVALID_HANDLE_VALUE );
    }

    if ( _hwnd && !DestroyWindow ( _hwnd ) )
    {
        GXWarningBox ( L"Освобождение HWND провалено" );
        GXLogA ( "GXRenderer::Destroy::Error - Освобождение HWND провалено\n" );
        _hwnd = static_cast<HWND> ( INVALID_HANDLE_VALUE );
    }

    if ( UnregisterClassW ( WINDOW_OPENGL_CLASS, _hinst ) ) return;

    GXWarningBox ( "Снятие регистрации класса окна провалено" );
    GXLogA ( "GXRenderer::Destroy::Error - Снятие регистрации класса окна провалено\n" );
    _hinst = static_cast<HINSTANCE> ( INVALID_HANDLE_VALUE );
}

GXBool GXCALL GXRenderer::MakeWindow ()
{
    WNDCLASSW wc;
    DWORD dwExStyle;
    DWORD dwStyle;

    memset ( &wc, 0, sizeof ( wc ) );
    wc.hInstance = _hinst;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    wc.lpszClassName = WINDOW_OPENGL_CLASS;
    wc.lpfnWndProc = &GXInput::InputProc;
    wc.hCursor = LoadCursor ( 0, IDC_ARROW );
    wc.hIcon = LoadIcon ( _hinst, MAKEINTRESOURCE ( GX_RID_EXE_MAINICON ) );

    if ( !RegisterClassW ( &wc ) )
    {
        GXWarningBox ( "Не удалось зарегистрировать класс окна" );
        GXLogA ( "GXRenderer::MakeWindow::Error - Не удалось зарегистрировать класс окна\n" );
        return GX_FALSE;
    }

    DEVMODEW dm;
    memset ( &dm, 0, sizeof ( dm ) );
    dm.dmSize = sizeof ( dm );
    dm.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH | DM_BITSPERPEL;

    if ( _isFullScreen )
    {
        dm.dmBitsPerPel = DEFAULT_COLOR_BITS;
        dm.dmPelsHeight = static_cast<DWORD> ( _height );
        dm.dmPelsWidth = static_cast<DWORD> ( _width );

        if ( ChangeDisplaySettingsW ( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
        {
            if ( MessageBoxW ( 0, L"Не удалось установить полноэкранный режим. Использовать обычный режим?", L"Проблемка", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES )
                _isFullScreen = FALSE;
            else
                return GX_FALSE;
        }
    }

    if ( _isFullScreen )
    {
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
    }
    else
    {
        if ( ChangeDisplaySettingsW ( nullptr, 0 ) != DISP_CHANGE_SUCCESSFUL )
        {
            GXWarningBox ( "Не удалось вернуть графические настройки по умолчанию" );
            GXLogA ( "GXRenderer::MakeWindow::Error - Не удалось вернуть графические настройки по умолчанию\n" );

            return GX_FALSE;
        }
        else
        {
            dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
            dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU | WS_CAPTION;
        }
    }

    _hwnd = CreateWindowExW ( dwExStyle, WINDOW_OPENGL_CLASS, _title, dwStyle, 0, 0, _width, _height, 0, 0, _hinst, 0 );

    if ( !_hwnd )
    {
        GXWarningBox ( "При создании окна произошла ошибка" );
        GXLogA ( "GXRenderer::MakeWindow::Error - При создании окна произошла ошибка\n" );
        Destroy ();
        return GX_FALSE;
    }

    RECT windowRect;
    RECT clientRect;

    GetWindowRect ( _hwnd, &windowRect );
    GetClientRect ( _hwnd, &clientRect );

    GXInt deltaHeight = _height - ( clientRect.bottom - clientRect.top );
    GXInt deltaWidth = _width - ( clientRect.right - clientRect.left );

    SetWindowPos ( _hwnd, HWND_TOP, 0, 0, ( windowRect.right - windowRect.left ) + deltaWidth, ( windowRect.bottom  - windowRect.top ) + deltaHeight, SWP_HIDEWINDOW );

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

    _hDC = GetDC ( _hwnd );

    if ( !_hDC )
    {
        GXWarningBox ( "Не удалось получить hDC" );
        GXLogA ( "GXRenderer::MakeWindow::Error - Не удалось получить hDC\n" );
        Destroy ();

        return GX_FALSE;
    }

    GXInt pixelFomat = ChoosePixelFormat ( _hDC, &pfd );

    if ( pixelFomat == 0 )
    {
        GXWarningBox ( "Не удалось найти подходящий формат пикселей" );
        GXLogA ( "GXRenderer::MakeWindow::Error - Не удалось найти подходящий формат пикселей\n" );
        Destroy ();

        return GX_FALSE;
    }

    if ( !SetPixelFormat ( _hDC, pixelFomat, &pfd ) )
    {
        GXWarningBox ( "Не удалось установить формат пикселей" );
        GXLogA ( "GXRenderer::MakeWindow::Error - Не удалось установить формат пикселей\n" );
        Destroy ();

        return GX_FALSE;
    }

    _hglRC = wglCreateContext ( _hDC );

    if ( _hglRC == nullptr )
    {
        GXWarningBox ( "Не удалось создать контекст рендеринга" );
        GXLogA ( "GXRenderer::MakeWindow::Error - Не удалось создать контекст рендеринга\n" );
        Destroy ();

        return GX_FALSE;
    }

    if ( !wglMakeCurrent ( _hDC, _hglRC ) )
    {
        GXWarningBox ( "Не удалось установить контекст рендеринга" );
        GXLogA ( "GXRenderer::MakeWindow::Error - Не удалось установить контекст рендеринга\n" );
        Destroy ();

        return GX_FALSE;
    }

    GXOpenGLInit ();

    wglMakeCurrent ( nullptr, nullptr );
    wglDeleteContext ( _hglRC );

    if ( !wglCreateContextAttribsARB )
    {
        GXWarningBox ( "wglCreateContextAttribsARB провален" );
        GXLogA ( "GXRenderer::MakeWindow::Error - wglCreateContextAttribsARB провален\n" );

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

    _hglRC = wglCreateContextAttribsARB ( _hDC, nullptr, attribs );

    if ( !_hglRC || !wglMakeCurrent ( _hDC, _hglRC ) )
    {
        GXWarningBox ( "Создание контекста OpenGL версии 3.3 провалено" );
        GXLogA ( "GXRenderer::MakeWindow::Error - Создание контекста OpenGL версии 3.3 провалено\n" );

        return GX_FALSE;
    }

    GXShaderProgram::InitPrecompiledShaderProgramSubsystem ();

    SetForegroundWindow ( _hwnd );
    SetFocus ( _hwnd );
    ReSizeScene ( _width, _height );
    InitOpenGL ();

    wglSwapIntervalEXT ( _vsync );

    gx_ui_Scale = GetDeviceCaps ( _hDC, LOGPIXELSX ) / 2.54f;
    InitRenderableObjects ();

    DrawScene ();

    ShowCursor ( 0 );

    return GX_TRUE;
}

GXVoid GXCALL GXRenderer::InitRenderableObjects ()
{
    _game->OnInit ();
    _isRenderableObjectInited = GX_TRUE;
}

GXVoid GXCALL GXRenderer::DeleteRenderableObjects ()
{
    _game->OnDestroy ();
    _isRenderableObjectInited = GX_FALSE;
}
