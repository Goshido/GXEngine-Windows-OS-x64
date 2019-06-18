// version 1.17

#ifndef GX_RENDERER
#define GX_RENDERER


#include "GXGame.h"
#include <GXCommon/GXThread.h>


class GXRendererResolutions final
{
    friend class GXRenderer;

    private:
        GXUShort        _total;
        GXUShort*       _wxh;

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

class GXRenderer final
{    
    private:
        static GXGame*          _game;

        static GXThread*        _thread;
        static GXBool           _loopFlag;

        static HWND             _hwnd;
        static HGLRC            _hglRC;
        static HINSTANCE        _hinst;
        static HDC              _hDC;

        static GXBool           _isFullScreen;
        static GXBool           _isSettingsChanged;

        static GXInt            _width;
        static GXInt            _height;
        static GXInt            _vsync;

        static GXWChar*         _title;

        static GXDouble         _lastTime;
        static GXDouble         _accumulator;
        static GXUInt           _currentFPS;
        static GXUShort         _fpsCounter;
        static GXDouble         _fpsTimer;

        static GXBool           _isRenderableObjectInited;

        static GXRenderer*      _instance;

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
