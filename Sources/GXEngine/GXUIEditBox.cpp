// version 1.7

#include <GXEngine/GXUIEditBox.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXTextValidator.h>
#include <GXEngine/GXInput.h>
#include <GXCommon/GXStrings.h>


#define GX_UI_DEFAULT_FONT                  L"Fonts/trebuc.ttf"
#define GX_UI_DEFAULT_FONT_SIZE             0.33f
#define GX_UI_DEFAULT_TEXT_ALIGNMENT        eGXUITextAlignment::Left
#define GX_UI_KEYSTATE_MASK                 0x8000
#define GX_UI_SEPARATOR_SYMBOL              L' '
#define GX_UI_EPSILON                       0.2f
#define GX_UI_DEFAULT_TEXT_LEFT_OFFSET      0.1f
#define GX_UI_DEFAULT_TEXT_RIGHT_OFFSET     0.1f

#define GX_CTRL_C                           0x0003
#define GX_CTRL_V                           0x0016
#define GX_CTRL_X                           0x0018
#define GX_CTRL_A                           0x0001
#define GX_HOME                             0x0017
#define GX_END                              0x0019
#define GX_DEL                              0x001A
#define GX_BACKSPACE                        0x001B
#define GX_LEFT_ARROW                       0x001C
#define GX_RIGHT_ARROW                      0x001D

#define GX_TEXT_GROW_FACTOR                 16u

//---------------------------------------------------------------------------------------------------------------------

struct GXUIEditBoxFontInfo final
{
    GXWChar*    _fontFile;
    GXUShort    _size;
};

//---------------------------------------------------------------------------------------------------------------------

GXUIEditBox::GXUIEditBox ( GXWidget* parent ):
    GXWidget ( parent ),
    _text ( nullptr ),
    _workingBuffer ( nullptr ),
    _textSymbols ( 0u ),
    _maxSymbols ( 0u ),
    _textLeftOffset ( GX_UI_DEFAULT_TEXT_LEFT_OFFSET * gx_ui_Scale ),
    _textRightOffset ( GX_UI_DEFAULT_TEXT_RIGHT_OFFSET * gx_ui_Scale ),
    _cursor ( 0 ),
    _selection ( 0 ),
    _alignment ( GX_UI_DEFAULT_TEXT_ALIGNMENT ),
    _validator ( nullptr ),
    _onFinishEditing ( nullptr ),
    _context ( nullptr ),
    _editCursor ( LoadCursorW ( nullptr, IDC_IBEAM ) ),
    _arrowCursor ( LoadCursorW ( nullptr, IDC_ARROW ) )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXFont" );
    _font = new GXFont ( GX_UI_DEFAULT_FONT, static_cast<GXUShort> ( GX_UI_DEFAULT_FONT_SIZE * gx_ui_Scale ) );
    _currentCursor = &_arrowCursor;
}

GXUIEditBox::~GXUIEditBox ()
{
    delete _font;
    GXSafeFree ( _text );
}

GXVoid GXUIEditBox::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    if ( message == eGXUIMessage::SetText )
    {
        _textSymbols = static_cast<GXUInt> ( GXWcslen ( static_cast<const GXWChar*> ( data ) ) );
        GXUInt size = sizeof ( GXWChar ) * ( _textSymbols + 1 );

        if ( _textSymbols > _maxSymbols )
        {
            GXSafeFree ( _text );
            GXSafeFree ( _workingBuffer );

            _text = static_cast<GXWChar*> ( malloc ( size ) );
            _workingBuffer = static_cast<GXWChar*> ( malloc ( size ) );

            _maxSymbols = _textSymbols;
        }

        memcpy ( _text, static_cast<const GXWChar*> ( data ), size );
        _cursor = _selection = 0;

        if ( _validator )
            _validator->Validate ( _text );

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::ClearText )
    {
        if ( _textSymbols == 0 ) return;

        _text[ 0 ] = 0;

        _textSymbols = 0;
        _cursor = _selection = 0;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::EditBoxSetFont )
    {
        const GXUIEditBoxFontInfo* fi = static_cast<const GXUIEditBoxFontInfo*> ( data );
        delete _font;

        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXFont" );
        _font = new GXFont ( fi->_fontFile, fi->_size );

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::SetTextAlignment )
    {
        const eGXUITextAlignment* newAlignment = static_cast<const eGXUITextAlignment*> ( data );
        this->_alignment = *newAlignment;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::LMBDown )
    {
        // ��������� ���� � ����������, ���� ������ � ������� �������.
        // ���������� ���� � ����������, ���� ������ �� � ������� �������,
        // � ��������� ��������� �� ������ ������.
        // ��������� ������ � ���������� �����, ���� ������ � ������� �������.
        // ���� ������ � ������� ������� � ����� Shift, �� �������� ������� ������.

        const GXVec2* pos = static_cast<const GXVec2*> ( data );

        if ( _boundsWorld.IsOverlaped ( pos->GetX (), pos->GetY (), 0.0f ) )
        {
            LockInput ();

            if ( GetKeyState ( VK_SHIFT ) & GX_UI_KEYSTATE_MASK )
                _cursor = GetSelectionPosition ( *pos );
            else
                _cursor = _selection = GetSelectionPosition ( *pos );
        }
        else
        {
            ReleaseInput ();

            if ( _validator && _validator->Validate ( _text ) && _onFinishEditing )
                _onFinishEditing ( _context, *this );
            else if ( _onFinishEditing )
                _onFinishEditing ( _context, *this );

            GXTouchSurface::GetInstance ().OnLeftMouseButtonDown ( *pos );
        }

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::DoubleClick )
    {
        if ( _textSymbols == 0u ) return;

        const GXVec2* pos = static_cast<const GXVec2*> ( data );
        GXInt current = GetSelectionPosition ( *pos );

        if ( _text[ current ] == GX_UI_SEPARATOR_SYMBOL ) return;

        GXInt end = current + 1;

        for ( ; end < static_cast<GXInt> ( _textSymbols ); ++end )
        {
            if ( _text[ end ] == GX_UI_SEPARATOR_SYMBOL )
                break;
        }

        GXInt begin = current;

        for ( ; begin > 0; --begin )
        {
            if ( _text[ begin ] == GX_UI_SEPARATOR_SYMBOL )
            {
                ++begin;
                break;
            }
        }

        if ( _selection != begin || _cursor != end )
        {
            _selection = begin;
            _cursor = end;

            if ( _renderer )
                _renderer->OnUpdate ();
        }

        return;
    }

    if ( message == eGXUIMessage::MouseOver )
    {
        SetCursor ( _editCursor );
        return;
    }

    if ( message == eGXUIMessage::MouseLeave )
    {
        SetCursor ( _arrowCursor );
        return;
    }

    if ( message == eGXUIMessage::MouseMove )
    {
        const GXVec2* pos = static_cast<const GXVec2*> ( data );

        if ( _boundsWorld.IsOverlaped ( pos->GetX (), pos->GetY (), 0.0f ) )
        {
            if ( _currentCursor != &_editCursor )
            {
                _currentCursor = &_editCursor;
                SetCursor ( _editCursor );
            }
        }
        else
        {
            if ( _currentCursor != &_arrowCursor)
            {
                _currentCursor = &_arrowCursor;
                SetCursor ( _arrowCursor );
            }
        }

        if ( _textSymbols == 0 ) return;

        if ( GetKeyState ( VK_LBUTTON ) & GX_UI_KEYSTATE_MASK )
        {
            GXInt current = GetSelectionPosition ( *pos );

            if ( current != _cursor )
            {
                _cursor = current;

                if ( _renderer )
                    _renderer->OnUpdate ();
            }
        }

        return;
    }

    if ( message == eGXUIMessage::AddSumbol )
    {
        // ������� �������� �������, END, HOME, ���������
        // Ctrl + V, Ctrl + C, Ctrl + X
        // Shift + HOME, Shift + END.
        // Del, Backspace
        // ���� ����������� ������.

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

                            if ( _renderer )
                                _renderer->OnUpdate ();
                        }
                    }

                    CloseClipboard ();
                }
            }
            break;

            case GX_CTRL_X:
            {
                CopyText ();

                if ( DeleteText () && _renderer )
                    _renderer->OnUpdate ();
            }
            break;

            case GX_CTRL_A:
            {
                if ( _cursor != (GXInt)_textSymbols || _selection != 0 )
                {
                    _cursor = (GXInt)_textSymbols;
                    _selection = 0;

                    if ( _renderer )
                        _renderer->OnUpdate ();
                }
            }
            break;

            case GX_HOME:
                UpdateCursor ( 0 );
            break;

            case GX_END:
                UpdateCursor ( static_cast<GXInt> ( _textSymbols ) );
            break;

            case GX_DEL:
            {
                if ( _cursor == _selection && _cursor == static_cast<GXInt> ( _textSymbols ) )
                    break;
                else if ( _cursor == _selection )
                    ++_selection;

                if ( DeleteText () && _renderer )
                    _renderer->OnUpdate ();
            }
            break;

            case GX_BACKSPACE:
            {
                if ( _cursor == _selection && _cursor == 0 )
                    break;
                else if ( _cursor == _selection )
                    --_cursor;

                if ( DeleteText () && _renderer )
                    _renderer->OnUpdate ();
            }
            break;

            case GX_LEFT_ARROW:
                UpdateCursor ( _cursor - 1 );
            break;

            case GX_RIGHT_ARROW:
                UpdateCursor ( _cursor + 1 );
            break;

            default:
            {
                // �������� ������
                static GXWChar buffer[ 2 ] = { 0 };
                buffer[ 0 ] = *symbol;

                DeleteText ();
                PasteText ( buffer );

                if ( _renderer )
                    _renderer->OnUpdate ();
            }
            break;
        }

        return;
    }

    if ( message == eGXUIMessage::EditBoxSetTextLeftOffset )
    {
        const GXFloat* offset = static_cast<const GXFloat*> ( data );

        if ( _textLeftOffset == *offset ) return;

        _textLeftOffset = *offset;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    if ( message == eGXUIMessage::EditBoxSetTextRightOffset )
    {
        const GXFloat* offset = static_cast<const GXFloat*> ( data );

        if ( _textRightOffset == *offset ) return;

        _textRightOffset = *offset;

        if ( _renderer )
            _renderer->OnUpdate ();

        return;
    }

    GXWidget::OnMessage ( message, data );
}

GXFloat GXUIEditBox::GetCursorOffset () const
{
    return GetSelectionOffset ( static_cast<GXUInt> ( _cursor ) );
}

GXFloat GXUIEditBox::GetSelectionBeginOffset () const
{
    return GetSelectionOffset ( _cursor > _selection ? static_cast<GXUInt> ( _cursor ) : static_cast<GXUInt> ( _selection ) );
}

GXFloat GXUIEditBox::GetSelectionEndOffset () const
{
    return GetSelectionOffset ( _cursor < _selection ? static_cast<GXUInt> ( _cursor ) : static_cast<GXUInt> ( _selection ) );
}

GXVoid GXUIEditBox::SetTextLeftOffset ( GXFloat offset )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::EditBoxSetTextLeftOffset, &offset, sizeof ( GXFloat ) );
}

GXFloat GXUIEditBox::GetTextLeftOffset () const
{
    return _textLeftOffset;
}

GXVoid GXUIEditBox::SetTextRightOffset ( GXFloat offset )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::EditBoxSetTextRightOffset, &offset, sizeof ( GXFloat ) );
}

GXFloat GXUIEditBox::GetTextRightOffset () const
{
    return _textRightOffset;
}

GXVoid GXUIEditBox::SetText ( const GXWChar* newText )
{
    if ( newText )
    {
        GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::SetText, newText, static_cast<GXUInt> ( ( GXWcslen ( newText ) + 1u ) * sizeof ( GXWChar ) ) );
        return;
    }

    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ClearText, nullptr, 0u );
}

const GXWChar* GXUIEditBox::GetText () const
{
    return _text;
}

GXVoid GXUIEditBox::SetAlignment ( eGXUITextAlignment newAlignment )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::SetTextAlignment, &newAlignment, sizeof ( eGXUITextAlignment ) );
}

eGXUITextAlignment GXUIEditBox::GetAlignment () const
{
    return _alignment;
}

GXVoid GXUIEditBox::SetFont ( const GXWChar* fontFile, GXUShort fontSize )
{
    if ( !fontFile ) return;

    GXUPointer size = ( GXWcslen ( fontFile ) + 1u ) * sizeof ( GXWChar );

    GXUIEditBoxFontInfo fi;
    fi._fontFile = static_cast<GXWChar*> ( gx_ui_MessageBuffer->Allocate ( size ) );
    memcpy ( fi._fontFile, fontFile, size );
    fi._size = fontSize;

    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::EditBoxSetFont, &fi, sizeof ( GXUIEditBoxFontInfo ) );
}

GXFont* GXUIEditBox::GetFont ()
{
    return _font;
}

GXBool GXUIEditBox::IsActive ()
{
    return GXTouchSurface::GetInstance ().GetLockedCursorWidget () == this;
}

GXVoid GXUIEditBox::SetValidator ( GXTextValidator &validatorObject )
{
    _validator = &validatorObject;
}

GXTextValidator* GXUIEditBox::GetValidator () const
{
    return _validator;
}

GXVoid GXUIEditBox::SetOnFinishEditingCallback ( GXVoid* handlerObject, PFNGXUIEDITBOXONFINISHEDITINGPROC callback )
{
    _context = handlerObject;
    _onFinishEditing = callback;
}

GXInt GXUIEditBox::GetSelectionPosition ( const GXVec2 &mousePosition ) const
{
    GXVec3 center;
    _boundsWorld.GetCenter ( center );
    GXFloat width = _boundsWorld.GetWidth ();
    GXFloat offset = 0.0f;
    GXFloat textLength = static_cast<GXFloat> ( _font->GetTextLength ( 0u, _text ) );

    switch ( _alignment )
    {
        case  eGXUITextAlignment::Center:
        {
            GXFloat textOffset = ( width - textLength ) * 0.5f;
            offset = GXClampf ( mousePosition.GetX () - _boundsWorld._min.GetX (), textOffset, width - textOffset ) - textOffset;
        }
        break;

        case  eGXUITextAlignment::Right:
        {
            GXFloat textOffset = width - textLength;
            offset = GXClampf ( mousePosition.GetX () - _boundsWorld._min.GetX () - _textRightOffset, textOffset, width ) - textOffset;
        }
        break;

        case eGXUITextAlignment::Left:
            textLength = static_cast<GXFloat> ( _font->GetTextLength ( 0u, _text ) );
            offset = GXClampf ( mousePosition.GetX () - _boundsWorld._min.GetX () + _textLeftOffset, _textLeftOffset, textLength );
        break;
    }

    if ( offset == 0.0f ) return 0;

    if ( abs ( offset - textLength ) < GX_UI_EPSILON ) 
        return static_cast<GXInt> ( GXWcslen ( _text ) );

    for ( GXUInt i = 0u; i < _textSymbols; ++i )
    {
        _workingBuffer[ i ] = _text[ i ];
        _workingBuffer[ i + 1 ] = 0;
        GXFloat currentOffset = static_cast<GXFloat> ( _font->GetTextLength ( 0u, _workingBuffer ) );

        if ( currentOffset > offset )
            return static_cast<GXInt> ( i );
    }

    return 0;
}

GXFloat GXUIEditBox::GetSelectionOffset ( GXUInt symbolIndex ) const
{
    if ( _textSymbols == 0 ) return 0.0f;

    memcpy ( _workingBuffer, _text, sizeof ( GXWChar ) * symbolIndex );
    _workingBuffer[ symbolIndex ] = 0;

    GXFloat textLength = static_cast<GXFloat> ( _font->GetTextLength ( 0u, _text ) );
    GXFloat textOffset = 0.0f;

    switch ( _alignment )
    {
        case eGXUITextAlignment::Center:
            textOffset = ( _boundsWorld.GetWidth () - textLength ) * 0.5f;
        break;

        case eGXUITextAlignment::Right:
            textOffset = _boundsWorld.GetWidth () - textLength - _textRightOffset;
        break;

        case eGXUITextAlignment::Left:
            textOffset = _textLeftOffset;
        break;
    }

    return textOffset + static_cast<GXFloat> ( _font->GetTextLength ( 0, _workingBuffer ) );
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
    GXInt c = GXClampi ( newCursor, 0, static_cast<GXInt> ( _textSymbols ) );

    if ( GetKeyState ( VK_SHIFT ) & GX_UI_KEYSTATE_MASK )
    {
        if ( c != _cursor )
        {
            _cursor = c;

            if ( _renderer )
                _renderer->OnUpdate ();
        }

        return;
    }

    if ( c != _cursor || c != _selection )
    {
        _cursor = _selection = c;

        if ( _renderer )
            _renderer->OnUpdate ();
    }
}

GXVoid GXUIEditBox::CopyText ()
{
    if ( _cursor == _selection ) return;

    GXInt begin;
    GXInt end;

    if ( _cursor < _selection )
    {
        begin = _cursor;
        end = _selection;
    }
    else
    {
        begin = _selection;
        end = _cursor;
    }

    GXUShort marked = static_cast<GXUShort> ( end - begin );
    GXUInt size = ( marked + 1 ) * sizeof ( GXWChar );
    HGLOBAL block = GlobalAlloc ( GMEM_MOVEABLE, size );
    GXWChar* clipData = static_cast<GXWChar*> ( GlobalLock ( block ) );
    memcpy ( clipData, _text + begin, size );
    clipData[ marked ] = 0;
    GlobalUnlock ( block );

    OpenClipboard ( static_cast<HWND> ( 0 ) );
    EmptyClipboard ();
    SetClipboardData ( CF_UNICODETEXT, clipData );
    CloseClipboard ();
}

GXVoid GXUIEditBox::PasteText ( const GXWChar* textToPaste )
{
    GXUInt numSymbols = static_cast<GXUInt> ( GXWcslen ( textToPaste ) );
    GXUInt totalSymbols = _textSymbols + numSymbols;

    if ( totalSymbols > _maxSymbols )
    {
        GXUInt size = ( totalSymbols + GX_TEXT_GROW_FACTOR + 1 ) * sizeof ( GXWChar );

        if ( _maxSymbols == 0 )
        {
            _text = static_cast<GXWChar*> ( malloc ( size ) );
            _workingBuffer = static_cast<GXWChar*> ( malloc ( size ) );

            _text[ 0 ] = 0;
        }
        else
        {
            free ( _workingBuffer );
            _workingBuffer = static_cast<GXWChar*> ( malloc ( size ) );

            GXWChar* newText = static_cast<GXWChar*> ( malloc ( size ) );
            memcpy ( newText, _text, ( _textSymbols + 1 ) * sizeof ( GXWChar ) );
            free ( _text );
            _text = newText;
        }

        _maxSymbols = totalSymbols + GX_TEXT_GROW_FACTOR;
    }

    memmove ( _text + _cursor + numSymbols, _text + _cursor, ( _textSymbols + 1 - _cursor ) * sizeof ( GXWChar ) );
    memcpy ( _text + _cursor, textToPaste, numSymbols * sizeof ( GXWChar ) );

    _cursor = _selection = static_cast<GXInt> ( _cursor + numSymbols );
    _textSymbols += numSymbols;
}

GXBool GXUIEditBox::DeleteText ()
{
    if ( _cursor == _selection ) return GX_FALSE;

    GXInt start = _cursor;
    GXInt end = _selection;

    if ( start > end )
    {
        start = _selection;
        end = _cursor;
    }

    memmove ( _text + start, _text + end, ( _textSymbols + 1 - end ) * sizeof ( GXWChar ) );
    _cursor = _selection = start;
    _textSymbols -= end - start;

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
