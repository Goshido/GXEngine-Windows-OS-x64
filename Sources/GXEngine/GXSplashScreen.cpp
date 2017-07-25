//version 1.0

#include <GXEngine/GXSplashScreen.h>
#include <GXEngine/GXResource.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>


#define SPLASH_SCREEN_WINDOW_CLASS				L"GX_SPLASH_SCREEN_CLASS"
#define SPLASH_SCREEN_IMAGE						L"Textures/System/GXEngine splash screen.png"
#define CENTIMETERS_PER_METER					100.0f
#define BITS_PER_CHANNEL						8
#define RGB_BYTES_PER_PIXEL						3
#define ARGB_BYTES_PER_PIXEL					4
#define NUMBER_PLANES							1
#define NO_PALLETE_VALUE						0
#define MAXIMUM_COLOR_USED_VALUE				0
#define ALL_COLOR_IMPORTANT_VALUE				0
#define SOURCE_CONSTANT_ALPHA					255
#define DEFAULT_ALPHA_FOR_RGB					255
#define STATUS_SUCCESS							0
#define STATUS_CAN_NOT_LOAD_IMAGE				1
#define STATUS_INVALID_CHANNEL_NUMBER			2
#define STATUS_CAN_NOT_REGISTER_CLASS			3
#define STATUS_CAN_NOT_CREATE_WINDOW			4
#define STATUS_CAN_NOT_FREE_WINDOW_RESOURCES	5


HWND GXSplashScreen::hwnd = (HWND)INVALID_HANDLE_VALUE;
HBITMAP GXSplashScreen::bitmap;
GXUInt GXSplashScreen::bitmapWidth = 0;
GXUInt GXSplashScreen::bitmapHeight = 0;
GXUByte* GXSplashScreen::pixels = nullptr;


GXSplashScreen::GXSplashScreen () :
thread ( &GXSplashScreen::MessageLoop, this )
{
	hwnd = (HWND)INVALID_HANDLE_VALUE;
	thread.Start ();
}

GXSplashScreen::~GXSplashScreen ()
{
	if ( hwnd == (HWND)INVALID_HANDLE_VALUE ) return;

	PostMessageW ( hwnd, WM_QUIT, 0, 0 );
	thread.Join ();
}

GXVoid GXSplashScreen::FillRGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const
{
	GXUPointer lineSizeDst = (GXUPointer)( ARGB_BYTES_PER_PIXEL * width );
	GXUPointer lineSizeSrc = (GXUPointer)( RGB_BYTES_PER_PIXEL * width );

	GXUByte* dst = (GXUByte*)malloc ( lineSizeDst * height );
	GXUPointer offsetDst = 0;
	GXUPointer offsetSrc = width * height * RGB_BYTES_PER_PIXEL - lineSizeSrc;

	for ( GXUShort h = 0; h < height; h++ )
	{
		for ( GXUShort w = 0; w < width; w++ )
		{
			dst[ offsetDst ] = DEFAULT_ALPHA_FOR_RGB;
			dst[ offsetDst + 1 ] = source[ offsetSrc ];
			dst[ offsetDst + 2 ] = source[ offsetSrc + 1 ];
			dst[ offsetDst + 3 ] = source[ offsetSrc + 2 ];

			offsetDst += ARGB_BYTES_PER_PIXEL;
			offsetSrc += RGB_BYTES_PER_PIXEL;
		}

		offsetSrc -= 2 * lineSizeSrc;
	}

	*destination = dst;
}

GXVoid GXSplashScreen::FillARGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const
{
	GXUPointer lineSize = (GXUPointer)( ARGB_BYTES_PER_PIXEL * width );
	GXUByte* dst = (GXUByte*)malloc ( lineSize * height );
	GXUPointer offset = 0;

	for ( GXUShort h = 0; h < height; h++ )
	{
		for ( GXUShort w = 0; w < width; w++ )
		{
			dst[ offset ] = source[ offset + 3 ];
			dst[ offset + 1 ] = source[ offset ];
			dst[ offset + 2 ] = source[ offset + 1 ];
			dst[ offset + 3 ] = source[ offset + 2 ];

			offset += ARGB_BYTES_PER_PIXEL;
		}
	}

	*destination = dst;
}

GXUPointer GXTHREADCALL GXSplashScreen::MessageLoop ( GXVoid* arg )
{
	GXSplashScreen* splashScreen = (GXSplashScreen*)arg;
	hwnd = (HWND)INVALID_HANDLE_VALUE;

	GXUPointer result = STATUS_SUCCESS;

	GXUByte numChannels = 0;
	GXUByte* data = nullptr;

	GXBool loopFlag = GX_TRUE;

	while ( loopFlag )
	{
		if ( !GXLoadImage ( SPLASH_SCREEN_IMAGE, bitmapWidth, bitmapHeight, numChannels, &data ) )
		{
			result = STATUS_CAN_NOT_LOAD_IMAGE;
			loopFlag = GX_FALSE;

			continue;
		}

		pixels = nullptr;

		switch ( numChannels )
		{
			case 3:
				splashScreen->FillRGB ( &pixels, data, (GXUShort)bitmapWidth, (GXUShort)bitmapHeight );
			break;

			case 4:
				splashScreen->FillARGB ( &pixels, data, (GXUShort)bitmapWidth, (GXUShort)bitmapHeight );
			break;

			default:
				free ( data );
				result = STATUS_INVALID_CHANNEL_NUMBER;
				loopFlag = GX_FALSE;
				continue;
			break;
		}

		free ( data );

		bitmap = CreateBitmap ( (int)bitmapWidth, (int)bitmapHeight, NUMBER_PLANES, 4 * BITS_PER_CHANNEL, pixels );

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
			hwnd = (HWND)INVALID_HANDLE_VALUE;
			result = STATUS_CAN_NOT_REGISTER_CLASS;
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

		GXInt x = monitorinfo.rcWork.left + ( monitorinfo.rcWork.right - monitorinfo.rcWork.left - bitmapWidth ) / 2;
		GXInt y = monitorinfo.rcWork.top + ( monitorinfo.rcWork.bottom - monitorinfo.rcWork.top - bitmapHeight ) / 2;

		DWORD dwExStyle = WS_EX_APPWINDOW;
		DWORD dwStyle = WS_POPUP;

		hwnd = CreateWindowExW ( dwExStyle, SPLASH_SCREEN_WINDOW_CLASS, nullptr, dwStyle, x, y, (int)bitmapWidth, (int)bitmapHeight, (HWND)0, (HMENU)NULL, hInst, nullptr );

		if ( !hwnd )
		{
			hwnd = (HWND)INVALID_HANDLE_VALUE;
			result = STATUS_CAN_NOT_CREATE_WINDOW;
			loopFlag = GX_FALSE;

			continue;
		}

		ShowWindow ( hwnd, SW_SHOW );

		while ( loopFlag )
		{
			MSG msg;
			GXBool ret = GetMessageW ( &msg, hwnd, 0, 0 ) == TRUE;

			TranslateMessage ( &msg );
			DispatchMessageW ( &msg );

			if ( !ret || msg.message == WM_QUIT )
			{
				loopFlag = GX_FALSE;
				break;
			}
		}
	}

	switch ( result )
	{
		case STATUS_SUCCESS:
		{
			DeleteObject ( bitmap );
			free ( pixels );

			if ( hwnd && !DestroyWindow ( hwnd ) )
			{
				GXLogW ( L"GXSplashScreen::MessageLoop::Error - ќсвобождение HWND провалено.\n" );
				result = STATUS_CAN_NOT_FREE_WINDOW_RESOURCES;
			}

			if ( !UnregisterClassW ( SPLASH_SCREEN_WINDOW_CLASS, GetModuleHandle ( 0 ) ) )
			{
				GXLogW ( L"GXSplashScreen::MessageLoop::Error - —н€тие регистрации класса окна провалено (ветвь 1).\n" );
				result = STATUS_CAN_NOT_FREE_WINDOW_RESOURCES;
			}
		}
		break;

		case STATUS_CAN_NOT_LOAD_IMAGE:
			GXLogW ( L"GXSplashScreen::MessageLoop::Error - Ќе могу загрузить изображение splash screen'а %s.\n", SPLASH_SCREEN_IMAGE );
		break;

		case STATUS_INVALID_CHANNEL_NUMBER:
			GXLogW ( L"GXSplashScreen::GXSplashScreen:Error - »зображение splash screen'а должно иметь 3 или 4 канала (тукущее %hhu).\n", numChannels );
		break;

		case STATUS_CAN_NOT_REGISTER_CLASS:
			GXLogW ( L"GXSplashScreen::MessageLoop::Error - Ќе удалось зарегистрировать класс окна.\n" );
		break;

		case STATUS_CAN_NOT_CREATE_WINDOW:
			GXLogW ( L"GXSplashScreen::MessageLoop::Error - Ќе могу создать окно.\n" );

			if ( !UnregisterClassW ( SPLASH_SCREEN_WINDOW_CLASS, GetModuleHandle ( 0 ) ) )
			{
				GXLogW ( L"GXSplashScreen::~GXSplashScreen::Error - —н€тие регистрации класса окна провалено (ветвь 2).\n" );
				result = STATUS_CAN_NOT_FREE_WINDOW_RESOURCES;
			}
		break;

		default:
			//NOTHING
		break;
	}

	return result;
}

LRESULT CALLBACK GXSplashScreen::WindowProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;

			HDC splashScreenDC = BeginPaint ( hwnd, &ps );
			HDC bitmapDC = CreateCompatibleDC ( splashScreenDC );
			HBITMAP oldBitmap = (HBITMAP)SelectObject ( bitmapDC, bitmap );

			if ( !BitBlt ( splashScreenDC, 0, 0, (int)bitmapWidth, (int)bitmapHeight, bitmapDC, 0, 0, SRCCOPY ) )
				GXLogW ( L"GXSplashScreen::WindowProc::Error - BitBlt не выполнилась.\n" );

			SelectObject ( bitmapDC, oldBitmap );
			DeleteDC ( bitmapDC );

			EndPaint ( hwnd, &ps );
		}
		return 0;

		default:
			return DefWindowProcW ( hwnd, message, wParam, lParam );
	}
}
