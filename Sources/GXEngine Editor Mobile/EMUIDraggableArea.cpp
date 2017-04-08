#include <GXEngine_Editor_Mobile/EMUIDraggableArea.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXTexture.h>
#include <GXEngine/GXUICommon.h>


#define BACKGROUND_TEXTURE	L"Textures/Editor Mobile/Default Diffuse.tex"

#define BACKGROUND_COLOR_R	48
#define BACKGROUND_COLOR_G	48
#define BACKGROUND_COLOR_B	48
#define BACKGROUND_COLOR_A	255

#define FRAME_COLOR_R		128
#define FRAME_COLOR_G		128
#define FRAME_COLOR_B		128
#define FRAME_COLOR_A		255


class EMUIDraggableAreaRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXTexture			background;

	public:
		EMUIDraggableAreaRenderer ( GXUIDragableArea* draggableAreaWidget );
		~EMUIDraggableAreaRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIDraggableAreaRenderer::EMUIDraggableAreaRenderer ( GXUIDragableArea* draggableAreaWidget ):
GXWidgetRenderer ( draggableAreaWidget )
{
	background = GXTexture::LoadTexture ( BACKGROUND_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );
	const GXAABB& boundsLocal = widget->GetBoundsLocal ();
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ) );
}

EMUIDraggableAreaRenderer::~EMUIDraggableAreaRenderer ()
{
	delete surface;
	GXTexture::RemoveTexture ( background );
}

GXVoid EMUIDraggableAreaRenderer::OnRefresh ()
{
	GXUIDragableArea* area = (GXUIDragableArea*)widget;

	surface->Reset ();
	GXFloat w = (GXFloat)surface->GetWidth ();
	GXFloat h = (GXFloat)surface->GetHeight ();

	GXImageInfo ii;
	ii.texture = &background;
	GXColorToVec4 ( ii.color, BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
	ii.insertX = ii.insertY = 1.5f;
	ii.insertWidth = w - 2.0f;
	ii.insertHeight = h - 2.0f;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;

	surface->AddImage ( ii );

	GXLineInfo li;
	GXColorToVec4 ( li.color, FRAME_COLOR_R, FRAME_COLOR_G, FRAME_COLOR_B, FRAME_COLOR_A );
	li.thickness = 1.0f;
	li.startPoint = GXCreateVec2 ( 1.5f, 0.5f );
	li.endPoint = GXCreateVec2 ( w - 0.5f, 0.5f );
	li.overlayType = eGXImageOverlayType::SimpleReplace;

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( w - 0.5f, 1.5f );
	li.endPoint = GXCreateVec2 ( w - 0.5f, h - 0.5f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( w - 1.5f, h - 0.5f );
	li.endPoint = GXCreateVec2 ( 0.5f, h - 0.5f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( 0.5f, h - 1.5f );
	li.endPoint = GXCreateVec2 ( 0.5f, 0.5f );

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
	GXVec3 location;
	surface->GetLocation ( location );
	delete surface;
	surface = new GXHudSurface ( width, height );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIDraggableAreaRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

//---------------------------------------------------------------

EMUIDraggableArea::EMUIDraggableArea ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXUIDragableArea ( parent ? parent->GetWidget () : nullptr );
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

GXVoid EMUIDraggableArea::SetOnResizeCallback ( GXVoid* handler, PFNGXUIDRAGABLEAREAONRESIZEPROC callback )
{
	widget->SetOnResizeCallback ( handler, callback );
}
