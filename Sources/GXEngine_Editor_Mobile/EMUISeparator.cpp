#include <GXEngine_Editor_Mobile/EMUISeparator.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXUICommon.h>


#define DEFAULT_WIDHT						3.0f
#define DEFAULT_HEIGHT						0.06f
#define DEFAULT_BOTTOM_X					0.1f
#define DEFAULT_BOTTOM_Y					0.1f

#define COLOR_R								128
#define COLOR_G								128
#define COLOR_B								128
#define COLOR_A								255

#define PIXEL_PERFECT_LOCATION_OFFSET_X		0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y		0.25f


class EMUISeparatorRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*	surface;

	public:
		EMUISeparatorRenderer ( GXWidget* widget );
		~EMUISeparatorRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUISeparatorRenderer::EMUISeparatorRenderer ( GXWidget* widget ):
GXWidgetRenderer ( widget )
{
	const GXAABB& boundsLocal = widget->GetBoundsWorld ();
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ) );
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
	GXColorToVec4 ( li.color, COLOR_R, COLOR_G, COLOR_B, COLOR_A );
	li.overlayType = eGXImageOverlayType::SimpleReplace;
	li.thickness = 1.0f;
	li.startPoint = GXCreateVec2 ( 0.0f, y );
	li.endPoint = GXCreateVec2 ( (GXFloat)surface->GetWidth (), y );

	surface->AddLine ( li );
}

GXVoid EMUISeparatorRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Render ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUISeparatorRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUISeparatorRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

//-------------------------------------------------------------

EMUISeparator::EMUISeparator ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXWidget ( parent ? parent->GetWidget () : nullptr );
	widget->Resize ( DEFAULT_BOTTOM_X * gx_ui_Scale, DEFAULT_BOTTOM_Y * gx_ui_Scale, DEFAULT_WIDHT * gx_ui_Scale, DEFAULT_HEIGHT * gx_ui_Scale );
	widget->SetRenderer ( new EMUISeparatorRenderer ( widget ) );
}

EMUISeparator::~EMUISeparator ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUISeparator::GetWidget () const
{
	return widget;
}

GXVoid EMUISeparator::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
	widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}
