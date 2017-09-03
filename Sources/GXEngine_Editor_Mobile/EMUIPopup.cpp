#include <GXEngine_Editor_Mobile/EMUIPopup.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXFont.h>
#include <GXCommon/GXStrings.h>


#define ANY_HEIGHT							1.0f

#define ITEM_WIDTH							5.0f
#define ITEM_HEIGHT							0.7f
#define BOTTOM_LEFT_X						0.1f
#define BOTTOM_LEFT_Y						0.1f

#define FONT								L"Fonts/trebuc.ttf"
#define FONT_SIZE							0.33f
#define TEXT_OFFSET_X						0.2f

#define BACKGROUND_COLOR_R					49
#define BACKGROUND_COLOR_G					48
#define BACKGROUND_COLOR_B					48
#define BACKGROUND_COLOR_A					255

#define BORDER_COLOR_R						128
#define BORDER_COLOR_G						128
#define BORDER_COLOR_B						128
#define BORDER_COLOR_A						255

#define ENABLE_ITEM_COLOR_R					115
#define ENABLE_ITEM_COLOR_G					185
#define ENABLE_ITEM_COLOR_B					0
#define ENABLE_ITEM_COLOR_A					255

#define DISABLE_ITEM_COLOR_R				136
#define DISABLE_ITEM_COLOR_G				136
#define DISABLE_ITEM_COLOR_B				136
#define DISABLE_ITEM_COLOR_A				255

#define HIGHTLIGHT_COLOR_R					255
#define HIGHTLIGHT_COLOR_G					255
#define HIGHTLIGHT_COLOR_B					255
#define HIGHTLIGHT_COLOR_A					38

#define DEFAULT_TEXTURE						L"Textures/System/Default_Diffuse.tga"

#define PIXEL_PERFECT_LOCATION_OFFSET_X		0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y		0.25f


class EMUIPopupRenderer : public GXWidgetRenderer
{
	private:
		GXFont			font;
		GXHudSurface*	surface;
		GXTexture2D		texture;
		GXDynamicArray	itemNames;

	public:
		explicit EMUIPopupRenderer ( GXUIPopup* widget );
		~EMUIPopupRenderer () override;

		GXVoid AddItem ( const GXWChar* name );

	protected:
		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIPopupRenderer::EMUIPopupRenderer ( GXUIPopup* widget ):
GXWidgetRenderer ( widget ), itemNames ( sizeof ( GXWChar* ) )
{
	font = GXFont::GetFont ( FONT, (GXUShort)( FONT_SIZE * gx_ui_Scale ) );
	texture = GXTexture2D::LoadTexture ( DEFAULT_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );
	surface = new GXHudSurface ( (GXUShort)widget->GetItemWidth (), (GXUShort)widget->GetItemHeight () );
}

EMUIPopupRenderer::~EMUIPopupRenderer ()
{
	GXFont::RemoveFont ( font );
	delete surface;
	GXTexture2D::RemoveTexture ( texture );

	GXWChar** names = (GXWChar**)itemNames.GetData ();
	GXUInt totalNames = itemNames.GetLength ();

	for ( GXUInt i = 0; i < totalNames; i++ )
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
	GXUIPopup* popup = (GXUIPopup*)widget;
	GXUByte totalItems = popup->GetTotalItems ();
	GXFloat itemHeight = popup->GetItemHeight ();
	GXFloat itemWidth = floorf ( popup->GetItemWidth () );
	GXFloat totalHeight = floorf ( GXGetAABBHeight ( popup->GetBoundsLocal () ) );

	surface->Reset ();

	GXImageInfo ii;
	GXColorToVec4 ( ii.color, BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
	ii.texture = &texture;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;
	ii.insertX = 1.0f + 0.1f;
	ii.insertY = 1.0f + 0.1f;
	ii.insertWidth = itemWidth - 2.0f - 0.2f;
	ii.insertHeight = totalHeight - 2.0f - 0.2f;
	
	surface->AddImage ( ii );

	GXLineInfo li;
	GXColorToVec4 ( li.color, BORDER_COLOR_R, BORDER_COLOR_G, BORDER_COLOR_B, BORDER_COLOR_A );
	li.thickness = 1.0f;
	li.overlayType = eGXImageOverlayType::SimpleReplace;
	li.startPoint = GXCreateVec2 ( 1.0f + 0.1f, 0.1f );
	li.endPoint = GXCreateVec2 ( itemWidth - 2.0f + 0.9f, 0.1f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( itemWidth - 1.0f + 0.9f, 1.0f + 0.1f );
	li.endPoint = GXCreateVec2 ( itemWidth - 1.0f + 0.9f, totalHeight - 2.0f + 0.9f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( itemWidth - 2.0f + 0.9f, totalHeight - 1.0f + 0.9f );
	li.endPoint = GXCreateVec2 ( 1.0f + 0.1f, totalHeight - 1.0f + 0.9f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( 0.1f, totalHeight - 2.0f + 0.9f );
	li.endPoint = GXCreateVec2 ( 0.1f, 1.0f + 0.1f );

	surface->AddLine ( li );

	GXUByte hightlighted = popup->GetSelectedItemIndex ();
	if ( hightlighted != GX_UI_POPUP_INVALID_INDEX )
	{
		GXColorToVec4 ( ii.color, HIGHTLIGHT_COLOR_R, HIGHTLIGHT_COLOR_G, HIGHTLIGHT_COLOR_B, HIGHTLIGHT_COLOR_A );
		ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
		ii.insertX = 0.5f;
		ii.insertY = totalHeight - 0.5f - ( 1.0f + (GXFloat)hightlighted ) * itemHeight;
		ii.insertWidth = itemWidth;
		ii.insertHeight = itemHeight;
		
		surface->AddImage ( ii );
	}

	const GXWChar** names = (const GXWChar**)itemNames.GetData ();
	GXPenInfo pi;
	pi.font = &font;
	pi.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
	pi.insertX = TEXT_OFFSET_X * gx_ui_Scale;
	pi.insertY = totalHeight - itemHeight + ( itemHeight - (GXFloat)font.GetSize () ) * 0.5f;

	for ( GXUByte i = 0; i < totalItems; i++ )
	{
		if ( popup->IsItemActive ( i ) )
			GXColorToVec4 ( pi.color, ENABLE_ITEM_COLOR_R, ENABLE_ITEM_COLOR_G, ENABLE_ITEM_COLOR_B, ENABLE_ITEM_COLOR_A );
		else
			GXColorToVec4 ( pi.color, DISABLE_ITEM_COLOR_R, DISABLE_ITEM_COLOR_G, DISABLE_ITEM_COLOR_B, DISABLE_ITEM_COLOR_A );

		surface->AddText ( pi, 0, names[ i ] );
		pi.insertY -= itemHeight;
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
	surface = new GXHudSurface ( width, height );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIPopupRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

//-------------------------------------------------------

EMUIPopup::EMUIPopup ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXUIPopup ( parent ? parent->GetWidget () : nullptr );
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
	EMUIPopupRenderer* renderer = (EMUIPopupRenderer*)widget->GetRenderer ();
	renderer->AddItem ( name );
	widget->AddItem ( handler, action );
}

GXVoid EMUIPopup::EnableItem ( GXUByte itemIndex )
{
	GXUIPopup* popup = (GXUIPopup*)widget;
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
