// version 1.7

#ifndef GX_UI_EDIT_BOX
#define GX_UI_EDIT_BOX


#include "GXUIStaticText.h"
#include "GXFont.h"


class GXUIEditBox;
typedef GXVoid ( GXCALL* PFNGXUIEDITBOXONFINISHEDITINGPROC ) ( GXVoid* handler, GXUIEditBox& editBox );


class GXTextValidator;
class GXUIEditBox final : public GXWidget
{
    private:
        GXWChar*                                _text;
        GXWChar*                                _workingBuffer;
        GXUInt                                  _textSymbols;
        GXUInt                                  _maxSymbols;

        GXFloat                                 _textLeftOffset;
        GXFloat                                 _textRightOffset;

        GXInt                                   _cursor;        // index before symbol
        GXInt                                   _selection;     // index before symbol
        eGXUITextAlignment                      _alignment;

        GXTextValidator*                        _validator;

        PFNGXUIEDITBOXONFINISHEDITINGPROC       _onFinishEditing;
        GXVoid*                                 _context;

        HCURSOR                                 _editCursor;
        HCURSOR                                 _arrowCursor;

        GXFont*                                 _font;
        const HCURSOR*                          _currentCursor;

    public:
        explicit GXUIEditBox ( GXWidget* parent );
        ~GXUIEditBox () override;

        GXVoid OnMessage ( eGXUIMessage message, const GXVoid* data ) override;

        GXFloat GetCursorOffset () const;
        GXFloat GetSelectionBeginOffset () const;
        GXFloat GetSelectionEndOffset () const;

        GXVoid SetTextLeftOffset ( GXFloat offset );
        GXFloat GetTextLeftOffset () const;

        GXVoid SetTextRightOffset ( GXFloat offset );
        GXFloat GetTextRightOffset () const;

        GXVoid SetText ( const GXWChar* newText );
        const GXWChar* GetText () const;

        GXVoid SetAlignment ( eGXUITextAlignment newAlignment );
        eGXUITextAlignment GetAlignment () const;

        GXVoid SetFont ( const GXWChar* fontFile, GXUShort fontSize );
        GXFont* GetFont ();

        GXBool IsActive ();

        GXVoid SetValidator ( GXTextValidator &validatorObject );
        GXTextValidator* GetValidator () const;

        GXVoid SetOnFinishEditingCallback ( GXVoid* handlerObject, PFNGXUIEDITBOXONFINISHEDITINGPROC callback );

    private:
        GXInt GetSelectionPosition ( const GXVec2 &mousePosition ) const;
        GXFloat GetSelectionOffset ( GXUInt symbolIndex ) const;

        GXVoid LockInput ();
        GXVoid ReleaseInput ();

        GXVoid UpdateCursor ( GXInt newCursor );

        GXVoid CopyText ();
        GXVoid PasteText ( const GXWChar* textToPaste );
        GXBool DeleteText ();

        static GXVoid GXCALL OnEnd ( GXVoid* handler );
        static GXVoid GXCALL OnHome ( GXVoid* handler );
        static GXVoid GXCALL OnDel ( GXVoid* handler );
        static GXVoid GXCALL OnBackspace ( GXVoid* handler );
        static GXVoid GXCALL OnLeftArrow ( GXVoid* handler );
        static GXVoid GXCALL OnRightArrow ( GXVoid* handler );
        static GXVoid GXCALL OnType ( GXVoid* handler, GXWChar symbol );

        GXUIEditBox () = delete;
        GXUIEditBox ( const GXUIEditBox &other ) = delete;
        GXUIEditBox& operator = ( const GXUIEditBox &other ) = delete;
};


#endif // GX_UI_EDIT_BOX
