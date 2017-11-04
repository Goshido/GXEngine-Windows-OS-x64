#include <GXEngine_Editor_Mobile/EMUI.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXLogger.h>


static EMUI* em_UIElements = nullptr;


EMUI::EMUI ( EMUI* parent )
{
	this->parent = parent;
	prev = nullptr;
	next = em_UIElements;

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

GXWidget* EMUI::GetWidget () const
{
	return nullptr;
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
