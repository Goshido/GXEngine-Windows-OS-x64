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
        GXFont              _font;
        GXHudSurface*       _surface;

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
    _font ( FONT, static_cast<GXUShort> ( FONT_SIZE * gx_ui_Scale ) )
{
    const GXAABB& boundsLocal = widget->GetBoundsWorld ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
}

EMUIStaticTextRenderer::~EMUIStaticTextRenderer ()
{
    delete _surface;
}

GXVoid EMUIStaticTextRenderer::OnRefresh ()
{
    const GXUIStaticText* staticText = static_cast<const GXUIStaticText*> ( widget );

    _surface->Reset ();
    const GXFloat h = static_cast<GXFloat> ( _surface->GetHeight () );
    const GXWChar* text = staticText->GetText ();

    if ( !text ) return;

    GXPenInfo pi;
    pi._font = &_font;
    pi._insertY = ( h - _font.GetSize () * 0.6f ) * 0.5f;
    pi._overlayType = eGXImageOverlayType::SimpleReplace;
    pi._color = staticText->GetTextColor ();

    switch ( staticText->GetAlignment () )
    {
        case eGXUITextAlignment::Left:
            pi._insertX = 0.0f;
        break;

        case eGXUITextAlignment::Right:
        {
            const GXFloat w = static_cast<GXFloat> ( _surface->GetWidth () );
            const GXFloat len = static_cast<GXFloat> ( _font.GetTextLength ( 0u, staticText->GetText () ) );
            pi._insertX = w - len;
        }
        break;

        case eGXUITextAlignment::Center:
        {
            const GXFloat w = static_cast<GXFloat> ( _surface->GetWidth () );
            const GXFloat len = static_cast<GXFloat> ( _font.GetTextLength ( 0u, staticText->GetText () ) );
            pi._insertX = ( w - len ) * 0.5f;
        }
        break;
    }

    _surface->AddText ( pi, 0u, staticText->GetText () );
}

GXVoid EMUIStaticTextRenderer::OnDraw ()
{
    glDisable ( GL_DEPTH_TEST );
    _surface->Render ();
    glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIStaticTextRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    _surface->GetLocation ( location );
    delete _surface;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( width, height );

    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

GXVoid EMUIStaticTextRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2 ] );
}

//------------------------------------------------------------------------------

EMUIStaticText::EMUIStaticText ( EMUI* parent ):
    EMUI ( parent ),
    _widget ( parent ? parent->GetWidget () : nullptr )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticTextRenderer" )
    _widget.SetRenderer ( new EMUIStaticTextRenderer ( &_widget ) );
}

EMUIStaticText::~EMUIStaticText ()
{
    delete _widget.GetRenderer ();
}

GXWidget* EMUIStaticText::GetWidget ()
{
    return &_widget;
}

GXVoid EMUIStaticText::SetText ( const GXString &text )
{
    _widget.SetText ( text );
}

const GXString& EMUIStaticText::GetText () const
{
    return _widget.GetText ();
}

GXVoid EMUIStaticText::SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    _widget.SetTextColor ( red, green, blue, alpha );
}

const GXColorRGB& EMUIStaticText::GetTextColor () const
{
    return _widget.GetTextColor ();
}

GXVoid EMUIStaticText::SetAlingment ( eGXUITextAlignment alignment )
{
    _widget.SetAlignment ( alignment );
}

eGXUITextAlignment EMUIStaticText::GetAlignment () const
{
    return _widget.GetAlignment ();
}

GXVoid EMUIStaticText::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
    _widget.Resize ( bottomLeftX, bottomLeftY, width, height );
}
