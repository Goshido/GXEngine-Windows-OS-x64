#ifndef EM_UI_COLOR_PICKER
#define EM_UI_COLOR_PICKER


#include <GXEngine_Editor_Mobile/EMUIDraggableArea.h>
#include <GXEngine_Editor_Mobile/EMUIStaticText.h>
#include <GXEngine_Editor_Mobile/EMUISeparator.h>
#include <GXEngine_Editor_Mobile/EMUIButton.h>
#include <GXEngine_Editor_Mobile/EMUIEditBox.h>
#include <GXEngine/GXUIInput.h>


class EMUIColorPicker;
typedef GXVoid ( GXCALL* PFNEMONHSVACOLORPROC ) ( GXVoid* handler, const GXColorHSV &color );
typedef GXVoid ( GXCALL* PFNEMONRGBACOLORPROC ) ( GXVoid* handler, const GXColorRGB &color );
typedef GXVoid ( GXCALL* PFNEMONRGBAUBYTECOLORPROC ) ( GXVoid* handler, GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );


class EMUIColorPicker final : public EMUI
{
	private:
		EMUIDraggableArea*			mainPanel;

		PFNEMONHSVACOLORPROC		OnHSVColor;
		PFNEMONRGBACOLORPROC		OnRGBColor;
		PFNEMONRGBAUBYTECOLORPROC	OnRGBUByteColor;
		GXVoid*						handler;

		GXWChar*					buffer;

		EMUIStaticText*				caption;
		EMUISeparator*				topSeparator;
		GXUIInput*					hsvColorWidget;
		GXUIInput*					currentColor;
		GXUIInput*					oldColor;
		EMUIButton*					addColor;
		GXUIInput*					savedColors[ 16 ];
		EMUISeparator*				middleSeparator;
		EMUIStaticText*				hLabel;
		EMUIEditBox*				h;
		EMUIStaticText*				rLabel;
		EMUIEditBox*				r;
		EMUIStaticText*				sLabel;
		EMUIEditBox*				s;
		EMUIStaticText*				gLabel;
		EMUIEditBox*				g;
		EMUIStaticText*				vLabel;
		EMUIEditBox*				v;
		EMUIStaticText*				bLabel;
		EMUIEditBox*				b;
		EMUIStaticText*				transparencyLabel;
		EMUIEditBox*				transparency;
		EMUISeparator*				bottomSeparator;
		EMUIButton*					cancel;
		EMUIButton*					pick;

		static EMUIColorPicker*		instance;

	public:
		static EMUIColorPicker& GetInstance ();
		~EMUIColorPicker () override;

		GXWidget* GetWidget () const override;

		GXVoid PickColor ( GXVoid* handlerObject, PFNEMONHSVACOLORPROC callback, const GXColorHSV &oldColorValue );
		GXVoid PickColor ( GXVoid* handlerObject, PFNEMONRGBACOLORPROC callback, const GXColorRGB &oldColorValue );
		GXVoid PickColor ( GXVoid* handlerObject, PFNEMONRGBAUBYTECOLORPROC callback, GXUByte oldRed, GXUByte oldGreen, GXUByte oldBlue, GXUByte oldAlpha );

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
