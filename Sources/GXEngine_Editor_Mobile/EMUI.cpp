#include <GXEngine_Editor_Mobile/EMUI.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXLogger.h>


static EMUI* em_UIElements = nullptr;


EMUI::EMUI ( EMUI* parent ):
    _next ( em_UIElements ),
    _previous ( nullptr ),
    _parent ( parent )
{
    if ( em_UIElements )
        em_UIElements->_previous = this;

    em_UIElements = this;
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

    em_UIElements = _next;
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
