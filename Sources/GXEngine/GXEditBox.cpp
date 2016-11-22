//version 1.2

#include <GXEngine/GXEditBox.h>
#include <GXEngine/GXUI.h>
#include <GXEngine/GXGlobals.h>
#include <GXCommon/GXStrings.h>


#define GX_SELECTION_NONE	0xFFFF

#define GX_CONTROL_C		0x0003
#define GX_CONTROL_V		0x0016
#define GX_CONTROL_X		0x0018
#define GX_CONTROL_A		0x0001


GXWChar* gx_edit_boxDefaultFont = L"../Fonts/trebuc.ttf";
GXEditBox* gx_edit_boxControl = 0;


GXEditBoxStyle::GXEditBoxStyle ()
{
	fontFile = gx_edit_boxDefaultFont;
	fontSize = 21;
	fontColor = GXVec4 ( 1.0f, 1.0f, 1.0f, 1.0f );
	kerning = GX_TRUE;
	selectColor = GXVec4 ( 1.0f, 1.0f, 1.0f, 0.5f );
	alignment = GX_LEFT;
	multiline = GX_FALSE;
	OnTextChanged = 0;
}

//---------------------------------------------------------------------------------------------------

GXEditBox::GXEditBox ( GXUShort width, GXUShort height )
{
	surface = 0;

	this->width = width;
	this->height = height;
	location = GXVec3 ( 0.0f, 0.0f, 10.0f );
	GXSetMat4Identity ( rotation );

	numSymbols = 1;
	totalSymbols = 64;
	text = (GXWChar*)malloc ( totalSymbols * sizeof ( GXWChar ) );
	text[ 0 ] = 0;

	carriage = 0;
	selection = GX_SELECTION_NONE;

	isStyleChanged = GX_FALSE;

	timer = 1.0f;

	isDelete = GX_FALSE;
	isUpdate = GX_TRUE;
	isEdit = GX_FALSE;

	font = GXGetFont ( gx_edit_boxDefaultFont );
	GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( gx_edit_boxDefaultFont ) + 1 );
	style.fontFile = (GXWChar*)malloc ( size );
	memcpy ( style.fontFile, gx_edit_boxDefaultFont, size );
}

GXVoid GXEditBox::Delete ()
{
	isDelete = GX_TRUE;
}

GXVoid GXEditBox::CaptureInput ()
{
	GXInput* input = gx_Core->GetInput ();

	input->BindKeyFunc ( &Backspace, VK_BACK, INPUT_DOWN );
	input->BindKeyFunc ( &Del, VK_DELETE, INPUT_DOWN );
	input->BindKeyFunc ( &MoveCarriageBack, VK_LEFT, INPUT_DOWN ); 
	input->BindKeyFunc ( &MoveCarriageForward, VK_RIGHT, INPUT_DOWN );
	input->BindKeyFunc ( &Home, VK_HOME, INPUT_DOWN ); 
	input->BindKeyFunc ( &End, VK_END, INPUT_DOWN );

	input->BindTypeFunc ( &AddSymbol );

	gx_edit_boxControl = this;

	isEdit = GX_TRUE;
}

GXVoid GXEditBox::ReleaseInput ()
{
	gx_edit_boxControl = 0;

	GXInput* input = gx_Core->GetInput ();

	input->UnBindKeyFunc ( VK_BACK, INPUT_DOWN );
	input->UnBindKeyFunc ( VK_DELETE, INPUT_DOWN );
	input->UnBindKeyFunc ( VK_LEFT, INPUT_DOWN ); 
	input->UnBindKeyFunc ( VK_RIGHT, INPUT_DOWN );
	input->UnBindKeyFunc ( VK_HOME, INPUT_DOWN ); 
	input->UnBindKeyFunc ( VK_END, INPUT_DOWN );

	input->UnBindTypeFunc ();

	isEdit = GX_FALSE;
}

GXVoid GXEditBox::SetTextStyle ( const GXEditBoxStyle &style )
{
	GXRemoveFont ( font );
	GXSafeFree ( this->style.fontFile );

	memcpy ( &this->style, &style, sizeof ( GXEditBoxStyle ) );

	if ( style.fontFile == 0 )
	{
		GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( gx_edit_boxDefaultFont ) + 1 );
		this->style.fontFile = (GXWChar*)malloc ( size );
		memcpy ( this->style.fontFile, gx_edit_boxDefaultFont, size );
	}
	else
	{
		GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( style.fontFile ) + 1 );
		this->style.fontFile = (GXWChar*)malloc ( size );
		memcpy ( this->style.fontFile, style.fontFile, size );
	}

	font = GXGetFont ( this->style.fontFile );

	isUpdate = GX_TRUE;
	isStyleChanged = GX_TRUE;
}

const GXWChar* GXEditBox::GetText ()
{
	return text;
}

GXVoid GXEditBox::AddText ( const GXWChar* text )
{
	gx_edit_boxControl = this;

	if ( selection != GX_SELECTION_NONE )
		DeleteSelected ();

	GXUShort c = GXWcslen ( text );

	GXUShort total = c + numSymbols;

	gx_uiMutex->Lock ();

	if ( total > totalSymbols )
	{
		while ( totalSymbols < total )
			totalSymbols += 64;

		GXWChar* temp = (GXWChar*)malloc ( totalSymbols * sizeof ( GXWChar ) );
		memcpy ( temp, this->text, carriage * sizeof ( GXWChar ) );
		memcpy ( temp + carriage, text, c * sizeof ( GXWChar ) );
		memcpy ( temp + carriage + c, this->text + carriage, ( numSymbols - carriage ) * sizeof ( GXWChar ) );

		numSymbols += c;
		carriage += c;

		free ( this->text );
		this->text = temp;
	}
	else
	{
		memmove ( this->text + carriage + c, this->text + carriage, ( numSymbols - carriage ) * sizeof ( GXWChar ) );
		memcpy ( this->text + carriage, text, c * sizeof ( GXWChar ) );

		numSymbols += c;
		carriage += c;
	}

	gx_uiMutex->Release ();

	TextChanged ();
	isUpdate = GX_TRUE;
}

GXVoid GXEditBox::MarkAll ()
{
	carriage = numSymbols - 1;
	selection = 0;

	isUpdate = GX_TRUE;
}

GXVoid GXEditBox::ReplaceText ( const GXWChar* text )
{
	carriage = 0;
	selection = GX_SELECTION_NONE;
	this->text[ 0 ] = 0;

	AddText ( text );
}

GXVoid GXEditBox::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	if ( surface )
		surface->SetLocation ( x, y, z );
	else
	{
		location.x = x;
		location.y = y;
		location.z = z;
	}
}

GXVoid GXEditBox::SetRotation ( const GXMat4 &rotation )
{
	if ( surface )
		surface->SetRotation ( rotation );
	else
		this->rotation = rotation;
}

GXVoid GXEditBox::Draw ()
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
		surface->SetScale ( (GXFloat)( width >> 1 ), (GXFloat)( height >> 1 ), 1.0f );

		GXGLTextureStruct ts;
		ts.width = 1;
		ts.height = 1;
		ts.format = GL_RGBA;
		ts.internalFormat = GL_RGBA8;
		ts.type = GL_UNSIGNED_BYTE;
		ts.wrap = GL_REPEAT;
		ts.anisotropy = 1;
		ts.resampling = GX_TEXTURE_RESAMPLING_NONE;
		ts.data = &style.selectColor;

		texture = GXCreateTexture ( ts );
	}

	UpdateContent ();
	surface->Draw ();
}

GXVoid GXEditBox::Update ( GXFloat delta )
{
	timer -= delta;

	if ( timer < 0.0f )
	{
		timer = 1.0f;
		isUpdate = GX_TRUE;
	}

	if ( timer < 0.5f )
		isUpdate = GX_TRUE;
}

GXEditBox::~GXEditBox ()
{
	ReleaseInput ();

	if ( surface )
	{
		delete surface;
		glDeleteTextures ( 1, &texture );
	}

	GXRemoveFont ( font );
	free ( style.fontFile );
}

GXVoid GXEditBox::UpdateContent ()
{
	if ( !isUpdate ) return;

	if ( isStyleChanged )
	{
		glBindTexture ( GL_TEXTURE_2D, texture );
		glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, style.selectColor.v );
		isStyleChanged = GX_FALSE;
	}

	surface->Reset ();

	GXPenInfo pi;
	pi.fontColor = &style.fontColor;
	pi.font = font;
	pi.fontSize = style.fontSize;
	pi.insertX = 0;
	pi.insertY = 3;
	pi.kerning = style.kerning;

	gx_uiMutex->Lock ();

	surface->AddText ( pi, L"%s", text );

	gx_uiMutex->Release ();

	if ( !isEdit ) return;

	if ( selection != GX_SELECTION_NONE )
	{
		GXGLTextureStruct ts;
		ts.width = 1;
		ts.height = 1;
		ts.internalFormat = GL_RGBA8;

		GXImageInfo ii;
		ii.overlayType = GX_ALPHA_TRANSPARENCY;
		ii.textureID = texture;
		ii.texture = &ts;
		ii.insertY = 0;
		ii.insertHeight = style.fontSize;

		GXUShort start;
		GXUShort end;
		if ( carriage <= selection )
		{
			start = carriage;
			end = selection;
		}
		else
		{
			start = selection;
			end = carriage;
		}

		gx_uiMutex->Lock ();

		GXWChar symbol = text[ start ];
		text[ start ] = 0;
		ii.insertX = surface->GetTextRasterLength ( font, style.fontSize, style.kerning, L"%s", text );
		text[ start ] = symbol;

		symbol = text[ end ];
		text[ end ] = 0;
		ii.insertWidth = surface->GetTextRasterLength ( font, style.fontSize, style.kerning, L"%s", text ) - ii.insertX;
		text[ end ] = symbol;

		gx_uiMutex->Release ();

		surface->AddImage ( ii );
	}

	if ( timer < 0.5f ) return;

	gx_uiMutex->Lock ();

	GXWChar symbol = text[ carriage ];
	text[ carriage ] = 0;
	pi.insertX = surface->GetTextRasterLength ( font, style.fontSize, style.kerning, L"%s", text );
	surface->AddText ( pi, L"|" );

	text[ carriage ] = symbol;

	gx_uiMutex->Release ();

	isUpdate = GX_FALSE;
}

GXVoid GXCALL GXEditBox::AddSymbol ( GXWChar symbol )
{
	switch ( symbol )
	{
		case GX_CONTROL_C:
			Copy ();
		return;

		case GX_CONTROL_V:
			Paste ();
		return;

		case GX_CONTROL_X:
			Cut ();
		return;

		case GX_CONTROL_A:
			SelectAll ();
		return;

		default:
		break;
	}

	if ( gx_edit_boxControl->selection != GX_SELECTION_NONE )
		DeleteSelected ();
	
	gx_uiMutex->Lock ();

	GXUShort numSymbols = gx_edit_boxControl->numSymbols + 1;
	if ( numSymbols > gx_edit_boxControl->totalSymbols )
	{
		GXUInt size = ( gx_edit_boxControl->totalSymbols + 64 ) * sizeof ( GXWChar );
		GXWChar* temp = (GXWChar*)malloc ( size );

		memcpy ( temp, gx_edit_boxControl->text, ( gx_edit_boxControl->carriage + 1 ) * sizeof ( GXWChar ) );
		gx_edit_boxControl->carriage++;
		temp[ gx_edit_boxControl->carriage ] = symbol;
		memcpy ( temp + gx_edit_boxControl->carriage + 1, gx_edit_boxControl->text + gx_edit_boxControl->carriage, ( gx_edit_boxControl->numSymbols - gx_edit_boxControl->carriage ) * sizeof ( GXWChar ) );

		gx_edit_boxControl->totalSymbols += 64;
		gx_edit_boxControl->numSymbols = numSymbols;

		free ( gx_edit_boxControl->text );
		gx_edit_boxControl->text = temp;
	}
	else
	{
		memmove ( gx_edit_boxControl->text + gx_edit_boxControl->carriage + 1, gx_edit_boxControl->text + gx_edit_boxControl->carriage, ( gx_edit_boxControl->numSymbols - gx_edit_boxControl->carriage ) * sizeof ( GXWChar ) );  
		gx_edit_boxControl->text[ gx_edit_boxControl->carriage ] = symbol;
		gx_edit_boxControl->carriage++;
		gx_edit_boxControl->numSymbols++;
	}

	gx_uiMutex->Release ();

	TextChanged ();
	gx_edit_boxControl->isUpdate = GX_TRUE;
}

GXVoid GXCALL GXEditBox::Backspace ()
{
	if ( gx_edit_boxControl->selection == GX_SELECTION_NONE && gx_edit_boxControl->carriage == 0 ) return;

	if ( gx_edit_boxControl->selection != GX_SELECTION_NONE )
		DeleteSelected ();
	else
	{
		gx_uiMutex->Lock ();

		memmove ( gx_edit_boxControl->text + gx_edit_boxControl->carriage - 1, gx_edit_boxControl->text + gx_edit_boxControl->carriage, ( gx_edit_boxControl->numSymbols - gx_edit_boxControl->carriage ) * sizeof ( GXWChar ) );  
		gx_edit_boxControl->carriage--;
		gx_edit_boxControl->numSymbols--;

		gx_uiMutex->Release ();
	}

	TextChanged ();
	gx_edit_boxControl->isUpdate = GX_TRUE;
}

GXVoid GXCALL GXEditBox::Del ()
{
	if ( gx_edit_boxControl->selection == GX_SELECTION_NONE && gx_edit_boxControl->carriage == ( gx_edit_boxControl->numSymbols - 1 ) ) return;

	if ( gx_edit_boxControl->selection != GX_SELECTION_NONE )
		DeleteSelected ();
	else
	{
		gx_uiMutex->Lock ();

		memmove ( gx_edit_boxControl->text + gx_edit_boxControl->carriage, gx_edit_boxControl->text + gx_edit_boxControl->carriage + 1, ( gx_edit_boxControl->numSymbols - gx_edit_boxControl->carriage - 1 ) * sizeof ( GXWChar ) );  
		gx_edit_boxControl->numSymbols--;

		gx_uiMutex->Release ();
	}

	TextChanged ();
	gx_edit_boxControl->isUpdate = GX_TRUE;
}

GXVoid GXCALL GXEditBox::MoveCarriageForward ()
{
	GXUShort stateL = GetKeyState ( VK_LSHIFT );
	GXUShort stateR = GetKeyState ( VK_RSHIFT );

	if ( !( stateL & 0x8000 ) && !( stateR & 0x8000 ) )
		gx_edit_boxControl->selection = GX_SELECTION_NONE;
	else if ( gx_edit_boxControl->selection == GX_SELECTION_NONE && gx_edit_boxControl->carriage < ( gx_edit_boxControl->numSymbols - 1 ) )
		gx_edit_boxControl->selection = gx_edit_boxControl->carriage;

	if ( gx_edit_boxControl->carriage < ( gx_edit_boxControl->numSymbols - 1 ) )
		gx_edit_boxControl->carriage++;

	gx_edit_boxControl->isUpdate = GX_TRUE;
}

GXVoid GXCALL GXEditBox::MoveCarriageBack ()
{
	GXUShort stateL = GetKeyState ( VK_LSHIFT );
	GXUShort stateR = GetKeyState ( VK_RSHIFT );

	if ( !( stateL & 0x8000 ) && !( stateR & 0x8000 ) )
		gx_edit_boxControl->selection = GX_SELECTION_NONE;
	else if ( gx_edit_boxControl->selection == GX_SELECTION_NONE && gx_edit_boxControl->carriage > 0 )
		gx_edit_boxControl->selection = gx_edit_boxControl->carriage;

	if ( gx_edit_boxControl->carriage > 0 )
		gx_edit_boxControl->carriage--;

	gx_edit_boxControl->isUpdate = GX_TRUE;
}

GXVoid GXCALL GXEditBox::Home ()
{
	GXUShort stateL = GetKeyState ( VK_LSHIFT );
	GXUShort stateR = GetKeyState ( VK_RSHIFT );

	if ( !( stateL & 0x8000 ) && !( stateR & 0x8000 ) )
		gx_edit_boxControl->selection = GX_SELECTION_NONE;
	else if ( gx_edit_boxControl->selection == GX_SELECTION_NONE && gx_edit_boxControl->carriage > 0 )
		gx_edit_boxControl->selection = gx_edit_boxControl->carriage;

	gx_edit_boxControl->carriage = 0;

	gx_edit_boxControl->isUpdate = GX_TRUE;
}

GXVoid GXCALL GXEditBox::End ()
{
	GXUShort stateL = GetKeyState ( VK_LSHIFT );
	GXUShort stateR = GetKeyState ( VK_RSHIFT );

	if ( !( stateL & 0x8000 ) && !( stateR & 0x8000 ) )
		gx_edit_boxControl->selection = GX_SELECTION_NONE;
	else if ( gx_edit_boxControl->selection == GX_SELECTION_NONE && gx_edit_boxControl->carriage < ( gx_edit_boxControl->numSymbols - 1 ) )
		gx_edit_boxControl->selection = gx_edit_boxControl->carriage;

	gx_edit_boxControl->carriage = gx_edit_boxControl->numSymbols - 1;

	gx_edit_boxControl->isUpdate = GX_TRUE;
}

GXVoid GXCALL GXEditBox::Copy ()
{
	if ( gx_edit_boxControl->selection == GX_SELECTION_NONE ) return;

	GXUShort marked = abs ( gx_edit_boxControl->selection - gx_edit_boxControl->carriage ) + 1;
	GXUInt size = ( marked + 1 ) * sizeof ( GXWChar );
	GXWChar* clipData = (GXWChar*)GlobalAlloc ( GMEM_FIXED, size );

	GXUShort start = ( gx_edit_boxControl->selection <= gx_edit_boxControl->carriage ) ? gx_edit_boxControl->selection : gx_edit_boxControl->carriage;

	memcpy ( clipData, gx_edit_boxControl->text + start, size );
	clipData[ marked ] = 0;

	OpenClipboard ( 0 );
	EmptyClipboard ();
	SetClipboardData ( CF_UNICODETEXT, clipData );
	CloseClipboard ();
}

GXVoid GXCALL GXEditBox::Paste ()
{
    if ( !IsClipboardFormatAvailable ( CF_UNICODETEXT ) )
        return;
    if ( !OpenClipboard ( 0 ) )
        return;
 
    HGLOBAL	hglb = GetClipboardData ( CF_UNICODETEXT );
    if ( hglb )
    {
        GXWChar* data = (GXWChar*)GlobalLock ( hglb );
        if ( data )
        {
			gx_edit_boxControl->AddText ( data );
            GlobalUnlock ( hglb );
        }
    }
    
    CloseClipboard ();

	TextChanged ();
}

GXVoid GXCALL GXEditBox::Cut ()
{
	if ( gx_edit_boxControl->selection == GX_SELECTION_NONE ) return;

	Copy ();
	DeleteSelected ();

	TextChanged ();
}

GXVoid GXCALL GXEditBox::SelectAll ()
{
	gx_edit_boxControl->MarkAll ();
}

GXVoid GXCALL GXEditBox::DeleteSelected ()
{
	GXUInt marked = abs ( gx_edit_boxControl->selection - gx_edit_boxControl->carriage );

	GXUShort start;
	if ( gx_edit_boxControl->selection <= gx_edit_boxControl->carriage )
		start = gx_edit_boxControl->selection;
	else
		start = gx_edit_boxControl->carriage;

	gx_uiMutex->Lock ();

	memmove ( gx_edit_boxControl->text + start, gx_edit_boxControl->text + start + marked, ( gx_edit_boxControl->numSymbols - marked ) * sizeof ( GXWChar ) );

	gx_edit_boxControl->numSymbols -= marked;
	gx_edit_boxControl->selection = GX_SELECTION_NONE;
	gx_edit_boxControl->carriage = start;

	gx_uiMutex->Release ();
}

GXVoid GXCALL GXEditBox::TextChanged ()
{
	if ( gx_edit_boxControl->style.OnTextChanged )
		gx_edit_boxControl->style.OnTextChanged ();
}