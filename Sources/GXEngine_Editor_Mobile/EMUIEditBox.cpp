#include <GXEngine_Editor_Mobile/EMUIEditBox.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXUICommon.h>


#define BACKGROUND_COLOR_R                  49u
#define BACKGROUND_COLOR_G                  48u
#define BACKGROUND_COLOR_B                  48u
#define BACKGROUND_COLOR_A                  255u

#define BORDER_COLOR_R                      128u
#define BORDER_COLOR_G                      128u
#define BORDER_COLOR_B                      128u
#define BORDER_COLOR_A                      255u

#define FONT_COLOR_R                        115u
#define FONT_COLOR_G                        185u
#define FONT_COLOR_B                        0u
#define FONT_COLOR_A                        255u

#define SELECTION_COLOR_R                   17u
#define SELECTION_COLOR_G                   24u
#define SELECTION_COLOR_B                   2u
#define SELECTION_COLOR_A                   255u

#define CURSOR_COLOR_R                      255u
#define CURSOR_COLOR_G                      255u
#define CURSOR_COLOR_B                      255u
#define CURSOR_COLOR_A                      255u

#define PIXEL_PERFECT_LOCATION_OFFSET_X     0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y     0.25f

#define DEFAULT_WIDTH                       4.5f
#define DEFAULT_HEIGHT                      0.6f
#define DEFAULT_BOTTOM_LEFT_X               0.1f
#define DEFAULT_BOTTOM_LEFT_Y               0.1f

#define DEFAULT_BACKGROUND                  L"Textures/System/Default_Diffuse.tga"
#define DEFAULT_FONT                        L"Fonts/trebuc.ttf"
#define DEFAULT_FONT_SIZE                   0.33f
#define DEFAULT_ALIGNMENT                   eGXUITextAlignment::Left

#define DEFAULT_TEXT_LEFT_OFFSET            0.1f
#define DEFAULT_TEXT_RIGHT_OFFSET           0.1f

//---------------------------------------------------------------------------------------------------------------------

class EMUIEditBoxRenderer final : public GXWidgetRenderer
{
    private:
        GXTexture2D         _background;
        GXHudSurface*       _surface;

    public:
        explicit EMUIEditBoxRenderer ( GXUIEditBox* widget );
        ~EMUIEditBoxRenderer () override;

    protected:
        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

    private:
        EMUIEditBoxRenderer () = delete;
        EMUIEditBoxRenderer ( const EMUIEditBoxRenderer &other ) = delete;
        EMUIEditBoxRenderer& operator = ( const EMUIEditBoxRenderer &other ) = delete;
};

EMUIEditBoxRenderer::EMUIEditBoxRenderer ( GXUIEditBox* widget ):
    GXWidgetRenderer ( widget ),
    _background ( DEFAULT_BACKGROUND, GX_FALSE, GX_FALSE )
{
    const GXAABB& boundsLocal = widget->GetBoundsWorld ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
}

EMUIEditBoxRenderer::~EMUIEditBoxRenderer ()
{
    delete _surface;
}

GXVoid EMUIEditBoxRenderer::OnRefresh ()
{
    GXUIEditBox* editBoxWidget = static_cast<GXUIEditBox*> ( widget );
    GXFont* font = editBoxWidget->GetFont ();
    const GXFloat width = static_cast<GXFloat> ( _surface->GetWidth () );
    const GXFloat height = static_cast<GXFloat> ( _surface->GetHeight () );
    const GXWChar* text = editBoxWidget->GetText ();

    _surface->Reset ();

    GXImageInfo ii;

    ii._texture = &_background;
    ii._color.From ( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
    ii._insertX = ii._insertY = 1.5f;
    ii._insertWidth = width - 2.0f;
    ii._insertHeight = height - 2.0f;
    ii._overlayType = eGXImageOverlayType::SimpleReplace;

    _surface->AddImage ( ii );

    if ( text )
    {
        GXFloat beginOffset = editBoxWidget->GetSelectionBeginOffset ();
        GXFloat endOffset = editBoxWidget->GetSelectionEndOffset ();

        if ( beginOffset != endOffset )
        {
            ii._color.From ( SELECTION_COLOR_R, SELECTION_COLOR_G, SELECTION_COLOR_B, SELECTION_COLOR_A );
            ii._insertX = beginOffset;
            ii._insertY = 0.0f;
            ii._insertWidth = endOffset - beginOffset;
            ii._insertHeight = height;

            _surface->AddImage ( ii );
        }

        GXPenInfo pi;
        pi._font = font;
        pi._insertY = ( height - font->GetSize () * 0.6f ) * 0.5f;
        pi._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
        pi._color.From ( FONT_COLOR_R, FONT_COLOR_G, FONT_COLOR_B, FONT_COLOR_A );

        switch ( editBoxWidget->GetAlignment () )
        {
        case eGXUITextAlignment::Left:
                pi._insertX = editBoxWidget->GetTextLeftOffset ();
            break;

            case eGXUITextAlignment::Right:
            {
                const GXFloat w = static_cast<const GXFloat> ( _surface->GetWidth () );
                const GXFloat len = static_cast<const GXFloat> ( font->GetTextLength ( 0u, text ) );
                pi._insertX = w - len - editBoxWidget->GetTextRightOffset ();
            }
            break;

            case eGXUITextAlignment::Center:
            {
                const GXFloat w = static_cast<const GXFloat> ( _surface->GetWidth () );
                const GXFloat len = static_cast<const GXFloat> ( font->GetTextLength ( 0u, text ) );
                pi._insertX = ( w - len ) * 0.5f;
            }
            break;
        }

        _surface->AddText ( pi, 0u, text );
    }

    GXLineInfo li;
    li._thickness = 1.0f;
    li._overlayType = eGXImageOverlayType::SimpleReplace;

    if ( editBoxWidget->IsActive () )
    {
        li._color.From ( CURSOR_COLOR_R, CURSOR_COLOR_G, CURSOR_COLOR_B, CURSOR_COLOR_A );
        li._startPoint.Init ( editBoxWidget->GetCursorOffset () + 0.5f, 0.5f );
        li._endPoint.Init ( li._startPoint.GetX (), height );
        _surface->AddLine ( li );
    }

    li._color.From ( BORDER_COLOR_R , BORDER_COLOR_G , BORDER_COLOR_B, BORDER_COLOR_A );
    
    li._startPoint.Init ( 0.5f, 0.5f );
    li._endPoint.Init ( width - 0.5f, 0.5f );

    _surface->AddLine ( li );

    li._startPoint.Init ( width - 0.5f, 0.5f );
    li._endPoint.Init ( width - 0.5f, height - 0.5f );

    _surface->AddLine ( li );

    li._startPoint.Init ( width - 0.5f, height - 0.5f );
    li._endPoint.Init ( 0.5f, height - 0.5f );

    _surface->AddLine ( li );

    li._startPoint.Init ( 0.5f, height - 0.5f );
    li._endPoint.Init ( 0.5f, 0.5f );

    _surface->AddLine ( li );
}

GXVoid EMUIEditBoxRenderer::OnDraw ()
{
    glDisable ( GL_DEPTH_TEST );
    _surface->Render ();
    glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIEditBoxRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXSafeDelete ( _surface );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( width, height );

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

GXVoid EMUIEditBoxRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

//---------------------------------------------------------------------------------------------------------------------

EMUIEditBox::EMUIEditBox ( EMUI* parent ):
    EMUI ( parent ),
    _widget ( new GXUIEditBox ( parent ? parent->GetWidget () : nullptr ) )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBoxRenderer" )
    _widget->SetRenderer ( new EMUIEditBoxRenderer ( _widget ) );
    _widget->Resize ( DEFAULT_BOTTOM_LEFT_X * gx_ui_Scale, DEFAULT_BOTTOM_LEFT_Y * gx_ui_Scale, DEFAULT_WIDTH * gx_ui_Scale, DEFAULT_HEIGHT * gx_ui_Scale );
    _widget->SetFont ( DEFAULT_FONT, static_cast<GXUShort> ( DEFAULT_FONT_SIZE * gx_ui_Scale ) );
    _widget->SetAlignment ( DEFAULT_ALIGNMENT );
    _widget->SetTextLeftOffset ( DEFAULT_TEXT_LEFT_OFFSET * gx_ui_Scale );
    _widget->SetTextRightOffset ( DEFAULT_TEXT_RIGHT_OFFSET * gx_ui_Scale );
}

EMUIEditBox::~EMUIEditBox ()
{
    delete _widget->GetRenderer ();
    delete _widget;
}

GXWidget* EMUIEditBox::GetWidget () const
{
    return _widget;
}

GXVoid EMUIEditBox::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
    _widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}

GXVoid EMUIEditBox::SetText ( const GXWChar* text )
{
    _widget->SetText ( text );
}

const GXWChar* EMUIEditBox::GetText () const
{
    return _widget->GetText ();
}

GXVoid EMUIEditBox::SetAlignment ( eGXUITextAlignment alignment )
{
    _widget->SetAlignment ( alignment );
}

eGXUITextAlignment EMUIEditBox::GetAlignment () const
{
    return _widget->GetAlignment ();
}

GXVoid EMUIEditBox::SetFont ( const GXWChar* fontFile, GXUShort fontSize )
{
    _widget->SetFont ( fontFile, fontSize );
}

GXFont* EMUIEditBox::GetFont () const
{
    return _widget->GetFont ();
}

GXVoid EMUIEditBox::SetValidator ( GXTextValidator& validator )
{
    _widget->SetValidator ( validator );
}

GXTextValidator* EMUIEditBox::GetValidator () const
{
    return _widget->GetValidator ();
}

GXVoid EMUIEditBox::SetOnFinishEditingCallback ( GXVoid* context, GXUIEditBoxOnCommitHandler callback )
{
    _widget->SetOnFinishEditingCallback ( context, callback );
}
