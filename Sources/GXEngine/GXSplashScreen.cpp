// version 1.0

#include <GXEngine/GXSplashScreen.h>
#include <GXEngine/GXResource.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>


#define SPLASH_SCREEN_WINDOW_CLASS				L"GX_SPLASH_SCREEN_CLASS"
#define SPLASH_SCREEN_IMAGE						L"../../Textures/System/GXEngine splash screen.png"
#define BITS_PER_CHANNEL						8
#define RGB_BYTES_PER_PIXEL						3
#define BGRA_BYTES_PER_PIXEL					4
#define NUMBER_PLANES							1
#define ALPHA_FOR_RGB							255


enum class eGXStatus : GXUByte
{
	Success = 0,
	CanNotLoadImage = 1,
	InvalidChannels = 2,
	CanNotRegisterClass = 3,
	CanNotCreateWindow = 4,
	CanNotFreeWindowResources = 5
};

//---------------------------------------------------------------------------------------------------------------

GXSplashScreen* GXSplashScreen::instance = nullptr;

GXSplashScreen& GXSplashScreen::GetInstance ()
{
	if ( !instance )
		instance = new GXSplashScreen ();

	return *instance;
}

GXSplashScreen::~GXSplashScreen ()
{
	if ( hwnd == (HWND)INVALID_HANDLE_VALUE ) return;

	PostMessageW ( hwnd, WM_QUIT, 0, 0 );
	thread->Join ();

	delete thread;

	instance = nullptr;
}

GXVoid GXSplashScreen::Show ()
{
	intend = eGXSplashScreenState::Visible;
}

GXVoid GXSplashScreen::Hide ()
{
	intend = eGXSplashScreenState::Hidden;
}

GXSplashScreen::GXSplashScreen ()
{
	thread = new GXThread ( &GXSplashScreen::MessageLoop, this );

	hwnd = (HWND)INVALID_HANDLE_VALUE;
	bitmap = (HBITMAP)INVALID_HANDLE_VALUE;
	bitmapWidth = 0;
	bitmapHeight = 0;
	pixels = nullptr;
	state = eGXSplashScreenState::Hidden;
	intend = eGXSplashScreenState::Hidden;

	thread->Start ();
}

GXVoid GXSplashScreen::FillRGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const
{
	GXUPointer lineSizeDst = (GXUPointer)( BGRA_BYTES_PER_PIXEL * width );
	GXUPointer lineSizeSrc = (GXUPointer)( RGB_BYTES_PER_PIXEL * width );

	GXUByte* dst = (GXUByte*)malloc ( lineSizeDst * height );
	GXUPointer offsetDst = 0;
	GXUPointer offsetSrc = ( height - 1 ) * lineSizeSrc;

	for ( GXUShort h = 0; h < height; h++ )
	{
		for ( GXUShort w = 0; w < width; w++ )
		{
			// BGRA model
			dst[ offsetDst ] = source[ offsetSrc + 2 ];
			dst[ offsetDst + 1 ] = source[ offsetSrc + 1 ];
			dst[ offsetDst + 2 ] = source[ offsetSrc ];
			dst[ offsetDst + 3 ] = ALPHA_FOR_RGB;

			offsetDst += BGRA_BYTES_PER_PIXEL;
			offsetSrc += RGB_BYTES_PER_PIXEL;
		}

		offsetSrc -= 2 * lineSizeSrc;
	}

	*destination = dst;
}

GXVoid GXSplashScreen::FillARGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const
{
	GXUPointer lineSize = (GXUPointer)( BGRA_BYTES_PER_PIXEL * width );
	GXUByte* dst = (GXUByte*)malloc ( lineSize * height );
	GXUPointer offsetDst = 0;
	GXUPointer offsetSrc = ( height - 1 ) * lineSize;

	for ( GXUShort h = 0; h < height; h++ )
	{
		for ( GXUShort w = 0; w < width; w++ )
		{
			// BGRA model
			dst[ offsetDst ] = source[ offsetSrc + 2 ];
			dst[ offsetDst + 1 ] = source[ offsetSrc + 1 ];
			dst[ offsetDst + 2 ] = source[ offsetSrc ];
			dst[ offsetDst + 3 ] = source[ offsetSrc + 3 ];

			offsetDst += BGRA_BYTES_PER_PIXEL;
			offsetSrc += BGRA_BYTES_PER_PIXEL;
		}

		offsetSrc -= 2 * lineSize;
	}

	*destination = dst;
}

GXUPointer GXTHREADCALL GXSplashScreen::MessageLoop ( GXVoid* arg, GXThread &thread )
{
	GXSplashScreen* splashScreen = (GXSplashScreen*)arg;
	splashScreen->hwnd = (HWND)INVALID_HANDLE_VALUE;

	eGXStatus status = eGXStatus::Success;

	GXUByte numChannels = 0;
	GXUByte* data = nullptr;

	GXBool loopFlag = GX_TRUE;
	GXUInt w = 0;
	GXUInt h = 0;

	while ( loopFlag )
	{
		if ( !GXLoadLDRImage ( SPLASH_SCREEN_IMAGE, w, h, numChannels, &data ) )
		{
			status = eGXStatus::CanNotLoadImage;
			loopFlag = GX_FALSE;

			continue;
		}

		splashScreen->bitmapWidth = (GXUShort)w;
		splashScreen->bitmapHeight = (GXUShort)h;

		splashScreen->pixels = nullptr;

		switch ( numChannels )
		{
			case 3:
				splashScreen->FillRGB ( &splashScreen->pixels, data, splashScreen->bitmapWidth, splashScreen->bitmapHeight );
			break;

			case 4:
				splashScreen->FillARGB ( &splashScreen->pixels, data, splashScreen->bitmapWidth, splashScreen->bitmapHeight );
			break;

			default:
				free ( data );
				status = eGXStatus::InvalidChannels;
				loopFlag = GX_FALSE;
				continue;
			break;
		}

		free ( data );

		splashScreen->bitmap = CreateBitmap ( (int)splashScreen->bitmapWidth, (int)splashScreen->bitmapHeight, NUMBER_PLANES, BGRA_BYTES_PER_PIXEL * BITS_PER_CHANNEL, splashScreen->pixels );

		HINSTANCE hInst = GetModuleHandle ( 0 );

		WNDCLASSW windowClass;
		memset ( &windowClass, 0, sizeof ( WNDCLASSW ) );
		windowClass.hInstance = hInst;
		windowClass.lpszClassName = SPLASH_SCREEN_WINDOW_CLASS;
		windowClass.lpfnWndProc = &GXSplashScreen::WindowProc;
		windowClass.hCursor = LoadCursorW ( 0, IDC_ARROW );
		windowClass.hIcon = LoadIconW ( hInst, MAKEINTRESOURCE ( GX_RID_EXE_MAINICON ) );

		if ( !RegisterClassW ( &windowClass ) )
		{
			splashScreen->hwnd = (HWND)INVALID_HANDLE_VALUE;
			status = eGXStatus::CanNotRegisterClass;
			loopFlag = GX_FALSE;

			continue;
		}

		POINT zeroPoint;
		zeroPoint.x = 0;
		zeroPoint.y = 0;

		HMONITOR primaryMonitor = MonitorFromPoint ( zeroPoint, MONITOR_DEFAULTTOPRIMARY );
		MONITORINFO monitorinfo;
		monitorinfo.cbSize = sizeof ( monitorinfo );
		GetMonitorInfoW ( primaryMonitor, &monitorinfo );

		GXInt x = monitorinfo.rcWork.left + ( monitorinfo.rcWork.right - monitorinfo.rcWork.left - splashScreen->bitmapWidth ) / 2;
		GXInt y = monitorinfo.rcWork.top + ( monitorinfo.rcWork.bottom - monitorinfo.rcWork.top - splashScreen->bitmapHeight ) / 2;

		DWORD dwExStyle = WS_EX_APPWINDOW;
		DWORD dwStyle = WS_POPUP;

		splashScreen->hwnd = CreateWindowExW ( dwExStyle, SPLASH_SCREEN_WINDOW_CLASS, nullptr, dwStyle, x, y, (int)splashScreen->bitmapWidth, (int)splashScreen->bitmapHeight, (HWND)0, (HMENU)NULL, hInst, nullptr );
		SetWindowLongPtrW ( splashScreen->hwnd, GWLP_USERDATA, (LONG_PTR)splashScreen );

		if ( !splashScreen->hwnd )
		{
			splashScreen->hwnd = (HWND)INVALID_HANDLE_VALUE;
			status = eGXStatus::CanNotCreateWindow;
			loopFlag = GX_FALSE;

			continue;
		}

		while ( loopFlag )
		{
			MSG msg;
			PeekMessageW ( &msg, splashScreen->hwnd, 0, 0, PM_REMOVE );

			TranslateMessage ( &msg );
			DispatchMessageW ( &msg );

			if ( msg.message == WM_QUIT )
			{
				loopFlag = GX_FALSE;
				break;
			}

			if ( splashScreen->state == splashScreen->intend ) continue;

			switch ( splashScreen->intend )
			{
				case eGXSplashScreenState::Visible:
					ShowWindow ( splashScreen->hwnd, SW_SHOW );
					splashScreen->state = splashScreen->intend;
				break;

				case eGXSplashScreenState::Hidden:
					ShowWindow ( splashScreen->hwnd, SW_HIDE );
					splashScreen->state = splashScreen->intend;
				break;

				default:
					// NOTHING
				break;
			}

			thread.Switch ();
		}
	}

	switch ( status )
	{
		case eGXStatus::Success:
		{
			DeleteObject ( splashScreen->bitmap );
			free ( splashScreen->pixels );

			if ( !DestroyWindow ( splashScreen->hwnd ) )
			{
				GXLogW ( L"GXSplashScreen::MessageLoop::Error - ќсвобождение HWND провалено.\n" );
				status = eGXStatus::CanNotFreeWindowResources;
			}

			if ( !UnregisterClassW ( SPLASH_SCREEN_WINDOW_CLASS, GetModuleHandle ( 0 ) ) )
			{
				GXLogW ( L"GXSplashScreen::MessageLoop::Error - —н€тие регистрации класса окна провалено (ветвь 1).\n" );
				status = eGXStatus::CanNotFreeWindowResources;
			}
		}
		break;

		case eGXStatus::CanNotLoadImage:
			GXLogW ( L"GXSplashScreen::MessageLoop::Error - Ќе могу загрузить изображение splash screen'а %s.\n", SPLASH_SCREEN_IMAGE );
		break;

		case eGXStatus::InvalidChannels:
			GXLogW ( L"GXSplashScreen::GXSplashScreen:Error - »зображение splash screen'а должно иметь 3 или 4 канала (тукущее %hhu).\n", numChannels );
		break;

		case eGXStatus::CanNotRegisterClass:
			GXLogW ( L"GXSplashScreen::MessageLoop::Error - Ќе удалось зарегистрировать класс окна.\n" );
		break;

		case eGXStatus::CanNotCreateWindow:
			GXLogW ( L"GXSplashScreen::MessageLoop::Error - Ќе могу создать окно.\n" );

			if ( !UnregisterClassW ( SPLASH_SCREEN_WINDOW_CLASS, GetModuleHandle ( 0 ) ) )
			{
				GXLogW ( L"GXSplashScreen::~GXSplashScreen::Error - —н€тие регистрации класса окна провалено (ветвь 2).\n" );
				status = eGXStatus::CanNotFreeWindowResources;
			}
		break;

		case eGXStatus::CanNotFreeWindowResources:
			// NOTHING
		break;

		default:
			// NOTHING
		break;
	}

	return (GXUPointer)status;
}

LRESULT CALLBACK GXSplashScreen::WindowProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
		case WM_PAINT:
		{
			GXSplashScreen* splashScreen = (GXSplashScreen*)GetWindowLongPtrW ( hwnd, GWLP_USERDATA );

			PAINTSTRUCT paintStruct;

			HDC splashScreenDC = BeginPaint ( hwnd, &paintStruct );
			HDC bitmapDC = CreateCompatibleDC ( splashScreenDC );
			HBITMAP oldBitmap = (HBITMAP)SelectObject ( bitmapDC, splashScreen->bitmap );

			if ( !BitBlt ( splashScreenDC, 0, 0, (int)splashScreen->bitmapWidth, (int)splashScreen->bitmapHeight, bitmapDC, 0, 0, SRCCOPY ) )
				GXLogW ( L"GXSplashScreen::WindowProc::Error - BitBlt не выполнилась.\n" );

			SelectObject ( bitmapDC, oldBitmap );
			DeleteDC ( bitmapDC );

			EndPaint ( hwnd, &paintStruct );
		}
		return 0;

		default:
			return DefWindowProcW ( hwnd, message, wParam, lParam );
	}
}
