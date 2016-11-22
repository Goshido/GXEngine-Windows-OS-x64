#include <GXEngine_Editor_Mobile/EMUISeparator.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXCommon.h>


#define EM_SEPARATOR_DEFAULT_WIDHT			3.0f
#define EM_SEPARATOR_DEFAULT_HEIGHT			0.06f
#define EM_SEPARATOR_DEFAULT_BOTTOM_X		0.1f
#define EM_SEPARATOR_DEFAULT_BOTTOM_Y		0.1f

#define EM_SEPARATOR_DEFAULT_COLOR_R		128
#define EM_SEPARATOR_DEFAULT_COLOR_G		128
#define EM_SEPARATOR_DEFAULT_COLOR_B		128
#define EM_SEPARATOR_DEFAULT_COLOR_A		255


class EMUISeparatorRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*	surface;

	public:
		EMUISeparatorRenderer ( GXWidget* widget );
		virtual ~EMUISeparatorRenderer ();

		virtual GXVoid OnRefresh ();
		virtual GXVoid OnDraw ();

	protected:
		virtual GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height );
		virtual GXVoid OnMoved ( GXFloat x, GXFloat y );
};

EMUISeparatorRenderer::EMUISeparatorRenderer ( GXWidget* widget ):
GXWidgetRenderer ( widget )
{
	surface = 0;
	const GXAABB& bounds = widget->GetBoundsWorld ();
	OnResized ( 0.0f, 0.0f, (GXUShort)GXGetAABBWidth ( bounds ), (GXUShort)GXGetAABBHeight ( bounds ) );
	EMSetHudSurfaceLocationWorld ( *surface, bounds, EMGetNextGUIForegroundZ (), gx_Core->GetRenderer ()->GetWidth (), gx_Core->GetRenderer ()->GetHeight () );
	OnRefresh ();
}

EMUISeparatorRenderer::~EMUISeparatorRenderer ()
{
	delete surface;
}

GXVoid EMUISeparatorRenderer::OnRefresh ()
{
	const GXAABB& bounds = widget->GetBoundsWorld ();

	surface->Reset ();
	GXFloat y = floorf ( 0.5f * surface->GetHeight () );

	GXLineInfo li;
	GXColorToVec4 ( li.color, EM_SEPARATOR_DEFAULT_COLOR_R, EM_SEPARATOR_DEFAULT_COLOR_G, EM_SEPARATOR_DEFAULT_COLOR_B, EM_SEPARATOR_DEFAULT_COLOR_A );
	li.overlayType = GX_SIMPLE_REPLACE;
	li.thickness = 1.0f;
	li.startPoint = GXCreateVec3 ( 0.0f, y, 0.0f );
	li.endPoint = GXCreateVec3 ( (GXFloat)surface->GetWidth (), y, 0.0f );

	surface->AddLine ( li );
}

GXVoid EMUISeparatorRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Draw ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUISeparatorRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height, GX_FALSE );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUISeparatorRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

//-------------------------------------------------------------

EMUISeparator::EMUISeparator ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXWidget ( parent ? parent->GetWidget () : 0 );
	widget->Resize ( EM_SEPARATOR_DEFAULT_BOTTOM_X * gx_ui_Scale, EM_SEPARATOR_DEFAULT_BOTTOM_Y * gx_ui_Scale, EM_SEPARATOR_DEFAULT_WIDHT * gx_ui_Scale, EM_SEPARATOR_DEFAULT_HEIGHT * gx_ui_Scale );
	widget->SetRenderer ( new EMUISeparatorRenderer ( widget ) );
}

EMUISeparator::~EMUISeparator ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUISeparator::GetWidget ()
{
	return widget;
}

GXVoid EMUISeparator::OnDrawMask ()
{
	//TODO
}

GXVoid EMUISeparator::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
	widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}
