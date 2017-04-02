#include <GXEngine_Editor_Mobile/EMTool.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXMemory.h>


EMTool* EMTool::activeTool = nullptr;
GXUIInput* EMTool::inputWidget = nullptr;
GXInt EMTool::inputWidgetRef = 0;


EMTool::EMTool ()
{
	actor = nullptr;

	inputWidgetRef++;
	if ( !inputWidget )
	{
		inputWidget = new GXUIInput ( nullptr, GX_FALSE );

		inputWidget->SetOnLeftMouseButtonDownCallback ( &EMTool::OnLMBDown );
		inputWidget->SetOnLeftMouseButtonUpCallback ( &EMTool::OnLMBUp );

		inputWidget->SetOnMiddleMouseButtonDownCallback ( &EMTool::OnMMBDown );
		inputWidget->SetOnMiddleMouseButtonUpCallback ( &EMTool::OnMMBUp );

		inputWidget->SetOnRightMouseButtonDownCallback ( &EMTool::OnRMBDown );
		inputWidget->SetOnRightMouseButtonUpCallback ( &EMTool::OnRMBUp );

		inputWidget->SetOnDoubleClickCallback ( &EMTool::OnDoubleClick );
		inputWidget->SetOnMouseMoveCallback ( &EMTool::OnMouseMove );
		inputWidget->SetOnMouseScrollCallback ( &EMTool::OnMouseScroll );

		GXTouchSurface::GetInstance ()->SetDefaultWidget ( inputWidget );
	}
}

EMTool::~EMTool ()
{
	inputWidgetRef--;
	if ( inputWidgetRef < 1 )
	{
		GXTouchSurface::GetInstance ()->SetDefaultWidget ( nullptr );
		GXSafeDelete ( inputWidget );
	}
}

GXVoid EMTool::Bind ()
{
	actor = nullptr;
}

GXVoid EMTool::SetActor ( EMActor* actor )
{
	this->actor = actor;
}

GXVoid EMTool::UnBind ()
{
	//NOTHING
}

GXVoid EMTool::OnViewerTransformChanged ()
{
	//NOTHING
}

GXVoid EMTool::OnDrawCommonPass ()
{
	//NOTHING
}

GXVoid EMTool::OnDrawHudColorPass ()
{
	//NOTHING
}

GXVoid EMTool::OnDrawHudMaskPass ()
{
	//NOTHING
}

GXVoid EMTool::OnMouseMove ( const GXVec2 &mousePosition )
{
	//NOTHING
}

GXVoid EMTool::OnMouseButton ( EGXInputMouseFlags mouseflags )
{
	//NOTHING
}

GXVoid EMTool::OnMouseWheel ( GXInt steps )
{
	//NOTHING
}

GXVoid EMTool::OnDoubleClick ( const GXVec2 &mousePosition )
{
	//NOTHING
}

GXVoid GXCALL EMTool::SetActiveTool ( EMTool* tool )
{
	if ( activeTool == tool ) return;

	activeTool = tool;

	if ( inputWidget )
		inputWidget->SetHandler ( activeTool );
}

GXVoid GXCALL EMTool::OnLMBDown ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y )
{
	if ( !handler ) return;

	EMTool* tool = (EMTool*)handler;

	GXVec2 mousePosition = GXCreateVec2 ( x, y );
	tool->OnMouseMove ( mousePosition );

	EGXInputMouseFlags flags = { 0 };
	flags.lmb = 1;
	tool->OnMouseButton ( flags );
}

GXVoid GXCALL EMTool::OnLMBUp ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y )
{
	if ( !handler ) return;

	EMTool* tool = (EMTool*)handler;

	GXVec2 mousePosition = GXCreateVec2 ( x, y );
	tool->OnMouseMove ( mousePosition );

	EGXInputMouseFlags flags = { 0 };
	tool->OnMouseButton ( flags );
}

GXVoid GXCALL EMTool::OnMMBDown ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y )
{
	if ( !handler ) return;

	EMTool* tool = (EMTool*)handler;

	GXVec2 mousePosition = GXCreateVec2 ( x, y );
	tool->OnMouseMove ( mousePosition );

	EGXInputMouseFlags flags = { 0 };
	flags.mmb = 1;
	tool->OnMouseButton ( flags );
}

GXVoid GXCALL EMTool::OnMMBUp ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y )
{
	if ( !handler ) return;

	EMTool* tool = (EMTool*)handler;

	GXVec2 mousePosition = GXCreateVec2 ( x, y );
	tool->OnMouseMove ( mousePosition );

	EGXInputMouseFlags flags = { 0 };
	tool->OnMouseButton ( flags );
}

GXVoid GXCALL EMTool::OnRMBDown ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y )
{
	if ( !handler ) return;

	EMTool* tool = (EMTool*)handler;

	GXVec2 mousePosition = GXCreateVec2 ( x, y );
	tool->OnMouseMove ( mousePosition );

	EGXInputMouseFlags flags = { 0 };
	flags.rmb = 1;
	tool->OnMouseButton ( flags );
}

GXVoid GXCALL EMTool::OnRMBUp ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y )
{
	if ( !handler ) return;

	EMTool* tool = (EMTool*)handler;

	GXVec2 mousePosition = GXCreateVec2 ( x, y );
	tool->OnMouseMove ( mousePosition );

	EGXInputMouseFlags flags = { 0 };
	tool->OnMouseButton ( flags );
}

GXVoid GXCALL EMTool::OnDoubleClick ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y )
{
	if ( !handler ) return;

	EMTool* tool = (EMTool*)handler;
	GXVec2 mousePosition = GXCreateVec2 ( x, y );
	tool->OnDoubleClick ( mousePosition );
}

GXVoid GXCALL EMTool::OnMouseMove ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y )
{
	if ( !handler ) return;

	EMTool* tool = (EMTool*)handler;
	GXVec2 mousePosition = GXCreateVec2 ( x, y );
	tool->OnMouseMove ( mousePosition );
}

GXVoid GXCALL EMTool::OnMouseScroll ( GXVoid* handler, GXUIInput* widget, GXFloat x, GXFloat y, GXFloat scroll )
{
	if ( !handler ) return;

	EMTool* tool = (EMTool*)handler;

	GXVec2 mousePosition = GXCreateVec2 ( x, y );
	tool->OnMouseMove ( mousePosition );

	tool->OnMouseWheel ( (GXInt)scroll );
}
