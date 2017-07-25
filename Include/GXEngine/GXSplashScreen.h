//version 1.0

#ifndef GX_SPLASH_SCREEN
#define GX_SPLASH_SCREEN


#include <GXCommon/GXThread.h>


class GXSplashScreen
{
	private:
		GXThread			thread;
		static HWND			hwnd;
		static HBITMAP		bitmap;
		static GXUInt		bitmapWidth;
		static GXUInt		bitmapHeight;
		static GXUByte*		pixels;

	public:
		GXSplashScreen ();
		~GXSplashScreen ();

	private:
		GXVoid FillRGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const;
		GXVoid FillARGB ( GXUByte** destination, const GXUByte* source, GXUShort width, GXUShort height ) const;

		static GXUPointer GXTHREADCALL MessageLoop ( GXVoid* arg );
		static LRESULT CALLBACK WindowProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
};


#endif //GX_SPLASH_SCREEN
