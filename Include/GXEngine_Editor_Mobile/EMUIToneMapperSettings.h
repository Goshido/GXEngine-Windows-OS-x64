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
        EMUIDraggableArea                   _mainPanel;

        EMUIStaticText*                     _caption;
        EMUISeparator*                      _topSeparator;
        EMUIStaticText*                     _gammaLabel;
        EMUIEditBox*                        _gamma;
        EMUIStaticText*                     _sensitivityLabel;
        EMUIEditBox*                        _sensitivity;
        EMUIStaticText*                     _eyeAdaptationSpeedLabel;
        EMUIEditBox*                        _eyeAdaptationSpeed;
        EMUIStaticText*                     _whiteIntensityLabel;
        EMUIEditBox*                        _whiteIntensity;
        EMUISeparator*                      _bottomSeparator;
        EMUIButton*                         _cancel;
        EMUIButton*                         _apply;

        static EMUIToneMapperSettings*      _instance;

    public:
        static EMUIToneMapperSettings& GXCALL GetInstance ();
        ~EMUIToneMapperSettings () override;

        GXWidget* GetWidget () override;

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
