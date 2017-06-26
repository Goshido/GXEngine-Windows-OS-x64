#ifndef EM_UI_SSAO_SETTINGS
#define EN_UI_SSAO_SETTINGS


#include "EMUIDraggableArea.h"
#include "EMUIStaticText.h"
#include "EMUISeparator.h"
#include "EMUIEditBox.h"
#include "EMUIButton.h"


class EMUISSAOSettings : public EMUI
{
	private:
		EMUIDraggableArea*			mainPanel;
		EMUIStaticText*				caption;
		EMUISeparator*				topSeparator;
		EMUIStaticText*				checkRadiusLabel;
		EMUIEditBox*				checkRadius;
		EMUIStaticText*				samplesLabel;
		EMUIEditBox*				samples;
		EMUIStaticText*				noiseTextureResolutionLabel;
		EMUIEditBox*				noiseTextureResolution;
		EMUIStaticText*				maxDistanceLabel;
		EMUIEditBox*				maxDistance;
		EMUISeparator*				bottomSeparator;
		EMUIButton*					cancel;
		EMUIButton*					apply;

		static EMUISSAOSettings*	instance;

	public:
		static EMUISSAOSettings& GetInstance ();
		~EMUISSAOSettings () override;

		GXWidget* GetWidget () const override;

		GXVoid Show ();
		GXVoid Hide ();

	private:
		EMUISSAOSettings ();

		GXVoid SyncSettings ();

		static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state );
		static GXVoid GXCALL OnResize ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height );
};


#endif //EM_UI_SSAO_SETTINGS
