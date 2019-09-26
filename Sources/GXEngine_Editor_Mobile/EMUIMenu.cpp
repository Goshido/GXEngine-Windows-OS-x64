#include <GXEngine_Editor_Mobile/EMUIMenu.h>
#include <GXEngine/GXUICommon.h>


#define DEFAULT_BOTTOM_LEFT_X               0.1f
#define DEFAULT_BOTTOM_LEFT_Y               0.1f
#define DEFAULT_TEXTURE                     "Textures/System/Default_Diffuse.tga"

#define ITEM_HEIGHT                         0.5f
#define ANY_WIDTH                           1.0f

#define FONT                                "Fonts/trebuc.ttf"
#define FONT_SIZE                           0.33f
#define TEXT_EXTEND                         0.5f

#define BACKGROUND_COLOR_R                  49u
#define BACKGROUND_COLOR_G                  48u
#define BACKGROUND_COLOR_B                  48u
#define BACKGROUND_COLOR_A                  255u

#define SELECT_COLOR_R                      115u
#define SELECT_COLOR_G                      185u
#define SELECT_COLOR_B                      0u
#define SELECT_COLOR_A                      102u

#define HIGHLIGHT_COLOR_R                   255u
#define HIGHLIGHT_COLOR_G                   255u
#define HIGHLIGHT_COLOR_B                   255u
#define HIGHLIGHT_COLOR_A                   38u

#define FONT_COLOR_R                        115u
#define FONT_COLOR_G                        185u
#define FONT_COLOR_B                        0u
#define FONT_COLOR_A                        255u

#define PIXEL_PERFECT_LOCATION_OFFSET_X     0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y     0.25f

//---------------------------------------------------------------------------------------------------------------------

class EMUIMenuRenderer final : public GXWidgetRenderer
{
    private:
        GXFont              _font;
        GXTexture2D         _texture;
        GXHudSurface*       _surface;

    public:
        explicit EMUIMenuRenderer ( GXUIMenu* widget );
        ~EMUIMenuRenderer () override;

        GXFloat GetTextWidth ( const GXString &text ) const;

    protected:
        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

    private:
        EMUIMenuRenderer () = delete;
        EMUIMenuRenderer ( const EMUIMenuRenderer &other ) = delete;
        EMUIMenuRenderer& operator = ( const EMUIMenuRenderer &other ) = delete;
};

EMUIMenuRenderer::EMUIMenuRenderer ( GXUIMenu* widget ):
    GXWidgetRenderer ( widget ),
    _font ( FONT, static_cast<GXUShort> ( FONT_SIZE * gx_ui_Scale ) ),
    _texture ( DEFAULT_TEXTURE, GX_FALSE, GX_FALSE )
{
    const GXAABB& boundsLocal = widget->GetBoundsLocal ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
}

EMUIMenuRenderer::~EMUIMenuRenderer ()
{
    delete _surface;
}

GXFloat EMUIMenuRenderer::GetTextWidth ( const GXString &text ) const
{
    return static_cast<GXFloat> ( _font.GetTextLength ( 0u, text ) );
}

GXVoid EMUIMenuRenderer::OnRefresh ()
{
    const GXUIMenu* menu = static_cast<const GXUIMenu*> ( widget );

    const GXFloat w = static_cast<const GXFloat> ( _surface->GetWidth () );
    const GXFloat h = static_cast<const GXFloat> ( _surface->GetHeight () );

    const GXUPointer totalItems = menu->GetTotalItems ();

    const GXUByte selectedItemIndex = menu->GetSelectedItemIndex ();
    const GXUByte highlightedItemIndex = menu->GetHighlightedItemIndex ();

    _surface->Reset ();

    GXImageInfo ii;
    ii._color.From ( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
    ii._texture = &_texture;
    ii._overlayType = eGXImageOverlayType::SimpleReplace;
    ii._insertX = 0.1f;
    ii._insertY = 0.1f;
    ii._insertWidth = w - 0.2f;
    ii._insertHeight = h - 0.2f;
    
    _surface->AddImage ( ii );

    if ( selectedItemIndex != GX_UI_MENU_INVALID_INDEX )
    {
        ii._color.From ( SELECT_COLOR_R, SELECT_COLOR_G, SELECT_COLOR_B, SELECT_COLOR_A );
        ii._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
        ii._insertX = menu->GetItemOffset ( selectedItemIndex ) + 0.1f;
        ii._insertY = 0.1f;
        ii._insertWidth = menu->GetItemWidth ( selectedItemIndex ) - 0.2f;
        ii._insertHeight = h - 0.2f;

        _surface->AddImage ( ii );
    }

    if ( highlightedItemIndex != GX_UI_MENU_INVALID_INDEX )
    {
        ii._color.From ( HIGHLIGHT_COLOR_R, HIGHLIGHT_COLOR_G, HIGHLIGHT_COLOR_B, HIGHLIGHT_COLOR_A );
        ii._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
        ii._insertX = menu->GetItemOffset ( highlightedItemIndex ) + 0.1f;
        ii._insertY = 0.1f;
        ii._insertWidth = menu->GetItemWidth ( highlightedItemIndex ) - 0.2f;
        ii._insertHeight = h - 0.2f;

        _surface->AddImage ( ii );
    }

    GXPenInfo pi;
    pi._color.From ( FONT_COLOR_R, FONT_COLOR_G, FONT_COLOR_B, FONT_COLOR_A );
    pi._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
    pi._font = &_font;
    pi._insertY = ( h - _font.GetSize () ) * 0.7f;

    for ( GXUPointer i = 0u; i < totalItems; ++i )
    {
        const GXString& itemName = menu->GetItemName ( i );
        const GXFloat itemOffset = menu->GetItemOffset ( i );
        const GXFloat itemWidth = menu->GetItemWidth ( i );
        const GXFloat textWidth = static_cast<const GXFloat> ( _font.GetTextLength ( 0u, itemName ) );

        pi._insertX = itemOffset + ( itemWidth - textWidth ) * 0.5f;

        _surface->AddText ( pi, 0u, itemName );
    }
}

GXVoid EMUIMenuRenderer::OnDraw ()
{
    _surface->Render ();
}

GXVoid EMUIMenuRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    delete _surface;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( width, height );

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

GXVoid EMUIMenuRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

//---------------------------------------------------------------------------------------------------------------------

EMUIMenu::EMUIMenu ( EMUI* parent ):
    EMUI ( parent ),
    _widget ( parent ? parent->GetWidget () : nullptr )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIMenuRenderer" )
    _widget.SetRenderer ( new EMUIMenuRenderer ( &_widget ) );
    _widget.Resize ( DEFAULT_BOTTOM_LEFT_X * gx_ui_Scale, DEFAULT_BOTTOM_LEFT_Y * gx_ui_Scale, ANY_WIDTH, ITEM_HEIGHT * gx_ui_Scale );
}

EMUIMenu::~EMUIMenu ()
{
    delete _widget.GetRenderer ();
}

GXWidget* EMUIMenu::GetWidget ()
{
    return &_widget;
}

GXVoid EMUIMenu::AddItem ( const GXString &name, EMUIPopup* popup )
{
    const EMUIMenuRenderer* renderer = static_cast<const EMUIMenuRenderer*> ( _widget.GetRenderer () );
    _widget.AddItem ( name, renderer->GetTextWidth ( name ) + TEXT_EXTEND * gx_ui_Scale, popup ? static_cast<GXUIPopup*> ( popup->GetWidget () ) : nullptr );
}

GXVoid EMUIMenu::SetLocation ( GXFloat leftBottomX, GXFloat leftBottomY )
{
    _widget.Resize ( leftBottomX, leftBottomY, ANY_WIDTH, ITEM_HEIGHT * gx_ui_Scale );
}

GXFloat EMUIMenu::GetHeight () const
{
    return ITEM_HEIGHT * gx_ui_Scale;
}
