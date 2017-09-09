//version 1.0

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

#define GX_TEXT_GROW_FACTOR					16


struct GXUIEditBoxFontInfo
{
	GXWChar*		fontFile;
	GXUShort		size;
};

//------------------------------------------------------------------------

GXUIEditBox::GXUIEditBox ( GXWidget* parent ) :
GXWidget ( parent )
{
	text = workingBuffer = nullptr;
	textSymbols = maxSymbols = 0;
	textLeftOffset = GX_UI_DEFAULT_TEXT_LEFT_OFFSET * gx_ui_Scale;
	textRightOffset = GX_UI_DEFAULT_TEXT_RIGHT_OFFSET * gx_ui_Scale;
	font = GXFont::GetFont ( GX_UI_DEFAULT_FONT, (GXUShort)( GX_UI_DEFAULT_FONT_SIZE * gx_ui_Scale ) );
	cursor = selection = 0;
	alignment = GX_UI_DEFAULT_TEXT_ALIGNMENT;
	editCursor = LoadCursorW ( 0, IDC_IBEAM );
	arrowCursor = LoadCursorW ( 0, IDC_ARROW );
	currentCursor = &arrowCursor;
	validator = nullptr;
	OnFinishEditing = nullptr;
	handler = nullptr;
}

GXUIEditBox::~GXUIEditBox ()
{
	GXFont::RemoveFont ( font );
	GXSafeFree ( text );
}

GXVoid GXUIEditBox::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_SET_TEXT:
		{
			textSymbols = GXWcslen ( (const GXWChar*)data );
			GXUInt size = sizeof ( GXWChar ) * ( textSymbols + 1 );
			if ( textSymbols > maxSymbols )
			{
				GXSafeFree ( text );
				GXSafeFree ( workingBuffer );

				text = (GXWChar*)malloc ( size );
				workingBuffer = (GXWChar*)malloc ( size );

				maxSymbols = textSymbols;
			}

			memcpy ( text, (const GXWChar*)data, size );
			cursor = selection = 0;

			if ( validator )
				validator->Validate ( text );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_CLEAR_TEXT:
		{
			if ( textSymbols == 0 )
				break;

			text[ 0 ] = 0;

			textSymbols = 0;
			cursor = selection = 0;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_EDIT_BOX_SET_FONT:
		{
			const GXUIEditBoxFontInfo* fi = (const GXUIEditBoxFontInfo*)data;
			GXFont::RemoveFont ( font );
			font = GXFont::GetFont ( fi->fontFile, fi->size );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_SET_TEXT_ALIGNMENT:
		{
			const eGXUITextAlignment* newAlignment = (const eGXUITextAlignment*)data;
			this->alignment = *newAlignment;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LMBDOWN:
		{
			//Захватить ввод с клавиатуры, если курсор в области виджета.
			//Освободить ввод с клавиатуры, если курсор не в области виджета,
			//и отправить сообщение на нужный виджет.
			//Поставить курсор в правильное место, если щелчок в области виджета.
			//Если щелчок в области виджета и зажат Shift, то выделить область текста.

			const GXVec2* pos = (const GXVec2*)data;

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
		}
		break;

		case GX_MSG_DOUBLE_CLICK:
		{
			if ( textSymbols > 0 )
			{
				const GXVec2* pos = (const GXVec2*)data;
				GXInt current = GetSelectionPosition ( *pos );

				if ( text[ current ] != GX_UI_SEPARATOR_SYMBOL )
				{
					GXInt end = current + 1;
					for ( ; end < (GXInt)textSymbols; end++ )
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
				}
			}
		}
		break;

		case GX_MSG_MOUSE_OVER:
			SetCursor ( editCursor );
		break;

		case GX_MSG_MOUSE_LEAVE:
			SetCursor ( arrowCursor );
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = (const GXVec2*)data;
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

			if ( textSymbols )
			{
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
			}
		}
		break;

		case GX_MSG_ADD_SYMBOL:
		{
			//Вставка обычного символа, END, HOME, стрелочки
			//Ctrl + V, Ctrl + C, Ctrl + X
			//Shift + HOME, Shift + END.
			//Del, Backspace
			//Учёт выделенного текста.

			const GXWChar* symbol = (const GXWChar*)data;

			switch ( *symbol )
			{
				case GX_CTRL_C:
					CopyText ();
				break;

				case GX_CTRL_V:
				{
					if ( IsClipboardFormatAvailable ( CF_UNICODETEXT ) && OpenClipboard ( 0 ) )
					{
						HGLOBAL block = GetClipboardData ( CF_UNICODETEXT );
						if ( block )
						{
							GXWChar* clipBoardData = (GXWChar*)GlobalLock ( block );

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
					UpdateCursor ( (GXInt)textSymbols );
				break;

				case GX_DEL:
				{
					if ( cursor == selection && cursor == (GXInt)textSymbols )
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
					//Добавить символ
					static GXWChar buffer[ 2 ] = { 0 };
					buffer[ 0 ] = *symbol;

					DeleteText ();
					PasteText ( buffer );

					if ( renderer )
						renderer->OnUpdate ();
				}
				break;
			}
		}
		break;

		case GX_MSG_EDIT_BOX_SET_TEXT_LEFT_OFFSET:
		{
			const GXFloat* offset = (const GXFloat*)data;
			if ( textLeftOffset != *offset )
			{
				textLeftOffset = *offset;
				if ( renderer )
					renderer->OnUpdate ();
			}
		}
		break;

		case GX_MSG_EDIT_BOX_SET_TEXT_RIGHT_OFFSET:
		{
			const GXFloat* offset = (const GXFloat*)data;
			if ( textRightOffset != *offset )
			{
				textRightOffset = *offset;
				if ( renderer )
					renderer->OnUpdate ();
			}
		}
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXFloat GXUIEditBox::GetCursorOffset () const
{
	return GetSelectionOffset ( (GXUInt)cursor );
}

GXFloat GXUIEditBox::GetSelectionBeginOffset () const
{
	return GetSelectionOffset ( cursor > selection ? (GXUInt)cursor : (GXUInt)selection );
}

GXFloat GXUIEditBox::GetSelectionEndOffset () const
{
	return GetSelectionOffset ( cursor < selection ? (GXUInt)cursor : (GXUInt)selection );
}

GXVoid GXUIEditBox::SetTextLeftOffset ( GXFloat offset )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_EDIT_BOX_SET_TEXT_LEFT_OFFSET, &offset, sizeof ( GXFloat ) );
}

GXFloat GXUIEditBox::GetTextLeftOffset () const
{
	return textLeftOffset;
}

GXVoid GXUIEditBox::SetTextRightOffset ( GXFloat offset )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_EDIT_BOX_SET_TEXT_RIGHT_OFFSET, &offset, sizeof ( GXFloat ) );
}

GXFloat GXUIEditBox::GetTextRightOffset () const
{
	return textRightOffset;
}

GXVoid GXUIEditBox::SetText ( const GXWChar* newText )
{
	if ( newText )
		GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SET_TEXT, newText, ( GXWcslen ( newText ) + 1 ) * sizeof ( GXWChar ) );
	else
		GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_CLEAR_TEXT, 0, 0 );
}

const GXWChar* GXUIEditBox::GetText () const
{
	return text;
}

GXVoid GXUIEditBox::SetAlignment ( eGXUITextAlignment newAlignment )
{
	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_SET_TEXT_ALIGNMENT, &newAlignment, sizeof ( eGXUITextAlignment ) );
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
	fi.fontFile = (GXWChar*)gx_ui_MessageBuffer->Allocate ( size );
	memcpy ( fi.fontFile, fontFile, size );
	fi.size = fontSize;

	GXTouchSurface::GetInstance ().SendMessage ( this, GX_MSG_EDIT_BOX_SET_FONT, &fi, sizeof ( GXUIEditBoxFontInfo ) );
}

GXFont* GXUIEditBox::GetFont ()
{
	return &font;
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
	GXFloat textLength = (GXFloat)font.GetTextLength ( 0, text );

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
			textLength = (GXFloat)font.GetTextLength ( 0, text );
			offset = GXClampf ( mousePosition.GetX () - boundsWorld.min.GetX () + textLeftOffset, textLeftOffset, textLength );
		break;
	}

	if ( offset == 0.0f ) return 0;
	if ( abs ( offset - textLength ) < GX_UI_EPSILON ) return (GXInt)GXWcslen ( text );

	for ( GXUInt i = 0; i < textSymbols; i++ )
	{
		workingBuffer[ i ] = text[ i ];
		workingBuffer[ i + 1 ] = 0;
		GXFloat currentOffset = (GXFloat)font.GetTextLength ( 0, workingBuffer );

		if ( currentOffset > offset )
			return (GXInt)i;
	}

	return 0;
}

GXFloat GXUIEditBox::GetSelectionOffset ( GXUInt symbolIndex ) const
{
	if ( textSymbols == 0 ) return 0.0f;

	memcpy ( workingBuffer, text, sizeof ( GXWChar ) * symbolIndex );
	workingBuffer[ symbolIndex ] = 0;

	GXFloat textLength = (GXFloat)font.GetTextLength ( 0, text );
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

	return textOffset + (GXFloat)font.GetTextLength ( 0, workingBuffer );
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
	GXInt c = GXClampi ( newCursor, 0, (GXInt)textSymbols );
	if ( GetKeyState ( VK_SHIFT ) & GX_UI_KEYSTATE_MASK )
	{
		if ( c != cursor )
		{
			cursor = c;
			if ( renderer )
				renderer->OnUpdate ();
		}
	}
	else
	{
		if ( c != cursor || c != selection )
		{
			cursor = selection = c;
			if ( renderer )
				renderer->OnUpdate ();
		}
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

	GXUShort marked = (GXUShort)( end - begin );
	GXUInt size = ( marked + 1 ) * sizeof ( GXWChar );
	HGLOBAL block = GlobalAlloc ( GMEM_MOVEABLE, size );
	GXWChar* clipData = (GXWChar*)GlobalLock ( block );
	memcpy ( clipData, text + begin, size );
	clipData[ marked ] = 0;
	GlobalUnlock ( block );

	OpenClipboard ( 0 );
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
			text = (GXWChar*)malloc ( size );
			workingBuffer = (GXWChar*)malloc ( size );

			text[ 0 ] = 0;
		}
		else
		{
			free ( workingBuffer );
			workingBuffer = (GXWChar*)malloc ( size );

			GXWChar* newText = (GXWChar*)malloc ( size );
			memcpy ( newText, text, ( textSymbols + 1 ) * sizeof ( GXWChar ) );
			free ( text );
			text = newText;
		}

		maxSymbols = totalSymbols + GX_TEXT_GROW_FACTOR;
	}

	memmove ( text + cursor + numSymbols, text + cursor, ( textSymbols + 1 - cursor ) * sizeof ( GXWChar ) );
	memcpy ( text + cursor, textToPaste, numSymbols * sizeof ( GXWChar ) );

	cursor = selection = (GXInt)( cursor + numSymbols );
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
	GXTouchSurface::GetInstance ().SendMessage ( (GXWidget*)handler, GX_MSG_ADD_SYMBOL, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnHome ( GXVoid* handler )
{
	static const GXWChar symbol = GX_HOME;
	GXTouchSurface::GetInstance ().SendMessage ( (GXWidget*)handler, GX_MSG_ADD_SYMBOL, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnDel ( GXVoid* handler )
{
	static const GXWChar symbol = GX_DEL;
	GXTouchSurface::GetInstance ().SendMessage ( (GXWidget*)handler, GX_MSG_ADD_SYMBOL, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnBackspace ( GXVoid* handler )
{
	static const GXWChar symbol = GX_BACKSPACE;
	GXTouchSurface::GetInstance ().SendMessage ( (GXWidget*)handler, GX_MSG_ADD_SYMBOL, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnLeftArrow ( GXVoid* handler )
{
	static const GXWChar symbol = GX_LEFT_ARROW;
	GXTouchSurface::GetInstance ().SendMessage ( (GXWidget*)handler, GX_MSG_ADD_SYMBOL, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnRightArrow ( GXVoid* handler )
{
	static const GXWChar symbol = GX_RIGHT_ARROW;
	GXTouchSurface::GetInstance ().SendMessage ( (GXWidget*)handler, GX_MSG_ADD_SYMBOL, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnType ( GXVoid* handler, GXWChar symbol )
{
	GXTouchSurface::GetInstance ().SendMessage ( (GXWidget*)handler, GX_MSG_ADD_SYMBOL, &symbol, sizeof ( GXWChar ) );
}
