#ifndef EM_UI_MOTION_BLUR_SETTINGS
#define EM_UI_MOTION_BLUR_SETTINGS


#include "EMUIDraggableArea.h"
#include "EMUIStaticText.h"
#include "EMUISeparator.h"
#include "EMUIEditBox.h"
#include "EMUIButton.h"


class EMUIMotionBlurSettings : public EMUI
{
	private:
		EMUIDraggableArea*		mainPanel;
		EMUIStaticText*			caption;
		EMUISeparator*			topSeparator;
		EMUIStaticText*			totalSamplesLabel;
		EMUIEditBox*			totalSamples;
		EMUIStaticText*			depthLimitLabel;
		EMUIEditBox*			depthLimit;
		EMUIStaticText*			exposureLabel;
		EMUIEditBox*			exposure;
		EMUISeparator*			bottomSeparator;
		EMUIButton*				cancel;
		EMUIButton*				apply;

	public:
		EMUIMotionBlurSettings ();
		~EMUIMotionBlurSettings () override;

		GXWidget* GetWidget () const override;

		GXVoid Show ();
		GXVoid Hide ();

	private:
		GXVoid ApplySettings () const;

		static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state );
		static GXVoid GXCALL OnResize ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height );
};


#endif //EM_UI_MOTION_BLUR_SETTINGS
