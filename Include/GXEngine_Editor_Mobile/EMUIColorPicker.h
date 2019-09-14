#ifndef EM_UI_COLOR_PICKER
#define EM_UI_COLOR_PICKER


#include <GXEngine_Editor_Mobile/EMUIDraggableArea.h>
#include <GXEngine_Editor_Mobile/EMUIStaticText.h>
#include <GXEngine_Editor_Mobile/EMUISeparator.h>
#include <GXEngine_Editor_Mobile/EMUIButton.h>
#include <GXEngine_Editor_Mobile/EMUIEditBox.h>
#include <GXEngine/GXUIInput.h>


class EMUIColorPicker;
typedef GXVoid ( GXCALL* EMColorPickerOnHSVAColorHandler ) ( GXVoid* context, const GXColorHSV &color );
typedef GXVoid ( GXCALL* EMColorPickerOnRGBAColorHandler ) ( GXVoid* context, const GXColorRGB &color );
typedef GXVoid ( GXCALL* EMColorPickerOnHSVAColorUByteHandler ) ( GXVoid* context, GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );


class EMUIColorPicker final : public EMUI
{
    private:
        EMUIDraggableArea                       _mainPanel;

        EMColorPickerOnHSVAColorHandler         _onHSVColor;
        EMColorPickerOnRGBAColorHandler         _onRGBColor;
        EMColorPickerOnHSVAColorUByteHandler    _onRGBUByteColor;
        GXVoid*                                 _context;

        GXWChar*                                _buffer;

        EMUIStaticText*                         _caption;
        EMUISeparator*                          _topSeparator;
        GXUIInput*                              _hsvColorWidget;
        GXUIInput*                              _currentColor;
        GXUIInput*                              _oldColor;
        EMUIButton*                             _addColor;
        GXUIInput*                              _savedColors[ 16u ];
        EMUISeparator*                          _middleSeparator;
        EMUIStaticText*                         _hLabel;
        EMUIEditBox*                            _h;
        EMUIStaticText*                         _rLabel;
        EMUIEditBox*                            _r;
        EMUIStaticText*                         _sLabel;
        EMUIEditBox*                            _s;
        EMUIStaticText*                         _gLabel;
        EMUIEditBox*                            _g;
        EMUIStaticText*                         _vLabel;
        EMUIEditBox*                            _v;
        EMUIStaticText*                         _bLabel;
        EMUIEditBox*                            _b;
        EMUIStaticText*                         _transparencyLabel;
        EMUIEditBox*                            _transparency;
        EMUISeparator*                          _bottomSeparator;
        EMUIButton*                             _cancel;
        EMUIButton*                             _pick;

        static EMUIColorPicker*                 _instance;

    public:
        static EMUIColorPicker& GetInstance ();
        ~EMUIColorPicker () override;

        GXWidget* GetWidget () override;

        GXVoid PickColor ( GXVoid* context, EMColorPickerOnHSVAColorHandler callback, const GXColorHSV &oldColorValue );
        GXVoid PickColor ( GXVoid* context, EMColorPickerOnRGBAColorHandler callback, const GXColorRGB &oldColorValue );
        GXVoid PickColor ( GXVoid* context, EMColorPickerOnHSVAColorUByteHandler callback, GXUByte oldRed, GXUByte oldGreen, GXUByte oldBlue, GXUByte oldAlpha );

    private:
        EMUIColorPicker ();

        GXVoid UpdateCurrentColor ( GXFloat hue, GXFloat saturation, GXFloat value, GXFloat alpha );
        GXVoid UpdateCurrentColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
        GXVoid UpdateCurrentColorWithCorrection ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );

        static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton& button, GXFloat x, GXFloat y, eGXMouseButtonState state );
        static GXVoid GXCALL OnLeftMouseButton ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );
        static GXVoid GXCALL OnResize ( GXVoid* handler, GXUIDragableArea& area, GXFloat width, GXFloat height );
        static GXVoid GXCALL OnFinishEditing ( GXVoid* handler, GXUIEditBox& editBox );

        EMUIColorPicker ( const EMUIColorPicker &other ) = delete;
        EMUIColorPicker& operator = ( const EMUIColorPicker &other ) = delete;
};


#endif // EM_UI_COLOR_PICKER
