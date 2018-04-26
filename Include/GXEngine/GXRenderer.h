// version 1.15

#ifndef GX_RENDERER
#define GX_RENDERER


#include "GXGame.h"
#include <GXCommon/GXThread.h>


class GXRendererResolutions
{
	friend class GXRenderer;

	private:
		GXUShort	total;
		GXUShort*	wxh;

	public:
		GXRendererResolutions ();
		~GXRendererResolutions ();

		GXVoid Cleanup ();

		GXUShort GetTotalResolutions ();
		GXVoid GetResolution ( GXUShort i, GXUShort &width, GXUShort &height );

	private:
		GXRendererResolutions ( const GXRendererResolutions &other ) = delete;
		GXRendererResolutions& operator = ( const GXRendererResolutions &other ) = delete;
};

class GXRenderer
{	
	private:
		static GXGame*				game;

		static GXThread*			thread;
		static GXBool				loopFlag;

		static HWND					hwnd;
		static HGLRC				hglRC;
		static HINSTANCE			hinst;
		static HDC					hDC;

		static GXBool				isFullScreen;
		static GXBool				isSettingsChanged;

		static GXInt				width;
		static GXInt				height;
		static GXInt				vsync;

		static GXWChar*				title;

		static GXDouble				lastTime;
		static GXDouble				accumulator;
		static GXUInt				currentFPS;
		static GXUShort				fpsCounter;
		static GXDouble				fpsTimer;

		static GXBool				isRenderableObjectInited;

		static GXRenderer*	instance;

	public:
		static GXRenderer& GXCALL GetInstance ();
		~GXRenderer ();

		GXVoid Start ( GXGame &gameObject );
		GXBool Shutdown ();

		GXVoid SetFullscreen ( GXBool enabled );
		GXVoid SetVSync ( GXBool enabled );
		GXVoid SetResolution ( GXInt frameWidth, GXInt frameHeight );
		GXVoid SetWindowName ( const GXWChar* name );
		GXVoid Show () const;
		GXVoid Hide () const;
		GXBool IsVisible () const;

		GXUInt GetCurrentFPS () const;
		GXVoid GetSupportedResolutions ( GXRendererResolutions &out ) const;

		GXInt GetWidth () const;
		GXInt GetHeight () const;

		static GXBool GXCALL UpdateRendererSettings ();
		static GXVoid GXCALL ReSizeScene ( GXInt frameWidth, GXInt frameHeight );
		static GXInt GXCDECLCALL GXResolutionCompare ( const GXVoid* a, const GXVoid* b );

	private:
		GXRenderer ();

		static GXUPointer GXTHREADCALL RenderLoop ( GXVoid* args, GXThread &threadObject );
		static GXVoid GXCALL InitOpenGL ();
		static GXVoid GXCALL DrawScene ();
		static GXVoid GXCALL Destroy ();
		static GXBool GXCALL MakeWindow ();
		static GXVoid GXCALL InitRenderableObjects ();
		static GXVoid GXCALL DeleteRenderableObjects ();
};


#endif //GX_RENDERER
