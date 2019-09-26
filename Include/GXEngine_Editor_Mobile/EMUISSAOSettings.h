#ifndef EM_UI_SSAO_SETTINGS
#define EN_UI_SSAO_SETTINGS


#include "EMUIDraggableArea.h"
#include "EMUIStaticText.h"
#include "EMUISeparator.h"
#include "EMUIEditBox.h"
#include "EMUIButton.h"


class EMUISSAOSettings final : public EMUI
{
    private:
        EMUIDraggableArea           _mainPanel;

        EMUIStaticText*             _caption;
        EMUISeparator*              _topSeparator;
        EMUIStaticText*             _checkRadiusLabel;
        EMUIEditBox*                _checkRadius;
        EMUIStaticText*             _samplesLabel;
        EMUIEditBox*                _samples;
        EMUIStaticText*             _noiseTextureResolutionLabel;
        EMUIEditBox*                _noiseTextureResolution;
        EMUIStaticText*             _maxDistanceLabel;
        EMUIEditBox*                _maxDistance;
        EMUISeparator*              _bottomSeparator;
        EMUIButton*                 _cancel;
        EMUIButton*                 _apply;

        // Optimization stuff.
        GXString                    _buffer;

        static EMUISSAOSettings*    _instance;

    public:
        static EMUISSAOSettings& GetInstance ();
        ~EMUISSAOSettings () override;

        GXWidget* GetWidget () override;

        GXVoid Show ();
        GXVoid Hide ();

    private:
        EMUISSAOSettings ();

        GXVoid SyncSettings ();

        static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton& button, GXFloat x, GXFloat y, eGXMouseButtonState state );
        static GXVoid GXCALL OnResize ( GXVoid* handler, GXUIDragableArea& area, GXFloat width, GXFloat height );

        EMUISSAOSettings ( const EMUISSAOSettings &other ) = delete;
        EMUISSAOSettings& operator = ( const EMUISSAOSettings &other ) = delete;
};


#endif // EM_UI_SSAO_SETTINGS
