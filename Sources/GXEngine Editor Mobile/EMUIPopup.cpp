#include <GXEngine_Editor_Mobile/EMUIPopup.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXFont.h>
#include <GXCommon/GXStrings.h>


#define EM_ANY_HEIGHT				1.0f

#define EM_DEFAULT_ITEM_WIDTH		5.0f
#define EM_DEFAULT_ITEM_HEIGHT		0.7f
#define EM_DEFAULT_BOTTOM_LEFT_X	0.1f
#define EM_DEFAULT_BOTTOM_LEFT_Y	0.1f

#define EM_DEFAULT_FONT				L"Fonts/trebuc.ttf"
#define EM_DEFAULT_FONT_SIZE		0.33f
#define EM_TEXT_OFFSET_X			0.2f

#define EM_BACKGROUND_COLOR_R		49
#define EM_BACKGROUND_COLOR_G		48
#define EM_BACKGROUND_COLOR_B		48
#define EM_BACKGROUND_COLOR_A		255

#define EM_BORDER_COLOR_R			128
#define EM_BORDER_COLOR_G			128
#define EM_BORDER_COLOR_B			128
#define EM_BORDER_COLOR_A			255

#define EM_ENABLE_ITEM_COLOR_R		115
#define EM_ENABLE_ITEM_COLOR_G		185
#define EM_ENABLE_ITEM_COLOR_B		0
#define EM_ENABLE_ITEM_COLOR_A		255

#define EM_DISABLE_ITEM_COLOR_R		136
#define EM_DISABLE_ITEM_COLOR_G		136
#define EM_DISABLE_ITEM_COLOR_B		136
#define EM_DISABLE_ITEM_COLOR_A		255

#define EM_HIGHTLIGHT_COLOR_R		255
#define EM_HIGHTLIGHT_COLOR_G		255
#define EM_HIGHTLIGHT_COLOR_B		255
#define EM_HIGHTLIGHT_COLOR_A		38

#define EM_DEFAULT_TEXTURE			L"Textures/System/Default_Diffuse.tga"


class EMUIPopupRenderer : public GXWidgetRenderer
{
	private:
		GXFont			font;
		GXHudSurface*	surface;
		GXTexture		texture;
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
	font = GXFont::GetFont ( EM_DEFAULT_FONT, (GXUShort)( EM_DEFAULT_FONT_SIZE * gx_ui_Scale ) );
	texture = GXTexture::LoadTexture ( EM_DEFAULT_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );
	surface = new GXHudSurface ( (GXShort)widget->GetItemWidth (), (GXUShort)widget->GetItemHeight () );
}

EMUIPopupRenderer::~EMUIPopupRenderer ()
{
	GXFont::RemoveFont ( font );
	delete surface;
	GXTexture::RemoveTexture ( texture );

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
	GXColorToVec4 ( ii.color, EM_BACKGROUND_COLOR_R, EM_BACKGROUND_COLOR_G, EM_BACKGROUND_COLOR_B, EM_BACKGROUND_COLOR_A );
	ii.texture = &texture;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;
	ii.insertX = 1.0f + 0.1f;
	ii.insertY = 1.0f + 0.1f;
	ii.insertWidth = itemWidth - 2.0f - 0.2f;
	ii.insertHeight = totalHeight - 2.0f - 0.2f;
	
	surface->AddImage ( ii );

	GXLineInfo li;
	GXColorToVec4 ( li.color, EM_BORDER_COLOR_R, EM_BORDER_COLOR_G, EM_BORDER_COLOR_B, EM_BORDER_COLOR_A );
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
		GXColorToVec4 ( ii.color, EM_HIGHTLIGHT_COLOR_R, EM_HIGHTLIGHT_COLOR_G, EM_HIGHTLIGHT_COLOR_B, EM_HIGHTLIGHT_COLOR_A );
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
	pi.insertX = EM_TEXT_OFFSET_X * gx_ui_Scale;
	pi.insertY = totalHeight - itemHeight + ( itemHeight - (GXFloat)font.GetSize () ) * 0.5f;

	for ( GXUByte i = 0; i < totalItems; i++ )
	{
		if ( popup->IsItemActive ( i ) )
			GXColorToVec4 ( pi.color, EM_ENABLE_ITEM_COLOR_R, EM_ENABLE_ITEM_COLOR_G, EM_ENABLE_ITEM_COLOR_B, EM_ENABLE_ITEM_COLOR_A );
		else
			GXColorToVec4 ( pi.color, EM_DISABLE_ITEM_COLOR_R, EM_DISABLE_ITEM_COLOR_G, EM_DISABLE_ITEM_COLOR_B, EM_DISABLE_ITEM_COLOR_A );

		const GXWChar* t = names[ i ];
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

	GXVec3 location;
	surface->GetLocation ( location );

	delete surface;
	surface = new GXHudSurface ( width, height );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIPopupRenderer::OnMoved ( GXFloat x, GXFloat y )
{
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
	widget->Resize ( EM_DEFAULT_BOTTOM_LEFT_X * gx_ui_Scale, EM_DEFAULT_BOTTOM_LEFT_Y * gx_ui_Scale, EM_DEFAULT_ITEM_WIDTH * gx_ui_Scale, EM_ANY_HEIGHT * gx_ui_Scale );
	widget->SetItemHeight ( EM_DEFAULT_ITEM_HEIGHT * gx_ui_Scale );
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
	widget->Resize ( x, y, EM_DEFAULT_ITEM_WIDTH * gx_ui_Scale, EM_ANY_HEIGHT );
}

GXVoid EMUIPopup::Show ( EMUI* owner )
{
	widget->Show ( owner->GetWidget () );
}
