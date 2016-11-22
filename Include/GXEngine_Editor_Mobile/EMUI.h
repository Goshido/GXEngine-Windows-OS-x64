#ifndef EM_UI
#define EM_UI


#include <GXEngine/GXWidget.h>
#include <GXEngine/GXHudSurface.h>


#define EM_UI_HUD_CAMERA_NEAR_Z		0.0f
#define EM_UI_HUD_CAMERA_FAR_Z		100.0f


class EMUI
{
	friend GXVoid GXCALL EMDrawUIMasks ();

	private:
		EMUI*	next;
		EMUI*	prev;

	protected:
		EMUI*	parent;

	public:
		EMUI ( EMUI* parent );
		virtual ~EMUI ();

		virtual GXWidget* GetWidget ();
		virtual GXVoid OnDrawMask ();

		GXVoid ToForeground ();
};


GXVoid GXCALL EMDrawUI ();
GXVoid GXCALL EMDrawUIMasks ();
GXFloat GXCALL EMGetNextGUIForegroundZ ();
GXVoid GXCALL EMSetHudSurfaceLocationWorld ( GXHudSurface &surface, const GXAABB &widgetBoundsWorld, GXFloat z, GXInt viewportWidth, GXInt viewportHeight );


#endif //EM_UI
