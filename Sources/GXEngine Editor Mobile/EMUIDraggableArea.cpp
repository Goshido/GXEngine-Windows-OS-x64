#include <GXEngine_Editor_Mobile/EMUIDraggableArea.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXTextureStorage.h>


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
	GXLoadTexture ( L"Textures/Editor Mobile/Default Diffuse.tex", background );
	const GXAABB& boundsLocal = widget->GetBoundsLocal ();
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ), GX_FALSE );
}

EMUIDraggableAreaRenderer::~EMUIDraggableAreaRenderer ()
{
	delete surface;
	GXRemoveTexture ( background );
}

GXVoid EMUIDraggableAreaRenderer::OnRefresh ()
{
	GXUIDragableArea* area = (GXUIDragableArea*)widget;

	surface->Reset ();
	GXFloat w = (GXFloat)surface->GetWidth ();
	GXFloat h = (GXFloat)surface->GetHeight ();

	GXImageInfo ii;
	ii.texture = background;
	GXColorToVec4 ( ii.color, 49, 48, 48, 255 );
	ii.insertX = ii.insertY = 1.5f;
	ii.insertWidth = w - 2.0f;
	ii.insertHeight = h - 2.0f;
	ii.overlayType = GX_SIMPLE_REPLACE;

	surface->AddImage ( ii );

	GXLineInfo li;
	GXColorToVec4 ( li.color, 128, 128, 128, 255 );
	li.thickness = 1.0f;
	li.startPoint = GXCreateVec3 ( 1.5f, 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( w - 0.5f, 0.5f, 0.0f );
	li.overlayType = GX_SIMPLE_REPLACE;

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( w - 0.5f, 1.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( w - 0.5f, h - 0.5f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( w - 1.5f, h - 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( 0.5f, h - 0.5f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( 0.5f, h - 1.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( 0.5f, 0.5f, 0.0f );

	surface->AddLine ( li );
}

GXVoid EMUIDraggableAreaRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Draw ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIDraggableAreaRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	GXVec3 location;
	surface->GetLocation ( location );
	delete surface;
	surface = new GXHudSurface ( width, height, GX_FALSE );
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

GXVoid EMUIDraggableArea::OnDrawMask ()
{
	//TODO
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
