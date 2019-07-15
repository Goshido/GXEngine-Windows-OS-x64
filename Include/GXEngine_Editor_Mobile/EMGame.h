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
#include <GXEngine/GXGame.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXUnlitColorMaterial.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXCameraOrthographic.h>
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
        static GXVoid GXCALL OnExit ( GXVoid* handler );
        static GXVoid GXCALL OnColorPicker ( GXVoid* handler );
        static GXVoid GXCALL OnPickRGBUByte ( GXVoid* handler, GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
        static GXVoid GXCALL OnShowMotionBlurSettings ( GXVoid* handler );
        static GXVoid GXCALL OnShowSSAOSettings ( GXVoid* handler );
        static GXVoid GXCALL OnShowToneMapperSettings ( GXVoid* handler );
        static GXVoid GXCALL OnMouseButton ( GXVoid* handler, GXInputMouseFlags mouseflags );
        static GXVoid GXCALL OnObject ( GXVoid* handler, GXVoid* object );
        static GXVoid GXCALL OnViewerTransformChanged ( GXVoid* handler );
        static GXVoid GXCALL OnOpenFile ( const GXWChar* filePath );

        EMGame ( const EMGame &other ) = delete;
        EMGame& operator = ( const EMGame &other ) = delete;
};


#endif // EM_MAIN
