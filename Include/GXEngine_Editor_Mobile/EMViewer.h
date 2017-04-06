#ifndef EM_VIEWER
#define EM_VIEWER


#include "EMActor.h"
#include <GXEngine/GXCameraPerspective.h>
#include <GXEngine/GXUIInput.h>


typedef GXVoid ( GXCALL* PFNEMONVIEWERTRANSFORMCHANGEDPROC ) ();


class EMViewer
{
	private:
		GXUIInput*							inputWidget;

		GXFloat								pitch;
		GXFloat								yaw;
		GXFloat								distance;
		GXVec3								origin;

		GXFloat								rotationSpeed;
		GXFloat								panSpeed;
		GXFloat								zoomSpeed;

		GXCameraPerspective					camera;
		EMActor*							target;
		PFNEMONVIEWERTRANSFORMCHANGEDPROC	OnViewerTransformChanged;

		GXBool								isAltPressed;
		GXBool								isMMBPressed;

		GXVec2								mousePosition;

		static EMViewer*					instance;

	public:
		~EMViewer ();

		GXVoid SetInputWidget ( GXUIInput &inputWidget );
		GXVoid SetTarget ( EMActor* actor );
		GXVoid SetOnViewerTransformChangedCallback ( PFNEMONVIEWERTRANSFORMCHANGEDPROC callback );
		GXCamera& GetCamera ();
		GXVoid UpdateMouse ( const GXVec2 &position );

		static EMViewer* GetInstance ();

	private:
		EMViewer ();

		GXVoid OnPan ( const GXVec2& mouseDelta );
		GXVoid OnRotate ( const GXVec2& mouseDelta );
		GXVoid OnZoom ( GXFloat mouseWheelSteps );

		GXVoid UpdateCamera ();
		GXFloat FixPitch ( GXFloat pitch );
		GXFloat FixYaw ( GXFloat yaw );

		GXUByte ResolveMode ( GXBool isAltPressed, GXBool isMMBPressed, GXBool isWheel );

		static GXVoid GXCALL OnLMBDownCallback ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnLMBUpCallback ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnMMBDownCallback ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnMMBUpCallback ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );

		static GXVoid GXCALL OnMouseScrollCallback ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y, GXFloat scroll );
		static GXVoid GXCALL OnMouseMoveCallback ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );

		static GXVoid GXCALL OnKeyDownCallback ( GXVoid* handler, GXUIInput* input, GXInt keyCode );
		static GXVoid GXCALL OnKeyUpCallback ( GXVoid* handler, GXUIInput* input, GXInt keyCode );
};


#endif //EM_VIEWER
