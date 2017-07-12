#include <GXEngine_Editor_Mobile/EMUIMenu.h>
#include <GXEngine/GXUICommon.h>


#define ANY_WIDTH							1.0f

#define ITEM_HEIGHT							0.5f
#define EM_DEFAULT_BOTTOM_LEFT_X			0.1f
#define EM_DEFAULT_BOTTOM_LEFT_Y			0.1f

#define FONT								L"Fonts/trebuc.ttf"
#define FONT_SIZE							0.33f
#define TEXT_EXTEND							0.5f

#define BACKGROUND_COLOR_R					49
#define BACKGROUND_COLOR_G					48
#define BACKGROUND_COLOR_B					48
#define BACKGROUND_COLOR_A					255

#define SELECT_COLOR_R						115
#define SELECT_COLOR_G						185
#define SELECT_COLOR_B						0
#define SELECT_COLOR_A						102

#define HIGHLIGHT_COLOR_R					255
#define HIGHLIGHT_COLOR_G					255
#define HIGHLIGHT_COLOR_B					255
#define HIGHLIGHT_COLOR_A					38

#define FONT_COLOR_R						115
#define FONT_COLOR_G						185
#define FONT_COLOR_B						0
#define FONT_COLOR_A						255

#define DEFAULT_TEXTURE						L"Textures/System/Default_Diffuse.tga"

#define PIXEL_PERFECT_LOCATION_OFFSET_X		0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y		0.25f


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
	font = GXFont::GetFont ( FONT, (GXUShort)( FONT_SIZE * gx_ui_Scale ) );
	texture = GXTexture::LoadTexture ( DEFAULT_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );
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
	GXColorToVec4 ( ii.color, BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
	ii.texture = &texture;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;
	ii.insertX = 0.1f;
	ii.insertY = 0.1f;
	ii.insertWidth = w - 0.2f;
	ii.insertHeight = h - 0.2f;
	
	surface->AddImage ( ii );

	if ( selectedItemIndex != GX_UI_MENU_INVALID_INDEX )
	{
		GXColorToVec4 ( ii.color, SELECT_COLOR_R, SELECT_COLOR_G, SELECT_COLOR_B, SELECT_COLOR_A );
		ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
		ii.insertX = menu->GetItemOffset ( selectedItemIndex ) + 0.1f;
		ii.insertY = 0.1f;
		ii.insertWidth = menu->GetItemWidth ( selectedItemIndex ) - 0.2f;
		ii.insertHeight = h - 0.2f;

		surface->AddImage ( ii );
	}

	if ( highlightedItemIndex != GX_UI_MENU_INVALID_INDEX )
	{
		GXColorToVec4 ( ii.color, HIGHLIGHT_COLOR_R, HIGHLIGHT_COLOR_G, HIGHLIGHT_COLOR_B, HIGHLIGHT_COLOR_A );
		ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
		ii.insertX = menu->GetItemOffset ( highlightedItemIndex ) + 0.1f;
		ii.insertY = 0.1f;
		ii.insertWidth = menu->GetItemWidth ( highlightedItemIndex ) - 0.2f;
		ii.insertHeight = h - 0.2f;

		surface->AddImage ( ii );
	}

	GXPenInfo pi;
	GXColorToVec4 ( pi.color, FONT_COLOR_R, FONT_COLOR_G, FONT_COLOR_B, FONT_COLOR_A );
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
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	delete surface;
	surface = new GXHudSurface ( width, height );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIMenuRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

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
	widget->Resize ( EM_DEFAULT_BOTTOM_LEFT_X * gx_ui_Scale, EM_DEFAULT_BOTTOM_LEFT_Y * gx_ui_Scale, ANY_WIDTH, ITEM_HEIGHT * gx_ui_Scale );
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
	widget->AddItem ( name, renderer->GetTextWidth ( name ) + TEXT_EXTEND * gx_ui_Scale, popup ? (GXUIPopup*)popup->GetWidget () : nullptr );
}

GXVoid EMUIMenu::SetLocation ( GXFloat leftBottomX, GXFloat leftBottomY )
{
	widget->Resize ( leftBottomX, leftBottomY, ANY_WIDTH, ITEM_HEIGHT * gx_ui_Scale );
}

GXFloat EMUIMenu::GetHeight () const
{
	return ITEM_HEIGHT * gx_ui_Scale;
}
