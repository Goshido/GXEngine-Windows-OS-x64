#include <GXEngine_Editor_Mobile/EMUI.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXLogger.h>


static EMUI* em_UIElements = nullptr;


EMUI::EMUI ( EMUI* parent ):
	next ( em_UIElements ),
	prev ( nullptr ),
	parent ( parent )
{
	if ( em_UIElements )
		em_UIElements->prev = this;

	em_UIElements = this;
}

EMUI::~EMUI ()
{
	if ( next ) next->prev = prev;

	if ( prev ) 
		prev->next = next;
	else
		em_UIElements = next;
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
