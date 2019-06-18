// version 1.5

#ifndef GX_SPLASH_SCREEN
#define GX_SPLASH_SCREEN


#include <GXCommon/GXThread.h>
#include <GXCommon/GXMemory.h>


enum class eGXSplashScreenState : GXUByte
{
    Visible,
    Hidden
};

class GXSplashScreen final : public GXMemoryInspector
{
    private:
        HWND                        _hwnd;
        HBITMAP                     _bitmap;
        GXUShort                    _bitmapWidth;
        GXUShort                    _bitmapHeight;
        GXUByte*                    _pixels;

        eGXSplashScreenState        _state;
        eGXSplashScreenState        _intend;

        GXThread*                   _thread;
        static GXSplashScreen*      _instance;

    public:
        static GXSplashScreen& GXCALL GetInstance ();
        ~GXSplashScreen ();

        GXVoid Show ();
        GXVoid Hide ();

    private:
        GXSplashScreen ();

        GXVoid FillRGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height );
        GXVoid FillARGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height );

        static GXUPointer GXTHREADCALL MessageLoop ( GXVoid* arg, GXThread &thread );
        static LRESULT CALLBACK WindowProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

        GXSplashScreen ( const GXSplashScreen &other ) = delete;
        GXSplashScreen& operator = ( const GXSplashScreen &other ) = delete;
};


#endif //GX_SPLASH_SCREEN
