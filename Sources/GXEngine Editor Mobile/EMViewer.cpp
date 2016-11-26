#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine_Editor_Mobile/EMGlobals.h>
#include <GXEngine/GXUICommon.h>


#define EM_VIEWER_ZOOM_MODE				0
#define EM_VIEWER_ROTATE_MODE			1
#define EM_VIEWER_PAN_MODE				2
#define EM_VIEWER_NOTHING_MODE			0xFF

#define EM_VIEWER_ZOOM_STEP				0.2f
#define EM_VIEWER_MIN_DISTANCE			0.1f
#define EM_VIEWER_MAX_DISTANCE			1.0e+30f
#define EM_VIEWER_ROTATION_SPEED		45.0f		//45 degrees per centimeter
#define EM_VIEWER_PAN_SPEED				0.2f		//0.2 unit per centimeter


EMViewer*	em_vc_Me = 0;


EMViewer::EMViewer ( PFNEMONVIEWERTRANSFORMCHANGEDPROC callback )
{
	OnViewerTransformChanged = callback;

	pitch = yaw = 0.0f;
	distance = 7.0f;
	target = 0;
	origin = GXCreateVec3 ( 2.0f, -1.0f, 0.0f );

	camera = new GXCameraPerspective ( GXDegToRad ( 60.0f ), gx_Core->GetRenderer()->GetWidth () / (GXFloat)gx_Core->GetRenderer()->GetHeight (), 0.1f, 1000.0f );

	UpdateCamera ();

	rotationSpeed = GXDegToRad ( EM_VIEWER_ROTATION_SPEED ) / gx_ui_Scale;
	panSpeed = EM_VIEWER_PAN_SPEED / gx_ui_Scale;
	zoomSpeed = EM_VIEWER_ZOOM_STEP;

	isAltPressed = isMMBPressed = GX_FALSE;

	em_vc_Me = this;
}

EMViewer::~EMViewer ()
{
	em_vc_Me = 0;
	delete camera;
}

GXVoid EMViewer::SetTarget ( EMActor* actor )
{
	this->target = actor;
}

GXVoid EMViewer::CaptureInput ()
{
	GXInput* input = gx_Core->GetInput ();

	input->BindMouseButtonsFunc ( &OnMouseButton );
	input->BindMouseWheelFunc ( &OnMouseWheel );
	input->BindMouseMoveFunc ( &OnMouseMove );

	input->BindKeyFunc ( &OnAltDown, 0, VK_MENU, INPUT_DOWN );
	input->BindKeyFunc ( &OnAltUp, 0, VK_MENU, INPUT_UP );
}

GXCamera* EMViewer::GetCamera ()
{
	return camera;
}

GXVoid EMViewer::UpdateMouse ( const GXVec2 &position )
{
	memcpy ( &mousePosition, &position, sizeof ( GXVec2 ) );
}

GXVoid EMViewer::OnPan ( const GXVec2& mouseDelta )
{
	const GXMat4& cameraTransform = camera->GetModelMatrix ();

	GXVec3 deltaRight;
	GXMulVec3Scalar ( deltaRight, cameraTransform.xv, -mouseDelta.x * panSpeed );
	GXVec3 deltaUp;
	GXMulVec3Scalar ( deltaUp, cameraTransform.yv, -mouseDelta.y * panSpeed );

	GXSumVec3Vec3 ( origin, origin, deltaRight );
	GXSumVec3Vec3 ( origin, origin, deltaUp );

	UpdateCamera ();
	OnViewerTransformChanged ();
}

GXVoid EMViewer::OnRotate ( const GXVec2& mouseDelta )
{
	GXFloat pitchDelta = -mouseDelta.y * rotationSpeed;
	GXFloat yawDelta = mouseDelta.x * rotationSpeed;

	pitch = FixPitch ( pitch + pitchDelta );
	yaw = FixYaw ( yaw + yawDelta );

	GXVec3 targetLocation;
	if ( target )
		targetLocation = target->GetTransform ().wv;
	else
		targetLocation = origin;

	GXVec3 targetOriginView;
	GXMulVec3Mat4AsPoint ( targetOriginView, targetLocation, camera->GetViewMatrix () );

	UpdateCamera ();
	GXVec3 tempTargetWorld;
	GXMulVec3Mat4AsPoint ( tempTargetWorld, targetOriginView, camera->GetModelMatrix () );

	GXVec3 deltaTargetWorld;
	GXSubVec3Vec3 ( deltaTargetWorld, targetLocation, tempTargetWorld );

	GXSumVec3Vec3 ( origin, origin, deltaTargetWorld );

	UpdateCamera ();
	OnViewerTransformChanged ();
}

GXVoid EMViewer::OnZoom ( GXInt mouseWheelSteps )
{
	distance = GXClampf ( distance - EM_VIEWER_ZOOM_STEP * mouseWheelSteps, EM_VIEWER_MIN_DISTANCE, EM_VIEWER_MAX_DISTANCE );
	UpdateCamera ();
	OnViewerTransformChanged ();
}

GXVoid EMViewer::UpdateCamera ()
{
	GXMat4 matrix;
	GXSetMat4RotationXY ( matrix, pitch, yaw );

	GXVec3 stick;
	GXMulVec3Scalar ( stick, matrix.zv, distance );

	GXSubVec3Vec3 ( matrix.wv, origin, stick );

	camera->SetModelMatrix ( matrix );
}

GXFloat EMViewer::FixPitch ( GXFloat pitch )
{
	return GXClampf ( pitch, -GX_MATH_HALFPI, GX_MATH_HALFPI );
}

GXFloat EMViewer::FixYaw ( GXFloat yaw )
{
	while ( yaw < -GX_MATH_PI ) yaw += GX_MATH_TWO_PI;
	while ( yaw > GX_MATH_PI ) yaw -= GX_MATH_TWO_PI;
	return yaw;
}

GXUByte EMViewer::ResolveMode ( GXBool isAltPressed, GXBool isMMBPressed, GXBool isWheel )
{
	if ( isWheel ) return EM_VIEWER_ZOOM_MODE;
	if ( isAltPressed && isMMBPressed ) return EM_VIEWER_ROTATE_MODE;
	if ( isMMBPressed ) return EM_VIEWER_PAN_MODE;
	return EM_VIEWER_NOTHING_MODE;
}

GXVoid GXCALL EMViewer::OnMouseMove ( GXInt win_x, GXInt win_y )
{
	GXVec2 mousePosition;
	mousePosition.x = (GXFloat)win_x;
	mousePosition.y = (GXFloat)( gx_Core->GetRenderer ()->GetHeight () - win_y );

	switch ( em_vc_Me->ResolveMode ( em_vc_Me->isAltPressed, em_vc_Me->isMMBPressed, GX_FALSE ) )
	{
		case EM_VIEWER_ROTATE_MODE:
		{
			GXVec2 delta;
			GXSubVec2Vec2 ( delta, mousePosition, em_vc_Me->mousePosition );
			em_vc_Me->OnRotate ( delta );
		}
		break;

		case EM_VIEWER_PAN_MODE:
		{
			GXVec2 delta;
			GXSubVec2Vec2 ( delta, mousePosition, em_vc_Me->mousePosition );
			em_vc_Me->OnPan ( delta );
		}
		break;

		default:
			// NOTHING
		break;
	}

	em_vc_Me->UpdateMouse ( mousePosition );
}

GXVoid GXCALL EMViewer::OnMouseButton ( EGXInputMouseFlags mouseflags )
{
	em_vc_Me->isMMBPressed = mouseflags.mmb;
}

GXVoid GXCALL EMViewer::OnMouseWheel ( GXInt steps )
{
	if ( em_vc_Me->ResolveMode ( em_vc_Me->isAltPressed, em_vc_Me->isMMBPressed, GX_TRUE ) == EM_VIEWER_ZOOM_MODE )
		em_vc_Me->OnZoom ( steps );
}

GXVoid GXCALL EMViewer::OnAltDown ( GXVoid* handler )
{
	em_vc_Me->isAltPressed = GX_TRUE;
}

GXVoid GXCALL EMViewer::OnAltUp ( GXVoid* handler )
{
	em_vc_Me->isAltPressed = GX_FALSE;
}
