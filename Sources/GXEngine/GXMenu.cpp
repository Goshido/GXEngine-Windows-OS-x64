//version 1.4

#include <GXEngine/GXMenu.h>
#include <GXEngine/GXUI.h>
#include <GXCommon/GXStrings.h>


#define GX_MENU_NONE				0xFFFFFFFF

#define GX_MENU_STRIPE				35
#define GX_MENU_SELECTOR_OFFSET		22.0f
#define GX_MENU_FONT_OFFSET			20
#define GX_MENU_FONT_SIZE			22
#define GX_MENU_PADDING				30

#define GX_MENU_ENABLE				0
#define GX_MENU_DISABLE				1
#define GX_MENU_GAP					2	

GXWChar* gx_menu_DefaultFontFile = L"../Fonts/trebuc.ttf";


GXMenuStyle::GXMenuStyle ()
{
	showSelector = GX_TRUE;
	showBackground = GX_FALSE;

	inactiveColor = GXVec4 ( 0.451f, 0.725f, 0.0f, 1.0f );
	selectColor = GXVec4 ( 1.0f, 1.0f, 1.0f, 1.0f );
	disableColor = GXVec4 ( 0.5f, 0.5f, 0.5f, 1.0f );

	fontFile = gx_menu_DefaultFontFile;
	fontSize = GX_MENU_FONT_SIZE;
	stripe = GX_MENU_STRIPE;
	selectorOffset = GX_MENU_SELECTOR_OFFSET;
	fontOffset = GX_MENU_FONT_OFFSET;
	padding = GX_MENU_PADDING;
}

//------------------------------------------------------------------------------------------

GXMenu::GXMenu ( GXUShort width, GXUShort height, PFNGXONFOCUSPROC onFocusFunc )
{
	OnFocus = onFocusFunc;

	this->width = width;
	this->height = height;

	surface = 0;

	totalItems = 0;
	itemNames = 0;
	itemAccess = 0;
	itemCallbacks = 0;

	selected = GX_MENU_NONE;
	isEnable = GX_TRUE;

	travelTimer = -1.0f;

	isInvalid = GX_TRUE;
	isDelete = GX_FALSE;

	location = GXVec3 ( 0.0f, 0.0f, 0.0f );
	GXSetMat4Identity ( rotation );

	selectorWidth = 10;

	font = GXGetFont ( gx_menu_DefaultFontFile );
	GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( gx_menu_DefaultFontFile ) + 1 );
	style.fontFile = (GXWChar*)malloc ( size );
	memcpy ( style.fontFile, gx_menu_DefaultFontFile, size );
}

GXVoid GXMenu::Delete ()
{
	isDelete = GX_TRUE;
}

GXVoid GXMenu::Reset ()
{	
	gx_uiMutex->Lock ();

	for ( GXUInt i = 0; i < totalItems; i++ )
	{
		free ( itemNames[ i ] );
	}

	GXSafeFree ( itemNames );
	GXSafeFree ( itemAccess );
	GXSafeFree ( itemCallbacks )

	totalItems = 0;
	selectorWidth = 10;

	selected = GX_MENU_NONE;

	isInvalid = GX_TRUE;

	gx_uiMutex->Release ();
}

GXVoid GXMenu::EnableItem ( GXUInt item )
{
	if ( item >= totalItems ) return;
	if ( itemAccess[ item ] == GX_MENU_ENABLE || itemAccess[ item ] == GX_MENU_GAP ) return;

	itemAccess[ item ] = GX_MENU_ENABLE;
	isInvalid = GX_TRUE;
}

GXVoid GXMenu::EnableMenu ()
{
	if ( isEnable ) return;

	isEnable = GX_TRUE;
	isInvalid = GX_TRUE;
}

GXVoid GXMenu::DisableItem ( GXUInt item )
{
	if ( item >= totalItems ) return;
	if ( itemAccess[ item ] == GX_MENU_DISABLE || itemAccess[ item ] == GX_MENU_GAP ) return;

	itemAccess[ item ] = GX_MENU_DISABLE;
	isInvalid = GX_TRUE;
}

GXVoid GXMenu::DisableMenu ()
{
	if ( !isEnable ) return;

	isEnable = GX_FALSE;
	isInvalid = GX_TRUE;
}

GXVoid GXMenu::OnSelect ()
{
	if ( totalItems == 0 || selected == GX_MENU_NONE ) return;

	if ( itemCallbacks[ selected ] && itemAccess[ selected ] == GX_MENU_ENABLE )
		itemCallbacks[ selected ] ();
}

GXVoid GXMenu::OnFocusPrevious ()
{
	if ( totalItems == 0 ) return;

	if ( selected == GX_MENU_NONE )
	{
		travelTimer = 0.25f;
		selected = 0;

		selectorFrom.x = selectorTo.x = 5.0f;
		selectorFrom.y = selectorTo.y = ( style.padding - GX_MENU_PADDING ) + style.selectorOffset + ( totalItems - 1 ) * style.stripe;

		if ( OnFocus && itemAccess[ selected ] != GX_MENU_GAP )
			OnFocus ( selected );

		return;
	}
	 
	if ( totalItems == 1 ) return;

	GXUInt	was = selected;

	do
	{
		if ( selected == 0 ) 
			selected = totalItems - 1;	
		else
			selected--;
	}
	while ( itemAccess[ selected ] == GX_MENU_GAP );

	travelTimer = 0.25f;

	selectorFrom = selectorTo;
	selectorTo.x = 5.0f;
	selectorTo.y = ( style.padding - GX_MENU_PADDING ) + style.selectorOffset + ( totalItems - selected - 1 ) * style.stripe;

	if ( OnFocus && itemAccess[ selected ] != GX_MENU_GAP )
		OnFocus ( selected );
}

GXVoid GXMenu::OnFocusNext ()
{
	if ( totalItems == 0 ) return;

	if ( selected == GX_MENU_NONE )
	{
		travelTimer = 0.25f;
		selected = 0;

		selectorFrom.x = selectorTo.x = 5.0f;
		selectorFrom.y = selectorTo.y = ( style.padding - GX_MENU_PADDING ) + style.selectorOffset + ( totalItems - 1 ) * style.stripe;

		if ( OnFocus && itemAccess[ selected ] != GX_MENU_GAP )
			OnFocus ( selected );

		return;
	}

	if ( totalItems == 1 ) return;

	GXUInt	was = selected;

	do
	{
		if ( selected == ( totalItems - 1 ) ) 
			selected = 0;
		else
			selected++;
	}
	while ( itemAccess[ selected ] == GX_MENU_GAP );

	travelTimer = 0.25f;

	selectorFrom = selectorTo;
	selectorTo.x = 5.0f;
	selectorTo.y = ( style.padding - GX_MENU_PADDING ) + style.selectorOffset + ( totalItems - selected - 1 ) * style.stripe;

	if ( OnFocus && itemAccess[ selected ] != GX_MENU_GAP )
		OnFocus ( selected );
}

GXVoid GXMenu::OnSetCursorPosition ( GXUShort x, GXUShort y )
{
	//TODO
}

GXVoid GXMenu::AddItem ( const GXWChar* name, PFNGXONSELECTPROC callback )
{
	gx_uiMutex->Lock ();

	isInvalid = GX_TRUE;

	GXUInt was = totalItems;
	totalItems++;

	GXWChar** newNames = (GXWChar**)malloc ( totalItems * sizeof ( GXWChar* ) );
	memcpy ( newNames, itemNames, was * sizeof ( GXWChar* ) );
	GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( name ) + 1 );
	newNames[ was ] = (GXWChar*)malloc ( size );
	memcpy ( newNames[ was ], name, size );

	GXUInt newSelectorWidth = surface->GetTextRasterLength ( font, style.fontSize, GX_TRUE, name ) + ( ( style.fontOffset - 5 ) << 1 );
	if ( newSelectorWidth > selectorWidth )
		selectorWidth = newSelectorWidth;

	GXUChar* newAccess = (GXUChar*)malloc ( totalItems * sizeof ( GXUChar ) );
	memcpy ( newAccess, itemAccess, was * sizeof ( GXUChar ) );
	newAccess[ was ] = GX_MENU_ENABLE;

	PFNGXONSELECTPROC* newCallbacks = (PFNGXONSELECTPROC*)malloc ( totalItems * sizeof ( PFNGXONSELECTPROC ) );
	memcpy ( newCallbacks, itemCallbacks, was * sizeof ( PFNGXONSELECTPROC ) );
	newCallbacks[ was ] = callback;

	GXSafeFree ( itemNames );
	GXSafeFree ( itemAccess );
	GXSafeFree ( itemCallbacks );

	itemNames = newNames;
	itemAccess = newAccess;
	itemCallbacks = newCallbacks;

	gx_uiMutex->Release ();
}

GXVoid GXMenu::AddGap ()
{
	gx_uiMutex->Lock ();

	isInvalid = GX_TRUE;

	GXUInt was = totalItems;
	totalItems++;

	GXWChar** newNames = (GXWChar**)malloc ( totalItems * sizeof ( GXWChar* ) );
	memcpy ( newNames, itemNames, was * sizeof ( GXWChar* ) );
	newNames[ was ] = 0;

	GXUChar* newAccess = (GXUChar*)malloc ( totalItems * sizeof ( GXUChar ) );
	memcpy ( newAccess, itemAccess, was * sizeof ( GXUChar ) );
	newAccess[ was ] = GX_MENU_GAP;

	PFNGXONSELECTPROC* newCallbacks = (PFNGXONSELECTPROC*)malloc ( totalItems * sizeof ( PFNGXONSELECTPROC ) );
	memcpy ( newCallbacks, itemCallbacks, was * sizeof ( PFNGXONSELECTPROC ) );
	newCallbacks[ was ] = 0;

	GXSafeFree ( itemNames );
	GXSafeFree ( itemAccess );
	GXSafeFree ( itemCallbacks );

	itemNames = newNames;
	itemAccess = newAccess;
	itemCallbacks = newCallbacks;

	gx_uiMutex->Release ();
}

GXVoid GXMenu::Update ( GXFloat delta )
{
	if ( travelTimer >= 0.0f )
	{
		travelTimer -= delta;

		GXFloat interpolation = cosf ( GX_MATH_HALFPI * travelTimer * 4.0f );
		selectorNow.x = selectorFrom.x + ( selectorTo.x - selectorFrom.x ) * interpolation;
		selectorNow.y = selectorFrom.y + ( selectorTo.y - selectorFrom.y ) * interpolation;

		isInvalid = GX_TRUE;
	}
}

GXVoid GXMenu::Draw ()
{
	if ( isDelete )
	{
		delete this;
		return;
	}

	if ( !surface )
	{
		surface = new GXHudSurface ( width, height );
		surface->SetLocation ( location );
		surface->SetRotation ( rotation );
		surface->SetScale ( width * 0.5f, height * 0.5f, 1.0f );

		GXLoadMTR ( L"../Materials/System/Menu.mtr", matInfo );
		GXGetTexture ( matInfo.textures[ 0 ] );  
		GXGetTexture ( matInfo.textures[ 1 ] );

		return;
	}

	UpdateSurface ();
	surface->Draw ();
}

GXVoid GXMenu::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	if ( surface )
		surface->SetLocation ( x, y, z );
	else	
		location = GXVec3 ( x, y, z );
}

GXVoid GXMenu::SetRotation ( const GXMat4 &rotation )
{
	if ( surface )
		surface->SetRotation ( rotation );
	else
		this->rotation = rotation;
}

GXVoid GXMenu::SetStyle ( const GXMenuStyle &style )
{
	GXRemoveFont ( font );
	free ( this->style.fontFile );

	memcpy ( &this->style, &style, sizeof ( GXMenuStyle ) );

	GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( style.fontFile ) + 1 );
	this->style.fontFile = (GXWChar*)malloc ( size );
	memcpy ( this->style.fontFile, style.fontFile, size );

	font = GXGetFont ( this->style.fontFile );
}

GXVoid GXMenu::SetFocus ( GXUInt item )
{
	if ( item >= totalItems ) return;

	selected = item;

	travelTimer = 0.25f;

	selectorTo.x = 5.0f;
	selectorTo.y = ( style.padding - GX_MENU_PADDING ) + style.selectorOffset + ( totalItems - selected - 1 ) * style.stripe;

	selectorNow = selectorFrom = selectorTo;
}

GXVoid GXMenu::SetVoidFocus ()
{
	selected = GX_MENU_NONE;
}

GXUInt GXMenu::GetFocus ()
{
	return selected;
}

GXUInt GXMenu::GetTotalItems ()
{
	return totalItems;
}

const GXWChar* GXMenu::GetItemText ( GXUInt item )
{
	if ( item >= totalItems ) return 0;
	if ( itemAccess[ item ] == GX_MENU_GAP ) return 0;

	return itemNames[ item ];
}

GXBool GXMenu::IsFocused ()
{
	if ( selected == GX_MENU_NONE ) return GX_FALSE;

	return GX_TRUE;
}

GXMenu::~GXMenu ()
{
	if ( surface )
	{
		delete surface;
		GXRemoveTexture ( matInfo.textures[ 0 ] );
		GXRemoveTexture ( matInfo.textures[ 1 ] );

		matInfo.Cleanup ();
	}
	
	for ( GXUInt i = 0; i < totalItems; i++ )
	{
		free ( itemNames[ i ] );
	}

	GXSafeFree ( itemNames );
	GXSafeFree ( itemAccess );
	GXSafeFree ( itemCallbacks );

	free ( style.fontFile );
	GXRemoveFont ( font );
}

GXVoid GXMenu::UpdateSurface ()
{
	if ( !isInvalid ) return;

	surface->Reset ();

	GXGLTextureStruct ts;
	GXImageInfo ii;
	ii.texture = &ts;

	if ( style.showBackground )
	{
		ts.width = matInfo.textures[ 0 ].usWidth;
		ts.height = matInfo.textures[ 0 ].usHeight;
		ts.internalFormat = matInfo.textures[ 0 ].bIsAlpha ? GL_RGBA8 : GL_RGB8;

		ii.insertX = -50;
		ii.insertY = 0;
		ii.overlayType = GX_SIMPLE_REPLACE;
		ii.textureID = matInfo.textures[ 0 ].uiId;
		ii.insertWidth = width + 50;
		ii.insertHeight = height;

		surface->AddImage ( ii );
	}

	if ( style.showSelector )
	{
		ts.width = matInfo.textures[ 1 ].usWidth;
		ts.height = matInfo.textures[ 1 ].usHeight;
		ts.internalFormat = matInfo.textures[ 1 ].bIsAlpha ? GL_RGBA8 : GL_RGB8;

		ii.insertX = (GXInt)selectorNow.x;
		ii.insertY = (GXInt)selectorNow.y;
		ii.overlayType = GX_ALPHA_ADD;
		ii.textureID = matInfo.textures[ 1 ].uiId;
		ii.insertWidth = (GXInt)selectorWidth;
		ii.insertHeight = (GXInt)( style.fontSize * 1.4f );

		surface->AddImage ( ii );
	}

	GXPenInfo pi;
	pi.font = font;
	pi.insertX = style.fontOffset;
	pi.fontSize = style.fontSize;
	pi.kerning = GX_TRUE;

	gx_uiMutex->Lock ();

	for ( GXUInt i = 0; i < totalItems; i++ )
	{
		GXUInt item = totalItems - 1 - i;

		if ( itemAccess[ item ] == GX_MENU_GAP ) continue;

		if ( isEnable )
		{
			if ( itemAccess[ item ] == GX_MENU_DISABLE )
				pi.fontColor = &style.disableColor;
			else if ( item == selected )
				pi.fontColor = &style.selectColor;
			else
				pi.fontColor = &style.inactiveColor;
		}
		else
			pi.fontColor = &style.disableColor;

		pi.insertY = style.padding + i * style.stripe;

		surface->AddText ( pi, L"%s", itemNames[ item ] );
	}

	gx_uiMutex->Release ();

	isInvalid = GX_FALSE;
}