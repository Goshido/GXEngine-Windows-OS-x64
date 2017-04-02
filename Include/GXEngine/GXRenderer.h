//version 1.13

#ifndef GX_RENDERER
#define GX_RENDERER


#include <GXCommon/GXThread.h>


typedef GXBool ( GXCALL* PFNGXONFRAMEPROC ) ( GXFloat deltatime );
typedef GXVoid ( GXCALL* PFNGXONINITRENDERABLEOBJECTSPROC ) ();
typedef GXVoid ( GXCALL* PFNGXONDELETERENDERABLEOBJECTSPROC ) ();


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
		static GXThread*							thread;

		static GXBool								loopFlag;

		static HWND									hwnd;
		static HGLRC								hglRC;
		static HINSTANCE							hinst;
		static HDC									hDC;

		static GXBool								isFullScreen;
		static GXBool								isSettingsChanged;

		static GXInt								width;
		static GXInt								height;
		static GXInt								vsync;

		static GXWChar*								title;

		static GXDouble								lastTime;
		static GXDouble								accumulator;
		static GXUInt								currentFPS;

		static PFNGXONFRAMEPROC						onFrameFunc;
		static PFNGXONINITRENDERABLEOBJECTSPROC		onInitRenderableObjectsFunc;
		static PFNGXONDELETERENDERABLEOBJECTSPROC	onDeleteRenderableObjectsFunc;
		static GXBool								isRenderableObjectInited;

		static GXRenderer*							instance;

	public:
		~GXRenderer ();

		GXVoid Start ();
		GXVoid Suspend ();
		GXBool Shutdown ();

		GXVoid SetFullscreen ( GXBool enabled );
		GXVoid SetVSync ( GXBool enabled );
		GXVoid SetResolution ( GXInt width, GXInt height );
		GXVoid SetWindowName ( const GXWChar* name );

		GXVoid SetOnFrameFunc ( PFNGXONFRAMEPROC callback );
		GXVoid SetOnInitRenderableObjectsFunc ( PFNGXONINITRENDERABLEOBJECTSPROC callback );
		GXVoid SetOnDeleteRenderableObjectsFunc ( PFNGXONDELETERENDERABLEOBJECTSPROC callback );

		GXUInt GetCurrentFPS () const;
		GXVoid GetSupportedResolutions ( GXRendererResolutions &out ) const;

		GXInt GetWidth () const;
		GXInt GetHeight () const;
		static GXBool GXCALL UpdateRendererSettings ();
		static GXVoid GXCALL ReSizeScene ( GXInt width, GXInt height );

		static GXRenderer* GXCALL GetInstance ();

	private:
		explicit GXRenderer ();

		static GXDword GXTHREADCALL RenderLoop ( GXVoid* args );
		static GXVoid GXCALL InitOpenGL ();
		static GXBool GXCALL DrawScene ();
		static GXVoid GXCALL Destroy ();
		static GXBool GXCALL MakeWindow ();
		static GXVoid GXCALL InitRenderableObjects ();
		static GXVoid GXCALL DeleteRenderableObjects ();
};


#endif //GX_RENDERER
