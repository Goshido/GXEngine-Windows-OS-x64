#include <GXEngine_Editor_Mobile/EMMoveTool.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXLogger.h>


#define MOVE_TOOL_LOCAL_MODE			0
#define MOVE_TOOL_WORLD_MODE			1

#define MOVE_TOOL_ACTIVE_AXIS_UNKNOWN	0xFF
#define MOVE_TOOL_ACTIVE_AXIS_X			0
#define MOVE_TOOL_ACTIVE_AXIS_Y			1
#define MOVE_TOOL_ACTIVE_AXIS_Z			2
#define MOVE_TOOL_GISMO_SIZE_FACTOR		0.1f

#define X_AXIS_COLOR_R					255
#define X_AXIS_COLOR_G					0
#define X_AXIS_COLOR_B					0
#define X_AXIS_COLOR_A					255

#define Y_AXIS_COLOR_R					0
#define Y_AXIS_COLOR_G					255
#define Y_AXIS_COLOR_B					0
#define Y_AXIS_COLOR_A					255

#define Z_AXIS_COLOR_R					0
#define Z_AXIS_COLOR_G					0
#define Z_AXIS_COLOR_B					255
#define Z_AXIS_COLOR_A					255

#define CENTER_COLOR_R					255
#define CENTER_COLOR_G					255
#define CENTER_COLOR_B					255
#define CENTER_COLOR_A					255

#define SELECTED_AXIS_COLOR_R			255
#define SELECTED_AXIS_COLOR_G			255
#define SELECTED_AXIS_COLOR_B			255
#define SELECTED_AXIS_COLOR_A			255


static EMMoveTool* em_mt_Me = nullptr;

EMMoveTool::EMMoveTool () :
xAxis ( L"3D Models/Editor Mobile/Move gismo X axis.stm" ),
xAxisMask ( L"3D Models/Editor Mobile/Move gismo X axis mask.stm" ),
yAxis ( L"3D Models/Editor Mobile/Move gismo Y axis.stm" ),
yAxisMask ( L"3D Models/Editor Mobile/Move gismo Y axis mask.stm" ),
zAxis ( L"3D Models/Editor Mobile/Move gismo Z axis.stm" ),
zAxisMask ( L"3D Models/Editor Mobile/Move gismo z axis mask.stm" ),
center ( L"3D Models/Editor Mobile/Move gismo center.stm" )
{
	mode = MOVE_TOOL_LOCAL_MODE;

	memset ( &startLocationWorld, 0, sizeof ( GXVec3 ) );
	memset ( &deltaWorld, 0, sizeof ( GXVec3 ) );
	GXSetMat4Identity ( gismoRotation );

	activeAxis = MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;
	isLMBPressed = GX_FALSE;
	mouseX = mouseY = 0xFFFF;
	gismoScaleCorrector = 1.0f;

	em_mt_Me = this;
}

EMMoveTool::~EMMoveTool ()
{
	//NOTHING
}

GXVoid EMMoveTool::Bind ()
{
	actor = nullptr;
	activeAxis = MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;
	isLMBPressed = GX_FALSE;

	EMRenderer::GetInstance ()->SetOnObjectCallback ( &OnObject );
}

GXVoid EMMoveTool::SetActor ( EMActor* actor )
{
	this->actor = actor;

	switch ( mode )
	{
		case MOVE_TOOL_LOCAL_MODE:
			SetLocalMode ();
		break;

		case MOVE_TOOL_WORLD_MODE:
			SetWorldMode ();
		break;
	}
}

GXVoid EMMoveTool::UnBind ()
{
	EMRenderer::GetInstance ()->SetOnObjectCallback ( 0 );
}

GXVoid EMMoveTool::OnViewerTransformChanged ()
{
	GXVec3 deltaView ( 0.0f, 0.0f, 0.0f );
	GXVec3 axisLocationView;
	GXMulVec3Mat4AsPoint ( axisLocationView, startLocationWorld, GXCamera::GetActiveCamera ()->GetViewMatrix () );

	gismoScaleCorrector = GetScaleCorrector ( axisLocationView, deltaView );
}

GXVoid EMMoveTool::OnDrawHudColorPass ()
{
	if ( !actor ) return;

	glDisable ( GL_BLEND );

	UpdateMeshTransform ( xAxis );
	if ( activeAxis == MOVE_TOOL_ACTIVE_AXIS_X )
		unlitColorMaterial.SetColor ( SELECTED_AXIS_COLOR_R, SELECTED_AXIS_COLOR_G, SELECTED_AXIS_COLOR_B, SELECTED_AXIS_COLOR_A );
	else
		unlitColorMaterial.SetColor ( X_AXIS_COLOR_R, X_AXIS_COLOR_G, X_AXIS_COLOR_B, X_AXIS_COLOR_A );

	unlitColorMaterial.Bind ( xAxis );
	xAxis.Render ();
	unlitColorMaterial.Unbind ();

	UpdateMeshTransform ( yAxis );
	if ( activeAxis == MOVE_TOOL_ACTIVE_AXIS_Y )
		unlitColorMaterial.SetColor ( SELECTED_AXIS_COLOR_R, SELECTED_AXIS_COLOR_G, SELECTED_AXIS_COLOR_B, SELECTED_AXIS_COLOR_A );
	else
		unlitColorMaterial.SetColor ( Y_AXIS_COLOR_R, Y_AXIS_COLOR_G, Y_AXIS_COLOR_B, Y_AXIS_COLOR_A );

	unlitColorMaterial.Bind ( yAxis );
	yAxis.Render ();
	unlitColorMaterial.Unbind ();

	UpdateMeshTransform ( zAxis );
	if ( activeAxis == MOVE_TOOL_ACTIVE_AXIS_Z )
		unlitColorMaterial.SetColor ( SELECTED_AXIS_COLOR_R, SELECTED_AXIS_COLOR_G, SELECTED_AXIS_COLOR_B, SELECTED_AXIS_COLOR_A );
	else
		unlitColorMaterial.SetColor ( Z_AXIS_COLOR_R, Z_AXIS_COLOR_G, Z_AXIS_COLOR_B, Z_AXIS_COLOR_A );

	unlitColorMaterial.Bind ( zAxis );
	zAxis.Render ();
	unlitColorMaterial.Unbind ();

	UpdateMeshTransform ( center );
	unlitColorMaterial.SetColor ( CENTER_COLOR_R, CENTER_COLOR_G, CENTER_COLOR_B, CENTER_COLOR_A );
	unlitColorMaterial.Bind ( center );
	center.Render ();
	unlitColorMaterial.Unbind ();

	glEnable ( GL_BLEND );
}

GXVoid EMMoveTool::OnDrawHudMaskPass ()
{
	if ( !actor ) return;
	
	EMRenderer* renderer = EMRenderer::GetInstance ();

	renderer->SetObjectMask ( (GXUPointer)( &xAxisMask ) );
	UpdateMeshTransform ( xAxisMask );
	objectMaskMaterial.Bind ( xAxisMask );
	xAxisMask.Render ();
	objectMaskMaterial.Unbind ();

	renderer->SetObjectMask ( (GXUPointer)( &yAxisMask ) );
	UpdateMeshTransform ( yAxisMask );
	objectMaskMaterial.Bind ( yAxisMask );
	yAxisMask.Render ();
	objectMaskMaterial.Unbind ();

	renderer->SetObjectMask ( (GXUPointer)( &zAxisMask ) );
	UpdateMeshTransform ( zAxisMask );
	objectMaskMaterial.Bind ( zAxisMask );
	zAxisMask.Render ();
	objectMaskMaterial.Unbind ();
}

GXBool EMMoveTool::OnMouseMove ( GXFloat x, GXFloat y )
{
	mouseX = (GXUShort)x;
	mouseY = (GXUShort)y;

	if ( isLMBPressed && activeAxis != MOVE_TOOL_ACTIVE_AXIS_UNKNOWN )
	{
		OnMoveActor ();
		return GX_TRUE;
	}

	return GX_FALSE;
}

GXBool EMMoveTool::OnLeftMouseButtonDown ( GXFloat x, GXFloat y )
{
	isLMBPressed = GX_TRUE;
	EMRenderer::GetInstance ()->GetObject ( (GXUShort)x, (GXUShort)y );
	return GX_FALSE;
}

GXBool EMMoveTool::OnLeftMouseButtonUp ( GXFloat /*x*/, GXFloat /*y*/ )
{
	if ( activeAxis == MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return GX_FALSE;

	GXSumVec3Vec3 ( startLocationWorld, startLocationWorld, deltaWorld );

	GXMat4 newTransform = actor->GetTransform ();
	newTransform.wv = startLocationWorld;
	actor->SetTransform ( newTransform );

	isLMBPressed = GX_FALSE;

	return GX_FALSE;
}

GXVoid EMMoveTool::SetLocalMode ()
{
	if ( !actor ) return;
	SetMode ( MOVE_TOOL_LOCAL_MODE );
}

GXVoid EMMoveTool::SetWorldMode ()
{
	if ( !actor ) return;
	SetMode ( MOVE_TOOL_WORLD_MODE );
}

GXVoid EMMoveTool::SetMode ( GXUByte mode )
{
	this->mode = mode;

	GXCamera* activeCamera = GXCamera::GetActiveCamera ();
	if ( !activeCamera ) return;

	const GXMat4& actorTransform = actor->GetTransform ();
	startLocationWorld = actorTransform.wv;
	memset ( &deltaWorld, 0, sizeof ( GXVec3 ) );

	switch ( mode )
	{
		case MOVE_TOOL_LOCAL_MODE:
			GXSetMat4ClearRotation ( gismoRotation, actorTransform );
		break;

		case MOVE_TOOL_WORLD_MODE:
			GXSetMat4Identity ( gismoRotation );
		break;
	}

	GXVec3 axisLocationView;
	GXMulVec3Mat4AsPoint ( axisLocationView, startLocationWorld, activeCamera->GetViewMatrix () );

	GXVec3 axisDirectionView;
	GetAxis ( axisDirectionView );

	GXVec3 rayView;
	GetRayPerspective ( rayView );

	axisStartParameter = GetAxisParameter ( axisLocationView, axisDirectionView, rayView );

	GXVec3 deltaView ( 0.0f, 0.0f, 0.0f );
	gismoScaleCorrector = GetScaleCorrector ( axisLocationView, deltaView );
}

GXVoid EMMoveTool::OnMoveActor ()
{
	if ( activeAxis == MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return;

	GXCamera* activeCamera = GXCamera::GetActiveCamera ();

	GXVec3 axisLocationView;
	GXMulVec3Mat4AsPoint ( axisLocationView, startLocationWorld, activeCamera->GetViewMatrix () );

	GXVec3 axisDirectionView;
	GetAxis ( axisDirectionView );

	GXVec3 rayView;
	GetRayPerspective ( rayView );

	GXFloat axisParameterDelta = GetAxisParameter ( axisLocationView, axisDirectionView, rayView ) - axisStartParameter;

	GXVec3 deltaView;
	GXMulVec3Scalar ( deltaView, axisDirectionView, axisParameterDelta );

	gismoScaleCorrector = GetScaleCorrector ( axisLocationView, deltaView );

	GXMulVec3Mat4AsNormal ( deltaWorld, deltaView, activeCamera->GetModelMatrix () );
	GXMat4 newTransform = actor->GetTransform ();
	GXSumVec3Vec3 ( newTransform.wv, startLocationWorld, deltaWorld );

	actor->SetTransform ( newTransform );
}

GXVoid EMMoveTool::GetAxis ( GXVec3& axisView )
{
	GXVec3 axisWorld;

	switch ( activeAxis )
	{
		case MOVE_TOOL_ACTIVE_AXIS_X:
			axisWorld = ( mode == MOVE_TOOL_WORLD_MODE ) ? GXCreateVec3 ( 1.0f, 0.0f, 0.0f ) : gismoRotation.xv;
		break;

		case MOVE_TOOL_ACTIVE_AXIS_Y:
			axisWorld = ( mode == MOVE_TOOL_WORLD_MODE ) ? GXCreateVec3 ( 0.0f, 1.0f, 0.0f ) : gismoRotation.yv;
		break;

		case MOVE_TOOL_ACTIVE_AXIS_Z:
			axisWorld = ( mode == MOVE_TOOL_WORLD_MODE ) ? GXCreateVec3 ( 0.0f, 0.0f, 1.0f ) : gismoRotation.zv;
		break;

		default:
			GXLogW ( L"EMMoveTool::GetAxis::Error - Неизвестная ось\n" );
			return;
		break;
	}

	GXMulVec3Mat4AsNormal ( axisView, axisWorld, GXCamera::GetActiveCamera ()->GetViewMatrix () );
}

GXVoid EMMoveTool::GetRayPerspective ( GXVec3 &rayView )
{
	GXRenderer* renderer = GXRenderer::GetInstance ();
	const GXMat4& proj_mat = GXCamera::GetActiveCamera ()->GetProjectionMatrix ();

	GXVec2 mouseCVV;
	mouseCVV.x = ( mouseX / (GXFloat)renderer->GetWidth () ) * 2.0f - 1.0f;
	mouseCVV.y = ( mouseY / (GXFloat)renderer->GetHeight () ) * 2.0f - 1.0f;

	GXGetRayPerspective ( rayView, proj_mat, mouseCVV );
}

GXFloat EMMoveTool::GetAxisParameter ( const GXVec3 &axisLocationView, const GXVec3 &axisDirectionView, const GXVec3 &rayView )
{
	GXFloat b = axisLocationView.x * rayView.x + axisLocationView.y * rayView.y + axisLocationView.z * rayView.z;
	GXFloat c = axisDirectionView.x * rayView.x + axisDirectionView.y * rayView.y + axisDirectionView.z * rayView.z;
	GXFloat d = axisDirectionView.x * axisDirectionView.x + axisDirectionView.y * axisDirectionView.y + axisDirectionView.z * axisDirectionView.z;
	GXFloat e = axisLocationView.x * axisDirectionView.x + axisLocationView.y * axisDirectionView.y + axisLocationView.z * axisDirectionView.z;
	GXFloat zeta = ( rayView.x * rayView.x + rayView.y * rayView.y + rayView.z * rayView.z ) * c;
	GXFloat omega = c * c;

	return ( zeta * e - b * omega ) / ( c * omega - zeta * d );
}

GXFloat EMMoveTool::GetScaleCorrector ( const GXVec3 &axisLocationView, const GXVec3 &deltaView )
{
	return ( axisLocationView.z + deltaView.z ) * MOVE_TOOL_GISMO_SIZE_FACTOR;
}

GXVoid EMMoveTool::UpdateMeshTransform ( EMMesh &mesh )
{
	const GXMat4& actorTransform = actor->GetTransform ();
	mesh.SetScale ( gismoScaleCorrector, gismoScaleCorrector, gismoScaleCorrector );
	mesh.SetRotation ( gismoRotation );
	mesh.SetLocation ( actorTransform.wv );
}

GXVoid GXCALL EMMoveTool::OnObject ( GXUPointer object )
{
	if ( object == (GXUPointer)( &em_mt_Me->xAxisMask ) )
		em_mt_Me->activeAxis = MOVE_TOOL_ACTIVE_AXIS_X;
	else if ( object == (GXUPointer)( &em_mt_Me->yAxisMask ) )
		em_mt_Me->activeAxis = MOVE_TOOL_ACTIVE_AXIS_Y;
	else if ( object == (GXUPointer)( &em_mt_Me->zAxisMask ) )
		em_mt_Me->activeAxis = MOVE_TOOL_ACTIVE_AXIS_Z;
	else
		em_mt_Me->activeAxis = MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;

	if ( em_mt_Me->activeAxis == MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return;

	switch ( em_mt_Me->mode )
	{
		case MOVE_TOOL_WORLD_MODE:
			em_mt_Me->SetWorldMode ();
		break;

		case MOVE_TOOL_LOCAL_MODE:
			em_mt_Me->SetLocalMode ();
		break;
	}
}
