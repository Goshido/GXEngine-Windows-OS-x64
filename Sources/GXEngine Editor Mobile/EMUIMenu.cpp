#include <GXEngine_Editor_Mobile/EMUIMenu.h>
#include <GXEngine/GXUICommon.h>


#define EM_ANY_WIDTH				1.0f

#define EM_ITEM_HEIGHT				0.5f
#define EM_DEFAULT_BOTTOM_LEFT_X	0.1f
#define EM_DEFAULT_BOTTOM_LEFT_Y	0.1f

#define EM_DEFAULT_FONT				L"Fonts/trebuc.ttf"
#define EM_DEFAULT_FONT_SIZE		0.33f
#define EM_TEXT_EXTEND				0.5f

#define EM_BACKGROUND_COLOR_R		49
#define EM_BACKGROUND_COLOR_G		48
#define EM_BACKGROUND_COLOR_B		48
#define EM_BACKGROUND_COLOR_A		255

#define EM_SELECT_COLOR_R			115
#define EM_SELECT_COLOR_G			185
#define EM_SELECT_COLOR_B			0
#define EM_SELECT_COLOR_A			102

#define EM_HIGHLIGHT_COLOR_R		255
#define EM_HIGHLIGHT_COLOR_G		255
#define EM_HIGHLIGHT_COLOR_B		255
#define EM_HIGHLIGHT_COLOR_A		38

#define EM_FONT_COLOR_R				115
#define EM_FONT_COLOR_G				185
#define EM_FONT_COLOR_B				0
#define EM_FONT_COLOR_A				255

#define EM_DEFAULT_TEXTURE			L"Textures/System/Default_Diffuse.tga"


class EMUIMenuRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXFont				font;
		GXTexture			texture;

	public:
		explicit EMUIMenuRenderer ( GXUIMenu* widget );
		~EMUIMenuRenderer () override;

		GXFloat GetTextWidth ( const GXWChar* text ) const;

	protected:
		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIMenuRenderer::EMUIMenuRenderer ( GXUIMenu* widget ):
GXWidgetRenderer ( widget )
{
	const GXAABB& boundsLocal = widget->GetBoundsLocal ();
	surface = new GXHudSurface ( (GXShort)GXGetAABBWidth ( boundsLocal ), (GXShort)GXGetAABBHeight ( boundsLocal ) );
	font = GXFont::GetFont ( EM_DEFAULT_FONT, (GXUShort)( EM_DEFAULT_FONT_SIZE * gx_ui_Scale ) );
	texture = GXTexture::LoadTexture ( EM_DEFAULT_TEXTURE, GX_FALSE );
}

EMUIMenuRenderer::~EMUIMenuRenderer ()
{
	delete surface;
	GXFont::RemoveFont ( font );
	GXTexture::RemoveTexture ( texture );
}

GXFloat EMUIMenuRenderer::GetTextWidth ( const GXWChar* text ) const
{
	return (GXFloat)font.GetTextLength ( 0, text );
}

GXVoid EMUIMenuRenderer::OnRefresh ()
{
	const GXUIMenu* menu = (const GXUIMenu*)widget;

	GXFloat w = (GXFloat)surface->GetWidth ();
	GXFloat h = (GXFloat)surface->GetHeight ();

	GXUByte totalItems = menu->GetTotalItems ();

	GXUByte selectedItemIndex = menu->GetSelectedItemIndex ();
	GXUByte highlightedItemIndex = menu->GetHighlightedItemIndex ();

	surface->Reset ();

	GXImageInfo ii;
	GXColorToVec4 ( ii.color, EM_BACKGROUND_COLOR_R, EM_BACKGROUND_COLOR_G, EM_BACKGROUND_COLOR_B, EM_BACKGROUND_COLOR_A );
	ii.texture = &texture;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;
	ii.insertX = 0.1f;
	ii.insertY = 0.1f;
	ii.insertWidth = w - 0.2f;
	ii.insertHeight = h - 0.2f;
	
	surface->AddImage ( ii );

	if ( selectedItemIndex != GX_UI_MENU_INVALID_INDEX )
	{
		GXColorToVec4 ( ii.color, EM_SELECT_COLOR_R, EM_SELECT_COLOR_G, EM_SELECT_COLOR_B, EM_SELECT_COLOR_A );
		ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
		ii.insertX = menu->GetItemOffset ( selectedItemIndex ) + 0.1f;
		ii.insertY = 0.1f;
		ii.insertWidth = menu->GetItemWidth ( selectedItemIndex ) - 0.2f;
		ii.insertHeight = h - 0.2f;

		surface->AddImage ( ii );
	}

	if ( highlightedItemIndex != GX_UI_MENU_INVALID_INDEX )
	{
		GXColorToVec4 ( ii.color, EM_HIGHLIGHT_COLOR_R, EM_HIGHLIGHT_COLOR_G, EM_HIGHLIGHT_COLOR_B, EM_HIGHLIGHT_COLOR_A );
		ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
		ii.insertX = menu->GetItemOffset ( highlightedItemIndex ) + 0.1f;
		ii.insertY = 0.1f;
		ii.insertWidth = menu->GetItemWidth ( highlightedItemIndex ) - 0.2f;
		ii.insertHeight = h - 0.2f;

		surface->AddImage ( ii );
	}

	GXPenInfo pi;
	GXColorToVec4 ( pi.color, EM_FONT_COLOR_R, EM_FONT_COLOR_G, EM_FONT_COLOR_B, EM_FONT_COLOR_A );
	pi.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
	pi.font = &font;
	pi.insertY = ( h - font.GetSize () ) * 0.7f;

	for ( GXUByte i = 0; i < totalItems; i++ )
	{
		const GXWChar* itemName = menu->GetItemName ( i );
		GXFloat itemOffset = menu->GetItemOffset ( i );
		GXFloat itemWidth = menu->GetItemWidth ( i );
		GXFloat textWidth = (GXFloat)font.GetTextLength ( 0, itemName );

		pi.insertX = itemOffset + ( itemWidth - textWidth ) * 0.5f;

		surface->AddText ( pi, 0, itemName );
	}
}

GXVoid EMUIMenuRenderer::OnDraw ()
{
	surface->Render ();
}

GXVoid EMUIMenuRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	delete surface;
	surface = new GXHudSurface ( width, height );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIMenuRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

//---------------------------------------------------------

EMUIMenu::EMUIMenu ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXUIMenu ( parent ? parent->GetWidget () : nullptr );
	widget->SetRenderer ( new EMUIMenuRenderer ( widget ) );
	widget->Resize ( EM_DEFAULT_BOTTOM_LEFT_X * gx_ui_Scale, EM_DEFAULT_BOTTOM_LEFT_Y * gx_ui_Scale, EM_ANY_WIDTH, EM_ITEM_HEIGHT * gx_ui_Scale );
}

EMUIMenu::~EMUIMenu ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUIMenu::GetWidget () const
{
	return widget;
}

GXVoid EMUIMenu::AddItem ( const GXWChar* name, EMUIPopup* popup )
{
	const EMUIMenuRenderer* renderer = (const EMUIMenuRenderer*)widget->GetRenderer ();
	widget->AddItem ( name, renderer->GetTextWidth ( name ) + EM_TEXT_EXTEND * gx_ui_Scale, popup ? (GXUIPopup*)popup->GetWidget () : nullptr );
}

GXVoid EMUIMenu::SetLocation ( GXFloat leftBottomX, GXFloat leftBottomY )
{
	widget->Resize ( leftBottomX, leftBottomY, EM_ANY_WIDTH, EM_ITEM_HEIGHT * gx_ui_Scale );
}

GXFloat EMUIMenu::GetHeight () const
{
	return EM_ITEM_HEIGHT * gx_ui_Scale;
}
