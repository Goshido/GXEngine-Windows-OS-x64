#ifndef EM_UI_TONE_MAPPER_SETTINGS
#define EM_UI_TONE_MAPPER_SETTINGS


#include "EMUIDraggableArea.h"
#include "EMUIStaticText.h"
#include "EMUISeparator.h"
#include "EMUIEditBox.h"
#include "EMUIButton.h"


class EMUIToneMapperSettings final : public EMUI
{
	private:
		EMUIDraggableArea*				mainPanel;

		EMUIStaticText*					caption;
		EMUISeparator*					topSeparator;
		EMUIStaticText*					gammaLabel;
		EMUIEditBox*					gamma;
		EMUIStaticText*					sensitivityLabel;
		EMUIEditBox*					sensitivity;
		EMUIStaticText*					eyeAdaptationSpeedLabel;
		EMUIEditBox*					eyeAdaptationSpeed;
		EMUIStaticText*					whiteIntensityLabel;
		EMUIEditBox*					whiteIntensity;
		EMUISeparator*					bottomSeparator;
		EMUIButton*						cancel;
		EMUIButton*						apply;

		static EMUIToneMapperSettings*	instance;

	public:
		static EMUIToneMapperSettings& GXCALL GetInstance ();
		~EMUIToneMapperSettings () override;

		GXWidget* GetWidget () const override;

		GXVoid Show ();
		GXVoid Hide ();

	private:
		EMUIToneMapperSettings ();

		GXVoid SyncSettings ();

		static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton& button, GXFloat x, GXFloat y, eGXMouseButtonState state );
		static GXVoid GXCALL OnResize ( GXVoid* handler, GXUIDragableArea& area, GXFloat width, GXFloat height );

		EMUIToneMapperSettings ( const EMUIToneMapperSettings &other ) = delete;
		EMUIToneMapperSettings& operator = ( const EMUIToneMapperSettings &other ) = delete;
};


#endif // EM_UI_TONE_MAPPER_SETTINGS
