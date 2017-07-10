#ifndef EM_UI_COLOR_PICKER
#define EM_UI_COLOR_PICKER


#include <GXEngine_Editor_Mobile/EMUIDraggableArea.h>
#include <GXEngine_Editor_Mobile/EMUIStaticText.h>
#include <GXEngine_Editor_Mobile/EMUISeparator.h>
#include <GXEngine_Editor_Mobile/EMUIButton.h>
#include <GXEngine_Editor_Mobile/EMUIEditBox.h>
#include <GXEngine/GXUIInput.h>



class EMUIColorPicker;
typedef GXVoid ( GXCALL* PFNEMONHSVCOLORPROC ) ( GXVoid* handler, GXFloat h, GXFloat s, GXFloat v, GXUByte alpha );
typedef GXVoid ( GXCALL* PFNEMONRGBCOLORPROC ) ( GXVoid* handler, GXUByte r, GXUByte g, GXUByte b, GXUByte alpha );


class EMUIColorPicker : public EMUI
{
	private:
		EMUIDraggableArea*			mainPanel;
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

		GXVec4						currentColorHSV;
		GXVec4						oldColorHSV;
		GXVec4						savedColorHSVs[ 16 ];

		PFNEMONHSVCOLORPROC			OnHSVColor;
		PFNEMONRGBCOLORPROC			OnRGBColor;
		GXVoid*						handler;

		GXWChar*					buffer;

		static EMUIColorPicker*		instance;

	public:
		static EMUIColorPicker& GetInstance ();
		~EMUIColorPicker () override;

		GXWidget* GetWidget () const override;

		GXVoid PickHSVColor ( GXVoid* handler, PFNEMONHSVCOLORPROC callback );
		GXVoid PickRGBColor ( GXVoid* handler, PFNEMONRGBCOLORPROC callback );

	private:
		EMUIColorPicker ();

		GXVoid UpdateCurrentColor ( GXFloat hue, GXFloat saturation, GXFloat value, GXFloat alpha );
		GXVoid UpdateCurrentColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );

		static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state );
		static GXVoid GXCALL OnLeftMouseButton ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y );
		static GXVoid GXCALL OnResize ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height );
};


#endif //EM_UI_COLOR_PICKER
