#include <GXEngine_Editor_Mobile/EMMoveTool.h>
#include <GXEngine_Editor_Mobile/EMGlobals.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXShaderUtils.h>
#include <GXCommon/GXLogger.h>


#define EM_MOVE_TOOL_LOCAL_MODE		0
#define EM_MOVE_TOOL_WORLD_MODE		1

#define EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN	0xFF
#define EM_MOVE_TOOL_ACTIVE_AXIS_X			0
#define EM_MOVE_TOOL_ACTIVE_AXIS_Y			1
#define EM_MOVE_TOOL_ACTIVE_AXIS_Z			2
#define EM_MOVE_TOOL_GISMO_SIZE_FACTOR		0.14f


EMMoveTool* em_mt_Me;

EMMoveTool::EMMoveTool ()
{
	mode = EM_MOVE_TOOL_LOCAL_MODE;

	memset ( startLocationWorld.v, 0, sizeof ( GXVec3 ) );
	memset ( deltaWorld.v, 0, sizeof ( GXVec3 ) );
	GXSetMat4Identity ( gismoRotation );

	memset ( &xAxis, 0, sizeof ( GXVAOInfo ) );

	activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;

	isDeleted = GX_FALSE;
	isLMBPressed = GX_FALSE;

	mouseX = mouseY = 0xFFFF;

	gismoScaleCorrector = 1.0f;

	em_mt_Me = this;
}

GXVoid EMMoveTool::Delete ()
{
	isDeleted = GX_TRUE;
}

GXVoid EMMoveTool::Bind ()
{
	em_Tool = this;
	actor = 0;
	activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;
	isLMBPressed = GX_FALSE;

	em_Renderer->SetOnObjectCallback ( &OnObject );
}

GXVoid EMMoveTool::SetActor ( EMActor* actor )
{
	this->actor = actor;

	switch ( mode )
	{
		case EM_MOVE_TOOL_LOCAL_MODE:
			SetLocalMode ();
		break;

		case EM_MOVE_TOOL_WORLD_MODE:
			SetWorldMode ();
		break;
	}
}

GXVoid EMMoveTool::UnBind ()
{
	em_Tool = 0;
	em_Renderer->SetOnObjectCallback ( 0 );
}

GXVoid EMMoveTool::OnViewerTransformChanged ()
{
	GXVec3 deltaView ( 0.0f, 0.0f, 0.0f );
	GXVec3 axisLocationView;
	GXMulVec3Mat4AsPoint ( axisLocationView, startLocationWorld, gx_ActiveCamera->GetViewMatrix () );

	gismoScaleCorrector = GetScaleCorrector ( axisLocationView, deltaView );
}

GXVoid EMMoveTool::OnDrawHudColorPass ()
{
	if ( xAxis.vao == 0 )
	{
		Load3DModels ();
		InitUniforms ();
	}

	if ( isDeleted )
	{
		delete this;
		return;
	}

	if ( !actor ) return;

	glUseProgram ( colorProgram.program );

	GXMat4 rot_mat;
	GXMat4 scale_mat;
	GXMat4 mod_mat;
	GXMat4 mod_view_proj_mat;

	const GXMat4& actorTransform = actor->GetTransform ();
	GXSetMat4Scale ( scale_mat, gismoScaleCorrector, gismoScaleCorrector, gismoScaleCorrector );
	GXMulMat4Mat4 ( mod_mat, gismoRotation, scale_mat );
	GXSetMat4TranslateTo ( mod_mat, actorTransform.m41, actorTransform.m42, actorTransform.m43 );
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, gx_ActiveCamera->GetViewProjectionMatrix () );

	glUniformMatrix4fv ( clr_mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glDisable ( GL_BLEND );

	if ( activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_X )
		glUniform4f ( clr_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f );
	else
		glUniform4f ( clr_colorLocation, 1.0f, 0.0f, 0.0f, 1.0f );
	glBindVertexArray ( xAxis.vao );
	glDrawArrays ( GL_TRIANGLES, 0, xAxis.numVertices );

	if ( activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_Y )
		glUniform4f ( clr_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f );
	else
		glUniform4f ( clr_colorLocation, 0.0f, 1.0f, 0.0f, 1.0f );
	glBindVertexArray ( yAxis.vao );
	glDrawArrays ( GL_TRIANGLES, 0, yAxis.numVertices );

	if ( activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_Z )
		glUniform4f ( clr_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f );
	else
		glUniform4f ( clr_colorLocation, 0.0f, 0.0f, 1.0f, 1.0f );
	glBindVertexArray ( zAxis.vao );
	glDrawArrays ( GL_TRIANGLES, 0, zAxis.numVertices );

	glUniform4f ( clr_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f );
	glBindVertexArray ( center.vao );
	glDrawArrays ( GL_TRIANGLES, 0, center.numVertices );

	glEnable ( GL_BLEND );

	glBindVertexArray ( 0 );
	glUseProgram ( 0 );
}

GXVoid EMMoveTool::OnDrawHudMaskPass ()
{
	if ( !actor || isDeleted ) return;
	
	glUseProgram ( maskProgram.program );

	GXMat4 rot_mat;
	GXMat4 scale_mat;
	GXMat4 mod_mat;
	GXMat4 mod_view_proj_mat;

	const GXMat4& actorTransform = actor->GetTransform ();
	GXSetMat4Scale ( scale_mat, gismoScaleCorrector, gismoScaleCorrector, gismoScaleCorrector );
	GXMulMat4Mat4 ( mod_mat, gismoRotation, scale_mat );
	GXSetMat4TranslateTo ( mod_mat, actorTransform.m41, actorTransform.m42, actorTransform.m43 );
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, gx_ActiveCamera->GetViewProjectionMatrix () );

	glUniformMatrix4fv ( msk_mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	em_Renderer->SetObjectMask ( (GXUPointer)( &xAxisMask ) );
	glBindVertexArray ( xAxisMask.vao );
	glDrawArrays ( GL_TRIANGLES, 0, xAxisMask.numVertices );

	em_Renderer->SetObjectMask ( (GXUPointer)( &yAxisMask ) );
	glBindVertexArray ( yAxisMask.vao );
	glDrawArrays ( GL_TRIANGLES, 0, yAxisMask.numVertices );

	em_Renderer->SetObjectMask ( (GXUPointer)( &zAxisMask ) );
	glBindVertexArray ( zAxisMask.vao );
	glDrawArrays ( GL_TRIANGLES, 0, zAxisMask.numVertices );

	glBindVertexArray ( 0 );
	glUseProgram ( 0 );
}

GXVoid EMMoveTool::OnMouseMove ( const GXVec2 &mousePosition )
{
	mouseX = (GXUShort)mousePosition.x;
	mouseY = (GXUShort)mousePosition.y;

	if ( isLMBPressed && activeAxis != EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN )
		OnMoveActor ();
}

GXVoid EMMoveTool::OnMouseButton ( EGXInputMouseFlags mouseflags )
{
	if ( mouseflags.lmb ) 
	{
		isLMBPressed = GX_TRUE;
		em_Renderer->GetObject ( mouseX, mouseY );
	}
	else if ( isLMBPressed && !mouseflags.lmb )
	{
		if ( activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return;

		GXSumVec3Vec3 ( startLocationWorld, startLocationWorld, deltaWorld );

		GXMat4 newTransform = actor->GetTransform ();
		newTransform.wv = startLocationWorld;
		actor->SetTransform ( newTransform );

		isLMBPressed = GX_FALSE;
	}
}

GXVoid EMMoveTool::SetLocalMode ()
{
	if ( !actor ) return;
	SetMode ( EM_MOVE_TOOL_LOCAL_MODE );
}

GXVoid EMMoveTool::SetWorldMode ()
{
	if ( !actor ) return;
	SetMode ( EM_MOVE_TOOL_WORLD_MODE );
}

EMMoveTool::~EMMoveTool ()
{
	if ( xAxis.vao == 0 ) return;

	GXRemoveVAO ( xAxis );
	GXRemoveVAO ( xAxisMask );
	GXRemoveVAO ( yAxis );
	GXRemoveVAO ( yAxisMask );
	GXRemoveVAO ( zAxis );
	GXRemoveVAO ( zAxisMask );
	GXRemoveVAO ( center );

	GXRemoveShaderProgram ( colorProgram );
	GXRemoveShaderProgram ( maskProgram );
}

GXVoid EMMoveTool::Load3DModels ()
{
	GXGetVAOFromNativeStaticMesh ( xAxis, L"3D Models/Editor Mobile/Move gismo X axis.stm" );
	GXGetVAOFromNativeStaticMesh ( xAxisMask, L"3D Models/Editor Mobile/Move gismo X axis mask.stm" );
	GXGetVAOFromNativeStaticMesh ( yAxis, L"3D Models/Editor Mobile/Move gismo Y axis.stm" );
	GXGetVAOFromNativeStaticMesh ( yAxisMask, L"3D Models/Editor Mobile/Move gismo Y axis mask.stm" );
	GXGetVAOFromNativeStaticMesh ( zAxis, L"3D Models/Editor Mobile/Move gismo Z axis.stm" );
	GXGetVAOFromNativeStaticMesh ( zAxisMask, L"3D Models/Editor Mobile/Move gismo z axis mask.stm" );
	GXGetVAOFromNativeStaticMesh ( center, L"3D Models/Editor Mobile/Move gismo center.stm" );

	GXGetShaderProgram ( colorProgram, L"Shaders/Editor Mobile/VertexOnly_vs.txt", 0, L"Shaders/Editor Mobile/Color_fs.txt" );
	GXGetShaderProgram ( maskProgram, L"Shaders/Editor Mobile/MaskVertexOnly_vs.txt", 0, L"Shaders/Editor Mobile/Mask_fs.txt" );
}

GXVoid EMMoveTool::InitUniforms ()
{
	clr_mod_view_proj_matLocation = GXGetUniformLocation ( colorProgram.program, "mod_view_proj_mat" );
	clr_colorLocation = GXGetUniformLocation ( colorProgram.program, "color" );
	msk_mod_view_proj_matLocation = GXGetUniformLocation ( maskProgram.program, "mod_view_proj_mat" );
}

GXVoid EMMoveTool::SetMode ( GXUByte mode )
{
	this->mode = mode;
	if ( !gx_ActiveCamera ) return;

	const GXMat4& actorTransform = actor->GetTransform ();
	startLocationWorld = actorTransform.wv;
	memset ( &deltaWorld, 0, sizeof ( GXVec3 ) );

	switch ( mode )
	{
		case EM_MOVE_TOOL_LOCAL_MODE:
			GXSetMat4ClearRotation ( gismoRotation, actorTransform );
		break;

		case EM_MOVE_TOOL_WORLD_MODE:
			GXSetMat4Identity ( gismoRotation );
		break;
	}

	GXVec3 axisLocationView;
	GXMulVec3Mat4AsPoint ( axisLocationView, startLocationWorld, gx_ActiveCamera->GetViewMatrix () );

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
	if ( activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return;

	GXVec3 axisLocationView;
	GXMulVec3Mat4AsPoint ( axisLocationView, startLocationWorld, gx_ActiveCamera->GetViewMatrix () );

	GXVec3 axisDirectionView;
	GetAxis ( axisDirectionView );

	GXVec3 rayView;
	GetRayPerspective ( rayView );

	GXFloat axisParameterDelta = GetAxisParameter ( axisLocationView, axisDirectionView, rayView ) - axisStartParameter;

	GXVec3 deltaView;
	GXMulVec3Scalar ( deltaView, axisDirectionView, axisParameterDelta );

	gismoScaleCorrector = GetScaleCorrector ( axisLocationView, deltaView );

	GXMulVec3Mat4AsNormal ( deltaWorld, deltaView, gx_ActiveCamera->GetModelMatrix () );
	GXMat4 newTransform = actor->GetTransform ();
	GXSumVec3Vec3 ( newTransform.wv, startLocationWorld, deltaWorld );

	actor->SetTransform ( newTransform );
}

GXVoid EMMoveTool::GetAxis ( GXVec3& axisView )
{
	GXVec3 axisWorld;

	switch ( activeAxis )
	{
		case EM_MOVE_TOOL_ACTIVE_AXIS_X:
			axisWorld = ( mode == EM_MOVE_TOOL_WORLD_MODE ) ? GXCreateVec3 ( 1.0f, 0.0f, 0.0f ) : gismoRotation.xv;
		break;

		case EM_MOVE_TOOL_ACTIVE_AXIS_Y:
			axisWorld = ( mode == EM_MOVE_TOOL_WORLD_MODE ) ? GXCreateVec3 ( 0.0f, 1.0f, 0.0f ) : gismoRotation.yv;
		break;

		case EM_MOVE_TOOL_ACTIVE_AXIS_Z:
			axisWorld = ( mode == EM_MOVE_TOOL_WORLD_MODE ) ? GXCreateVec3 ( 0.0f, 0.0f, 1.0f ) : gismoRotation.zv;
		break;

		default:
			GXLogW ( L"EMMoveTool::GetAxis::Error - Неизвестная ось\n" );
			return;
		break;
	}

	GXMulVec3Mat4AsNormal ( axisView, axisWorld, gx_ActiveCamera->GetViewMatrix () );
}

GXVoid EMMoveTool::GetRayPerspective ( GXVec3 &rayView )
{
	GXRenderer* renderer = gx_Core->GetRenderer ();
	const GXMat4& proj_mat = gx_ActiveCamera->GetProjectionMatrix ();

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
	return ( axisLocationView.z + deltaView.z ) * EM_MOVE_TOOL_GISMO_SIZE_FACTOR;
}

GXVoid GXCALL EMMoveTool::OnObject ( GXUPointer object )
{
	if ( object == (GXUPointer)( &em_mt_Me->xAxisMask ) )
		em_mt_Me->activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_X;
	else if ( object == (GXUPointer)( &em_mt_Me->yAxisMask ) )
		em_mt_Me->activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_Y;
	else if ( object == (GXUPointer)( &em_mt_Me->zAxisMask ) )
		em_mt_Me->activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_Z;
	else
		em_mt_Me->activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;

	if ( em_mt_Me->activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return;

	switch ( em_mt_Me->mode )
	{
		case EM_MOVE_TOOL_WORLD_MODE:
			em_mt_Me->SetWorldMode ();
		break;

		case EM_MOVE_TOOL_LOCAL_MODE:
			em_mt_Me->SetLocalMode ();
		break;
	}
}
