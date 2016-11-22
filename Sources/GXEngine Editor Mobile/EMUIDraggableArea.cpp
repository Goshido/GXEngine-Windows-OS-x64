#include <GXEngine_Editor_Mobile/EMUIDraggableArea.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXTextureStorage.h>


class EMUIDraggableAreaRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXFloat				layer;
		GXTexture			background;

	public:
		EMUIDraggableAreaRenderer ( GXUIDragableArea* draggableAreaWidget );
		virtual ~EMUIDraggableAreaRenderer ();

		virtual GXVoid OnRefresh ();
		virtual GXVoid OnDraw ();

	protected:
		virtual GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height );
		virtual GXVoid OnMoved ( GXFloat x, GXFloat y );
};

EMUIDraggableAreaRenderer::EMUIDraggableAreaRenderer ( GXUIDragableArea* draggableAreaWidget ):
GXWidgetRenderer ( draggableAreaWidget )
{
	GXUShort w = (GXUShort)( gx_ui_Scale * 10.0f );
	GXUShort h = (GXUShort)( gx_ui_Scale * 7.0f );

	surface = new GXHudSurface ( w, h, GX_FALSE );
	GXLoadTexture ( L"Textures/Editor Mobile/Default Diffuse.tex", background );
	layer = EMGetNextGUIForegroundZ ();
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
	delete surface;
	surface = new GXHudSurface ( width, height, GX_FALSE );
	surface->SetLocation ( x, y, layer );
}

GXVoid EMUIDraggableAreaRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	surface->SetLocation ( x, y, layer );
}

//---------------------------------------------------------------

EMUIDraggableArea::EMUIDraggableArea ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXUIDragableArea ( parent ? parent->GetWidget () : 0 );
	widget->SetRenderer ( new EMUIDraggableAreaRenderer ( widget ) );
}

EMUIDraggableArea::~EMUIDraggableArea ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUIDraggableArea::GetWidget ()
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
