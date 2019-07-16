#include <GXEngine_Editor_Mobile/EMUISeparator.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXUICommon.h>


#define DEFAULT_WIDHT                       3.0f
#define DEFAULT_HEIGHT                      0.06f
#define DEFAULT_BOTTOM_X                    0.1f
#define DEFAULT_BOTTOM_Y                    0.1f

#define COLOR_R                             128u
#define COLOR_G                             128u
#define COLOR_B                             128u
#define COLOR_A                             255u

#define PIXEL_PERFECT_LOCATION_OFFSET_X     0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y     0.25f

//---------------------------------------------------------------------------------------------------------------------

class EMUISeparatorRenderer final : public GXWidgetRenderer
{
    private:
        GXHudSurface*       _surface;

    public:
        explicit EMUISeparatorRenderer ( GXWidget* widget );
        ~EMUISeparatorRenderer () override;

        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;

    protected:
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

    private:
        EMUISeparatorRenderer () = delete;
        EMUISeparatorRenderer ( const EMUISeparatorRenderer &other ) = delete;
        EMUISeparatorRenderer& operator = ( const EMUISeparatorRenderer &other ) = delete;
};

EMUISeparatorRenderer::EMUISeparatorRenderer ( GXWidget* widget ):
    GXWidgetRenderer ( widget )
{
    const GXAABB& boundsLocal = widget->GetBoundsWorld ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    _surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
}

EMUISeparatorRenderer::~EMUISeparatorRenderer ()
{
    delete _surface;
}

GXVoid EMUISeparatorRenderer::OnRefresh ()
{
    _surface->Reset ();
    const GXFloat y = floorf ( 0.5f * _surface->GetHeight () );

    GXLineInfo li;
    li._color.From ( COLOR_R, COLOR_G, COLOR_B, COLOR_A );
    li._overlayType = eGXImageOverlayType::SimpleReplace;
    li._thickness = 1.0f;
    li._startPoint.Init ( 0.0f, y );
    li._endPoint.Init ( static_cast<GXFloat> ( _surface->GetWidth () ), y );

    _surface->AddLine ( li );
}

GXVoid EMUISeparatorRenderer::OnDraw ()
{
    glDisable ( GL_DEPTH_TEST );
    _surface->Render ();
    glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUISeparatorRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXSafeDelete ( _surface );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    _surface = new GXHudSurface ( width, height );

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

GXVoid EMUISeparatorRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

//---------------------------------------------------------------------------------------------------------------------

EMUISeparator::EMUISeparator ( EMUI* parent ):
    EMUI ( parent ),
    _widget ( new GXWidget ( parent ? parent->GetWidget () : nullptr ) )
{
    _widget->Resize ( DEFAULT_BOTTOM_X * gx_ui_Scale, DEFAULT_BOTTOM_Y * gx_ui_Scale, DEFAULT_WIDHT * gx_ui_Scale, DEFAULT_HEIGHT * gx_ui_Scale );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparatorRenderer" );
    _widget->SetRenderer ( new EMUISeparatorRenderer ( _widget ) );
}

EMUISeparator::~EMUISeparator ()
{
    delete _widget->GetRenderer ();
    delete _widget;
}

GXWidget* EMUISeparator::GetWidget () const
{
    return _widget;
}

GXVoid EMUISeparator::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
    _widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}
