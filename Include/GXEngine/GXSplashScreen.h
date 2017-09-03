//version 1.0

#ifndef GX_SPLASH_SCREEN
#define GX_SPLASH_SCREEN


#include <GXCommon/GXThread.h>


enum class eGXSplashScreenState : GXUByte
{
	Visible,
	Hidden
};


class GXSplashScreen
{
	private:
		GXThread*				thread;
		HWND					hwnd;
		HBITMAP					bitmap;
		GXUShort				bitmapWidth;
		GXUShort				bitmapHeight;
		GXUByte*				pixels;

		eGXSplashScreenState	state;
		eGXSplashScreenState	intend;

		static GXSplashScreen*	instance;

	public:
		static GXSplashScreen& GetInstance ();
		~GXSplashScreen ();

		GXVoid Show ();
		GXVoid Hide ();

	private:
		GXSplashScreen ();

		GXVoid FillRGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const;
		GXVoid FillARGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const;

		static GXUPointer GXTHREADCALL MessageLoop ( GXVoid* arg, GXThread &thread );
		static LRESULT CALLBACK WindowProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
};


#endif //GX_SPLASH_SCREEN
