#include <GXEngine_Editor_Mobile/EMUI.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXLogger.h>


EMUI* EMUI::_uiElements = nullptr;

EMUI::EMUI ( EMUI* parent )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "EMUI" )
    _next ( _uiElements ),
    _previous ( nullptr ),
    _parent ( parent )
{
    if ( _uiElements )
        _uiElements->_previous = this;

    _uiElements = this;
}

EMUI::~EMUI ()
{
    if ( _next )
        _next->_previous = _previous;

    if ( _previous )
    {
        _previous->_next = _next;
        return;
    }

    _uiElements = _next;
}

GXVoid EMUI::ToForeground ()
{
    GetWidget ()->ToForeground ();
}

//--------------------------------------------------------------

GXVoid GXCALL EMDrawUI ()
{
    GXTouchSurface::GetInstance ().DrawWidgets ();
}
