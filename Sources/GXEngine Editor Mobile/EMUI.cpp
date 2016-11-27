#include <GXEngine_Editor_Mobile/EMUI.h>
#include <GXEngine_Editor_Mobile/EMGlobals.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXLogger.h>


#define EM_UI_FOREGROUND_STEP		0.0001f


EMUI*		em_UIElements = 0;
GXFloat		em_UIForegroundZ = 99.9999f;


EMUI::EMUI ( EMUI* parent )
{
	this->parent = parent;
	prev = 0;
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
	return 0;
}

GXVoid EMUI::OnDrawMask ()
{
	//NOTHING
}

GXVoid EMUI::ToForeground ()
{
	GetWidget ()->ToForeground ();
}

//--------------------------------------------------------------

GXVoid GXCALL EMDrawUI ()
{
	GXTouchSurface::GetInstance ()->DrawWidgets ();
}

GXVoid GXCALL EMDrawUIMasks ()
{
	em_Renderer->SetObjectMask ( 0x00000000 );

	for ( EMUI* ui = em_UIElements; ui; ui = ui->next )
		ui->OnDrawMask ();
}

GXFloat GXCALL EMGetNextGUIForegroundZ ()
{
	//em_UIForegroundZ -= EM_UI_FOREGROUND_STEP;
	if ( em_UIForegroundZ < 0.0f )
		GXLogW ( L"EMGetNextGUIForegroundZ::Warning - Значение em_UIForegroundZ меньше нуля (%f)\n", em_UIForegroundZ );

	return em_UIForegroundZ;
}

GXVoid GXCALL EMSetHudSurfaceLocationWorld ( GXHudSurface &surface, const GXAABB &widgetBoundsWorld, GXFloat z, GXInt viewportWidth, GXInt viewportHeight )
{
	GXVec3 locationWorld;
	GXGetAABBCenter ( locationWorld, widgetBoundsWorld );

	locationWorld.x -= 0.5f * viewportWidth;
	locationWorld.y -= 0.5f * viewportHeight;
	locationWorld.z = z;

	surface.SetLocation ( locationWorld );
}
