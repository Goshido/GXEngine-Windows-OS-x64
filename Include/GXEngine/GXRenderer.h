//version 1.14

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
};

class GXRenderer
{	
	private:
		static GXGame*		game;

		static GXThread*	thread;
		static GXBool		loopFlag;

		static HWND			hwnd;
		static HGLRC		hglRC;
		static HINSTANCE	hinst;
		static HDC			hDC;

		static GXBool		isFullScreen;
		static GXBool		isSettingsChanged;

		static GXInt		width;
		static GXInt		height;
		static GXInt		vsync;

		static GXWChar*		title;

		static GXDouble		lastTime;
		static GXDouble		accumulator;
		static GXUInt		currentFPS;
		static GXUShort		fpsCounter;
		static GXDouble		fpsTimer;

		static GXBool		isRenderableObjectInited;

		static GXRenderer*	instance;

	public:
		~GXRenderer ();

		GXVoid Start ( GXGame &game );
		GXBool Shutdown ();

		GXVoid SetFullscreen ( GXBool enabled );
		GXVoid SetVSync ( GXBool enabled );
		GXVoid SetResolution ( GXInt width, GXInt height );
		GXVoid SetWindowName ( const GXWChar* name );

		GXUInt GetCurrentFPS () const;
		GXVoid GetSupportedResolutions ( GXRendererResolutions &out ) const;

		GXInt GetWidth () const;
		GXInt GetHeight () const;
		static GXBool GXCALL UpdateRendererSettings ();
		static GXVoid GXCALL ReSizeScene ( GXInt width, GXInt height );

		static GXRenderer* GXCALL GetInstance ();

	private:
		GXRenderer ();

		static GXDword GXTHREADCALL RenderLoop ( GXVoid* args );
		static GXVoid GXCALL InitOpenGL ();
		static GXVoid GXCALL DrawScene ();
		static GXVoid GXCALL Destroy ();
		static GXBool GXCALL MakeWindow ();
		static GXVoid GXCALL InitRenderableObjects ();
		static GXVoid GXCALL DeleteRenderableObjects ();
};


#endif //GX_RENDERER
