// version 1.3

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

//---------------------------------------------------------------------------------------------------------------

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
    if ( hwnd == static_cast<HWND> ( INVALID_HANDLE_VALUE ) ) return;

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

GXSplashScreen::GXSplashScreen ():
    hwnd ( static_cast<HWND> ( INVALID_HANDLE_VALUE ) ),
    bitmap ( static_cast<HBITMAP> ( INVALID_HANDLE_VALUE ) ),
    bitmapWidth ( 0u ),
    bitmapHeight ( 0u ),
    pixels ( nullptr ),
    state ( eGXSplashScreenState::Hidden ),
    intend ( eGXSplashScreenState::Hidden )
{
    thread = new GXThread ( &GXSplashScreen::MessageLoop, this );
    thread->Start ();
}

GXVoid GXSplashScreen::FillRGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const
{
    GXUPointer lineSizeDst = static_cast<GXUPointer> ( BGRA_BYTES_PER_PIXEL * width );
    GXUPointer lineSizeSrc = static_cast<GXUPointer> ( RGB_BYTES_PER_PIXEL * width );

    GXUByte* dst = static_cast<GXUByte*> ( malloc ( lineSizeDst * height ) );
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

GXVoid GXSplashScreen::FillARGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const
{
    GXUPointer lineSize = static_cast<GXUPointer> ( BGRA_BYTES_PER_PIXEL * width );
    GXUByte* dst = static_cast<GXUByte*> ( malloc ( lineSize * height ) );
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
    splashScreen->hwnd = static_cast<HWND> ( INVALID_HANDLE_VALUE );

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

        splashScreen->bitmapWidth = static_cast<GXUShort> ( w );
        splashScreen->bitmapHeight = static_cast<GXUShort> ( h );

        splashScreen->pixels = nullptr;

        switch ( numChannels )
        {
            case 3u:
                splashScreen->FillRGB ( &splashScreen->pixels, data, splashScreen->bitmapWidth, splashScreen->bitmapHeight );
            break;

            case 4u:
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
        windowClass.hCursor = LoadCursor ( 0, IDC_ARROW );
        windowClass.hIcon = LoadIcon ( hInst, MAKEINTRESOURCE ( GX_RID_EXE_MAINICON ) );

        if ( !RegisterClassW ( &windowClass ) )
        {
            splashScreen->hwnd = static_cast<HWND> ( INVALID_HANDLE_VALUE );
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
        SetWindowLongPtrW ( splashScreen->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> ( splashScreen ) );

        if ( !splashScreen->hwnd )
        {
            splashScreen->hwnd = static_cast<HWND> ( INVALID_HANDLE_VALUE );
            status = eGXStatus::CanNotCreateWindow;
            loopFlag = GX_FALSE;

            continue;
        }

        while ( loopFlag )
        {
            MSG msg;
            PeekMessageW ( &msg, splashScreen->hwnd, 0, 0u, PM_REMOVE );

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

            if ( !UnregisterClassW ( SPLASH_SCREEN_WINDOW_CLASS, GetModuleHandleW ( nullptr ) ) )
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

            if ( !UnregisterClassW ( SPLASH_SCREEN_WINDOW_CLASS, GetModuleHandleW ( nullptr ) ) )
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
            HBITMAP oldBitmap = static_cast<HBITMAP> ( SelectObject ( bitmapDC, splashScreen->bitmap ) );

            if ( !BitBlt ( splashScreenDC, 0, 0, static_cast<int> ( splashScreen->bitmapWidth ), static_cast<int> ( splashScreen->bitmapHeight ), bitmapDC, 0, 0, SRCCOPY ) )
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
