#include <GXEngine_Editor_Mobile/EMUIButton.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXStrings.h>


#define DEFAULT_WIDTH                       4.0f
#define DEFAULT_HEIGHT                      0.5f
#define DEFAULT_LEFT_BOTTOM_X               0.1f
#define DEFAULT_LEFT_BOTTOM_Y               0.1f

#define DEFAULT_CAPTION                     L"Кнопка"
#define DEFAULT_FONT                        L"Fonts/trebuc.ttf"
#define DEFAULT_FONT_SIZE                   0.33f

#define DISABLE_BACKGROUND_COLOR_R          29u
#define DISABLE_BACKGROUND_COLOR_G          29u
#define DISABLE_BACKGROUND_COLOR_B          29u
#define DISABLE_BACKGROUND_COLOR_A          255u

#define DISABLE_FONT_COLOR_R                60
#define DISABLE_FONT_COLOR_G                100
#define DISABLE_FONT_COLOR_B                16
#define DISABLE_FONT_COLOR_A                255

#define HIGHLIGHTED_BACKGROUND_COLOR_R      49u
#define HIGHLIGHTED_BACKGROUND_COLOR_G      48u
#define HIGHLIGHTED_BACKGROUND_COLOR_B      48u
#define HIGHLIGHTED_BACKGROUND_COLOR_A      255u

#define HIGHLIGHTED_FONT_COLOR_R            115u
#define HIGHLIGHTED_FONT_COLOR_G            185u
#define HIGHLIGHTED_FONT_COLOR_B            0u
#define HIGHLIGHTED_FONT_COLOR_A            255u

#define PRESSED_BACKGROUND_COLOR_R          83u
#define PRESSED_BACKGROUND_COLOR_G          116u
#define PRESSED_BACKGROUND_COLOR_B          20u
#define PRESSED_BACKGROUND_COLOR_A          255u

#define PRESSED_FONT_COLOR_R                142u
#define PRESSED_FONT_COLOR_G                255u
#define PRESSED_FONT_COLOR_B                5u
#define PRESSED_FONT_COLOR_A                255u

#define NORMAL_BACKGROUND_COLOR_R           46u
#define NORMAL_BACKGROUND_COLOR_G           64u
#define NORMAL_BACKGROUND_COLOR_B           11u
#define NORMAL_BACKGROUND_COLOR_A           255u

#define NORMAL_FONT_COLOR_R                 115u
#define NORMAL_FONT_COLOR_G                 185u
#define NORMAL_FONT_COLOR_B                 0u
#define NORMAL_FONT_COLOR_A                 255u

#define BORDER_COLOR_R                      128u
#define BORDER_COLOR_G                      128u
#define BORDER_COLOR_B                      128u
#define BORDER_COLOR_A                      255u

#define PIXEL_PERFECT_TEXT_OFFSET_X         0.5f
#define PIXEL_PERFECT_TEXT_OFFSET_Y         0.25f

#define PIXEL_PERFECT_LOCATION_OFFSET_X     0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y     0.25f

#define BACKGROUND_TEXTURE                  L"Textures/System/Default_Diffuse.tga"

//----------------------------------------------------------------------------------

class EMUIButtonRenderer final : public GXWidgetRenderer
{
    private:
        GXFont              font;
        GXTexture2D         background;
        GXHudSurface*       surface;
        GXWChar*            caption;

    public:
        explicit EMUIButtonRenderer ( GXUIButton* buttonWidget );
        ~EMUIButtonRenderer () override;

        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;

        GXVoid SetCaption ( const GXWChar* newCaption );

    protected:
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

    private:
        EMUIButtonRenderer () = delete;
        EMUIButtonRenderer ( const EMUIButtonRenderer &other ) = delete;
        EMUIButtonRenderer& operator = ( const EMUIButtonRenderer &other ) = delete;
};

EMUIButtonRenderer::EMUIButtonRenderer ( GXUIButton* buttonWidget ):
    GXWidgetRenderer ( buttonWidget ),
    font ( DEFAULT_FONT, static_cast<GXUShort> ( DEFAULT_FONT_SIZE * gx_ui_Scale ) ),
    background ( BACKGROUND_TEXTURE, GX_FALSE, GX_FALSE )
{
    const GXAABB& boundsLocal = widget->GetBoundsLocal ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );

    GXWcsclone ( &caption, DEFAULT_CAPTION );
}

EMUIButtonRenderer::~EMUIButtonRenderer ()
{
    GXSafeFree ( caption );

    delete surface;
}

GXVoid EMUIButtonRenderer::OnRefresh ()
{
    GXUIButton* button = static_cast<GXUIButton*> ( widget );

    surface->Reset ();
    GXFloat w = static_cast<GXFloat> ( surface->GetWidth () );
    GXFloat h = static_cast<GXFloat> ( surface->GetHeight () );

    GXImageInfo ii;
    GXPenInfo pi;

    ii._texture = &background;

    if ( button->IsDisabled () )
    {
        ii._color.From ( DISABLE_BACKGROUND_COLOR_R, DISABLE_BACKGROUND_COLOR_G, DISABLE_BACKGROUND_COLOR_B, DISABLE_BACKGROUND_COLOR_A );
        pi._color.From ( DISABLE_FONT_COLOR_R, DISABLE_FONT_COLOR_G, DISABLE_FONT_COLOR_B, DISABLE_FONT_COLOR_A );
    }
    else if ( !button->IsHighlighted () )
    {
        ii._color.From ( HIGHLIGHTED_BACKGROUND_COLOR_R, HIGHLIGHTED_BACKGROUND_COLOR_G, HIGHLIGHTED_BACKGROUND_COLOR_B, HIGHLIGHTED_BACKGROUND_COLOR_A );
        pi._color.From ( HIGHLIGHTED_FONT_COLOR_R, HIGHLIGHTED_FONT_COLOR_G, HIGHLIGHTED_FONT_COLOR_B, HIGHLIGHTED_FONT_COLOR_A );
    }
    else if ( button->IsPressed () )
    {
        ii._color.From ( PRESSED_BACKGROUND_COLOR_R, PRESSED_BACKGROUND_COLOR_G, PRESSED_BACKGROUND_COLOR_B, PRESSED_BACKGROUND_COLOR_A );
        pi._color.From ( PRESSED_FONT_COLOR_R, PRESSED_FONT_COLOR_G, PRESSED_FONT_COLOR_B, PRESSED_FONT_COLOR_A );
    }
    else
    {
        ii._color.From ( NORMAL_BACKGROUND_COLOR_R, NORMAL_BACKGROUND_COLOR_G, NORMAL_BACKGROUND_COLOR_B, NORMAL_BACKGROUND_COLOR_A );
        pi._color.From ( NORMAL_FONT_COLOR_R, NORMAL_FONT_COLOR_G, NORMAL_FONT_COLOR_B, NORMAL_FONT_COLOR_A );
    }

    ii._insertX = ii._insertY = 1.5f;
    ii._insertWidth = w - 2.0f;
    ii._insertHeight = h - 2.0f;
    ii._overlayType = eGXImageOverlayType::SimpleReplace;

    surface->AddImage ( ii );

    if ( !caption ) return;

    GXInt len = static_cast<GXInt> ( font.GetTextLength ( 0u, caption ) );
    pi._font = &font;
    pi._insertX = ( ii._insertWidth - len ) * 0.5f + PIXEL_PERFECT_TEXT_OFFSET_X;
    pi._insertY = ( ii._insertHeight - font.GetSize () * 0.61f ) * 0.5f + PIXEL_PERFECT_TEXT_OFFSET_Y;
    pi._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

    surface->AddText ( pi, 0, caption );

    GXLineInfo li;
    li._color.From ( BORDER_COLOR_R, BORDER_COLOR_G, BORDER_COLOR_B, BORDER_COLOR_A );
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

GXVoid EMUIButtonRenderer::OnDraw ()
{
    glDisable ( GL_DEPTH_TEST );
    surface->Render ();
    glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIButtonRenderer::SetCaption ( const GXWChar* newCaption )
{
    GXSafeFree ( caption );

    if ( newCaption )
    {
        GXWcsclone ( &caption, newCaption );
        return;
    }

    caption = nullptr;
}

GXVoid EMUIButtonRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXSafeDelete ( surface );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    surface = new GXHudSurface ( width, height );

    GXVec3 location;
    surface->GetLocation ( location );
    surface->SetLocation ( x, y, location._data[ 2 ] );
}

GXVoid EMUIButtonRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    surface->GetLocation ( location );
    surface->SetLocation ( x, y, location._data[ 2 ] );
}

//----------------------------------------------------------------------------------

EMUIButton::EMUIButton ( EMUI* parent ):
    EMUI ( parent ),
    widget ( new GXUIButton ( parent ? parent->GetWidget () : nullptr ) )
{
    widget->Resize ( DEFAULT_LEFT_BOTTOM_X * gx_ui_Scale, DEFAULT_LEFT_BOTTOM_Y * gx_ui_Scale, DEFAULT_WIDTH * gx_ui_Scale, DEFAULT_HEIGHT * gx_ui_Scale );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButtonRenderer" );
    widget->SetRenderer ( new EMUIButtonRenderer ( widget ) );
}

EMUIButton::~EMUIButton ()
{
    delete widget->GetRenderer ();
    delete widget;
}

GXWidget* EMUIButton::GetWidget () const
{
    return widget;
}

GXVoid EMUIButton::Enable ()
{
    widget->Enable ();
}

GXVoid EMUIButton::Disable ()
{
    widget->Disable ();
}

GXVoid EMUIButton::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
    widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}

GXVoid EMUIButton::SetCaption ( const GXWChar* caption )
{
    EMUIButtonRenderer* renderer = static_cast<EMUIButtonRenderer*> ( widget->GetRenderer () );
    renderer->SetCaption ( caption );
    widget->Redraw ();
}

GXVoid EMUIButton::Show ()
{
    widget->Show ();
}

GXVoid EMUIButton::Hide ()
{
    widget->Hide ();
}

GXVoid EMUIButton::SetOnLeftMouseButtonCallback ( GXVoid* handler, PFNGXONMOUSEBUTTONPROC callback )
{
    widget->SetOnLeftMouseButtonCallback ( handler, callback );
}
