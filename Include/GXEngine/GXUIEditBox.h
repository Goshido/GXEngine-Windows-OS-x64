// version 1.8

#ifndef GX_UI_EDIT_BOX
#define GX_UI_EDIT_BOX


#include "GXUIStaticText.h"
#include "GXFont.h"


class GXUIEditBox;
typedef GXVoid ( GXCALL* GXUIEditBoxOnCommitHandler ) ( GXVoid* context, GXUIEditBox &editBox );
typedef GXVoid ( GXUIEditBox::* GXUIEditBoxOnMessageHandler ) ( const GXVoid* data );

class GXUIEditBoxMessageHandlerNode final : public GXUIWidgetMessageHandlerNode
{
    private:
        GXUIEditBoxOnMessageHandler     _handler;

    public:
        GXUIEditBoxMessageHandlerNode ();

        // Special probe constructor.
        explicit GXUIEditBoxMessageHandlerNode ( eGXUIMessage message );
        ~GXUIEditBoxMessageHandlerNode () override;

        GXVoid HandleMassage ( const GXVoid* data ) override;

        GXVoid Init ( GXUIEditBox &editBox, eGXUIMessage message, GXUIEditBoxOnMessageHandler handler );

    private:
        GXUIEditBoxMessageHandlerNode ( const GXUIEditBoxMessageHandlerNode &other ) = delete;
        GXUIEditBoxMessageHandlerNode& operator = ( const GXUIEditBoxMessageHandlerNode &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

class GXTextValidator;
class GXUIEditBox final : public GXWidget
{
    private:
        GXStringSymbol*                     _text;
        GXStringSymbol*                     _workingBuffer;
        GXUInt                              _textSymbols;
        GXUInt                              _maxSymbols;

        GXFloat                             _textLeftOffset;
        GXFloat                             _textRightOffset;

        GXInt                               _cursor;        // index before symbol
        GXInt                               _selection;     // index before symbol
        eGXUITextAlignment                  _alignment;

        GXTextValidator*                    _validator;

        GXUIEditBoxOnCommitHandler          _onFinishEditing;
        GXVoid*                             _context;

        HCURSOR                             _editCursor;
        HCURSOR                             _arrowCursor;

        GXBool                              _isCacheValid;

        // Optimization stuff.
        GXString                            _cache;
        GXString                            _tmp;
        GXUIEditBoxMessageHandlerNode       _messageHandlers[ 12u ];

        GXFont*                             _font;
        const HCURSOR*                      _currentCursor;

    public:
        explicit GXUIEditBox ( GXWidget* parent );
        ~GXUIEditBox () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

        GXFloat GetCursorOffset ();
        GXFloat GetSelectionBeginOffset ();
        GXFloat GetSelectionEndOffset ();

        GXVoid SetTextLeftOffset ( GXFloat offset );
        GXFloat GetTextLeftOffset () const;

        GXVoid SetTextRightOffset ( GXFloat offset );
        GXFloat GetTextRightOffset () const;

        GXVoid SetText ( const GXString &text );
        const GXString& GetText ();

        GXVoid SetAlignment ( eGXUITextAlignment alignment );
        eGXUITextAlignment GetAlignment () const;

        GXVoid SetFont ( const GXString &fontFile, GXUShort fontSize );
        GXFont* GetFont ();

        GXBool IsActive ();

        GXVoid SetValidator ( GXTextValidator &validator );
        GXTextValidator* GetValidator () const;

        GXVoid SetOnFinishEditingCallback ( GXVoid* context, GXUIEditBoxOnCommitHandler callback );

    private:
        GXInt GetSelectionPosition ( const GXVec2 &mousePosition );
        GXFloat GetSelectionOffset ( GXUInt symbolIndex );

        GXVoid InitMessageHandlers ();

        GXVoid LockInput ();
        GXVoid ReleaseInput ();

        GXVoid UpdateCursor ( GXInt cursor );

        // Note method does nothing if cache is valid.
        GXVoid UpdateCache ();

        GXVoid CopyText ();
        GXVoid PasteText ( const GXString &textToPaste );
        GXBool DeleteText ();

        // Message handlers
        GXVoid OnAddSymbol ( const GXVoid* data );
        GXVoid OnClearText ( const GXVoid* data );
        GXVoid OnDoubleClick ( const GXVoid* data );
        GXVoid OnEditBoxSetFont ( const GXVoid* data );
        GXVoid OnEditBoxSetTextLeftOffset ( const GXVoid* data );
        GXVoid OnEditBoxSetTextRightOffset ( const GXVoid* data );
        GXVoid OnLMBDown ( const GXVoid* data );
        GXVoid OnMouseLeave ( const GXVoid* data );
        GXVoid OnMouseMove ( const GXVoid* data );
        GXVoid OnMouseOver ( const GXVoid* data );
        GXVoid OnSetText ( const GXVoid* data );
        GXVoid OnSetTextAlighment ( const GXVoid* data );

        static GXVoid GXCALL OnEnd ( GXVoid* context );
        static GXVoid GXCALL OnHome ( GXVoid* context );
        static GXVoid GXCALL OnDel ( GXVoid* context );
        static GXVoid GXCALL OnBackspace ( GXVoid* context );
        static GXVoid GXCALL OnLeftArrow ( GXVoid* context );
        static GXVoid GXCALL OnRightArrow ( GXVoid* context );
        static GXVoid GXCALL OnType ( GXVoid* context, GXWChar symbol );

        GXUIEditBox () = delete;
        GXUIEditBox ( const GXUIEditBox &other ) = delete;
        GXUIEditBox& operator = ( const GXUIEditBox &other ) = delete;
};


#endif // GX_UI_EDIT_BOX
