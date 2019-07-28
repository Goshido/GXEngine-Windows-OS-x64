// version 1.5

#include <GXEngine/GXSplashScreen.h>
#include <GXEngine/GXResource.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXImageLoader.h>
#include <GXCommon/GXLogger.h>


#define SPLASH_SCREEN_WINDOW_CLASS      L"GX_SPLASH_SCREEN_CLASS"
#define SPLASH_SCREEN_IMAGE             L"../../Textures/System/GXEngine splash screen.png"
#define BITS_PER_CHANNEL                8u
#define RGB_BYTES_PER_PIXEL             3u
#define BGRA_BYTES_PER_PIXEL            4u
#define NUMBER_PLANES                   1u
#define ALPHA_FOR_RGB                   255u

//---------------------------------------------------------------------------------------------------------------------

enum class eGXStatus : GXUByte
{
    Success = 0,
    CanNotLoadImage = 1,
    InvalidChannels = 2,
    CanNotRegisterClass = 3,
    CanNotCreateWindow = 4,
    CanNotFreeWindowResources = 5
};

//---------------------------------------------------------------------------------------------------------------------

GXSplashScreen* GXSplashScreen::_instance = nullptr;

GXSplashScreen& GXSplashScreen::GetInstance ()
{
    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXSplashScreen" )
        _instance = new GXSplashScreen ();
    }

    return *_instance;
}

GXSplashScreen::~GXSplashScreen ()
{
    if ( _hwnd == static_cast<HWND> ( INVALID_HANDLE_VALUE ) ) return;

    PostMessageW ( _hwnd, WM_QUIT, 0, 0 );
    _thread->Join ();

    delete _thread;

    _instance = nullptr;
}

GXVoid GXSplashScreen::Show ()
{
    _intend = eGXSplashScreenState::Visible;
}

GXVoid GXSplashScreen::Hide ()
{
    _intend = eGXSplashScreenState::Hidden;
}

GXSplashScreen::GXSplashScreen ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXSplashScreen" )
    _hwnd ( static_cast<HWND> ( INVALID_HANDLE_VALUE ) ),
    _bitmap ( static_cast<HBITMAP> ( INVALID_HANDLE_VALUE ) ),
    _bitmapWidth ( 0u ),
    _bitmapHeight ( 0u ),
    _pixels ( nullptr ),
    _state ( eGXSplashScreenState::Hidden ),
    _intend ( eGXSplashScreenState::Hidden )
{
    _thread = new GXThread ( &GXSplashScreen::MessageLoop, this );
    _thread->Start ();
}

GXVoid GXSplashScreen::FillRGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height )
{
    GXUPointer lineSizeDst = static_cast<GXUPointer> ( BGRA_BYTES_PER_PIXEL * width );
    GXUPointer lineSizeSrc = static_cast<GXUPointer> ( RGB_BYTES_PER_PIXEL * width );

    GXUByte* dst = static_cast<GXUByte*> ( Malloc ( lineSizeDst * height ) );
    GXUPointer offsetDst = 0u;
    GXUPointer offsetSrc = ( height - 1u ) * lineSizeSrc;

    for ( GXUShort h = 0u; h < height; ++h )
    {
        for ( GXUShort w = 0u; w < width; ++w )
        {
            // BGRA model
            dst[ offsetDst ] = source[ offsetSrc + 2u ];
            dst[ offsetDst + 1u ] = source[ offsetSrc + 1u ];
            dst[ offsetDst + 2u ] = source[ offsetSrc ];
            dst[ offsetDst + 3u ] = ALPHA_FOR_RGB;

            offsetDst += BGRA_BYTES_PER_PIXEL;
            offsetSrc += RGB_BYTES_PER_PIXEL;
        }

        offsetSrc -= 2u * lineSizeSrc;
    }

    *destination = dst;
}

GXVoid GXSplashScreen::FillARGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height )
{
    GXUPointer lineSize = static_cast<GXUPointer> ( BGRA_BYTES_PER_PIXEL * width );
    GXUByte* dst = static_cast<GXUByte*> ( Malloc ( lineSize * height ) );
    GXUPointer offsetDst = 0u;
    GXUPointer offsetSrc = ( height - 1u ) * lineSize;

    for ( GXUShort h = 0u; h < height; ++h )
    {
        for ( GXUShort w = 0u; w < width; ++w )
        {
            // BGRA model
            dst[ offsetDst ] = source[ offsetSrc + 2u ];
            dst[ offsetDst + 1u ] = source[ offsetSrc + 1u ];
            dst[ offsetDst + 2u ] = source[ offsetSrc ];
            dst[ offsetDst + 3u ] = source[ offsetSrc + 3u ];

            offsetDst += BGRA_BYTES_PER_PIXEL;
            offsetSrc += BGRA_BYTES_PER_PIXEL;
        }

        offsetSrc -= 2u * lineSize;
    }

    *destination = dst;
}

GXUPointer GXTHREADCALL GXSplashScreen::MessageLoop ( GXVoid* arg, GXThread &thread )
{
    GXSplashScreen* splashScreen = static_cast<GXSplashScreen*> ( arg );
    splashScreen->_hwnd = static_cast<HWND> ( INVALID_HANDLE_VALUE );

    eGXStatus status = eGXStatus::Success;

    GXUByte numChannels = 0u;
    GXUByte* data = nullptr;

    GXBool loopFlag = GX_TRUE;
    GXUInt w = 0u;
    GXUInt h = 0u;

    while ( loopFlag )
    {
        if ( !GXLoadLDRImage ( SPLASH_SCREEN_IMAGE, w, h, numChannels, &data ) )
        {
            status = eGXStatus::CanNotLoadImage;
            loopFlag = GX_FALSE;

            continue;
        }

        splashScreen->_bitmapWidth = static_cast<GXUShort> ( w );
        splashScreen->_bitmapHeight = static_cast<GXUShort> ( h );

        splashScreen->_pixels = nullptr;

        switch ( numChannels )
        {
            case 3u:
                splashScreen->FillRGB ( &splashScreen->_pixels, data, splashScreen->_bitmapWidth, splashScreen->_bitmapHeight );
            break;

            case 4u:
                splashScreen->FillARGB ( &splashScreen->_pixels, data, splashScreen->_bitmapWidth, splashScreen->_bitmapHeight );
            break;

            default:
                _instance->Free ( data );
                status = eGXStatus::InvalidChannels;
                loopFlag = GX_FALSE;
                continue;
            break;
        }

        _instance->Free ( data );

        splashScreen->_bitmap = CreateBitmap ( (int)splashScreen->_bitmapWidth, (int)splashScreen->_bitmapHeight, NUMBER_PLANES, BGRA_BYTES_PER_PIXEL * BITS_PER_CHANNEL, splashScreen->_pixels );

        HINSTANCE hInst = GetModuleHandle ( 0 );

        WNDCLASSW windowClass;
        memset ( &windowClass, 0, sizeof ( WNDCLASSW ) );
        windowClass.hInstance = hInst;
        windowClass.lpszClassName = SPLASH_SCREEN_WINDOW_CLASS;
        windowClass.lpfnWndProc = &GXSplashScreen::WindowProc;
        windowClass.hCursor = LoadCursor ( 0, IDC_ARROW );
        windowClass.hIcon = LoadIcon ( hInst, MAKEINTRESOURCE ( GX_RID_EXE_MAINICON ) );

        if ( !RegisterClassW ( &windowClass ) )
        {
            splashScreen->_hwnd = static_cast<HWND> ( INVALID_HANDLE_VALUE );
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

        GXInt x = monitorinfo.rcWork.left + ( monitorinfo.rcWork.right - monitorinfo.rcWork.left - splashScreen->_bitmapWidth ) / 2;
        GXInt y = monitorinfo.rcWork.top + ( monitorinfo.rcWork.bottom - monitorinfo.rcWork.top - splashScreen->_bitmapHeight ) / 2;

        DWORD dwExStyle = WS_EX_APPWINDOW;
        DWORD dwStyle = WS_POPUP;

        splashScreen->_hwnd = CreateWindowExW ( dwExStyle, SPLASH_SCREEN_WINDOW_CLASS, nullptr, dwStyle, x, y, (int)splashScreen->_bitmapWidth, (int)splashScreen->_bitmapHeight, (HWND)0, (HMENU)NULL, hInst, nullptr );
        SetWindowLongPtrW ( splashScreen->_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> ( splashScreen ) );

        if ( !splashScreen->_hwnd )
        {
            splashScreen->_hwnd = static_cast<HWND> ( INVALID_HANDLE_VALUE );
            status = eGXStatus::CanNotCreateWindow;
            loopFlag = GX_FALSE;

            continue;
        }

        while ( loopFlag )
        {
            MSG msg;
            PeekMessageW ( &msg, splashScreen->_hwnd, 0, 0u, PM_REMOVE );

            TranslateMessage ( &msg );
            DispatchMessageW ( &msg );

            if ( msg.message == WM_QUIT )
            {
                loopFlag = GX_FALSE;
                break;
            }

            if ( splashScreen->_state == splashScreen->_intend ) continue;

            switch ( splashScreen->_intend )
            {
                case eGXSplashScreenState::Visible:
                    ShowWindow ( splashScreen->_hwnd, SW_SHOW );
                    splashScreen->_state = splashScreen->_intend;
                break;

                case eGXSplashScreenState::Hidden:
                    ShowWindow ( splashScreen->_hwnd, SW_HIDE );
                    splashScreen->_state = splashScreen->_intend;
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
            DeleteObject ( splashScreen->_bitmap );
            _instance->Free ( splashScreen->_pixels );

            if ( !DestroyWindow ( splashScreen->_hwnd ) )
            {
                GXLogA ( "GXSplashScreen::MessageLoop::Error - Освобождение HWND провалено.\n" );
                status = eGXStatus::CanNotFreeWindowResources;
            }

            if ( !UnregisterClassW ( SPLASH_SCREEN_WINDOW_CLASS, GetModuleHandleW ( nullptr ) ) )
            {
                GXLogA ( "GXSplashScreen::MessageLoop::Error - Снятие регистрации класса окна провалено (ветвь 1).\n" );
                status = eGXStatus::CanNotFreeWindowResources;
            }
        }
        break;

        case eGXStatus::CanNotLoadImage:
            GXLogA ( "GXSplashScreen::MessageLoop::Error - Не могу загрузить изображение splash screen'а %S.\n", SPLASH_SCREEN_IMAGE );
        break;

        case eGXStatus::InvalidChannels:
            GXLogA ( "GXSplashScreen::GXSplashScreen:Error - Изображение splash screen'а должно иметь 3 или 4 канала (тeкущее %hhu).\n", numChannels );
        break;

        case eGXStatus::CanNotRegisterClass:
            GXLogA ( "GXSplashScreen::MessageLoop::Error - Не удалось зарегистрировать класс окна.\n" );
        break;

        case eGXStatus::CanNotCreateWindow:
            GXLogA ( "GXSplashScreen::MessageLoop::Error - Не могу создать окно.\n" );

            if ( !UnregisterClassW ( SPLASH_SCREEN_WINDOW_CLASS, GetModuleHandleW ( nullptr ) ) )
            {
                GXLogA ( "GXSplashScreen::~GXSplashScreen::Error - Снятие регистрации класса окна провалено (ветвь 2).\n" );
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

    return static_cast<GXUPointer> ( status );
}

LRESULT CALLBACK GXSplashScreen::WindowProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch ( message )
    {
        case WM_PAINT:
        {
            GXSplashScreen* splashScreen = reinterpret_cast<GXSplashScreen*> ( GetWindowLongPtrW ( hwnd, GWLP_USERDATA ) );

            PAINTSTRUCT paintStruct;

            HDC splashScreenDC = BeginPaint ( hwnd, &paintStruct );
            HDC bitmapDC = CreateCompatibleDC ( splashScreenDC );
            HBITMAP oldBitmap = static_cast<HBITMAP> ( SelectObject ( bitmapDC, splashScreen->_bitmap ) );

            if ( !BitBlt ( splashScreenDC, 0, 0, static_cast<int> ( splashScreen->_bitmapWidth ), static_cast<int> ( splashScreen->_bitmapHeight ), bitmapDC, 0, 0, SRCCOPY ) )
                GXLogA ( "GXSplashScreen::WindowProc::Error - BitBlt не выполнилась.\n" );

            SelectObject ( bitmapDC, oldBitmap );
            DeleteDC ( bitmapDC );

            EndPaint ( hwnd, &paintStruct );
        }
        return 0;

        default:
        return DefWindowProcW ( hwnd, message, wParam, lParam );
    }
}
