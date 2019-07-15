#include <GXEngine_Editor_Mobile/EMTool.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXMemory.h>


EMTool* EMTool::_activeTool = nullptr;

EMTool::EMTool ():
    _actor ( nullptr )
{
    // NOTHING
}

EMTool::~EMTool ()
{
    // NOTHING
}

GXVoid EMTool::Bind ()
{
    _actor = nullptr;
}

GXVoid EMTool::SetActor ( EMActor* newActor )
{
    _actor = newActor;
}

GXVoid EMTool::UnBind ()
{
    // NOTHING
}

GXVoid EMTool::OnViewerTransformChanged ()
{
    // NOTHING
}

GXVoid EMTool::OnDrawCommonPass ()
{
    // NOTHING
}

GXVoid EMTool::OnDrawHudColorPass ()
{
    // NOTHING
}

GXVoid EMTool::OnDrawHudMaskPass ()
{
    // NOTHING
}

GXBool EMTool::OnLeftMouseButtonDown ( GXFloat /*x*/, GXFloat /*y*/ )
{
    return GX_FALSE;
}

GXBool EMTool::OnLeftMouseButtonUp ( GXFloat /*x*/, GXFloat /*y*/ )
{
    return GX_FALSE;
}

GXBool EMTool::OnMouseMove ( GXFloat /*x*/, GXFloat /*y*/ )
{
    return GX_FALSE;
}

GXBool EMTool::OnMouseScroll ( GXFloat /*x*/, GXFloat /*y*/, GXFloat /*scroll*/ )
{
    return GX_FALSE;
}

GXBool EMTool::OnKeyDown ( GXInt /*keyCode*/ )
{
    return GX_FALSE;
}

GXBool EMTool::OnKeyUp ( GXInt /*keyCode*/ )
{
    return GX_FALSE;
}

EMTool* GXCALL EMTool::GetActiveTool ()
{
    return _activeTool;
}

GXVoid GXCALL EMTool::SetActiveTool ( EMTool* tool )
{
    _activeTool = tool;
}
