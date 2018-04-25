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
#define EM_VIEWER_ROTATION_SPEED		45.0f		// 45 degrees per centimeter
#define EM_VIEWER_PAN_SPEED				0.2f		// 0.2 unit per centimeter
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

GXVoid EMViewer::SetInputWidget ( GXUIInput &newInputWidget )
{
	inputWidget = &newInputWidget;

	inputWidget->SetOnLeftMouseButtonDownCallback ( &EMViewer::OnLMBDownCallback );
	inputWidget->SetOnLeftMouseButtonUpCallback ( &EMViewer::OnLMBUpCallback );
	inputWidget->SetOnMiddleMouseButtonDownCallback ( &EMViewer::OnMMBDownCallback );
	inputWidget->SetOnMiddleMouseButtonUpCallback ( &EMViewer::OnMMBUpCallback );
	inputWidget->SetOnMouseMoveCallback ( &EMViewer::OnMouseMoveCallback );
	inputWidget->SetOnMouseScrollCallback ( &EMViewer::OnMouseScrollCallback );
	inputWidget->SetOnKeyDownCallback ( &EMViewer::OnKeyDownCallback );
	inputWidget->SetOnKeyUpCallback ( &EMViewer::OnKeyUpCallback );

	inputWidget->SetHandler ( this );
}

GXVoid EMViewer::SetTarget ( EMActor* actor )
{
	this->target = actor;
}

GXVoid EMViewer::SetOnViewerTransformChangedCallback ( GXVoid* handlerObject, PFNEMONVIEWERTRANSFORMCHANGEDPROC callback )
{
	handler = handlerObject;
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

EMViewer::EMViewer ():
	inputWidget ( nullptr ),
	handler ( nullptr ),
	OnViewerTransformChanged ( nullptr ),
	pitch ( EM_DEFAULT_PITCH_RAD ),
	yaw ( EM_DEFAULT_YAW_RAD ),
	distance ( EM_DEFAULT_DISTANCE ),
	origin ( EM_DEFAULT_ORIGIN_X, EM_DEFAULT_ORIGIN_Y, EM_DEFAULT_ORIGIN_Z ),
	target ( nullptr ),
	rotationSpeed ( GXDegToRad ( EM_VIEWER_ROTATION_SPEED ) / gx_ui_Scale ),
	panSpeed ( EM_VIEWER_PAN_SPEED / gx_ui_Scale ),
	zoomSpeed ( EM_VIEWER_ZOOM_STEP ),
	camera ( GXDegToRad ( EM_VIEWER_FOVY_DEGREES ), 1.0f, EM_VIEWER_Z_NEAR, EM_VIEWER_Z_FAR ),
	isAltPressed ( GX_FALSE ),
	isMMBPressed ( GX_FALSE )
{
	GXRenderer& renderer = GXRenderer::GetInstance ();
	camera.SetAspectRatio ( renderer.GetWidth () / static_cast<GXFloat> ( renderer.GetHeight () ) );

	UpdateCamera ();
}

GXVoid EMViewer::OnPan ( const GXVec2& mouseDelta )
{
	const GXMat4& cameraTransform = camera.GetCurrentFrameModelMatrix ();

	GXVec3 deltaRight;
	GXVec3 tmp;
	cameraTransform.GetX ( tmp );
	deltaRight.Multiply ( tmp, -mouseDelta.GetX () * panSpeed );

	GXVec3 deltaUp;
	cameraTransform.GetY ( tmp );
	deltaUp.Multiply ( tmp, -mouseDelta.GetY () * panSpeed );

	origin.Sum ( origin, deltaRight );
	origin.Sum ( origin, deltaUp );

	UpdateCamera ();

	if ( !OnViewerTransformChanged ) return;

	OnViewerTransformChanged ( handler );
}

GXVoid EMViewer::OnRotate ( const GXVec2& mouseDelta )
{
	GXFloat pitchDelta = -mouseDelta.GetY () * rotationSpeed;
	GXFloat yawDelta = mouseDelta.GetX () * rotationSpeed;

	pitch = FixPitch ( pitch + pitchDelta );
	yaw = FixYaw ( yaw + yawDelta );

	GXVec3 targetLocation;

	if ( target )
		target->GetTransform ().GetLocation ( targetLocation );
	else
		targetLocation = origin;

	GXVec3 targetOriginView;
	camera.GetCurrentFrameViewMatrix ().MultiplyAsPoint ( targetOriginView, targetLocation );

	UpdateCamera ();
	GXVec3 tempTargetWorld;
	camera.GetCurrentFrameModelMatrix ().MultiplyAsPoint ( tempTargetWorld, targetOriginView );

	GXVec3 deltaTargetWorld;
	deltaTargetWorld.Substract ( targetLocation, tempTargetWorld );

	origin.Sum ( origin, deltaTargetWorld );

	UpdateCamera ();
	
	if ( !OnViewerTransformChanged ) return;

	OnViewerTransformChanged ( handler );
}

GXVoid EMViewer::OnZoom ( GXFloat mouseWheelSteps )
{
	distance = GXClampf ( distance - EM_VIEWER_ZOOM_STEP * mouseWheelSteps, EM_VIEWER_MIN_DISTANCE, EM_VIEWER_MAX_DISTANCE );
	UpdateCamera ();

	if ( !OnViewerTransformChanged ) return;

	OnViewerTransformChanged ( handler );
}

GXVoid EMViewer::UpdateCamera ()
{
	GXMat4 matrix;
	matrix.RotationXY ( pitch, yaw );

	GXVec3 tmp;
	GXVec3 stick;
	matrix.GetZ ( tmp );
	stick.Multiply ( tmp, distance );

	tmp.Substract ( origin, stick );
	matrix.SetW ( tmp );

	camera.SetCurrentFrameModelMatrix ( matrix );
}

GXFloat EMViewer::FixPitch ( GXFloat currentPitch )
{
	return GXClampf ( currentPitch, -GX_MATH_HALF_PI, GX_MATH_HALF_PI );
}

GXFloat EMViewer::FixYaw ( GXFloat currentYaw )
{
	while ( currentYaw < -GX_MATH_PI ) currentYaw += GX_MATH_DOUBLE_PI;

	while ( currentYaw > GX_MATH_PI ) currentYaw -= GX_MATH_DOUBLE_PI;

	return currentYaw;
}

GXUByte EMViewer::ResolveMode ( GXBool isAltDown, GXBool isMMBDown, GXBool isWheel )
{
	if ( isWheel ) return EM_VIEWER_ZOOM_MODE;

	if ( isAltDown && isMMBDown ) return EM_VIEWER_ROTATE_MODE;

	if ( isMMBDown ) return EM_VIEWER_PAN_MODE;

	return EM_VIEWER_NOTHING_MODE;
}

GXVoid GXCALL EMViewer::OnLMBDownCallback ( GXVoid* /*handler*/, GXUIInput& /*input*/, GXFloat x, GXFloat y )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( !tool ) return;

	tool->OnLeftMouseButtonDown ( x, y );
}

GXVoid GXCALL EMViewer::OnLMBUpCallback ( GXVoid* /*handler*/, GXUIInput& /*input*/, GXFloat x, GXFloat y )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( !tool ) return;

	tool->OnLeftMouseButtonUp ( x, y );
}

GXVoid GXCALL EMViewer::OnMMBDownCallback ( GXVoid* handler, GXUIInput& /*input*/, GXFloat /*x*/, GXFloat /*y*/ )
{
	EMViewer* viewer = static_cast<EMViewer*> ( handler );
	viewer->isMMBPressed = GX_TRUE;
}

GXVoid GXCALL EMViewer::OnMMBUpCallback ( GXVoid* handler, GXUIInput& /*input*/, GXFloat /*x*/, GXFloat /*y*/ )
{
	EMViewer* viewer = static_cast<EMViewer*> ( handler );
	viewer->isMMBPressed = GX_FALSE;
}

GXVoid GXCALL EMViewer::OnMouseScrollCallback ( GXVoid* handler, GXUIInput& /*input*/, GXFloat x, GXFloat y, GXFloat scroll )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( tool && tool->OnMouseScroll ( x, y, scroll ) ) return;

	EMViewer* viewer = static_cast<EMViewer*> ( handler );

	if ( viewer->ResolveMode ( viewer->isAltPressed, viewer->isMMBPressed, GX_TRUE ) != EM_VIEWER_ZOOM_MODE ) return;

	viewer->OnZoom ( scroll );
}

GXVoid GXCALL EMViewer::OnMouseMoveCallback ( GXVoid* handler, GXUIInput& /*input*/, GXFloat x, GXFloat y )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( tool && tool->OnMouseMove ( x, y ) ) return;

	EMViewer* viewer = static_cast<EMViewer*> ( handler );
	GXVec2 mousePosition ( x, y );

	switch ( viewer->ResolveMode ( viewer->isAltPressed, viewer->isMMBPressed, GX_FALSE ) )
	{
		case EM_VIEWER_ROTATE_MODE:
		{
			GXVec2 delta;
			delta.Substract ( mousePosition, viewer->mousePosition );
			viewer->OnRotate ( delta );
		}
		break;

		case EM_VIEWER_PAN_MODE:
		{
			GXVec2 delta;
			delta.Substract ( mousePosition, viewer->mousePosition );
			viewer->OnPan ( delta );
		}
		break;

		default:
			// NOTHING
		break;
	}

	viewer->UpdateMouse ( mousePosition );
}

GXVoid GXCALL EMViewer::OnKeyDownCallback ( GXVoid* handler, GXUIInput& /*input*/, GXInt keyCode )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( tool && tool->OnKeyDown ( keyCode ) ) return;

	if ( keyCode != VK_MENU ) return;

	EMViewer* viewer = static_cast<EMViewer*> ( handler );
	viewer->isAltPressed = GX_TRUE;
}

GXVoid GXCALL EMViewer::OnKeyUpCallback ( GXVoid* handler, GXUIInput& /*input*/, GXInt keyCode )
{
	EMTool* tool = EMTool::GetActiveTool ();

	if ( tool && tool->OnKeyUp ( keyCode ) ) return;

	if ( keyCode != VK_MENU ) return;

	EMViewer* viewer = static_cast<EMViewer*> ( handler );
	viewer->isAltPressed = GX_FALSE;
}
