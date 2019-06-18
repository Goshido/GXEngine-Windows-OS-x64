#include <GXEngine_Editor_Mobile/EMUIDraggableArea.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXUICommon.h>


#define BACKGROUND_TEXTURE                  L"Textures/Editor Mobile/Default Diffuse.tex"

#define BACKGROUND_COLOR_R                  48u
#define BACKGROUND_COLOR_G                  48u
#define BACKGROUND_COLOR_B                  48u
#define BACKGROUND_COLOR_A                  255u

#define FRAME_COLOR_R                       128u
#define FRAME_COLOR_G                       128u
#define FRAME_COLOR_B                       128u
#define FRAME_COLOR_A                       255u

#define PIXEL_PERFECT_LOCATION_OFFSET_X     0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y     0.25f

//---------------------------------------------------------------------------------------------------------------------

class EMUIDraggableAreaRenderer final : public GXWidgetRenderer
{
    private:
        GXTexture2D         background;
        GXHudSurface*       surface;

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
    GXWidgetRenderer ( draggableAreaWidget ),
    background ( BACKGROUND_TEXTURE, GX_FALSE, GX_FALSE )
{
    const GXAABB& boundsLocal = widget->GetBoundsLocal ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
}

EMUIDraggableAreaRenderer::~EMUIDraggableAreaRenderer ()
{
    delete surface;
}

GXVoid EMUIDraggableAreaRenderer::OnRefresh ()
{
    surface->Reset ();
    GXFloat w = static_cast<GXFloat> ( surface->GetWidth () );
    GXFloat h = static_cast<GXFloat> ( surface->GetHeight () );

    GXImageInfo ii;
    ii._texture = &background;
    ii._color.From ( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
    ii._insertX = ii._insertY = 1.5f;
    ii._insertWidth = w - 2.0f;
    ii._insertHeight = h - 2.0f;
    ii._overlayType = eGXImageOverlayType::SimpleReplace;

    surface->AddImage ( ii );

    GXLineInfo li;
    li._color.From ( FRAME_COLOR_R, FRAME_COLOR_G, FRAME_COLOR_B, FRAME_COLOR_A );
    li._thickness = 1.0f;
    li._startPoint.Init ( 1.5f, 0.5f );
    li._endPoint.Init ( w - 0.5f, 0.5f );
    li._overlayType = eGXImageOverlayType::SimpleReplace;

    surface->AddLine ( li );

    li._startPoint.Init ( w - 0.5f, 1.5f );
    li._endPoint.Init ( w - 0.5f, h - 0.5f );

    surface->AddLine ( li );

    li._startPoint.Init ( w - 1.5f, h - 0.5f );
    li._endPoint.Init ( 0.5f, h - 0.5f );

    surface->AddLine ( li );

    li._startPoint.Init ( 0.5f, h - 1.5f );
    li._endPoint.Init ( 0.5f, 0.5f );

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

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
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

//---------------------------------------------------------------------------------------------------------------------

EMUIDraggableArea::EMUIDraggableArea ( EMUI* parent ):
    EMUI ( parent ),
    widget ( new GXUIDragableArea ( parent ? parent->GetWidget () : nullptr ) )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIDraggableAreaRenderer" );
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
