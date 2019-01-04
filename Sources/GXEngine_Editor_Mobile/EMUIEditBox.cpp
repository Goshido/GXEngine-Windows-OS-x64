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
        GXTexture2D         background;
        GXHudSurface*       surface;

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
    background ( DEFAULT_BACKGROUND, GX_FALSE, GX_FALSE )
{
    const GXAABB& boundsLocal = widget->GetBoundsWorld ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
}

EMUIEditBoxRenderer::~EMUIEditBoxRenderer ()
{
    delete surface;
}

GXVoid EMUIEditBoxRenderer::OnRefresh ()
{
    GXUIEditBox* editBoxWidget = static_cast<GXUIEditBox*> ( widget );
    GXFont* font = editBoxWidget->GetFont ();
    GXFloat width = static_cast<GXFloat> ( surface->GetWidth () );
    GXFloat height = static_cast<GXFloat> ( surface->GetHeight () );
    const GXWChar* text = editBoxWidget->GetText ();

    surface->Reset ();

    GXImageInfo ii;

    ii.texture = &background;
    ii.color.From ( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
    ii.insertX = ii.insertY = 1.5f;
    ii.insertWidth = width - 2.0f;
    ii.insertHeight = height - 2.0f;
    ii.overlayType = eGXImageOverlayType::SimpleReplace;

    surface->AddImage ( ii );

    if ( text )
    {
        GXFloat beginOffset = editBoxWidget->GetSelectionBeginOffset ();
        GXFloat endOffset = editBoxWidget->GetSelectionEndOffset ();

        if ( beginOffset != endOffset )
        {
            ii.color.From ( SELECTION_COLOR_R, SELECTION_COLOR_G, SELECTION_COLOR_B, SELECTION_COLOR_A );
            ii.insertX = beginOffset;
            ii.insertY = 0.0f;
            ii.insertWidth = endOffset - beginOffset;
            ii.insertHeight = height;

            surface->AddImage ( ii );
        }

        GXPenInfo pi;
        pi.font = font;
        pi.insertY = ( height - font->GetSize () * 0.6f ) * 0.5f;
        pi.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
        pi.color.From ( FONT_COLOR_R, FONT_COLOR_G, FONT_COLOR_B, FONT_COLOR_A );

        switch ( editBoxWidget->GetAlignment () )
        {
        case eGXUITextAlignment::Left:
                pi.insertX = editBoxWidget->GetTextLeftOffset ();
            break;

            case eGXUITextAlignment::Right:
            {
                GXFloat w = static_cast<GXFloat> ( surface->GetWidth () );
                GXFloat len = static_cast<GXFloat> ( font->GetTextLength ( 0u, text ) );
                pi.insertX = w - len - editBoxWidget->GetTextRightOffset ();
            }
            break;

            case eGXUITextAlignment::Center:
            {
                GXFloat w = static_cast<GXFloat> ( surface->GetWidth () );
                GXFloat len = static_cast<GXFloat> ( font->GetTextLength ( 0u, text ) );
                pi.insertX = ( w - len ) * 0.5f;
            }
            break;
        }

        surface->AddText ( pi, 0u, text );
    }

    GXLineInfo li;
    li.thickness = 1.0f;
    li.overlayType = eGXImageOverlayType::SimpleReplace;

    if ( editBoxWidget->IsActive () )
    {
        li.color.From ( CURSOR_COLOR_R, CURSOR_COLOR_G, CURSOR_COLOR_B, CURSOR_COLOR_A );
        li.startPoint.Init ( editBoxWidget->GetCursorOffset () + 0.5f, 0.5f );
        li.endPoint.Init ( li.startPoint.GetX (), height );
        surface->AddLine ( li );
    }

    li.color.From ( BORDER_COLOR_R , BORDER_COLOR_G , BORDER_COLOR_B, BORDER_COLOR_A );
    
    li.startPoint.Init ( 0.5f, 0.5f );
    li.endPoint.Init ( width - 0.5f, 0.5f );

    surface->AddLine ( li );

    li.startPoint.Init ( width - 0.5f, 0.5f );
    li.endPoint.Init ( width - 0.5f, height - 0.5f );

    surface->AddLine ( li );

    li.startPoint.Init ( width - 0.5f, height - 0.5f );
    li.endPoint.Init ( 0.5f, height - 0.5f );

    surface->AddLine ( li );

    li.startPoint.Init ( 0.5f, height - 0.5f );
    li.endPoint.Init ( 0.5f, 0.5f );

    surface->AddLine ( li );
}

GXVoid EMUIEditBoxRenderer::OnDraw ()
{
    glDisable ( GL_DEPTH_TEST );
    surface->Render ();
    glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIEditBoxRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXSafeDelete ( surface );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    surface = new GXHudSurface ( width, height );

    GXVec3 location;
    surface->GetLocation ( location );
    surface->SetLocation ( x, y, location.data[ 2 ] );
}

GXVoid EMUIEditBoxRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    surface->GetLocation ( location );
    surface->SetLocation ( x, y, location.data[ 2 ] );
}

//---------------------------------------------------------------------------------------------------------------------

EMUIEditBox::EMUIEditBox ( EMUI* parent ):
    EMUI ( parent ),
    widget ( new GXUIEditBox ( parent ? parent->GetWidget () : nullptr ) )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBoxRenderer" );
    widget->SetRenderer ( new EMUIEditBoxRenderer ( widget ) );
    widget->Resize ( DEFAULT_BOTTOM_LEFT_X * gx_ui_Scale, DEFAULT_BOTTOM_LEFT_Y * gx_ui_Scale, DEFAULT_WIDTH * gx_ui_Scale, DEFAULT_HEIGHT * gx_ui_Scale );
    widget->SetFont ( DEFAULT_FONT, static_cast<GXUShort> ( DEFAULT_FONT_SIZE * gx_ui_Scale ) );
    widget->SetAlignment ( DEFAULT_ALIGNMENT );
    widget->SetTextLeftOffset ( DEFAULT_TEXT_LEFT_OFFSET * gx_ui_Scale );
    widget->SetTextRightOffset ( DEFAULT_TEXT_RIGHT_OFFSET * gx_ui_Scale );
}

EMUIEditBox::~EMUIEditBox ()
{
    delete widget->GetRenderer ();
    delete widget;
}

GXWidget* EMUIEditBox::GetWidget () const
{
    return widget;
}

GXVoid EMUIEditBox::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
    widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}

GXVoid EMUIEditBox::SetText ( const GXWChar* text )
{
    widget->SetText ( text );
}

const GXWChar* EMUIEditBox::GetText () const
{
    return widget->GetText ();
}

GXVoid EMUIEditBox::SetAlignment ( eGXUITextAlignment alignment )
{
    widget->SetAlignment ( alignment );
}

eGXUITextAlignment EMUIEditBox::GetAlignment () const
{
    return widget->GetAlignment ();
}

GXVoid EMUIEditBox::SetFont ( const GXWChar* fontFile, GXUShort fontSize )
{
    widget->SetFont ( fontFile, fontSize );
}

GXFont* EMUIEditBox::GetFont () const
{
    return widget->GetFont ();
}

GXVoid EMUIEditBox::SetValidator ( GXTextValidator& validator )
{
    widget->SetValidator ( validator );
}

GXTextValidator* EMUIEditBox::GetValidator () const
{
    return widget->GetValidator ();
}

GXVoid EMUIEditBox::SetOnFinishEditingCallback ( GXVoid* handler, PFNGXUIEDITBOXONFINISHEDITINGPROC callback )
{
    widget->SetOnFinishEditingCallback ( handler, callback );
}
