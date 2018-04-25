#ifndef EM_VIEWER
#define EM_VIEWER


#include "EMActor.h"
#include <GXEngine/GXCameraPerspective.h>
#include <GXEngine/GXUIInput.h>


typedef GXVoid ( GXCALL* PFNEMONVIEWERTRANSFORMCHANGEDPROC ) ( GXVoid* handler );


class EMViewer
{
	private:
		GXUIInput*							inputWidget;

		GXVoid*								handler;
		PFNEMONVIEWERTRANSFORMCHANGEDPROC	OnViewerTransformChanged;

		GXFloat								pitch;
		GXFloat								yaw;
		GXFloat								distance;
		GXVec3								origin;

		EMActor*							target;

		GXFloat								rotationSpeed;
		GXFloat								panSpeed;
		GXFloat								zoomSpeed;

		GXCameraPerspective					camera;

		GXBool								isAltPressed;
		GXBool								isMMBPressed;

		GXVec2								mousePosition;

		static EMViewer*					instance;

	public:
		~EMViewer ();

		GXVoid SetInputWidget ( GXUIInput &newInputWidget );
		GXVoid SetTarget ( EMActor* actor );
		GXVoid SetOnViewerTransformChangedCallback ( GXVoid* handlerObject, PFNEMONVIEWERTRANSFORMCHANGEDPROC callback );
		GXCamera& GetCamera ();
		GXVoid UpdateMouse ( const GXVec2 &position );

		static EMViewer* GetInstance ();

	private:
		EMViewer ();

		GXVoid OnPan ( const GXVec2& mouseDelta );
		GXVoid OnRotate ( const GXVec2& mouseDelta );
		GXVoid OnZoom ( GXFloat mouseWheelSteps );

		GXVoid UpdateCamera ();
		GXFloat FixPitch ( GXFloat currentPitch );
		GXFloat FixYaw ( GXFloat currentYaw );

		GXUByte ResolveMode ( GXBool isAltDown, GXBool isMMBDown, GXBool isWheel );

		static GXVoid GXCALL OnLMBDownCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnLMBUpCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnMMBDownCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnMMBUpCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );

		static GXVoid GXCALL OnMouseScrollCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y, GXFloat scroll );
		static GXVoid GXCALL OnMouseMoveCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );

		static GXVoid GXCALL OnKeyDownCallback ( GXVoid* handler, GXUIInput& input, GXInt keyCode );
		static GXVoid GXCALL OnKeyUpCallback ( GXVoid* handler, GXUIInput& input, GXInt keyCode );

		EMViewer ( const EMViewer &other ) = delete;
		EMViewer& operator = ( const EMViewer &other ) = delete;
};


#endif // EM_VIEWER
