#ifndef EM_VIEWER
#define EM_VIEWER


#include <GXEngine/GXCameraPerspective.h>
#include <GXEngine/GXInput.h>
#include "EMActor.h"


typedef GXVoid ( GXCALL* PFNEMONVIEWERTRANSFORMCHANGEDPROC ) ();


class EMViewer
{
	private:
		GXFloat								pitch;
		GXFloat								yaw;
		GXFloat								distance;
		GXVec3								origin;

		GXFloat								rotationSpeed;
		GXFloat								panSpeed;
		GXFloat								zoomSpeed;

		GXCameraPerspective*				camera;
		EMActor*							target;
		PFNEMONVIEWERTRANSFORMCHANGEDPROC	OnViewerTransformChanged;

		GXBool								isAltPressed;
		GXBool								isMMBPressed;

		GXVec2								mousePosition;

		static EMViewer*					instance;

	public:
		~EMViewer ();

		GXVoid SetTarget ( EMActor* actor );
		GXVoid SetOnViewerTransformChangedCallback ( PFNEMONVIEWERTRANSFORMCHANGEDPROC callback );
		GXVoid CaptureInput ();
		GXCamera* GetCamera ();
		GXVoid UpdateMouse ( const GXVec2 &position );

		static EMViewer* GetInstance ();

	private:
		explicit EMViewer ();

		GXVoid OnPan ( const GXVec2& mouseDelta );
		GXVoid OnRotate ( const GXVec2& mouseDelta );
		GXVoid OnZoom ( GXInt mouseWheelSteps );

		GXVoid UpdateCamera ();
		GXFloat FixPitch ( GXFloat pitch );
		GXFloat FixYaw ( GXFloat yaw );

		GXUByte ResolveMode ( GXBool isAltPressed, GXBool isMMBPressed, GXBool isWheel );

		static GXVoid GXCALL OnMouseMove ( GXInt win_x, GXInt win_y );
		static GXVoid GXCALL OnMouseButton ( EGXInputMouseFlags mouseflags );
		static GXVoid GXCALL OnMouseWheel ( GXInt steps );

		static GXVoid GXCALL OnAltDown ( GXVoid* handler );
		static GXVoid GXCALL OnAltUp ( GXVoid* handler );
};


#endif //EM_VIEWER
