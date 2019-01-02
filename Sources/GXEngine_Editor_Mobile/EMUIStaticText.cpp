#include <GXEngine_Editor_Mobile/EMUIStaticText.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXFont.h>


#define FONT                                L"Fonts/trebuc.ttf"
#define FONT_SIZE                           0.33f

#define PIXEL_PERFECT_LOCATION_OFFSET_X     0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y     0.25f

//---------------------------------------------------------------------------------------------------------------------

class EMUIStaticTextRenderer final : public GXWidgetRenderer
{
    private:
        GXFont              font;
        GXHudSurface*       surface;

    public:
        explicit EMUIStaticTextRenderer ( GXUIStaticText* staticTextWidget );
        ~EMUIStaticTextRenderer () override;

        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;

    protected:
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

    private:
        EMUIStaticTextRenderer () = delete;
        EMUIStaticTextRenderer ( const EMUIStaticTextRenderer &other ) = delete;
        EMUIStaticTextRenderer& operator = ( const EMUIStaticTextRenderer &other ) = delete;
};

EMUIStaticTextRenderer::EMUIStaticTextRenderer ( GXUIStaticText* staticTextWidget ):
    GXWidgetRenderer ( staticTextWidget ),
    font ( FONT, static_cast<GXUShort> ( FONT_SIZE * gx_ui_Scale ) )
{
    const GXAABB& boundsLocal = widget->GetBoundsWorld ();
    surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
}

EMUIStaticTextRenderer::~EMUIStaticTextRenderer ()
{
    delete surface;
}

GXVoid EMUIStaticTextRenderer::OnRefresh ()
{
    GXUIStaticText* staticText = static_cast<GXUIStaticText*> ( widget );

    surface->Reset ();
    GXFloat h = static_cast<GXFloat> ( surface->GetHeight () );
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
            pi.insertX = 0.0f;
        break;

        case eGXUITextAlignment::Right:
        {
            GXFloat w = static_cast<GXFloat> ( surface->GetWidth () );
            GXFloat len = static_cast<GXFloat> ( font.GetTextLength ( 0u, staticText->GetText () ) );
            pi.insertX = w - len;
        }
        break;

        case eGXUITextAlignment::Center:
        {
            GXFloat w = static_cast<GXFloat> ( surface->GetWidth () );
            GXFloat len = static_cast<GXFloat> ( font.GetTextLength ( 0u, staticText->GetText () ) );
            pi.insertX = ( w - len ) * 0.5f;
        }
        break;
    }

    surface->AddText ( pi, 0u, staticText->GetText () );
}

GXVoid EMUIStaticTextRenderer::OnDraw ()
{
    glDisable ( GL_DEPTH_TEST );
    surface->Render ();
    glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIStaticTextRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    surface->GetLocation ( location );
    delete surface;
    surface = new GXHudSurface ( width, height );
    surface->SetLocation ( x, y, location.data[ 2 ] );
}

GXVoid EMUIStaticTextRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    surface->GetLocation ( location );
    surface->SetLocation ( x, y, location.data[ 2 ] );
}

//------------------------------------------------------------------------------

EMUIStaticText::EMUIStaticText ( EMUI* parent ) :
    EMUI ( parent ),
    widget ( new GXUIStaticText ( parent ? parent->GetWidget () : nullptr ) )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticTextRenderer" );
    widget->SetRenderer ( new EMUIStaticTextRenderer ( widget ) );
}

EMUIStaticText::~EMUIStaticText ()
{
    delete widget->GetRenderer ();
    delete widget;
}

GXWidget* EMUIStaticText::GetWidget () const
{
    return widget;
}

GXVoid EMUIStaticText::SetText ( const GXWChar* text )
{
    widget->SetText ( text );
}

const GXWChar* EMUIStaticText::GetText () const
{
    return widget->GetText ();
}

GXVoid EMUIStaticText::SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    widget->SetTextColor ( red, green, blue, alpha );
}

const GXColorRGB& EMUIStaticText::GetTextColor () const
{
    return widget->GetTextColor ();
}

GXVoid EMUIStaticText::SetAlingment ( eGXUITextAlignment alignment )
{
    widget->SetAlignment ( alignment );
}

eGXUITextAlignment EMUIStaticText::GetAlignment () const
{
    return widget->GetAlignment ();
}

GXVoid EMUIStaticText::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
    widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}
