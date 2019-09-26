// version 1.8

#include <GXEngine/GXUIEditBox.h>
#include <GXEngine/GXUIMessage.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXTextValidator.h>
#include <GXEngine/GXInput.h>
#include <GXCommon/GXStrings.h>


#define GX_UI_DEFAULT_FONT                  "Fonts/trebuc.ttf"
#define GX_UI_DEFAULT_FONT_SIZE             0.33f
#define GX_UI_DEFAULT_TEXT_ALIGNMENT        eGXUITextAlignment::Left
#define GX_UI_KEYSTATE_MASK                 0x8000
#define GX_UI_SEPARATOR_SYMBOL              ' '
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

GXUIEditBoxMessageHandlerNode::GXUIEditBoxMessageHandlerNode ()
{
    // NOTHING
}

GXUIEditBoxMessageHandlerNode::GXUIEditBoxMessageHandlerNode ( eGXUIMessage message ):
    GXUIWidgetMessageHandlerNode ( message )
{
    // NOTHING
}

GXUIEditBoxMessageHandlerNode::~GXUIEditBoxMessageHandlerNode ()
{
    // NOTHING
}

GXVoid GXUIEditBoxMessageHandlerNode::Init ( GXUIEditBox &editBox, eGXUIMessage message, GXUIEditBoxOnMessageHandler handler )
{
    _message = message;
    _handler = handler;
    _widget = &editBox;
}

GXVoid GXUIEditBoxMessageHandlerNode::HandleMassage ( const GXVoid* data )
{
    GXUIEditBox* editBox = static_cast<GXUIEditBox*> ( _widget );

    // Note this is C++ syntax for invoke class method.
    ( editBox->*_handler ) ( data );
}

//---------------------------------------------------------------------------------------------------------------------

struct GXUIEditBoxFontInfo final
{
    GXUTF16*    _fontFile;
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
    _arrowCursor ( LoadCursorW ( nullptr, IDC_ARROW ) ),
    _isCacheValid ( GX_TRUE )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXFont" )
    _font = new GXFont ( GX_UI_DEFAULT_FONT, static_cast<GXUShort> ( GX_UI_DEFAULT_FONT_SIZE * gx_ui_Scale ) );
    _currentCursor = &_arrowCursor;

    InitMessageHandlers ();
}

GXUIEditBox::~GXUIEditBox ()
{
    delete _font;
    SafeFree ( reinterpret_cast<GXVoid**> ( &_text ) );
    SafeFree ( reinterpret_cast<GXVoid**> ( &_workingBuffer ) );
}

GXVoid GXUIEditBox::OnMessage ( eGXUIMessage message, const GXVoid* data )
{
    GXUIEditBoxMessageHandlerNode probe ( message );
    GXAVLTreeNode* findResult = _messageHandlerTree.Find ( probe );

    if ( !findResult )
    {
        GXWidget::OnMessage ( message, data );
        return;
    }

    GXUIEditBoxMessageHandlerNode* targetHandler = static_cast<GXUIEditBoxMessageHandlerNode*> ( findResult );
    targetHandler->HandleMassage ( data );
}

GXFloat GXUIEditBox::GetCursorOffset ()
{
    return GetSelectionOffset ( static_cast<GXUInt> ( _cursor ) );
}

GXFloat GXUIEditBox::GetSelectionBeginOffset ()
{
    return GetSelectionOffset ( _cursor > _selection ? static_cast<GXUInt> ( _cursor ) : static_cast<GXUInt> ( _selection ) );
}

GXFloat GXUIEditBox::GetSelectionEndOffset ()
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

GXVoid GXUIEditBox::SetText ( const GXString &text )
{
    if ( text.IsNull () )
    {
        GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::ClearText, nullptr, 0u );
        return;
    }

    // Kung-Fu: Task is to create pesistent GXString in raw memory.
    // So will be used placement new. This prevents calling destructor for GXString object.
    // The ownership will be transmitted manualy in OnSetText. The temp object will be destructed in OnSetText.

    GXUByte memory[ sizeof ( GXString ) ];
    const GXString* string = ::new ( memory ) GXString ( text );
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::SetText, string, static_cast<GXUInt> ( sizeof ( GXString ) ) );
}

const GXString& GXUIEditBox::GetText ()
{
    UpdateCache ();
    return _cache;
}

GXVoid GXUIEditBox::SetAlignment ( eGXUITextAlignment alignment )
{
    GXTouchSurface::GetInstance ().SendMessage ( this, eGXUIMessage::SetTextAlignment, &alignment, sizeof ( eGXUITextAlignment ) );
}

eGXUITextAlignment GXUIEditBox::GetAlignment () const
{
    return _alignment;
}

GXVoid GXUIEditBox::SetFont ( const GXString &fontFile, GXUShort fontSize )
{
    if ( fontFile.IsNull () || fontFile.IsEmpty () ) return;

    GXUPointer size;
    const GXUTF16* data = fontFile.ToUTF16 ( size );

    GXUIEditBoxFontInfo fi;
    fi._fontFile = static_cast<GXUTF16*> ( gx_ui_MessageBuffer->Allocate ( size ) );
    memcpy ( fi._fontFile, data, size );
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

GXVoid GXUIEditBox::SetValidator ( GXTextValidator &validator )
{
    _validator = &validator;
}

GXTextValidator* GXUIEditBox::GetValidator () const
{
    return _validator;
}

GXVoid GXUIEditBox::SetOnFinishEditingCallback ( GXVoid* context, GXUIEditBoxOnCommitHandler callback )
{
    _context = context;
    _onFinishEditing = callback;
}

GXInt GXUIEditBox::GetSelectionPosition ( const GXVec2 &mousePosition )
{
    GXVec3 center;
    _boundsWorld.GetCenter ( center );
    GXFloat width = _boundsWorld.GetWidth ();
    GXFloat offset = 0.0f;

    UpdateCache ();

    GXFloat textLength = static_cast<GXFloat> ( _font->GetTextLength ( 0u, _cache ) );

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
            offset = GXClampf ( mousePosition.GetX () - _boundsWorld._min.GetX () + _textLeftOffset, _textLeftOffset, textLength );
        break;
    }

    if ( offset == 0.0f )
        return 0;

    if ( abs ( offset - textLength ) < GX_UI_EPSILON ) 
        return static_cast<GXInt> ( _textSymbols );

    for ( GXUInt i = 0u; i < _textSymbols; ++i )
    {
        _workingBuffer[ i ] = _text[ i ];
        _workingBuffer[ i + 1u ] = '\0';

        _tmp.FromSymbols ( _workingBuffer );
        GXFloat currentOffset = static_cast<GXFloat> ( _font->GetTextLength ( 0u, _tmp ) );

        if ( currentOffset <= offset ) continue;

        return static_cast<GXInt> ( i );
    }

    return 0;
}

GXFloat GXUIEditBox::GetSelectionOffset ( GXUInt symbolIndex )
{
    if ( _textSymbols == 0 )
        return 0.0f;

    UpdateCache ();

    GXFloat textLength = static_cast<GXFloat> ( _font->GetTextLength ( 0u, _cache ) );
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

    memcpy ( _workingBuffer, _text, sizeof ( GXStringSymbol ) * symbolIndex );
    _workingBuffer[ symbolIndex ] = '\0';
    _tmp.FromSymbols ( _workingBuffer );

    return textOffset + static_cast<GXFloat> ( _font->GetTextLength ( 0, _tmp ) );
}

GXVoid GXUIEditBox::InitMessageHandlers ()
{
    _messageHandlers[ 0u ].Init ( *this, eGXUIMessage::AddSymbol, &GXUIEditBox::OnAddSymbol );
    _messageHandlerTree.Add ( _messageHandlers[ 0u ] );

    _messageHandlers[ 1u ].Init ( *this, eGXUIMessage::ClearText, &GXUIEditBox::OnClearText );
    _messageHandlerTree.Add ( _messageHandlers[ 1u ] );

    _messageHandlers[ 2u ].Init ( *this, eGXUIMessage::DoubleClick, &GXUIEditBox::OnDoubleClick );
    _messageHandlerTree.Add ( _messageHandlers[ 2u ] );

    _messageHandlers[ 3u ].Init ( *this, eGXUIMessage::EditBoxSetFont, &GXUIEditBox::OnEditBoxSetFont );
    _messageHandlerTree.Add ( _messageHandlers[ 3u ] );

    _messageHandlers[ 4u ].Init ( *this, eGXUIMessage::EditBoxSetTextLeftOffset, &GXUIEditBox::OnEditBoxSetTextLeftOffset );
    _messageHandlerTree.Add ( _messageHandlers[ 4u ] );

    _messageHandlers[ 5u ].Init ( *this, eGXUIMessage::EditBoxSetTextRightOffset, &GXUIEditBox::OnEditBoxSetTextRightOffset );
    _messageHandlerTree.Add ( _messageHandlers[ 5u ] );

    _messageHandlers[ 6u ].Init ( *this, eGXUIMessage::LMBDown, &GXUIEditBox::OnLMBDown );
    _messageHandlerTree.Add ( _messageHandlers[ 6u ] );

    _messageHandlers[ 7u ].Init ( *this, eGXUIMessage::MouseLeave, &GXUIEditBox::OnMouseLeave );
    _messageHandlerTree.Add ( _messageHandlers[ 7u ] );

    _messageHandlers[ 8u ].Init ( *this, eGXUIMessage::MouseMove, &GXUIEditBox::OnMouseMove );
    _messageHandlerTree.Add ( _messageHandlers[ 8u ] );

    _messageHandlers[ 9u ].Init ( *this, eGXUIMessage::MouseOver, &GXUIEditBox::OnMouseOver );
    _messageHandlerTree.Add ( _messageHandlers[ 10u ] );

    _messageHandlers[ 10u ].Init ( *this, eGXUIMessage::SetText, &GXUIEditBox::OnSetText );
    _messageHandlerTree.Add ( _messageHandlers[ 10u ] );

    _messageHandlers[ 11u ].Init ( *this, eGXUIMessage::SetTextAlignment, &GXUIEditBox::OnSetTextAlighment );
    _messageHandlerTree.Add ( _messageHandlers[ 11u ] );
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

GXVoid GXUIEditBox::UpdateCursor ( GXInt cursor )
{
    const GXInt c = GXClampi ( cursor, 0, static_cast<GXInt> ( _textSymbols ) );

    if ( GetKeyState ( VK_SHIFT ) & GX_UI_KEYSTATE_MASK )
    {
        if ( c == _cursor ) return;

        _cursor = c;

        if ( !_renderer ) return;

        _renderer->OnUpdate ();
        return;
    }

    if ( c == _cursor && c == _selection ) return;

    _cursor = _selection = c;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIEditBox::UpdateCache ()
{
    if ( _isCacheValid ) return;

    _cache.FromSymbols ( _text );
    _isCacheValid = GX_TRUE;
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

    UpdateCache ();

    GXString marked = _cache.GetMiddle ( static_cast<GXUPointer> ( begin ), static_cast<GXUPointer> ( end - 1 ) );
    GXUPointer size = 0u;
    const GXWChar* source = marked.ToSystemWideString ( size );

    HGLOBAL block = GlobalAlloc ( GMEM_MOVEABLE, size );
    GXWChar* clipData = static_cast<GXWChar*> ( GlobalLock ( block ) );
    memcpy ( clipData, source, size );
    GlobalUnlock ( block );

    OpenClipboard ( nullptr );
    EmptyClipboard ();
    SetClipboardData ( CF_UNICODETEXT, clipData );
    CloseClipboard ();
}

GXVoid GXUIEditBox::PasteText ( const GXString &textToPaste )
{
    GXUInt numSymbols = static_cast<GXUInt> ( textToPaste.GetSymbolCount () );
    GXUInt totalSymbols = _textSymbols + numSymbols;

    if ( totalSymbols > _maxSymbols )
    {
        GXUInt size = ( totalSymbols + GX_TEXT_GROW_FACTOR + 1 ) * sizeof ( GXStringSymbol );

        if ( _maxSymbols == 0u )
        {
            _text = static_cast<GXStringSymbol*> ( Malloc ( size ) );
            _workingBuffer = static_cast<GXStringSymbol*> ( Malloc ( size ) );

            _text[ 0u ] = '\0';
        }
        else
        {
            Free ( _workingBuffer );
            _workingBuffer = static_cast<GXStringSymbol*> ( Malloc ( size ) );

            GXStringSymbol* newText = static_cast<GXStringSymbol*> ( Malloc ( size ) );
            memcpy ( newText, _text, ( _textSymbols + 1 ) * sizeof ( GXStringSymbol ) );
            Free ( _text );
            _text = newText;
        }

        _maxSymbols = totalSymbols + GX_TEXT_GROW_FACTOR;
    }

    textToPaste.ToSymbols ( _workingBuffer, _maxSymbols );

    memmove ( _text + _cursor + numSymbols, _text + _cursor, ( static_cast<GXInt> ( _textSymbols ) + 1 - _cursor ) * sizeof ( GXStringSymbol ) );
    memcpy ( _text + _cursor, _workingBuffer, numSymbols * sizeof ( GXStringSymbol ) );

    _cursor = _selection = _cursor + static_cast<GXInt> ( numSymbols );
    _textSymbols += numSymbols;

    _isCacheValid = GX_FALSE;
}

GXBool GXUIEditBox::DeleteText ()
{
    if ( _cursor == _selection )
        return GX_FALSE;

    GXInt start = _cursor;
    GXInt end = _selection;

    if ( start > end )
    {
        start = _selection;
        end = _cursor;
    }

    memmove ( _text + start, _text + end, ( _textSymbols + 1 - end ) * sizeof ( GXStringSymbol ) );
    _cursor = _selection = start;
    _textSymbols -= end - start;
    _isCacheValid = GX_FALSE;

    return GX_TRUE;
}

GXVoid GXUIEditBox::OnAddSymbol ( const GXVoid* data )
{
    // Symbol inserting, END, HOME, arrows
    // Ctrl + V, Ctrl + C, Ctrl + X
    // Shift + HOME, Shift + END.
    // Del, Backspace
    // Selected text.

    const GXWChar* symbol = static_cast<const GXWChar*> ( data );

    switch ( *symbol )
    {
        case GX_CTRL_C:
            CopyText ();
        break;

        case GX_CTRL_V:
        {
            if ( !IsClipboardFormatAvailable ( CF_UNICODETEXT ) || !OpenClipboard ( nullptr ) ) break;

            HGLOBAL block = GetClipboardData ( CF_UNICODETEXT );

            if ( !block )
            {
                CloseClipboard ();
                break;
            }

            GXWChar* clipBoardData = static_cast<GXWChar*> ( GlobalLock ( block ) );

            if ( !clipBoardData )
            {
                CloseClipboard ();
                break;
            }

            DeleteText ();
            PasteText ( clipBoardData );
            GlobalUnlock ( block );

            if ( _renderer )
            {
                _renderer->OnUpdate ();
            }

            CloseClipboard ();
        }
        break;

        case GX_CTRL_X:
        {
            CopyText ();

            if ( !DeleteText () || !_renderer ) break;

            _renderer->OnUpdate ();
        }
        break;

        case GX_CTRL_A:
        {
            if ( _cursor == static_cast<GXInt> ( _textSymbols ) && _selection == 0 ) break;

            _cursor = static_cast<GXInt> ( _textSymbols );
            _selection = 0;

            if ( !_renderer ) break;

            _renderer->OnUpdate ();
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
            if ( _cursor == _selection && _cursor == static_cast<GXInt> ( _textSymbols ) ) break;

            if ( _cursor == _selection )
                ++_selection;

            if ( !DeleteText () || !_renderer ) break;

            _renderer->OnUpdate ();
        }
        break;

        case GX_BACKSPACE:
        {
            if ( _cursor == _selection && _cursor == 0 ) break;

            if ( _cursor == _selection )
                --_cursor;

            if ( !DeleteText () || !_renderer ) break;

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
            // Addding one symbol
            static GXWChar buffer[ 2u ] = { 0u };
            buffer[ 0u ] = *symbol;

            _tmp.FromSystemWideString ( buffer );

            DeleteText ();
            PasteText ( _tmp );

            if ( !_renderer ) break;

            _renderer->OnUpdate ();
        }
        break;
    }
}

GXVoid GXUIEditBox::OnClearText ( const GXVoid* /*data*/ )
{
    if ( _textSymbols == 0u ) return;

    _text[ 0u ] = '\0';

    _textSymbols = 0u;
    _cursor = _selection = 0;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIEditBox::OnDoubleClick ( const GXVoid* data )
{
    if ( _textSymbols == 0u ) return;

    const GXVec2* pos = static_cast<const GXVec2*> ( data );
    GXInt current = GetSelectionPosition ( *pos );

    if ( _text[ current ] == GX_UI_SEPARATOR_SYMBOL ) return;

    GXInt end = current + 1;

    for ( ; end < static_cast<GXInt> ( _textSymbols ); ++end )
    {
        if ( _text[ end ] != GX_UI_SEPARATOR_SYMBOL ) continue;

        break;
    }

    GXInt begin = current;

    for ( ; begin > 0; --begin )
    {
        if ( _text[ begin ] != GX_UI_SEPARATOR_SYMBOL ) continue;

        ++begin;
        break;
    }

    if ( _selection == begin && _cursor == end ) return;

    _selection = begin;
    _cursor = end;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIEditBox::OnEditBoxSetFont ( const GXVoid* data )
{
    const GXUIEditBoxFontInfo* fi = static_cast<const GXUIEditBoxFontInfo*> ( data );
    delete _font;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXFont" )
    _font = new GXFont ( fi->_fontFile, fi->_size );

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIEditBox::OnEditBoxSetTextLeftOffset ( const GXVoid* data )
{
    const GXFloat* offset = static_cast<const GXFloat*> ( data );

    if ( _textLeftOffset == *offset ) return;

    _textLeftOffset = *offset;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIEditBox::OnEditBoxSetTextRightOffset ( const GXVoid* data )
{
    const GXFloat* offset = static_cast<const GXFloat*> ( data );

    if ( _textRightOffset == *offset ) return;

    _textRightOffset = *offset;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIEditBox::OnLMBDown ( const GXVoid* data )
{
    // Capture keyboard inpur if cursor is under widget area.
    // Release keyboard capture if cursor is not under widget area. Send message to appropriate widget.
    // Set cursor in right position if click occurs in widget area.
    // Select text ara if a click occurs in widget area and Shift key is pressed.

    const GXVec2* pos = static_cast<const GXVec2*> ( data );

    if ( _boundsWorld.IsOverlaped ( pos->_data[ 0u ], pos->_data[ 1u ], 0.0f ) )
    {
        LockInput ();

        if ( GetKeyState ( VK_SHIFT ) & GX_UI_KEYSTATE_MASK )
        {
            _cursor = GetSelectionPosition ( *pos );
        }
        else
        {
            _cursor = _selection = GetSelectionPosition ( *pos );
        }
    }
    else
    {
        ReleaseInput ();
        UpdateCache ();

        if ( _validator && _validator->Validate ( _cache ) && _onFinishEditing )
        {
            _onFinishEditing ( _context, *this );
        }
        else if ( _onFinishEditing )
        {
            _onFinishEditing ( _context, *this );
        }

        GXTouchSurface::GetInstance ().OnLeftMouseButtonDown ( *pos );
    }

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIEditBox::OnMouseLeave ( const GXVoid* /*data*/ )
{
    SetCursor ( _arrowCursor );
}

GXVoid GXUIEditBox::OnMouseMove ( const GXVoid* data )
{
    const GXVec2* pos = static_cast<const GXVec2*> ( data );

    if ( _boundsWorld.IsOverlaped ( pos->_data[ 0u ], pos->_data[ 1u ], 0.0f ) )
    {
        if ( _currentCursor != &_editCursor )
        {
            _currentCursor = &_editCursor;
            SetCursor ( _editCursor );
        }
    }
    else
    {
        if ( _currentCursor != &_arrowCursor )
        {
            _currentCursor = &_arrowCursor;
            SetCursor ( _arrowCursor );
        }
    }

    if ( _textSymbols == 0 ) return;

    if ( !( GetKeyState ( VK_LBUTTON ) & GX_UI_KEYSTATE_MASK ) ) return;

    GXInt current = GetSelectionPosition ( *pos );

    if ( current == _cursor ) return;

    _cursor = current;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIEditBox::OnMouseOver ( const GXVoid* /*data*/ )
{
    SetCursor ( _editCursor );
}

GXVoid GXUIEditBox::OnSetText ( const GXVoid* data )
{
    const GXString* newText = static_cast<const GXString*> ( data );

    // Optimization: making unique string copy.
    _tmp.FromUTF16 ( static_cast<const GXUTF16*> ( *newText ) );

    // Clean up placement new stuff.
    // see GXUIEditBox::SetText method.
    newText->~GXString ();

    _textSymbols = static_cast<GXUInt> ( _tmp.GetSymbolCount () );
    _isCacheValid = GX_FALSE;

    if ( _textSymbols <= _maxSymbols )
    {
        _tmp.ToSymbols ( _text, _maxSymbols + 1u );
        _cursor = _selection = 0;
    }
    else
    {
        SafeFree ( reinterpret_cast<GXVoid**> ( &_text ) );
        SafeFree ( reinterpret_cast<GXVoid**> ( &_workingBuffer ) );

        _text = _tmp.ToSymbols ();
        _workingBuffer = static_cast<GXStringSymbol*> ( Malloc ( ( _textSymbols + 1u ) * sizeof ( GXStringSymbol ) ) );

        _maxSymbols = _textSymbols;
    }

    if ( _validator )
        _validator->Validate ( _tmp );

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXUIEditBox::OnSetTextAlighment ( const GXVoid* data )
{
    const eGXUITextAlignment* newAlignment = static_cast<const eGXUITextAlignment*> ( data );
    this->_alignment = *newAlignment;

    if ( !_renderer ) return;

    _renderer->OnUpdate ();
}

GXVoid GXCALL GXUIEditBox::OnEnd ( GXVoid* context )
{
    static const GXWChar symbol = GX_END;
    GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( context ), eGXUIMessage::AddSymbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnHome ( GXVoid* context )
{
    static const GXWChar symbol = GX_HOME;
    GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( context ), eGXUIMessage::AddSymbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnDel ( GXVoid* context )
{
    static const GXWChar symbol = GX_DEL;
    GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( context ), eGXUIMessage::AddSymbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnBackspace ( GXVoid* context )
{
    static const GXWChar symbol = GX_BACKSPACE;
    GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( context ), eGXUIMessage::AddSymbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnLeftArrow ( GXVoid* context )
{
    static const GXWChar symbol = GX_LEFT_ARROW;
    GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( context ), eGXUIMessage::AddSymbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnRightArrow ( GXVoid* context )
{
    static const GXWChar symbol = GX_RIGHT_ARROW;
    GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( context ), eGXUIMessage::AddSymbol, &symbol, sizeof ( GXWChar ) );
}

GXVoid GXCALL GXUIEditBox::OnType ( GXVoid* context, GXWChar symbol )
{
    GXTouchSurface::GetInstance ().SendMessage ( static_cast<GXWidget*> ( context ), eGXUIMessage::AddSymbol, &symbol, sizeof ( GXWChar ) );
}
