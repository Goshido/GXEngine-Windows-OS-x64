#ifndef EM_UI_MOTION_BLUR_SETTINGS
#define EM_UI_MOTION_BLUR_SETTINGS


#include "EMUIDraggableArea.h"
#include "EMUIStaticText.h"
#include "EMUISeparator.h"
#include "EMUIEditBox.h"
#include "EMUIButton.h"


class EMUIMotionBlurSettings final : public EMUI
{
    private:
        EMUIDraggableArea                   _mainPanel;
        EMUIStaticText*                     _caption;
        EMUISeparator*                      _topSeparator;
        EMUIStaticText*                     _maxSamplesLabel;
        EMUIEditBox*                        _maxSamples;
        EMUIStaticText*                     _depthLimitLabel;
        EMUIEditBox*                        _depthLimit;
        EMUIStaticText*                     _exposureLabel;
        EMUIEditBox*                        _exposure;
        EMUISeparator*                      _bottomSeparator;
        EMUIButton*                         _cancel;
        EMUIButton*                         _apply;

        static EMUIMotionBlurSettings*      _instance;

    public:
        static EMUIMotionBlurSettings& GetInstance ();
        ~EMUIMotionBlurSettings () override;

        GXWidget* GetWidget () override;

        GXVoid Show ();
        GXVoid Hide ();

    private:
        EMUIMotionBlurSettings ();

        GXVoid SyncSettings ();

        static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton& button, GXFloat x, GXFloat y, eGXMouseButtonState state );
        static GXVoid GXCALL OnResize ( GXVoid* handler, GXUIDragableArea& area, GXFloat width, GXFloat height );

        EMUIMotionBlurSettings ( const EMUIMotionBlurSettings &other ) = delete;
        EMUIMotionBlurSettings& operator = ( const EMUIMotionBlurSettings &other ) = delete;
};


#endif // EM_UI_MOTION_BLUR_SETTINGS
