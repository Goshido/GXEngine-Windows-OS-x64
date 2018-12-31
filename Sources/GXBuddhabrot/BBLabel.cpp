#include <GXBuddhabrot/BBLabel.h>
#include <GXBuddhabrot/BBGUIMeta.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXUICommon.h>


#define PIXEL_PERFECT_OFFSET_X      0.25f
#define PIXEL_PERFECT_OFFSET_Y      0.25f

//---------------------------------------------------------------------------------------------------------------------

class BBLabelRenderer final : public GXWidgetRenderer
{
    private:
        GXFont          font;
        GXHudSurface    surface;

    public:
        explicit BBLabelRenderer ( GXWidget* widget );
        ~BBLabelRenderer () override;

    private:
        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

        BBLabelRenderer () = delete;
        BBLabelRenderer ( const BBLabelRenderer &other ) = delete;
        BBLabelRenderer& operator = ( const BBLabelRenderer &other ) = delete;
};

BBLabelRenderer::BBLabelRenderer ( GXWidget* widget ):
    GXWidgetRenderer ( widget ),
    font ( BB_GUI_MAIN_FONT, static_cast<GXUShort> ( BB_GUI_MAIN_NORMAL_FONT_SIZE * gx_ui_Scale  ) ),
    surface ( static_cast<GXUShort> ( widget->GetBoundsLocal ().GetWidth () ), static_cast<GXUShort> ( widget->GetBoundsLocal ().GetHeight () ) )
{
    // NOTHING
}

BBLabelRenderer::~BBLabelRenderer ()
{
    // NOTHING
}

GXVoid BBLabelRenderer::OnRefresh ()
{
    GXUIStaticText* staticText = static_cast<GXUIStaticText*> ( widget );

    surface.Reset ();
    GXFloat h = static_cast<GXFloat> ( surface.GetHeight () );
    const GXWChar* text = staticText->GetText ();

    if ( !text ) return;

    GXPenInfo pi;
    pi.font = &font;
    pi.insertY = ( h - font.GetSize () * 0.6f ) * 0.5f;
    pi.overlayType = eGXImageOverlayType::SimpleReplace;
    pi.color = staticText->GetTextColor ();

    switch ( staticText->GetAlignment () )
    {
        case eGXUITextAlignment::Left:
        {
            pi.insertX = 0.0f;
        }
        break;

        case eGXUITextAlignment::Right:
        {
            GXFloat w = static_cast<GXFloat> ( surface.GetWidth () );
            GXFloat len = static_cast<GXFloat> ( font.GetTextLength ( 0u, staticText->GetText () ) );
            pi.insertX = w - len;
        }
        break;

        case eGXUITextAlignment::Center:
        {
            GXFloat w = static_cast<GXFloat> ( surface.GetWidth () );
            GXFloat len = static_cast<GXFloat> ( font.GetTextLength ( 0u, staticText->GetText () ) );
            pi.insertX = ( w - len ) * 0.5f;
        }
        break;
    }

    surface.AddText ( pi, 0u, staticText->GetText () );
}

GXVoid BBLabelRenderer::OnDraw ()
{
    GXOpenGLState state;
    state.Save ();

    glDisable ( GL_DEPTH_TEST );
    surface.Render ();

    state.Restore ();
}

GXVoid BBLabelRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_OFFSET_Y;

    const GXVec3& location = surface.GetLocation ();
    surface.Resize ( width, height );
    surface.SetLocation ( x, y, location.data[ 2u ] );
}

GXVoid BBLabelRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_OFFSET_Y;

    const GXVec3& location = surface.GetLocation ();
    surface.SetLocation ( x, y, location.data[ 2u ] );
}

//---------------------------------------------------------------------------------------------------------------------

BBLabel::BBLabel ( GXWidget* parent ):
    staticText ( parent )
{
    staticText.SetRenderer ( new BBLabelRenderer ( &staticText ) );
    staticText.SetTextColor ( BB_GUI_COLOR_HUE_2_RED, BB_GUI_COLOR_HUE_2_GREEN, BB_GUI_COLOR_HUE_2_BLUE, BB_GUI_COLOR_HUE_2_ALPHA );
}

BBLabel::~BBLabel ()
{
    delete staticText.GetRenderer ();
}

GXVoid BBLabel::SetText ( const GXWChar* text )
{
    staticText.SetText ( text );
}

GXVoid BBLabel::SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    staticText.SetTextColor ( red, green, blue, alpha );
}

GXVoid BBLabel::SetAlignment ( eGXUITextAlignment alignment )
{
    staticText.SetAlignment ( alignment );
}

const GXWChar* BBLabel::GetText () const
{
    return staticText.GetText ();
}

const GXColorRGB& BBLabel::GetTextColor () const
{
    return staticText.GetTextColor ();
}

eGXUITextAlignment BBLabel::GetAlignment () const
{
    return staticText.GetAlignment ();
}

GXVoid BBLabel::Resize ( GXFloat x, GXFloat y, GXFloat width, GXFloat height )
{
    staticText.Resize ( x, y, width, height );
}
