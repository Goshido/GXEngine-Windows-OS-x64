#ifndef EM_UI
#define EM_UI


#include <GXEngine/GXWidget.h>
#include <GXEngine/GXHudSurface.h>


#define EM_UI_HUD_CAMERA_NEAR_Z     0.0f
#define EM_UI_HUD_CAMERA_FAR_Z      100.0f


class EMUI
{
    private:
        EMUI*       _next;
        EMUI*       _previous;

    protected:
        EMUI*       _parent;

    public:
        explicit EMUI ( EMUI* parent );
        virtual ~EMUI ();

        virtual GXWidget* GetWidget () const = 0;

        GXVoid ToForeground ();

    private:
        EMUI () = delete;
        EMUI ( const EMUI &other ) = delete;
        EMUI& operator = ( const EMUI &other ) = delete;
};

GXVoid GXCALL EMDrawUI ();


#endif // EM_UI
