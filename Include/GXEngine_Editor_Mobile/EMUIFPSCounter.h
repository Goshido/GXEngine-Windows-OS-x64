#ifndef EM_UI_FPS_COUNTER
#define EM_UI_FPS_COUNTER


#include <GXEngine/GXHudSurface.h>


class EMUIFPSCounter final
{
    private:
        GXUInt                      _lastFPS;
        GXFont                      _font;

        GXHudSurface*               _surface;
        GXWChar                     _fpsBuffer[ 16u ];

        static EMUIFPSCounter*      _instance;

    public:
        static EMUIFPSCounter& GetInstance ();
        ~EMUIFPSCounter ();

        void Render ();

    private:
        EMUIFPSCounter ();

        EMUIFPSCounter ( const EMUIFPSCounter &other ) = delete;
        EMUIFPSCounter& operator = ( const EMUIFPSCounter &other ) = delete;
};


#endif // EM_UI_FPS_COUNTER
