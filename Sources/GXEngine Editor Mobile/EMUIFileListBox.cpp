#include <GXEngine_Editor_Mobile/EMUIFileListBox.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXStrings.h>


#define DEFAULT_WIDTH						7.0f
#define DEFAULT_HEIGHT						6.0f
#define DEFAULT_LEFT_BOTTOM_X				0.1f
#define DEFAULT_LEFT_BOTTOM_Y				0.1f

#define FONT								L"Fonts/trebuc.ttf"
#define FONT_SIZE							0.33f
#define ITEM_HEIGHT							0.85f
#define ITEM_NAME_OFFSET_X					0.9f

#define ITEM_NAME_COLOR_R					255
#define ITEM_NAME_COLOR_G					255
#define ITEM_NAME_COLOR_B					255
#define ITEM_NAME_COLOR_A					255

#define BACKGROUND_COLOR_R					49
#define BACKGROUND_COLOR_G					48
#define BACKGROUND_COLOR_B					48
#define BACKGROUND_COLOR_A					255

#define RECTANGLE							L"Textures/System/Default_Diffuse.tga"

#define FOLDER_ICON							L"Textures/Editor Mobile/gui_folder_icon.png"
#define FOLDER_ICON_ASPECT					1.3f
#define FOLDER_ICON_HEIGHT					0.54f

#define FILE_ICON							L"Textures/Editor Mobile/gui_file_icon.png"
#define FILE_ICON_ASPECT					0.75f
#define FILE_ICON_HEIGHT					0.6f

#define HIGHLIGHTED_COLOR_R					25
#define HIGHLIGHTED_COLOR_G					25
#define HIGHLIGHTED_COLOR_B					25
#define HIGHLIGHTED_COLOR_A					25

#define SELECTED_COLOR_R					115
#define SELECTED_COLOR_G					185
#define SELECTED_COLOR_B					0
#define SELECTED_COLOR_A					102

#define ICON_OFFSET_X						0.1f

#define PIXEL_PERFECT_LOCATION_OFFSET_X		0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y		0.25f


EMUIFileListBoxItem::EMUIFileListBoxItem ( eEMUIFileListBoxItemType type, const GXWChar* name )
{
	this->type = type;
	GXWcsclone ( &this->name, name );
}

EMUIFileListBoxItem::~EMUIFileListBoxItem ()
{
	GXSafeFree ( name );
}

//---------------------------------------------------------

class EMUIFileListBoxRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXFont				font;
		GXTexture			rectangle;
		GXTexture			fileIcon;
		GXTexture			folderIcon;

	public:
		explicit EMUIFileListBoxRenderer ( GXUIListBox* widget );
		~EMUIFileListBoxRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIFileListBoxRenderer::EMUIFileListBoxRenderer ( GXUIListBox* widget ) :
GXWidgetRenderer ( widget )
{
	font = GXFont::GetFont ( FONT, (GXUShort)( FONT_SIZE * gx_ui_Scale ) );

	rectangle = GXTexture::LoadTexture ( RECTANGLE, GX_FALSE, GL_CLAMP_TO_EDGE );
	fileIcon = GXTexture::LoadTexture ( FILE_ICON, GX_FALSE, GL_CLAMP_TO_EDGE );
	folderIcon = GXTexture::LoadTexture ( FOLDER_ICON, GX_FALSE, GL_CLAMP_TO_EDGE );

	const GXAABB& boundsLocal = widget->GetBoundsWorld ();
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ) );
}

EMUIFileListBoxRenderer::~EMUIFileListBoxRenderer ()
{
	delete surface;
	GXFont::RemoveFont ( font );

	GXTexture::RemoveTexture ( rectangle );
	GXTexture::RemoveTexture ( folderIcon );
	GXTexture::RemoveTexture ( fileIcon );
}

GXVoid EMUIFileListBoxRenderer::OnRefresh ()
{
	GXUIListBox* listBoxWidget = (GXUIListBox*)widget;

	GXUInt totalItems = listBoxWidget->GetTotalItems ();
	GXUIListBoxItem* items = listBoxWidget->GetItems ();
	GXFloat itemHeight = listBoxWidget->GetItemHeight ();
	GXFloat width = GXGetAABBWidth ( widget->GetBoundsLocal () );
	GXFloat height = GXGetAABBHeight ( widget->GetBoundsLocal () );

	surface->Reset ();
	GXImageInfo ii;
	GXColorToVec4 ( ii.color, BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
	ii.insertX = ii.insertY = 0.0f;
	ii.insertWidth = width;
	ii.insertHeight = height;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;
	ii.texture = &rectangle;

	surface->AddImage ( ii );

	GXFloat folderIconHeight = FOLDER_ICON_HEIGHT * gx_ui_Scale;
	GXFloat folderIconWidth = folderIconHeight * FOLDER_ICON_ASPECT;
	GXFloat folderIconYOffset = ( itemHeight - folderIconHeight ) * 0.5f;

	GXFloat fileIconHeight = FILE_ICON_HEIGHT * gx_ui_Scale;
	GXFloat fileIconWidth = fileIconHeight * FILE_ICON_ASPECT;
	GXFloat fileIconYOffset = ( itemHeight - fileIconWidth ) * 0.5f;

	GXPenInfo pi;
	GXColorToVec4 ( pi.color, ITEM_NAME_COLOR_R, ITEM_NAME_COLOR_G, ITEM_NAME_COLOR_B, ITEM_NAME_COLOR_A );
	pi.font = &font;
	pi.insertX = ITEM_NAME_OFFSET_X * gx_ui_Scale;
	pi.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

	GXFloat iconXOffset = ICON_OFFSET_X * gx_ui_Scale;
	ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

	GXVec4 highlightedColor;
	GXColorToVec4 ( highlightedColor,HIGHLIGHTED_COLOR_R, HIGHLIGHTED_COLOR_G, HIGHLIGHTED_COLOR_B, HIGHLIGHTED_COLOR_A );

	GXVec4 selectedColor;
	GXColorToVec4 ( selectedColor, SELECTED_COLOR_R, SELECTED_COLOR_G, SELECTED_COLOR_B, SELECTED_COLOR_A );

	GXVec4 iconColor;
	GXColorToVec4 ( iconColor, 255, 255, 255, 255 );

	GXFloat itemNameYOffset = ( itemHeight - font.GetSize () * 0.5f ) * 0.5f;

	for ( GXUInt i = 0; i < totalItems; i++ )
	{
		if ( !listBoxWidget->IsItemVisible ( i ) )
		{
			items = items->next;
			continue;
		}

		EMUIFileListBoxItem* item = (EMUIFileListBoxItem*)items->data;
		GXFloat yOffset = listBoxWidget->GetItemLocalOffsetY ( i );

		if ( items->isSelected )
		{
			ii.insertY = yOffset;
			ii.insertX = 0.0f;
			ii.insertWidth = width;
			ii.insertHeight = itemHeight;
			ii.color = selectedColor;
			ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
			ii.texture = &rectangle;

			surface->AddImage ( ii );
		}

		if ( items->isHighlighted )
		{
			ii.insertY = yOffset;
			ii.insertX = 0.0f;
			ii.insertWidth = width;
			ii.insertHeight = itemHeight;
			ii.color = highlightedColor;
			ii.overlayType = eGXImageOverlayType::AlphaAdd;
			ii.texture = &rectangle;

			surface->AddImage ( ii );
		}

		switch ( item->type )
		{
			case eEMUIFileListBoxItemType::File:
				ii.insertY = yOffset + fileIconYOffset;
				ii.insertWidth = fileIconWidth;
				ii.insertHeight = fileIconHeight;
				ii.texture = &fileIcon;
			break;

			case eEMUIFileListBoxItemType::Folder:
				ii.insertY = yOffset + folderIconYOffset;
				ii.insertWidth = folderIconWidth;
				ii.insertHeight = folderIconHeight;
				ii.texture = &folderIcon;
			break;
		}

		ii.insertX = iconXOffset;
		ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
		ii.color = iconColor;

		surface->AddImage ( ii );

		pi.insertY = yOffset + itemNameYOffset;
		surface->AddText ( pi, 0, item->name );

		yOffset -= itemHeight;
		items = items->next;
	}
}

GXVoid EMUIFileListBoxRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Render ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIFileListBoxRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIFileListBoxRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

//---------------------------------------------------------

EMUIFileListBox::EMUIFileListBox ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXUIListBox ( parent ? parent->GetWidget () : nullptr, &EMUIFileListBox::ItemDestructor );
	widget->Resize ( DEFAULT_LEFT_BOTTOM_X * gx_ui_Scale, DEFAULT_LEFT_BOTTOM_Y * gx_ui_Scale, DEFAULT_WIDTH * gx_ui_Scale, DEFAULT_HEIGHT * gx_ui_Scale );
	widget->SetItemHeight ( ITEM_HEIGHT * gx_ui_Scale );
	widget->SetRenderer ( new EMUIFileListBoxRenderer ( widget ) );
}

EMUIFileListBox::~EMUIFileListBox ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUIFileListBox::GetWidget () const
{
	return widget;
}

GXVoid EMUIFileListBox::Resize ( GXFloat leftBottomX, GXFloat leftBottomY, GXFloat width, GXFloat height )
{
	widget->Resize ( leftBottomX, leftBottomY, width, height );
}

GXVoid EMUIFileListBox::AddFolder ( const GXWChar* name )
{
	widget->AddItem ( new EMUIFileListBoxItem ( eEMUIFileListBoxItemType::Folder, name ) );
}

GXVoid EMUIFileListBox::AddFile ( const GXWChar* name )
{
	widget->AddItem ( new EMUIFileListBoxItem ( eEMUIFileListBoxItemType::File, name ) );
}

GXVoid EMUIFileListBox::Clear ()
{
	widget->RemoveAllItems ();
}

GXVoid EMUIFileListBox::AddItems ( const EMUIFileListBoxItem* itemArray, GXUInt items )
{
	if ( items == 0 || !itemArray ) return;

	EMUIFileListBoxItem** elements = (EMUIFileListBoxItem**)malloc ( items * sizeof ( EMUIFileListBoxItem* ) );
	for ( GXUInt i = 0; i < items; i++ )
		elements[ i ] = new EMUIFileListBoxItem ( itemArray[ i ].type, itemArray[ i ].name );
	
	widget->AddItems ( (GXVoid**)elements, items );

	free ( elements );
}

GXVoid EMUIFileListBox::Redraw ()
{
	widget->Redraw ();
}

const GXVoid* EMUIFileListBox::GetSelectedItem () const
{
	return widget->GetSelectedItem ();
}

GXVoid EMUIFileListBox::SetOnItemSelectedCallback ( GXVoid* handler, PFNGXUILISTBOXONITEMSELECTEDPROC callback )
{
	widget->SetOnItemSelectedCallback ( handler, callback );
}

GXVoid EMUIFileListBox::SetOnItemDoubleClickedCallbak ( GXVoid* handler, PFNGXUILISTBOXONITEMDOUBLECLICKEDPROC callback )
{
	widget->SetOnItemDoubleClickedCallback ( handler, callback );
}

GXVoid GXCALL EMUIFileListBox::ItemDestructor ( GXVoid* itemData )
{
	EMUIFileListBoxItem* p = (EMUIFileListBoxItem*)itemData;
	delete p;
}
