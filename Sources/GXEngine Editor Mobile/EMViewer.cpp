#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine_Editor_Mobile/EMTool.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXUICommon.h>


#define EM_VIEWER_ZOOM_MODE				0
#define EM_VIEWER_ROTATE_MODE			1
#define EM_VIEWER_PAN_MODE				2
#define EM_VIEWER_NOTHING_MODE			0xFF

#define EM_VIEWER_ZOOM_STEP				2.0f
#define EM_VIEWER_MIN_DISTANCE			0.1f
#define EM_VIEWER_MAX_DISTANCE			1.0e+30f
#define EM_VIEWER_ROTATION_SPEED		45.0f		//45 degrees per centimeter
#define EM_VIEWER_PAN_SPEED				0.2f		//0.2 unit per centimeter
#define EM_VIEWER_FOVY_DEGREES			60.0f

#define EM_DEFAULT_DISTANCE				7.0f

#define EM_DEFAULT_ORIGIN_X				2.0f
#define EM_DEFAULT_ORIGIN_Y				-1.0f
#define EM_DEFAULT_ORIGIN_Z				0.0f

#define EM_DEFAULT_PITCH_RAD			0.0f
#define EM_DEFAULT_YAW_RAD				0.0f

#define EM_VIEWER_Z_NEAR				0.1f
#define EM_VIEWER_Z_FAR					1000.0f


EMViewer* EMViewer::instance = nullptr;

EMViewer::~EMViewer ()
{
	if ( inputWidget )
	{
		inputWidget->SetOnLeftMouseButtonDownCallback ( nullptr );
		inputWidget->SetOnLeftMouseButtonUpCallback ( nullptr );
		inputWidget->SetOnMiddleMouseButtonDownCallback ( nullptr );
		inputWidget->SetOnMiddleMouseButtonUpCallback ( nullptr );
		inputWidget->SetOnRightMouseButtonDownCallback ( nullptr );
		inputWidget->SetOnRightMouseButtonUpCallback ( nullptr );
		inputWidget->SetOnDoubleClickCallback ( nullptr );
		inputWidget->SetOnMouseMoveCallback ( nullptr );
		inputWidget->SetOnMouseScrollCallback ( nullptr );
		inputWidget->SetOnKeyDownCallback ( nullptr );
		inputWidget->SetOnKeyUpCallback ( nullptr );

		inputWidget->SetHandler ( nullptr );
	}

	instance = nullptr;
}

GXVoid EMViewer::SetInputWidget ( GXUIInput &inputWidget )
{
	inputWidget.SetOnLeftMouseButtonDownCallback ( &EMViewer::OnLMBDownCallback );
	inputWidget.SetOnLeftMouseButtonUpCallback ( &EMViewer::OnLMBUpCallback );
	inputWidget.SetOnMiddleMouseButtonDownCallback ( &EMViewer::OnMMBDownCallback );
	inputWidget.SetOnMiddleMouseButtonUpCallback ( &EMViewer::OnMMBUpCallback );
	inputWidget.SetOnMouseMoveCallback ( &EMViewer::OnMouseMoveCallback );
	inputWidget.SetOnMouseScrollCallback ( &EMViewer::OnMouseScrollCallback );
	inputWidget.SetOnKeyDownCallback ( &EMViewer::OnKeyDownCallback );
	inputWidget.SetOnKeyUpCallback ( &EMViewer::OnKeyUpCallback );

	inputWidget.SetHandler ( this );
	this->inputWidget = &inputWidget;
}

GXVoid EMViewer::SetTarget ( EMActor* actor )
{
	this->target = actor;
}

GXVoid EMViewer::SetOnViewerTransformChangedCallback ( PFNEMONVIEWERTRANSFORMCHANGEDPROC callback )
{
	OnViewerTransformChanged = callback;
}

GXCamera& EMViewer::GetCamera ()
{
	return camera;
}

GXVoid EMViewer::UpdateMouse ( const GXVec2 &position )
{
	memcpy ( &mousePosition, &position, sizeof ( GXVec2 ) );
}

EMViewer* EMViewer::GetInstance ()
{
	if ( !instance )
		instance = new EMViewer ();

	return instance;
}

EMViewer::EMViewer ()
{
	inputWidget = nullptr;

	OnViewerTransformChanged = nullptr;

	pitch = EM_DEFAULT_PITCH_RAD;
	yaw = EM_DEFAULT_YAW_RAD;
	distance = EM_DEFAULT_DISTANCE;
	target = nullptr;
	origin = GXCreateVec3 ( EM_DEFAULT_ORIGIN_X, EM_DEFAULT_ORIGIN_Y, EM_DEFAULT_ORIGIN_Z );

	GXRenderer* renderer = GXRenderer::GetInstance ();
	camera = GXCameraPerspective ( GXDegToRad ( EM_VIEWER_FOVY_DEGREES ), renderer->GetWidth () / (GXFloat)renderer->GetHeight (), EM_VIEWER_Z_NEAR, EM_VIEWER_Z_FAR );

	UpdateCamera ();

	rotationSpeed = GXDegToRad ( EM_VIEWER_ROTATION_SPEED ) / gx_ui_Scale;
	panSpeed = EM_VIEWER_PAN_SPEED / gx_ui_Scale;
	zoomSpeed = EM_VIEWER_ZOOM_STEP;

	isAltPressed = isMMBPressed = GX_FALSE;
}

GXVoid EMViewer::OnPan ( const GXVec2& mouseDelta )
{
	const GXMat4& cameraTransform = camera.GetModelMatrix ();

	GXVec3 deltaRight;
	GXMulVec3Scalar ( deltaRight, cameraTransform.xv, -mouseDelta.x * panSpeed );
	GXVec3 deltaUp;
	GXMulVec3Scalar ( deltaUp, cameraTransform.yv, -mouseDelta.y * panSpeed );

	GXSumVec3Vec3 ( origin, origin, deltaRight );
	GXSumVec3Vec3 ( origin, origin, deltaUp );

	UpdateCamera ();

	if ( OnViewerTransformChanged )
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
	GXMulVec3Mat4AsPoint ( targetOriginView, targetLocation, camera.GetViewMatrix () );

	UpdateCamera ();
	GXVec3 tempTargetWorld;
	GXMulVec3Mat4AsPoint ( tempTargetWorld, targetOriginView, camera.GetModelMatrix () );

	GXVec3 deltaTargetWorld;
	GXSubVec3Vec3 ( deltaTargetWorld, targetLocation, tempTargetWorld );

	GXSumVec3Vec3 ( origin, origin, deltaTargetWorld );

	UpdateCamera ();
	
	if ( OnViewerTransformChanged )
		OnViewerTransformChanged ();
}

GXVoid EMViewer::OnZoom ( GXFloat mouseWheelSteps )
{
	distance = GXClampf ( distance - EM_VIEWER_ZOOM_STEP * mouseWheelSteps, EM_VIEWER_MIN_DISTANCE, EM_VIEWER_MAX_DISTANCE );
	UpdateCamera ();

	if ( OnViewerTransformChanged )
		OnViewerTransformChanged ();
}

GXVoid EMViewer::UpdateCamera ()
{
	GXMat4 matrix;
	GXSetMat4RotationXY ( matrix, pitch, yaw );

	GXVec3 stick;
	GXMulVec3Scalar ( stick, matrix.zv, distance );

	GXSubVec3Vec3 ( matrix.wv, origin, stick );

	camera.SetModelMatrix ( matrix );
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

GXVoid GXCALL EMViewer::OnLMBDownCallback ( GXVoid* /*handler*/, GXUIInput* /*input*/, GXFloat x, GXFloat y )
{
	EMTool* tool = EMTool::GetActiveTool ();
	if ( !tool ) return;
	tool->OnLeftMouseButtonDown ( x, y );
}

GXVoid GXCALL EMViewer::OnLMBUpCallback ( GXVoid* /*handler*/, GXUIInput* /*input*/, GXFloat x, GXFloat y )
{
	EMTool* tool = EMTool::GetActiveTool ();
	if ( !tool ) return;
	tool->OnLeftMouseButtonUp ( x, y );
}

GXVoid GXCALL EMViewer::OnMMBDownCallback ( GXVoid* handler, GXUIInput* /*input*/, GXFloat x, GXFloat y )
{
	EMViewer* viewer = (EMViewer*)handler;
	viewer->isMMBPressed = GX_TRUE;
}

GXVoid GXCALL EMViewer::OnMMBUpCallback ( GXVoid* handler, GXUIInput* /*input*/, GXFloat x, GXFloat y )
{
	EMViewer* viewer = (EMViewer*)handler;
	viewer->isMMBPressed = GX_FALSE;
}

GXVoid GXCALL EMViewer::OnMouseScrollCallback ( GXVoid* handler, GXUIInput* /*input*/, GXFloat x, GXFloat y, GXFloat scroll )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( tool && tool->OnMouseScroll ( x, y, scroll ) ) return;

	EMViewer* viewer = (EMViewer*)handler;

	if ( viewer->ResolveMode ( viewer->isAltPressed, viewer->isMMBPressed, GX_TRUE ) == EM_VIEWER_ZOOM_MODE )
		viewer->OnZoom ( scroll );
}

GXVoid GXCALL EMViewer::OnMouseMoveCallback ( GXVoid* handler, GXUIInput* /*input*/, GXFloat x, GXFloat y )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( tool && tool->OnMouseMove ( x, y ) ) return;

	EMViewer* viewer = (EMViewer*)handler;
	GXVec2 mousePosition ( x, y );

	switch ( viewer->ResolveMode ( viewer->isAltPressed, viewer->isMMBPressed, GX_FALSE ) )
	{
		case EM_VIEWER_ROTATE_MODE:
		{
			GXVec2 delta;
			GXSubVec2Vec2 ( delta, mousePosition, viewer->mousePosition );
			viewer->OnRotate ( delta );
		}
		break;

		case EM_VIEWER_PAN_MODE:
		{
			GXVec2 delta;
			GXSubVec2Vec2 ( delta, mousePosition, viewer->mousePosition );
			viewer->OnPan ( delta );
		}
		break;

		default:
			// NOTHING
		break;
	}

	viewer->UpdateMouse ( mousePosition );
}

GXVoid GXCALL EMViewer::OnKeyDownCallback ( GXVoid* handler, GXUIInput* /*input*/, GXInt keyCode )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( tool && tool->OnKeyDown ( keyCode ) ) return;

	if ( keyCode != VK_MENU ) return;

	EMViewer* viewer = (EMViewer*)handler;
	viewer->isAltPressed = GX_TRUE;
}

GXVoid GXCALL EMViewer::OnKeyUpCallback ( GXVoid* handler, GXUIInput* /*input*/, GXInt keyCode )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( tool && tool->OnKeyUp ( keyCode ) ) return;

	if ( keyCode != VK_MENU ) return;

	EMViewer* viewer = (EMViewer*)handler;
	viewer->isAltPressed = GX_FALSE;
}
