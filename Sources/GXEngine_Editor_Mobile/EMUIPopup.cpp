#include <GXEngine_Editor_Mobile/EMUIPopup.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXFont.h>
#include <GXCommon/GXStrings.h>


#define ANY_HEIGHT                          1.0f

#define ITEM_WIDTH                          5.0f
#define ITEM_HEIGHT                         0.7f
#define BOTTOM_LEFT_X                       0.1f
#define BOTTOM_LEFT_Y                       0.1f

#define FONT                                L"Fonts/trebuc.ttf"
#define FONT_SIZE                           0.33f
#define TEXT_OFFSET_X                       0.2f

#define BACKGROUND_COLOR_R                  49u
#define BACKGROUND_COLOR_G                  48u
#define BACKGROUND_COLOR_B                  48u
#define BACKGROUND_COLOR_A                  255u

#define BORDER_COLOR_R                      128u
#define BORDER_COLOR_G                      128u
#define BORDER_COLOR_B                      128u
#define BORDER_COLOR_A                      255u

#define ENABLE_ITEM_COLOR_R                 115u
#define ENABLE_ITEM_COLOR_G                 185u
#define ENABLE_ITEM_COLOR_B                 0u
#define ENABLE_ITEM_COLOR_A                 255u

#define DISABLE_ITEM_COLOR_R                136u
#define DISABLE_ITEM_COLOR_G                136u
#define DISABLE_ITEM_COLOR_B                136u
#define DISABLE_ITEM_COLOR_A                255u

#define HIGHTLIGHT_COLOR_R                  255u
#define HIGHTLIGHT_COLOR_G                  255u
#define HIGHTLIGHT_COLOR_B                  255u
#define HIGHTLIGHT_COLOR_A                  38u

#define DEFAULT_TEXTURE                     L"Textures/System/Default_Diffuse.tga"

#define PIXEL_PERFECT_LOCATION_OFFSET_X     0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y     0.25f

//---------------------------------------------------------------------------------------------------------------------

class EMUIPopupRenderer final : public GXWidgetRenderer
{
    private:
        GXFont              font;
        GXDynamicArray      itemNames;
        GXTexture2D         texture;

        GXHudSurface*       surface;

    public:
        explicit EMUIPopupRenderer ( GXUIPopup* widget );
        ~EMUIPopupRenderer () override;

        GXVoid AddItem ( const GXWChar* name );

    protected:
        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

    private:
        EMUIPopupRenderer () = delete;
        EMUIPopupRenderer ( const EMUIPopupRenderer &other ) = delete;
        EMUIPopupRenderer& operator = ( const EMUIPopupRenderer &other ) = delete;
};

EMUIPopupRenderer::EMUIPopupRenderer ( GXUIPopup* widget ):
    GXWidgetRenderer ( widget ),
    font ( FONT, static_cast<GXUShort> ( FONT_SIZE * gx_ui_Scale ) ),
    itemNames ( sizeof ( GXWChar* ) ),
    texture ( DEFAULT_TEXTURE, GX_FALSE, GX_FALSE )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    surface = new GXHudSurface ( static_cast<GXUShort> ( widget->GetItemWidth () ), static_cast<GXUShort> ( widget->GetItemHeight () ) );
}

EMUIPopupRenderer::~EMUIPopupRenderer ()
{
    delete surface;

    GXWChar** names = reinterpret_cast<GXWChar**> ( itemNames.GetData () );
    GXUByte totalNames = static_cast<GXUByte> ( itemNames.GetLength () );

    for ( GXUByte i = 0u; i < totalNames; ++i )
        free ( names[ i ] );
}

GXVoid EMUIPopupRenderer::AddItem ( const GXWChar* name )
{
    GXWChar* newItem = nullptr;
    GXWcsclone ( &newItem, name );
    itemNames.SetValue ( itemNames.GetLength (), &newItem );
}

GXVoid EMUIPopupRenderer::OnRefresh ()
{
    GXUIPopup* popup = static_cast<GXUIPopup*> ( widget );
    GXUByte totalItems = popup->GetTotalItems ();
    GXFloat itemHeight = popup->GetItemHeight ();
    GXFloat itemWidth = floorf ( popup->GetItemWidth () );
    GXFloat totalHeight = floorf ( popup->GetBoundsLocal ().GetHeight () );

    surface->Reset ();

    GXImageInfo ii;
    ii._color.From ( static_cast<GXUByte> ( BACKGROUND_COLOR_R ), static_cast<GXUByte> ( BACKGROUND_COLOR_G ), static_cast<GXUByte> ( BACKGROUND_COLOR_B ), static_cast<GXUByte> ( BACKGROUND_COLOR_A ) );
    ii._texture = &texture;
    ii._overlayType = eGXImageOverlayType::SimpleReplace;
    ii._insertX = 1.0f + 0.1f;
    ii._insertY = 1.0f + 0.1f;
    ii._insertWidth = itemWidth - 2.0f - 0.2f;
    ii._insertHeight = totalHeight - 2.0f - 0.2f;
    
    surface->AddImage ( ii );

    GXLineInfo li;
    li._color.From ( static_cast<GXUByte> ( BORDER_COLOR_R ), static_cast<GXUByte> ( BORDER_COLOR_G ), static_cast<GXUByte> ( BORDER_COLOR_B ), static_cast<GXUByte> ( BORDER_COLOR_A ) );
    li._thickness = 1.0f;
    li._overlayType = eGXImageOverlayType::SimpleReplace;
    li._startPoint.Init (1.0f + 0.1f, 0.1f );
    li._endPoint.Init ( itemWidth - 2.0f + 0.9f, 0.1f );

    surface->AddLine ( li );

    li._startPoint.Init ( itemWidth - 1.0f + 0.9f, 1.0f + 0.1f );
    li._endPoint.Init ( itemWidth - 1.0f + 0.9f, totalHeight - 2.0f + 0.9f );

    surface->AddLine ( li );

    li._startPoint.Init ( itemWidth - 2.0f + 0.9f, totalHeight - 1.0f + 0.9f );
    li._endPoint.Init ( 1.0f + 0.1f, totalHeight - 1.0f + 0.9f );

    surface->AddLine ( li );

    li._startPoint.Init ( 0.1f, totalHeight - 2.0f + 0.9f );
    li._endPoint.Init ( 0.1f, 1.0f + 0.1f );

    surface->AddLine ( li );

    GXUByte hightlighted = popup->GetSelectedItemIndex ();

    if ( hightlighted != static_cast<GXUByte> ( GX_UI_POPUP_INVALID_INDEX ) )
    {
        ii._color.From ( static_cast<GXUByte> ( HIGHTLIGHT_COLOR_R ), static_cast<GXUByte> ( HIGHTLIGHT_COLOR_G ), static_cast<GXUByte> ( HIGHTLIGHT_COLOR_B ), static_cast<GXUByte> ( HIGHTLIGHT_COLOR_A ) );
        ii._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
        ii._insertX = 0.5f;
        ii._insertY = totalHeight - 0.5f - ( 1.0f + (GXFloat)hightlighted ) * itemHeight;
        ii._insertWidth = itemWidth;
        ii._insertHeight = itemHeight;
        
        surface->AddImage ( ii );
    }

    const GXWChar** names = reinterpret_cast<const GXWChar**> ( itemNames.GetData () );
    GXPenInfo pi;
    pi._font = &font;
    pi._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
    pi._insertX = TEXT_OFFSET_X * gx_ui_Scale;
    pi._insertY = totalHeight - itemHeight + ( itemHeight - (GXFloat)font.GetSize () ) * 0.5f;

    for ( GXUByte i = 0; i < totalItems; ++i )
    {
        if ( popup->IsItemActive ( i ) )
            pi._color.From ( static_cast<GXUByte> ( ENABLE_ITEM_COLOR_R ), static_cast<GXUByte> ( ENABLE_ITEM_COLOR_G ), static_cast<GXUByte> ( ENABLE_ITEM_COLOR_B ), static_cast<GXUByte> ( ENABLE_ITEM_COLOR_A ) );
        else
            pi._color.From ( static_cast<GXUByte> ( DISABLE_ITEM_COLOR_R ), static_cast<GXUByte> ( DISABLE_ITEM_COLOR_G ), static_cast<GXUByte> ( DISABLE_ITEM_COLOR_B ), static_cast<GXUByte> ( DISABLE_ITEM_COLOR_A ) );

        surface->AddText ( pi, 0, names[ i ] );
        pi._insertY -= itemHeight;
    }
}

GXVoid EMUIPopupRenderer::OnDraw ()
{
    surface->Render ();
}

GXVoid EMUIPopupRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    if ( height == 0 ) return;

    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    surface->GetLocation ( location );

    delete surface;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    surface = new GXHudSurface ( width, height );

    surface->SetLocation ( x, y, location._data[ 2u ] );
}

GXVoid EMUIPopupRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    surface->GetLocation ( location );
    surface->SetLocation ( x, y, location._data[ 2 ] );
}

//-------------------------------------------------------

EMUIPopup::EMUIPopup ( EMUI* parent ):
    EMUI ( parent ),
    widget ( new GXUIPopup ( parent ? parent->GetWidget () : nullptr ) )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIPopupRenderer" );
    widget->SetRenderer ( new EMUIPopupRenderer ( widget ) );
    widget->Resize ( BOTTOM_LEFT_X * gx_ui_Scale, BOTTOM_LEFT_Y * gx_ui_Scale, ITEM_WIDTH * gx_ui_Scale, ANY_HEIGHT * gx_ui_Scale );
    widget->SetItemHeight ( ITEM_HEIGHT * gx_ui_Scale );
}

EMUIPopup::~EMUIPopup ()
{
    delete widget->GetRenderer ();
    delete widget;
}

GXWidget* EMUIPopup::GetWidget () const
{
    return widget;
}

GXVoid EMUIPopup::AddItem ( const GXWChar* name, GXVoid* handler, PFNGXONUIPOPUPACTIONPROC action )
{
    EMUIPopupRenderer* renderer = static_cast<EMUIPopupRenderer*> ( widget->GetRenderer () );
    renderer->AddItem ( name );
    widget->AddItem ( handler, action );
}

GXVoid EMUIPopup::EnableItem ( GXUByte itemIndex )
{
    GXUIPopup* popup = static_cast<GXUIPopup*> ( widget );
    popup->EnableItem ( itemIndex );
}

GXVoid EMUIPopup::DisableItem ( GXUByte itemIndex )
{
    widget->DisableItem ( itemIndex );
}

GXVoid EMUIPopup::SetLocation ( GXFloat x, GXFloat y )
{
    widget->Resize ( x, y, ITEM_WIDTH * gx_ui_Scale, ANY_HEIGHT );
}

GXVoid EMUIPopup::Show ( EMUI* owner )
{
    widget->Show ( owner->GetWidget () );
}
