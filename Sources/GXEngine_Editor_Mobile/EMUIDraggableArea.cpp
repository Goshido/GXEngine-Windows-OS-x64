#include <GXEngine_Editor_Mobile/EMUIDraggableArea.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXUICommon.h>


#define BACKGROUND_TEXTURE					L"Textures/Editor Mobile/Default Diffuse.tex"

#define BACKGROUND_COLOR_R					48u
#define BACKGROUND_COLOR_G					48u
#define BACKGROUND_COLOR_B					48u
#define BACKGROUND_COLOR_A					255u

#define FRAME_COLOR_R						128u
#define FRAME_COLOR_G						128u
#define FRAME_COLOR_B						128u
#define FRAME_COLOR_A						255u

#define PIXEL_PERFECT_LOCATION_OFFSET_X		0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y		0.25f

//---------------------------------------------------------------

class EMUIDraggableAreaRenderer : public GXWidgetRenderer
{
	private:
		GXTexture2D			background;
		GXHudSurface*		surface;

	public:
		explicit EMUIDraggableAreaRenderer ( GXUIDragableArea* draggableAreaWidget );
		~EMUIDraggableAreaRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

	private:
		EMUIDraggableAreaRenderer () = delete;
		EMUIDraggableAreaRenderer ( const EMUIDraggableAreaRenderer &other ) = delete;
		EMUIDraggableAreaRenderer& operator = ( const EMUIDraggableAreaRenderer &other ) = delete;
};

EMUIDraggableAreaRenderer::EMUIDraggableAreaRenderer ( GXUIDragableArea* draggableAreaWidget ):
	GXWidgetRenderer ( draggableAreaWidget )
{
	background = GXTexture2D::LoadTexture ( BACKGROUND_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );
	const GXAABB& boundsLocal = widget->GetBoundsLocal ();
	surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
}

EMUIDraggableAreaRenderer::~EMUIDraggableAreaRenderer ()
{
	delete surface;
	GXTexture2D::RemoveTexture ( background );
}

GXVoid EMUIDraggableAreaRenderer::OnRefresh ()
{
	surface->Reset ();
	GXFloat w = static_cast<GXFloat> ( surface->GetWidth () );
	GXFloat h = static_cast<GXFloat> ( surface->GetHeight () );

	GXImageInfo ii;
	ii.texture = &background;
	ii.color.From ( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
	ii.insertX = ii.insertY = 1.5f;
	ii.insertWidth = w - 2.0f;
	ii.insertHeight = h - 2.0f;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;

	surface->AddImage ( ii );

	GXLineInfo li;
	li.color.From ( FRAME_COLOR_R, FRAME_COLOR_G, FRAME_COLOR_B, FRAME_COLOR_A );
	li.thickness = 1.0f;
	li.startPoint.Init ( 1.5f, 0.5f );
	li.endPoint.Init ( w - 0.5f, 0.5f );
	li.overlayType = eGXImageOverlayType::SimpleReplace;

	surface->AddLine ( li );

	li.startPoint.Init ( w - 0.5f, 1.5f );
	li.endPoint.Init ( w - 0.5f, h - 0.5f );

	surface->AddLine ( li );

	li.startPoint.Init ( w - 1.5f, h - 0.5f );
	li.endPoint.Init ( 0.5f, h - 0.5f );

	surface->AddLine ( li );

	li.startPoint.Init ( 0.5f, h - 1.5f );
	li.endPoint.Init ( 0.5f, 0.5f );

	surface->AddLine ( li );
}

GXVoid EMUIDraggableAreaRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Render ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIDraggableAreaRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	delete surface;
	surface = new GXHudSurface ( width, height );
	surface->SetLocation ( x, y, location.data[ 2 ] );
}

GXVoid EMUIDraggableAreaRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.data[ 2 ] );
}

//---------------------------------------------------------------

EMUIDraggableArea::EMUIDraggableArea ( EMUI* parent ):
	EMUI ( parent ),
	widget ( new GXUIDragableArea ( parent ? parent->GetWidget () : nullptr ) )
{
	widget->SetRenderer ( new EMUIDraggableAreaRenderer ( widget ) );
}

EMUIDraggableArea::~EMUIDraggableArea ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUIDraggableArea::GetWidget () const
{
	return widget;
}

GXVoid EMUIDraggableArea::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
	widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}

GXVoid EMUIDraggableArea::Show ()
{
	widget->Show ();
}

GXVoid EMUIDraggableArea::Hide ()
{
	widget->Hide ();
}

GXVoid EMUIDraggableArea::SetHeaderHeight ( GXFloat headerHeight )
{
	widget->SetHeaderHeight ( headerHeight );
}

GXVoid EMUIDraggableArea::SetMinimumWidth ( GXFloat width )
{
	widget->SetMinimumWidth ( width );
}

GXVoid EMUIDraggableArea::SetMinimumHeight ( GXFloat height )
{
	widget->SetMinimumHeight ( height );
}

GXVoid EMUIDraggableArea::SetOnResizeCallback ( GXVoid* handler, PFNGXUIDRAGABLEAREAONRESIZEPROC callback )
{
	widget->SetOnResizeCallback ( handler, callback );
}
