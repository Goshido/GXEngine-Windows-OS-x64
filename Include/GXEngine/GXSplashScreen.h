// version 1.4

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
        HWND                        hwnd;
        HBITMAP                     bitmap;
        GXUShort                    bitmapWidth;
        GXUShort                    bitmapHeight;
        GXUByte*                    pixels;

        eGXSplashScreenState        state;
        eGXSplashScreenState        intend;

        GXThread*                   thread;
        static GXSplashScreen*      instance;

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
