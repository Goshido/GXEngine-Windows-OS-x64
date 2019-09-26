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

        EMGame ( const EMGame &other ) = delete;
        EMGame& operator = ( const EMGame &other ) = delete;
};


#endif // EM_MAIN
