// version 1.3

#include <GXEngine/GXUIEditBox.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXTextValidator.h>
#include <GXEngine/GXInput.h>
#include <GXCommon/GXStrings.h>


#define GX_UI_DEFAULT_FONT					L"Fonts/trebuc.ttf"
#define GX_UI_DEFAULT_FONT_SIZE				0.33f
#define GX_UI_DEFAULT_TEXT_ALIGNMENT		eGXUITextAlignment::Left
#define GX_UI_KEYSTATE_MASK					0x8000
#define GX_UI_SEPARATOR_SYMBOL				L' '
#define GX_UI_EPSILON						0.2f
#define GX_UI_DEFAULT_TEXT_LEFT_OFFSET		0.1f
#define GX_UI_DEFAULT_TEXT_RIGHT_OFFSET		0.1f

#define GX_CTRL_C							0x0003
#define GX_CTRL_V							0x0016
#define GX_CTRL_X							0x0018
#define GX_CTRL_A							0x0001
#define GX_HOME								0x0017
#define GX_END								0x0019
#define GX_DEL								0x001A
#define GX_BACKSPACE						0x001B
#define GX_LEFT_ARROW						0x001C
#define GX_RIGHT_ARROW						0x001D

#define GX_TEXT_GROW_FACTOR					16u

//---------------------------------------------------------------------------------------------------------------------

struct GXUIEditBoxFontInfo
{
	GXWChar*		fontFile;
	GXUShort		size;
};

//---------------------------------------------------------------------------------------------------------------------

GXUIEditBox::GXUIEditBox ( GXWidget* parent ):
	GXWidget ( parent ),
	text ( nullptr ),
	workingBuffer ( nullptr ),
	textSymbols ( 0u ),
	maxSymbols ( 0u ),
	textLeftOffset ( GX_UI_DEFAULT_TEXT_LEFT_OFFSET * gx_ui_Scale ),
	textRightOffset ( GX_UI_DEFAULT_TEXT_RIGHT_OFFSET * gx_ui_Scale ),
	font ( new GXFont ( GX_UI_DEFAULT_FONT, static_cast<GXUShort> ( GX_UI_DEFAULT_FONT_SIZE * gx_ui_Scale ) ) ),
	cursor ( 0 ),
	selection ( 0 ),
	alignment ( GX_UI_DEFAULT_TEXT_ALIGNMENT ),
	validator ( nullptr ),
	OnFinishEditing ( nullptr ),
	handler ( nullptr ),
	editCursor ( LoadCursorW ( 0, IDC_IBEAM ) ),
	arrowCursor ( LoadCursorW ( 0, IDC_ARROW ) )
{
	currentCursor = &arrowCursor;
}

GXUIEditBox::~GXUIEditBox ()
{
	delete font;
	GXSafeFree ( text );
}

GXVoid GXUIEditBox::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
	if ( message == eGXUIMessage::SetText )
	{
		textSymbols = GXWcslen ( static_cast<const GXWChar*> ( data ) );
		GXUInt size = sizeof ( GXWChar ) * ( textSymbols + 1 );

		if ( textSymbols > maxSymbols )
		{
			GXSafeFree ( text );
			GXSafeFree ( workingBuffer );

			text = static_cast<GXWChar*> ( malloc ( size ) );
			workingBuffer = static_cast<GXWChar*> ( malloc ( size ) );

			maxSymbols = textSymbols;
		}

		memcpy ( text, static_cast<const GXWChar*> ( data ), size );
		cursor = selection = 0;

		if ( validator )
			validator->Validate ( text );

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::ClearText )
	{
		if ( textSymbols == 0 ) return;

		text[ 0 ] = 0;

		textSymbols = 0;
		cursor = selection = 0;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::EditBoxSetFont )
	{
		const GXUIEditBoxFontInfo* fi = static_cast<const GXUIEditBoxFontInfo*> ( data );
		delete font;
		font = new GXFont ( fi->fontFile, fi->size );

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::SetTextAlignment )
	{
		const eGXUITextAlignment* newAlignment = static_cast<const eGXUITextAlignment*> ( data );
		this->alignment = *newAlignment;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::LMBDown )
	{
		// Захватить ввод с клавиатуры, если курсор в области виджета.
		// Освободить ввод с клавиатуры, если курсор не в области виджета,
		// и отправить сообщение на нужный виджет.
		// Поставить курсор в правильное место, если щелчок в области виджета.
		// Если щелчок в области виджета и зажат Shift, то выделить область текста.

		const GXVec2* pos = static_cast<const GXVec2*> ( data );

		if ( boundsWorld.IsOverlaped ( pos->GetX (), pos->GetY (), 0.0f ) )
		{
			LockInput ();

			if ( GetKeyState ( VK_SHIFT ) & GX_UI_KEYSTATE_MASK )
				cursor = GetSelectionPosition ( *pos );
			else
				cursor = selection = GetSelectionPosition ( *pos );
		}
		else
		{
			ReleaseInput ();

			if ( validator && validator->Validate ( text ) && OnFinishEditing )
				OnFinishEditing ( handler, *this );
			else if ( OnFinishEditing )
				OnFinishEditing ( handler, *this );

			GXTouchSurface::GetInstance ().OnLeftMouseButtonDown ( *pos );
		}

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::DoubleClick )
	{
		if ( textSymbols == 0u ) return;

		const GXVec2* pos = static_cast<const GXVec2*> ( data );
		GXInt current = GetSelectionPosition ( *pos );

		if ( text[ current ] == GX_UI_SEPARATOR_SYMBOL ) return;

		GXInt end = current + 1;

		for ( ; end < static_cast<GXInt> ( textSymbols ); end++ )
		{
			if ( text[ end ] == GX_UI_SEPARATOR_SYMBOL )
				break;
		}

		GXInt begin = current;

		for ( ; begin > 0; begin-- )
		{
			if ( text[ begin ] == GX_UI_SEPARATOR_SYMBOL )
			{
				begin++;
				break;
			}
		}

		if ( selection != begin || cursor != end )
		{
			selection = begin;
			cursor = end;

			if ( renderer )
				renderer->OnUpdate ();
		}

		return;
	}

	if ( message == eGXUIMessage::MouseOver )
	{
		SetCursor ( editCursor );
		return;
	}

	if ( message == eGXUIMessage::MouseLeave )
	{
		SetCursor ( arrowCursor );
		return;
	}

	if ( message == eGXUIMessage::MouseMove )
	{
		const GXVec2* pos = static_cast<const GXVec2*> ( data );

		if ( boundsWorld.IsOverlaped ( pos->GetX (), pos->GetY (), 0.0f ) )
		{
			if ( currentCursor != &editCursor )
			{
				currentCursor = &editCursor;
				SetCursor ( editCursor );
			}
		}
		else
		{
			if ( currentCursor != &arrowCursor)
			{
				currentCursor = &arrowCursor;
				SetCursor ( arrowCursor );
			}
		}

		if ( textSymbols == 0 ) return;

		if ( GetKeyState ( VK_LBUTTON ) & GX_UI_KEYSTATE_MASK )
		{
			GXInt current = GetSelectionPosition ( *pos );

			if ( current != cursor )
			{
				cursor = current;

				if ( renderer )
					renderer->OnUpdate ();
			}
		}

		return;
	}

	if ( message == eGXUIMessage::AddSumbol )
	{
		// Вставка обычного символа, END, HOME, стрелочки
		// Ctrl + V, Ctrl + C, Ctrl + X
		// Shift + HOME, Shift + END.
		// Del, Backspace
		// Учёт выделенного текста.

		const GXWChar* symbol = static_cast<const GXWChar*> ( data );

		switch ( *symbol )
		{
			case GX_CTRL_C:
				CopyText ();
			break;

			case GX_CTRL_V:
			{
				if ( IsClipboardFormatAvailable ( CF_UNICODETEXT ) && OpenClipboard ( static_cast<HWND> ( 0 ) ) )
				{
					HGLOBAL block = GetClipboardData ( CF_UNICODETEXT );

					if ( block )
					{
						GXWChar* clipBoardData = static_cast<GXWChar*> ( GlobalLock ( block ) );

						if ( clipBoardData )
						{
							DeleteText ();
							PasteText ( clipBoardData );
							GlobalUnlock ( block );

							if ( renderer )
								renderer->OnUpdate ();
						}
					}

					CloseClipboard ();
				}
			}
			break;

			case GX_CTRL_X:
			{
				CopyText ();

				if ( DeleteText () && renderer )
					renderer->OnUpdate ();
			}
			break;

			case GX_CTRL_A:
			{
				if ( cursor != (GXInt)textSymbols || selection != 0 )
				{
					cursor = (GXInt)textSymbols;
					selection = 0;

					if ( renderer )
						renderer->OnUpdate ();
				}
			}
			break;

			case GX_HOME:
				UpdateCursor ( 0 );
			break;

			case GX_END:
				UpdateCursor ( static_cast<GXInt> ( textSymbols ) );
			break;

			case GX_DEL:
			{
				if ( cursor == selection && cursor == static_cast<GXInt> ( textSymbols ) )
					break;
				else if ( cursor == selection )
					selection++;

				if ( DeleteText () && renderer )
					renderer->OnUpdate ();
			}
			break;

			case GX_BACKSPACE:
			{
				if ( cursor == selection && cursor == 0 )
					break;
				else if ( cursor == selection )
					cursor--;

				if ( DeleteText () && renderer )
					renderer->OnUpdate ();
			}
			break;

			case GX_LEFT_ARROW:
				UpdateCursor ( cursor - 1 );
			break;

			case GX_RIGHT_ARROW:
				UpdateCursor ( cursor + 1 );
			break;

			default:
			{
				// Добавить символ
				static GXWChar buffer[ 2 ] = { 0 };
				buffer[ 0 ] = *symbol;

				DeleteText ();
				PasteText ( buffer );

				if ( renderer )
					renderer->OnUpdate ();
			}
			break;
		}

		return;
	}

	if ( message == eGXUIMessage::EditBoxSetTextLeftOffset )
	{
		const GXFloat* offset = static_cast<const GXFloat*> ( data );

		if ( textLeftOffset == *offset ) return;

		textLeftOffset = *offset;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	if ( message == eGXUIMessage::EditBoxSetTextRightOffset )
	{
		const GXFloat* offset = static_cast<const GXFloat*> ( data );

		if ( textRightOffset == *offset ) return;

		textRightOffset = *offset;

		if ( renderer )
			renderer->OnUpdate ();

		return;
	}

	GXWidget::OnMessage ( message, data );
}

GXFloat GXUIEditBox::GetCursorOffset () const
{
	return GetSelectionOffset ( static_cast<GXUInt> ( cursor ) );
}

GXFloat GXUIEditBox::GetSelectionBeginOffset () const
{
	return GetSelectionOffset ( cursor > selection ? static_cast<GXUInt> ( cursor ) : static_cast<GXUInt> ( selection ) );
}

GXFloat GXUIEditBox::GetSelectionEndOffset () const
{
	return GetSelectionOffset ( cursor < selection ? static_cast<GXUInt> ( cursor ) : static_cast<GXUInt> ( selection ) );
}

GXVoid GXUIEditBox::SetTextLeftOffset ( GXFloat offset )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::EditBoxSetTextLeftOffset, &offset, sizeof ( GXFloat ) );
}

GXFloat GXUIEditBox::GetTextLeftOffset () const
{
	return textLeftOffset;
}

GXVoid GXUIEditBox::SetTextRightOffset ( GXFloat offset )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::EditBoxSetTextRightOffset, &offset, sizeof ( GXFloat ) );
}

GXFloat GXUIEditBox::GetTextRightOffset () const
{
	return textRightOffset;
}

GXVoid GXUIEditBox::SetText ( const GXWChar* newText )
{
	if ( newText )
		GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::SetText, newText, ( GXWcslen ( newText ) + 1 ) * sizeof ( GXWChar ) );
	else
		GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ClearText, 0, 0 );
}

const GXWChar* GXUIEditBox::GetText () const
{
	return text;
}

GXVoid GXUIEditBox::SetAlignment ( eGXUITextAlignment newAlignment )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::SetTextAlignment, &newAlignment, sizeof ( eGXUITextAlignment ) );
}

eGXUITextAlignment GXUIEditBox::GetAlignment () const
{
	return alignment;
}

GXVoid GXUIEditBox::SetFont ( const GXWChar* fontFile, GXUShort fontSize )
{
	if ( !fontFile ) return;

	GXUInt size = ( GXWcslen ( fontFile ) + 1 ) * sizeof ( GXWChar );

	GXUIEditBoxFontInfo fi;
	fi.fontFile = static_cast<GXWChar*> ( gx_ui_MessageBuffer->Allocate ( size ) );
	memcpy ( fi.fontFile, fontFile, size );
	fi.size = fontSize;

	GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::EditBoxSetFont, &fi, sizeof ( GXUIEditBoxFontInfo ) );
}

GXFont* GXUIEditBox::GetFont ()
{
	return font;
}

GXBool GXUIEditBox::IsActive ()
{
	return GXTouchSurface::GetInstance ().GetLockedCursorWidget () == this;
}

GXVoid GXUIEditBox::SetValidator ( GXTextValidator &validatorObject )
{
	validator = &validatorObject;
}

GXTextValidator* GXUIEditBox::GetValidator () const
{
	return validator;
}

GXVoid GXUIEditBox::SetOnFinishEditingCallback ( GXVoid* handlerObject, PFNGXUIEDITBOXONFINISHEDITINGPROC callback )
{
	handler = handlerObject;
	OnFinishEditing = callback;
}

GXInt GXUIEditBox::GetSelectionPosition ( const GXVec2 &mousePosition ) const
{
	GXVec3 center;
	boundsWorld.GetCenter ( center );
	GXFloat width = boundsWorld.GetWidth ();
	GXFloat offset = 0.0f;
	GXFloat textLength = static_cast<GXFloat> ( font->GetTextLength ( 0u, text ) );

	switch ( alignment )
	{
		case  eGXUITextAlignment::Center:
		{
			GXFloat textOffset = ( width - textLength ) * 0.5f;
			offset = GXClampf ( mousePosition.GetX () - boundsWorld.min.GetX (), textOffset, width - textOffset ) - textOffset;
		}
		break;

		case  eGXUITextAlignment::Right:
		{
			GXFloat textOffset = width - textLength;
			offset = GXClampf ( mousePosition.GetX () - boundsWorld.min.GetX () - textRightOffset, textOffset, width ) - textOffset;
		}
		break;

		case eGXUITextAlignment::Left:
			textLength = static_cast<GXFloat> ( font->GetTextLength ( 0u, text ) );
			offset = GXClampf ( mousePosition.GetX () - boundsWorld.min.GetX () + textLeftOffset, textLeftOffset, textLength );
		break;
	}

	if ( offset == 0.0f ) return 0;

	if ( abs ( offset - textLength ) < GX_UI_EPSILON ) 
		return static_cast<GXInt> ( GXWcslen ( text ) );

	for ( GXUInt i = 0u; i < textSymbols; i++ )
	{
		workingBuffer[ i ] = text[ i ];
		workingBuffer[ i + 1 ] = 0;
		GXFloat currentOffset = static_cast<GXFloat> ( font->GetTextLength ( 0u, workingBuffer ) );

		if ( currentOffset > offset )
			return static_cast<GXInt> ( i );
	}

	return 0;
}

GXFloat GXUIEditBox::GetSelectionOffset ( GXUInt symbolIndex ) const
{
	if ( textSymbols == 0 ) return 0.0f;

	memcpy ( workingBuffer, text, sizeof ( GXWChar ) * symbolIndex );
	workingBuffer[ symbolIndex ] = 0;

	GXFloat textLength = static_cast<GXFloat> ( font->GetTextLength ( 0u, text ) );
	GXFloat textOffset = 0.0f;

	switch ( alignment )
	{
		case eGXUITextAlignment::Center:
			textOffset = ( boundsWorld.GetWidth () - textLength ) * 0.5f;
		break;

		case eGXUITextAlignment::Right:
			textOffset = boundsWorld.GetWidth () - textLength - textRightOffset;
		break;

		case eGXUITextAlignment::Left:
			textOffset = textLeftOffset;
		break;
	}

	return textOffset + static_cast<GXFloat> ( font->GetTextLength ( 0, workingBuffer ) );
}

GXVoid GXUIEditBox::LockInput ()
{
	GXTouchSurface::GetInstance ().LockCursor ( this );

	GXInput& input = GXInput::GetInstance ();
	input.BindKeyCallback ( this, &GXUIEditBox::OnBackspace, VK_BACK, eGXInputButtonState::Down );
	input.BindKeyCallback ( this, &GXUIEditBox::OnDel, VK_DELETE, eGXInputButtonState::Down );
	input.BindKeyCallback ( this, &GXUIEditBox::OnLeftArrow, VK_LEFT, eGXInputButtonState::Down );
	input.BindKeyCallback ( this, &GXUIEditBox::OnRightArrow, VK_RIGHT, eGXInputButtonState::Down );
	input.BindKeyCallback ( this, &GXUIEditBox::OnHome, VK_HOME, eGXInputButtonState::Down );
	input.BindKeyCallback ( this, &GXUIEditBox::OnEnd, VK_END, eGXInputButtonState::Down );
	input.BindTypeCallback ( this, &GXUIEditBox::OnType );
}

GXVoid GXUIEditBox::ReleaseInput ()
{
	GXTouchSurface::GetInstance ().ReleaseCursor ();

	GXInput& input = GXInput::GetInstance ();
	input.UnbindKeyCallback ( VK_BACK, eGXInputButtonState::Down );
	input.UnbindKeyCallback ( VK_DELETE, eGXInputButtonState::Down );
	input.UnbindKeyCallback ( VK_LEFT, eGXInputButtonState::Down );
	input.UnbindKeyCallback ( VK_RIGHT, eGXInputButtonState::Down );
	input.UnbindKeyCallback ( VK_HOME, eGXInputButtonState::Down );
	input.UnbindKeyCallback ( VK_END, eGXInputButtonState::Down );
	input.UnbindTypeCallback ();
}

GXVoid GXUIEditBox::UpdateCursor ( GXInt newCursor )
{
	GXInt c = GXClampi ( newCursor, 0, static_cast<GXInt> ( textSymbols ) );

	if ( GetKeyState ( VK_SHIFT ) & GX_UI_KEYSTATE_MASK )
	{
		if ( c != cursor )
		{
			cursor = c;

			if ( renderer )
				renderer->OnUpdate ();
		}

		return;
	}

	if ( c != cursor || c != selection )
	{
		cursor = selection = c;

		if ( renderer )
			renderer->OnUpdate ();
	}
}

GXVoid GXUIEditBox::CopyText ()
{
	if ( cursor == selection ) return;

	GXInt begin;
	GXInt end;

	if ( cursor < selection )
	{
		begin = cursor;
		end = selection;
	}
	else
	{
		begin = selection;
		end = cursor;
	}

	GXUShort marked = static_cast<GXUShort> ( end - begin );
	GXUInt size = ( marked + 1 ) * sizeof ( GXWChar );
	HGLOBAL block = GlobalAlloc ( GMEM_MOVEABLE, size );
	GXWChar* clipData = static_cast<GXWChar*> ( GlobalLock ( block ) );
	memcpy ( clipData, text + begin, size );
	clipData[ marked ] = 0;
	GlobalUnlock ( block );

	OpenClipboard ( static_cast<HWND> ( 0 ) );
	EmptyClipboard ();
	SetClipboardData ( CF_UNICODETEXT, clipData );
	CloseClipboard ();
}

GXVoid GXUIEditBox::PasteText ( const GXWChar* textToPaste )
{
	GXUInt numSymbols = GXWcslen ( textToPaste );
	GXUInt totalSymbols = textSymbols + numSymbols;

	if ( totalSymbols > maxSymbols )
	{
		GXUInt size = ( totalSymbols + GX_TEXT_GROW_FACTOR + 1 ) * sizeof ( GXWChar );

		if ( maxSymbols == 0 )
		{
			text = static_cast<GXWChar*> ( malloc ( size ) );
			workingBuffer = static_cast<GXWChar*> ( malloc ( size ) );

			text[ 0 ] = 0;
		}
		else
		{
			free ( workingBuffer );
			workingBuffer = static_cast<GXWChar*> ( malloc ( size ) );

			GXWChar* newText = static_cast<GXWChar*> ( malloc ( size ) );
			memcpy ( newText, text, ( textSymbols + 1 ) * sizeof ( GXWChar ) );
			free ( text );
			text = newText;
		}

		maxSymbols = totalSymbols + GX_TEXT_GROW_FACTOR;
	}

	memmove ( text + cursor + numSymbols, text + cursor, ( textSymbols + 1 - cursor ) * sizeof ( GXWChar ) );
	memcpy ( text + cursor, textToPaste, numSymbols * sizeof ( GXWChar ) );

	cursor = selection = static_cast<GXInt> ( cursor + numSymbols );
	textSymbols += numSymbols;
}

GXBool GXUIEditBox::DeleteText ()
{
	if ( cursor == selection ) return GX_FALSE;

	GXInt start = cursor;
	GXInt end = selection;

	if ( start > end )
	{
		start = selection;
		end = cursor;
	}

	memmove ( text + start, text + end, ( textSymbols + 1 - end ) * sizeof ( GXWChar ) );
	cursor = selection = start;
	textSymbols -= end - start;

	return GX_TRUE;
}

GXVoid GXCALL GXUIEditBox::OnEnd ( GXVoid* handler )
{
	static const GXWChar symbol = GX_END;
	GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( handler ), eGXUIMessage::AddSumbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnHome ( GXVoid* handler )
{
	static const GXWChar symbol = GX_HOME;
	GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( handler ), eGXUIMessage::AddSumbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnDel ( GXVoid* handler )
{
	static const GXWChar symbol = GX_DEL;
	GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( handler ), eGXUIMessage::AddSumbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnBackspace ( GXVoid* handler )
{
	static const GXWChar symbol = GX_BACKSPACE;
	GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( handler ), eGXUIMessage::AddSumbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnLeftArrow ( GXVoid* handler )
{
	static const GXWChar symbol = GX_LEFT_ARROW;
	GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( handler ), eGXUIMessage::AddSumbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnRightArrow ( GXVoid* handler )
{
	static const GXWChar symbol = GX_RIGHT_ARROW;
	GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( handler ), eGXUIMessage::AddSumbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnType ( GXVoid* handler, GXWChar symbol )
{
	GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( handler ), eGXUIMessage::AddSumbol, &symbol, sizeof ( GXWChar ) );
}
