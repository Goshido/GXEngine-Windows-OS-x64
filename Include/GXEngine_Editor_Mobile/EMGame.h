#ifndef EM_MAIN
#define EM_MAIN


#include "EMUIButton.h"
#include "EMUIMenu.h"
#include "EMUIOpenFile.h"
#include "EMUIPopup.h"
#include "EMUIEditBox.h"
#include "EMDirectedLightActor.h"
#include "EMUnitActor.h"
#include "EMMeshActor.h"
#include "EMMoveTool.h"
#include "EMPhysicsDrivenActor.h"
#include "EMFluttershy.h"
#include "EMLightProbe.h"
#include <GXEngine/GXCameraOrthographic.h>
#include <GXEngine/GXGame.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXUnlitColorMaterial.h>
#include <GXPhysics/GXGravity.h>


class EMGame final : public GXGame
{
    private:
        GXCameraOrthographic*       _hudCamera;

        EMUIOpenFile*               _openFile;

        EMUIMenu*                   _menu;
        EMUIPopup*                  _filePopup;
        EMUIPopup*                  _createPopup;
        EMUIPopup*                  _toolsPopup;
        EMUIPopup*                  _utilityPopup;
        EMUIPopup*                  _effectsPopup;

        GXHudSurface*               _physicsInfo;
        GXMeshGeometry*             _physicsContactPointMesh;
        GXUnlitColorMaterial*       _physicsContactPointMaterial;

        GXGravity                   _gravity;

        EMDirectedLightActor*       _directedLight;

        EMUnitActor*                _unitActor;
        EMPhysicsDrivenActor*       _colliderOne;
        EMPhysicsDrivenActor*       _colliderTwo;
        EMPhysicsDrivenActor*       _kinematicPlane;

        EMMoveTool*                 _moveTool;

        GXMeshGeometry*             _contactLocationMesh;
        GXUnlitColorMaterial*       _contactLocationMaterial;

        GXMeshGeometry*             _contactNormalMesh;
        GXUnlitColorMaterial*       _contactNormalMaterial;

        GXTextureCubeMap*           _environmentMap;
        GXTextureCubeMap*           _lightProbeSourceTexture;

        EMLightProbe*               _lightProbe;

        GXUIInput*                  _uiInput;

        GXFont*                     _physicsInfoFont;
        GXTexture2D                 _physicsInfoBackgroundTexture;

        // Optimization
        GXString                    _buffer;

        GXString                    _allocatedEdgeFormat;
        GXString                    _allocatedFaceFormat;
        GXString                    _allocatedSupportPointFormat;
        GXString                    _contactFormat;
        GXString                    _epaIterationFormat;
        GXString                    _gjkIterationFormat;
        GXString                    _penetrationDepthFormat;
        GXString                    _usedEdgeFormat;
        GXString                    _usedFaceFormat;
        GXString                    _usedSupportPointFormat;

        static GXBool               _isContact;
        static GXMat4               _contactNormalTransform;

    public:
        EMGame ();
        ~EMGame () override;

        GXVoid OnInit () override;
        GXVoid OnResize ( GXInt width, GXInt height ) override;
        GXVoid OnFrame ( GXFloat deltaTime ) override;
        GXVoid OnDestroy () override;

    private:
        GXVoid InitStrings ( const GXLocale &locale );

        GXVoid BindDesktopInput ();
        GXVoid UnbindDesktopInput ();

        static GXVoid GXCALL OnExit ( GXVoid* context );
        static GXVoid GXCALL OnColorPicker ( GXVoid* context );
        static GXVoid GXCALL OnPickRGBUByte ( GXVoid* context, GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
        static GXVoid GXCALL OnShowMotionBlurSettings ( GXVoid* context );
        static GXVoid GXCALL OnShowSSAOSettings ( GXVoid* context );
        static GXVoid GXCALL OnShowToneMapperSettings ( GXVoid* context );
        static GXVoid GXCALL OnMouseButton ( GXVoid* handler, GXInputMouseFlags mouseflags );
        static GXVoid GXCALL OnObject ( GXVoid* context, GXVoid* object );
        static GXVoid GXCALL OnViewerTransformChanged ( GXVoid* context );
        static GXVoid GXCALL OnOpenFile ( const GXString &filePath );

        static GXVoid GXCALL OnLShiftDown ( GXVoid* context );
        static GXVoid GXCALL OnLShiftUp ( GXVoid* context );
        static GXVoid GXCALL OnRShiftDown ( GXVoid* context );
        static GXVoid GXCALL OnRShiftUp ( GXVoid* context );

        static GXVoid GXCALL OnLCtrlDown ( GXVoid* context );
        static GXVoid GXCALL OnLCtrlUp ( GXVoid* context );
        static GXVoid GXCALL OnRCtrlDown ( GXVoid* context );
        static GXVoid GXCALL OnRCtrlUp ( GXVoid* context );

        static GXVoid GXCALL OnLAltDown ( GXVoid* context );
        static GXVoid GXCALL OnLAltUp ( GXVoid* context );
        static GXVoid GXCALL OnRAltDown ( GXVoid* context );
        static GXVoid GXCALL OnRAltUp ( GXVoid* context );

        static GXVoid GXCALL OnTabDown ( GXVoid* context );
        static GXVoid GXCALL OnTabUp ( GXVoid* context );

        static GXVoid GXCALL OnCapsLockDown ( GXVoid* context );
        static GXVoid GXCALL OnCapsLockUp ( GXVoid* context );

        static GXVoid GXCALL OnNumpad0Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad0Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpad1Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad1Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpad2Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad2Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpad3Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad3Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpad4Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad4Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpad5Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad5Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpad6Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad6Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpad7Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad7Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpad8Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad8Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpad9Down ( GXVoid* context );
        static GXVoid GXCALL OnNumpad9Up ( GXVoid* context );

        static GXVoid GXCALL OnNumpadDotDown ( GXVoid* context );
        static GXVoid GXCALL OnNumpadDotUp ( GXVoid* context );

        static GXVoid GXCALL OnNumpadEnterDown ( GXVoid* context );
        static GXVoid GXCALL OnNumpadEnterUp ( GXVoid* context );

        static GXVoid GXCALL OnNumpadPlusDown ( GXVoid* context );
        static GXVoid GXCALL OnNumpadPlusUp ( GXVoid* context );

        static GXVoid GXCALL OnNumpadMinusDown ( GXVoid* context );
        static GXVoid GXCALL OnNumpadMinusUp ( GXVoid* context );

        static GXVoid GXCALL OnNumpadMultiplyDown ( GXVoid* context );
        static GXVoid GXCALL OnNumpadMultiplyUp ( GXVoid* context );

        static GXVoid GXCALL OnNumpadDivideDown ( GXVoid* context );
        static GXVoid GXCALL OnNumpadDivideUp ( GXVoid* context );

        static GXVoid GXCALL OnNumLockDown ( GXVoid* context );
        static GXVoid GXCALL OnNumLockUp ( GXVoid* context );

        static GXVoid GXCALL OnContextMenuDown ( GXVoid* context );
        static GXVoid GXCALL OnContextMenuUp ( GXVoid* context );

        static GXVoid GXCALL OnF1Down ( GXVoid* context );
        static GXVoid GXCALL OnF1Up ( GXVoid* context );

        static GXVoid GXCALL OnF2Down ( GXVoid* context );
        static GXVoid GXCALL OnF2Up ( GXVoid* context );

        static GXVoid GXCALL OnF3Down ( GXVoid* context );
        static GXVoid GXCALL OnF3Up ( GXVoid* context );

        static GXVoid GXCALL OnF4Down ( GXVoid* context );
        static GXVoid GXCALL OnF4Up ( GXVoid* context );

        static GXVoid GXCALL OnF5Down ( GXVoid* context );
        static GXVoid GXCALL OnF5Up ( GXVoid* context );

        static GXVoid GXCALL OnF6Down ( GXVoid* context );
        static GXVoid GXCALL OnF6Up ( GXVoid* context );

        static GXVoid GXCALL OnF7Down ( GXVoid* context );
        static GXVoid GXCALL OnF7Up ( GXVoid* context );

        static GXVoid GXCALL OnF8Down ( GXVoid* context );
        static GXVoid GXCALL OnF8Up ( GXVoid* context );

        static GXVoid GXCALL OnF9Down ( GXVoid* context );
        static GXVoid GXCALL OnF9Up ( GXVoid* context );

        static GXVoid GXCALL OnF10Down ( GXVoid* context );
        static GXVoid GXCALL OnF10Up ( GXVoid* context );

        static GXVoid GXCALL OnF11Down ( GXVoid* context );
        static GXVoid GXCALL OnF11Up ( GXVoid* context );

        static GXVoid GXCALL OnF12Down ( GXVoid* context );
        static GXVoid GXCALL OnF12Up ( GXVoid* context );

        static GXVoid GXCALL OnZeroDown ( GXVoid* context );
        static GXVoid GXCALL OnZeroUp ( GXVoid* context );

        static GXVoid GXCALL OnOneDown ( GXVoid* context );
        static GXVoid GXCALL OnOneUp ( GXVoid* context );

        static GXVoid GXCALL OnTwoDown ( GXVoid* context );
        static GXVoid GXCALL OnTwoUp ( GXVoid* context );

        static GXVoid GXCALL OnThreeDown ( GXVoid* context );
        static GXVoid GXCALL OnThreeUp ( GXVoid* context );

        static GXVoid GXCALL OnFourDown ( GXVoid* context );
        static GXVoid GXCALL OnFourUp ( GXVoid* context );

        static GXVoid GXCALL OnFiveDown ( GXVoid* context );
        static GXVoid GXCALL OnFiveUp ( GXVoid* context );

        static GXVoid GXCALL OnSixDown ( GXVoid* context );
        static GXVoid GXCALL OnSixUp ( GXVoid* context );

        static GXVoid GXCALL OnSevenDown ( GXVoid* context );
        static GXVoid GXCALL OnSevenUp ( GXVoid* context );

        static GXVoid GXCALL OnEightDown ( GXVoid* context );
        static GXVoid GXCALL OnEightUp ( GXVoid* context );

        static GXVoid GXCALL OnNineDown ( GXVoid* context );
        static GXVoid GXCALL OnNineUp ( GXVoid* context );

        static GXVoid GXCALL OnMinusDown ( GXVoid* context );
        static GXVoid GXCALL OnMinusUp ( GXVoid* context );

        static GXVoid GXCALL OnEqualDown ( GXVoid* context );
        static GXVoid GXCALL OnEqualUp ( GXVoid* context );

        static GXVoid GXCALL OnLeftDown ( GXVoid* context );
        static GXVoid GXCALL OnLeftUp ( GXVoid* context );

        static GXVoid GXCALL OnRightDown ( GXVoid* context );
        static GXVoid GXCALL OnRightUp ( GXVoid* context );

        static GXVoid GXCALL OnUpDown ( GXVoid* context );
        static GXVoid GXCALL OnUpUp ( GXVoid* context );

        static GXVoid GXCALL OnDownDown ( GXVoid* context );
        static GXVoid GXCALL OnDownUp ( GXVoid* context );

        static GXVoid GXCALL OnDelDown ( GXVoid* context );
        static GXVoid GXCALL OnDelUp ( GXVoid* context );

        static GXVoid GXCALL OnInsDown ( GXVoid* context );
        static GXVoid GXCALL OnInsUp ( GXVoid* context );

        static GXVoid GXCALL OnHomeDown ( GXVoid* context );
        static GXVoid GXCALL OnHomeUp ( GXVoid* context );

        static GXVoid GXCALL OnEndDown ( GXVoid* context );
        static GXVoid GXCALL OnEndUp ( GXVoid* context );

        static GXVoid GXCALL OnPageUpDown ( GXVoid* context );
        static GXVoid GXCALL OnPageUpUp ( GXVoid* context );

        static GXVoid GXCALL OnPageDownDown ( GXVoid* context );
        static GXVoid GXCALL OnPageDownUp ( GXVoid* context );

        static GXVoid GXCALL OnScrollLockDown ( GXVoid* context );
        static GXVoid GXCALL OnScrollLockUp ( GXVoid* context );

        static GXVoid GXCALL OnPauseDown ( GXVoid* context );
        static GXVoid GXCALL OnPauseUp ( GXVoid* context );

        EMGame ( const EMGame &other ) = delete;
        EMGame& operator = ( const EMGame &other ) = delete;
};


#endif // EM_MAIN
