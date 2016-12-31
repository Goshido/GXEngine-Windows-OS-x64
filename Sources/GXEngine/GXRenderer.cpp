//version 1.13

#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXPhysXFrontend.h>
#include <GXEngine/GXResource.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXInput.h>
#include <GXEngine/GXEngineSettings.h>
#include <GXCommon/GXTime.h>
#include <GXCommon/GXStrings.h>


#define GX_WINDOW_OPENGL_CLASS	L"GX_RENDERER_CLASS"
#define GX_DEFAULT_WINDOW_NAME	L"GXEngine"

#define GX_DEFAULT_COLOR_BITS	32
#define GX_DEFAULT_DEPTH_BITS	24
#define GX_DEFAULT_STENCIL_BITS	8


GXRendererResolutions::GXRendererResolutions ()
{
	total = 0;
	wxh = 0;
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

GXThread*								GXRenderer::thread = nullptr;

GXBool									GXRenderer::loopFlag = GX_TRUE;

HWND									GXRenderer::hwnd = (HWND)INVALID_HANDLE_VALUE;
HINSTANCE								GXRenderer::hinst = (HINSTANCE)INVALID_HANDLE_VALUE;
HGLRC									GXRenderer::hglRC = (HGLRC)INVALID_HANDLE_VALUE;
HDC										GXRenderer::hDC = (HDC)INVALID_HANDLE_VALUE;

GXBool									GXRenderer::isFullScreen;
GXBool									GXRenderer::isSettingsChanged;

GXInt									GXRenderer::width;
GXInt									GXRenderer::height;
GXInt									GXRenderer::vsync;

GXWChar*								GXRenderer::title = nullptr;

GXUInt									GXRenderer::currentFPS = 0;
GXDouble								GXRenderer::accumulator = 0.0;
GXDouble								GXRenderer::lastTime;

PFNGXONFRAMEPROC						GXRenderer::onFrameFunc = nullptr;
PFNGXONINITRENDERABLEOBJECTSPROC		GXRenderer::onInitRenderableObjectsFunc = nullptr;
PFNGXONDELETERENDERABLEOBJECTSPROC		GXRenderer::onDeleteRenderableObjectsFunc = nullptr;
GXBool									GXRenderer::isRenderableObjectInited = GX_FALSE;

GXRenderer*								GXRenderer::instance = nullptr;


GXRenderer::~GXRenderer ()
{
	if ( isFullScreen )
	{
		if ( ChangeDisplaySettingsW ( 0, 0 )  != DISP_CHANGE_SUCCESSFUL )
		{
			GXDebugBox ( L"Не удалось вернуть графические настройки по умолчанию" );
			GXLogW ( L"GXRenderer::~GXRenderer::Error - Не удалось вернуть графические настройки по умолчанию\n" );
		}
	}

	GXSafeFree ( title );
	delete thread;

	instance = nullptr;
}

GXVoid GXRenderer::Start ()
{
	thread->Resume ();
}

GXVoid GXRenderer::Suspend ()
{
	thread->Suspend ();
}

GXBool GXRenderer::Shutdown ()
{
	loopFlag = GX_FALSE;
	thread->Resume ();
	thread->Join ();

	return GX_TRUE;
}

GXVoid GXRenderer::SetFullscreen ( GXBool enabled )
{
	if ( isFullScreen == enabled ) return;

	isFullScreen = enabled;

	DWORD exStyle = 0;
	DWORD style = 0;

	if ( isFullScreen )
	{
		DEVMODEW dm;
		memset ( &dm, 0, sizeof ( dm ) );
		dm.dmSize = sizeof ( dm );
		dm.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;

		dm.dmPelsWidth = width;
		dm.dmPelsHeight = height;

		if ( ChangeDisplaySettingsW ( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
			MessageBoxW ( 0, L"Не удалось поменять оконный режим", L"Проблемка", MB_ICONEXCLAMATION );

		exStyle = WS_EX_APPWINDOW;
		style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
	}
	else
	{
		ChangeDisplaySettingsW ( 0, 0 );

		exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
	}

	SetWindowLongW ( hwnd, GWL_EXSTYLE, exStyle );
	SetWindowLongW ( hwnd, GWL_STYLE, style );

	if ( isFullScreen )
		SetWindowPos ( hwnd, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW );
	else
	{
		RECT windowRect;
		RECT clientRect;

		GetWindowRect ( hwnd, &windowRect );
		GetClientRect ( hwnd, &clientRect );

		GXInt deltaHeight = height - ( clientRect.bottom - clientRect.top );
		GXInt deltaWidth = width - ( clientRect.right - clientRect.left );

		SetWindowPos ( hwnd, HWND_TOP, 0, 0, ( windowRect.right - windowRect.left ) + deltaWidth, ( windowRect.bottom  - windowRect.top ) + deltaHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW );
	}
}

GXVoid GXRenderer::SetVSync ( GXBool enabled )
{
	GXInt t = enabled ? 1 : 0;
	if ( t == vsync ) return;

	vsync = t;
	wglSwapIntervalEXT ( vsync );
}

GXVoid GXRenderer::SetResolution ( GXInt width, GXInt height )
{
	if ( this->width == width && this->height == height ) return;

	this->width = width;
	this->height = height;

	ReSizeScene ( this->width, this->height );

	if ( isFullScreen )
	{
		DEVMODE dm;
		memset ( &dm, 0, sizeof ( dm ) );
		dm.dmSize = sizeof ( dm );
		dm.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;

		dm.dmPelsWidth = width;
		dm.dmPelsHeight = height;

		if ( ChangeDisplaySettingsW ( &dm, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
			MessageBoxW ( 0, L"Не удалось поменять оконный режим", L"Проблемка", MB_ICONEXCLAMATION );

		SetWindowPos ( hwnd, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW );
	}
	else
	{
		RECT windowRect;
		RECT clientRect;

		GetWindowRect ( hwnd, &windowRect );
		GetClientRect ( hwnd, &clientRect );

		GXInt deltaHeight = height - ( clientRect.bottom - clientRect.top );
		GXInt deltaWidth = width - ( clientRect.right - clientRect.left );

		SetWindowPos ( hwnd, HWND_TOP, 0, 0, ( windowRect.right - windowRect.left ) + deltaWidth, ( windowRect.bottom  - windowRect.top ) + deltaHeight, SWP_SHOWWINDOW );
	}
}

GXVoid GXRenderer::SetWindowTitle ( const GXWChar* title )
{
	GXSafeFree ( this->title );
	GXWcsclone ( &this->title, title );
	SetWindowTextW ( hwnd, this->title );
}

GXVoid GXRenderer::SetOnFrameFunc ( PFNGXONFRAMEPROC callback )
{
	this->onFrameFunc = callback;
}

GXVoid GXRenderer::SetOnInitRenderableObjectsFunc ( PFNGXONINITRENDERABLEOBJECTSPROC callback )
{
	this->onInitRenderableObjectsFunc = callback;
}

GXVoid GXRenderer::SetOnDeleteRenderableObjectsFunc ( PFNGXONDELETERENDERABLEOBJECTSPROC callback )
{
	this->onDeleteRenderableObjectsFunc = callback;
}

GXUInt GXRenderer::GetCurrentFPS () const
{
	return currentFPS;
}

GXInt GXResolutionCompare ( const GXVoid* a, const GXVoid* b )
{
	const GXUShort* itemA = (GXUShort*)a;
	const GXUShort* itemB = (GXUShort*)b;

	GXUInt keyA = 10000 * itemA[ 0 ] + itemA[ 1 ];
	GXUInt keyB = 10000 * itemB[ 0 ] + itemB[ 1 ];

	if ( keyA < keyB ) return -1;
	if ( keyA > keyB ) return 1;

	return 0;
}

GXVoid GXRenderer::GetSupportedResolutions ( GXRendererResolutions &out ) const
{
	DEVMODEW dm;
	dm.dmSize = sizeof ( dm );

	GXUShort rawTotal;
	for ( rawTotal = 0; EnumDisplaySettingsW ( 0, rawTotal, &dm ); rawTotal++ );

	GXRendererResolutions temp;
	temp.wxh = (GXUShort*)malloc ( ( rawTotal << 1 ) * sizeof ( GXUShort ) );

	DWORD	lastWidth = 0;
	DWORD	lastHeight = 0;

	out.total = 0;
	for ( GXUShort i = 0; i < rawTotal; i++ )
	{
		EnumDisplaySettingsW ( 0, i, &dm );

		GXBool hit = GX_FALSE;
		for ( GXUShort j = 0; j < out.total; j++ )
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

		temp.wxh[ out.total << 1 ] = (GXUShort)lastWidth;
		temp.wxh[ ( out.total << 1 ) + 1 ] = (GXUShort)lastHeight;

		out.total++;
	}

	GXUInt size = ( out.total << 1 ) * sizeof ( GXUShort );
	out.wxh = (GXUShort*)malloc ( size );
	memcpy ( out.wxh, temp.wxh, size );

	temp.Cleanup ();

	qsort ( out.wxh, out.total, 2 * sizeof ( GXUShort ), &GXResolutionCompare );
}

GXVoid GXRenderer::SetWindowName ( const GXWChar* name )
{
	GXSafeFree ( title );
	GXWcsclone ( &title, name );

	if ( hwnd == (HWND)INVALID_HANDLE_VALUE ) return;

	SetWindowTextW ( hwnd, title );
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

GXVoid GXCALL GXRenderer::ReSizeScene ( GXInt width, GXInt height  )
{
	if ( height == 0 )
		height = 1;

	GXRenderer::width = width;
	GXRenderer::height = height;
}

GXRenderer* GXCALL GXRenderer::GetInstance ()
{
	if ( !instance )
		instance = new GXRenderer ();

	return instance;
}

GXRenderer::GXRenderer ()
{
	isRenderableObjectInited = GX_FALSE;
	hinst = GetModuleHandle ( 0 );

	isFullScreen = !gx_EngineSettings.windowed;

	width = gx_EngineSettings.rendererWidth;
	height = gx_EngineSettings.rendererHeight;

	vsync = ( gx_EngineSettings.vSync ) ? 1 : 0;

	currentFPS = 0;
	lastTime = GXGetProcessorTicks ();
	accumulator = 0.0;

	isSettingsChanged = GX_FALSE;
	loopFlag = GX_TRUE;
	thread = new GXThread ( &RenderLoop, 0, GX_SUSPEND );

	SetWindowName ( GX_DEFAULT_WINDOW_NAME );
}

GXDword GXTHREADCALL GXRenderer::RenderLoop ( GXVoid* args )
{
	if ( !MakeWindow () ) return 0;

	MSG msg;
	while ( loopFlag )
	{
		if ( !isRenderableObjectInited )
			InitRenderableObjects ();

		if ( PeekMessage ( &msg, 0, 0, 0, PM_REMOVE ) ) 
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
				if ( DrawScene () )
					SwapBuffers ( hDC );
			}
		}
		SwitchToThread ();
	}
	Destroy ();
	return 0;
}

GXVoid GXCALL GXRenderer::InitOpenGL ()
{
	glClearDepth ( 1.0f );
	glEnable ( GL_DEPTH_TEST );
	glDepthFunc ( GL_LEQUAL );
	glEnable ( GL_CULL_FACE );
	glCullFace ( GL_FRONT );
	GXCheckOpenGLError ();
}

GXUShort gx_renderer_fpsCounter = 0;
GXDouble gx_renderer_fpsTimer = 0.0;

GXBool GXCALL GXRenderer::DrawScene ()
{
	GXGetPhysXInstance ()->DoSimulate ();

	GXDouble newtime = GXGetProcessorTicks ();
	GXDouble delta = ( newtime - lastTime ) / GXGetProcessorTicksPerSecond ();
	lastTime = newtime;
	
	accumulator += delta;
	gx_renderer_fpsTimer += delta;
	if ( gx_renderer_fpsTimer >= 1.0 )
	{
		currentFPS = gx_renderer_fpsCounter;
		gx_renderer_fpsTimer = 0.0;
		gx_renderer_fpsCounter = 0;
	}

	if ( accumulator <= 0.001 ) return GX_FALSE;

	if ( onFrameFunc )
	{
		gx_renderer_fpsCounter++;

		GXFloat update = (GXFloat)accumulator;
		accumulator = 0.0;
		return onFrameFunc ( update );
	}

	return GX_FALSE;
}

GXVoid GXCALL GXRenderer::Destroy ()
{
	DeleteRenderableObjects ();
	if ( isFullScreen )
	{
		ChangeDisplaySettingsW ( 0, 0 );
		while ( ShowCursor ( TRUE ) <= 0 );
	}

	if ( hglRC )
	{
		if ( !wglMakeCurrent ( 0, 0 ) )
		{
			GXDebugBox ( L"Освобождение контекстов устройства и редеринга провалено" );
			GXLogW ( L"GXRenderer::Destroy::Error - Освобождение контекстов устройства и редеринга провалено\n" );
		}

		if ( !wglDeleteContext ( hglRC ) )
		{
			GXDebugBox ( L"Удаление контекста редеринга провалено" );
			GXLogW ( L"GXRenderer::Destroy::Error - Удаление контекста редеринга провалено\n" );
		}

		hglRC = (HGLRC)INVALID_HANDLE_VALUE;
	}

	if ( hDC && !DeleteDC ( hDC ) )
	{
		GXDebugBox ( L"Освобождение контекста устройства провалено" );
		GXLogW ( L"GXRenderer::Destroy::Error - Освобождение контекста устройства провалено\n" );
		hDC = (HDC)INVALID_HANDLE_VALUE;
	}

	if ( hwnd && !DestroyWindow ( hwnd ) )
	{
		GXDebugBox ( L"Освобождение HWND провалено" );
		GXLogW ( L"GXRenderer::Destroy::Error - Освобождение HWND провалено\n" );
		hwnd = (HWND)INVALID_HANDLE_VALUE;
	}

	if ( !UnregisterClassW ( GX_WINDOW_OPENGL_CLASS, hinst ) )
	{
		GXDebugBox ( L"Снятие регистрации класса окна провалено" );
		GXLogW ( L"GXRenderer::Destroy::Error - Снятие регистрации класса окна провалено\n" );
		hinst = (HINSTANCE)INVALID_HANDLE_VALUE;
	}
}

GXBool GXCALL GXRenderer::MakeWindow ()
{
	GLuint PixelFomat;
	WNDCLASSW wc;
	DWORD dwExStyle;
	DWORD dwStyle;
	
	memset ( &wc, 0, sizeof ( wc ) );
	wc.hInstance = hinst;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wc.lpszClassName = GX_WINDOW_OPENGL_CLASS;
	wc.lpfnWndProc = &GXInput::InputProc;
	wc.hCursor = LoadCursorW ( 0, IDC_ARROW );
	wc.hIcon = LoadIconW ( hinst, MAKEINTRESOURCE ( GX_RID_EXE_MAINICON ) );

	if ( !RegisterClassW ( &wc ) )
	{
		GXDebugBox ( L"Не удалось зарегистрировать класс окна" );
		GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось зарегистрировать класс окна\n" );
		return GX_FALSE;
	}

	DEVMODEW dm;
	memset ( &dm, 0, sizeof ( dm ) );
	dm.dmSize = sizeof ( dm );
	dm.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH | DM_BITSPERPEL;
	if ( isFullScreen )
	{
		dm.dmBitsPerPel = GX_DEFAULT_COLOR_BITS;
		dm.dmPelsHeight = height;
		dm.dmPelsWidth = width;
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
		if ( ChangeDisplaySettingsW ( 0, 0 ) != DISP_CHANGE_SUCCESSFUL )
		{
			GXDebugBox ( L"Не удалось вернуть графические настройки по умолчанию" );
			GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось вернуть графические настройки по умолчанию\n" );
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_SYSMENU | WS_CAPTION;
		}
	}

	hwnd = CreateWindowExW ( dwExStyle, GX_WINDOW_OPENGL_CLASS, title, dwStyle, 0, 0, width, height, 0, 0, hinst, 0 );

	if ( !hwnd )
	{
		GXDebugBox ( L"При создании окна произошла ошибка" );
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

	SetWindowPos ( hwnd, HWND_TOP, 0, 0, ( windowRect.right - windowRect.left ) + deltaWidth, ( windowRect.bottom  - windowRect.top ) + deltaHeight, SWP_SHOWWINDOW );

	static  PIXELFORMATDESCRIPTOR pfd;
	memset ( &pfd, 0, sizeof ( pfd ) );
	pfd.nSize = sizeof ( PIXELFORMATDESCRIPTOR );
	pfd.nVersion = 1;
	pfd.cColorBits = GX_DEFAULT_COLOR_BITS;
	pfd.cDepthBits = GX_DEFAULT_DEPTH_BITS;
	pfd.cStencilBits = GX_DEFAULT_STENCIL_BITS;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |	PFD_DOUBLEBUFFER; 
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.iLayerType = PFD_MAIN_PLANE;
	if ( !( hDC = GetDC ( hwnd ) ) )
	{
		GXDebugBox ( L"Не удалось получить hDC" );
		GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось получить hDC\n" );
		Destroy ();

		return GX_FALSE;
	}

	if ( !( PixelFomat = ChoosePixelFormat ( hDC, &pfd ) ) )
	{
		GXDebugBox ( L"Не удалось найти подходящий формат пикселей" );
		GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось найти подходящий формат пикселей\n" );
		Destroy ();

		return GX_FALSE;
	}

	if ( !SetPixelFormat ( hDC, PixelFomat, &pfd ) )
	{
		GXDebugBox ( L"Не удалось установить формат пикселей" );
		GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось установить формат пикселей\n" );
		Destroy ();

		return GX_FALSE;
	}

	if ( !( hglRC = wglCreateContext ( hDC ) ) )
	{
		GXDebugBox ( L"Не удалось создать контекст рендеринга" );
		GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось создать контекст рендеринга\n" );
		Destroy ();

		return GX_FALSE;
	}

	if ( !wglMakeCurrent ( hDC, hglRC ) )
	{
		GXDebugBox ( L"Не удалось установить контекст рендеринга" );
		GXLogW ( L"GXRenderer::MakeWindow::Error - Не удалось установить контекст рендеринга\n" );
		Destroy ();

		return GX_FALSE;
	}

	GXOpenGLInit ();

	wglMakeCurrent ( 0, 0 );
	wglDeleteContext ( hglRC );
	if ( !wglCreateContextAttribsARB )
	{
		GXDebugBox ( L"wglCreateContextAttribsARB провален" );
		GXLogW ( L"GXRenderer::MakeWindow::Error - wglCreateContextAttribsARB провален\n" );

		return GX_FALSE;
	}

	GXInt attribs [] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB,	3,
		WGL_CONTEXT_MINOR_VERSION_ARB,	3,
		WGL_CONTEXT_FLAGS_ARB,			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB,	WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	hglRC = wglCreateContextAttribsARB ( hDC, 0, attribs );
	if ( !hglRC || !wglMakeCurrent ( hDC, hglRC ) )
	{
			GXDebugBox ( L"Создание контекста OpenGL версии 3.3 провалено" );
			GXLogW ( L"GXRenderer::MakeWindow::Error - Создание контекста OpenGL версии 3.3 провалено\n" );

			return GX_FALSE;
	}

	ShowWindow ( hwnd, SW_SHOWNORMAL );
	SetForegroundWindow ( hwnd );
	SetFocus ( hwnd );
	ReSizeScene ( width, height );
	InitOpenGL ();

	wglSwapIntervalEXT ( vsync );

	gx_ui_Scale = GetDeviceCaps ( hDC, LOGPIXELSX ) / 2.54f;
	InitRenderableObjects ();
	ShowCursor ( 0 );

	return GX_TRUE;
}

GXVoid GXCALL GXRenderer::InitRenderableObjects ()
{	
	if ( !onInitRenderableObjectsFunc ) return;

	onInitRenderableObjectsFunc ();
	isRenderableObjectInited = GX_TRUE;
}

GXVoid GXCALL GXRenderer::DeleteRenderableObjects ()
{
	if ( !onDeleteRenderableObjectsFunc ) return;

	onDeleteRenderableObjectsFunc ();
	isRenderableObjectInited = GX_FALSE;
}