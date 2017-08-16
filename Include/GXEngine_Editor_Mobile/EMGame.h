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
#include <GXEngine/GXCameraOrthographic.h>
#include <GXPhysics/GXSphereShape.h>


class EMGame : public GXGame
{
	private:
		GXCameraOrthographic*	hudCamera;

		EMUIOpenFile*			openFile;

		EMUIMenu*				menu;
		EMUIPopup*				filePopup;
		EMUIPopup*				createPopup;
		EMUIPopup*				toolsPopup;
		EMUIPopup*				utilityPopup;
		EMUIPopup*				effectsPopup;

		GXHudSurface*			physicsInfo;
		GXFont					physicsInfoFont;
		GXTexture2D				physicsInfoBackgroundTexture;

		EMDirectedLightActor*	directedLight;

		EMUnitActor*			unitActor;
		EMPhysicsDrivenActor*	colliderOne;
		EMPhysicsDrivenActor*	colliderTwo;
		EMMeshActor*			sphere;
		EMMeshActor*			plane;

		EMMoveTool*				moveTool;

		EMFluttershy*			fluttershy;

		GXTextureCubeMap*		environmentMap;
		GXTextureCubeMap*		lightProbeSourceTexture;

		EMLightProbe*			lightProbe;

		GXUIInput*				uiInput;

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
		static GXVoid GXCALL OnPickRGBA ( GXVoid* handler, GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		static GXVoid GXCALL OnShowMotionBlurSettings ( GXVoid* handler );
		static GXVoid GXCALL OnShowSSAOSettings ( GXVoid* handler );
		static GXVoid GXCALL OnShowToneMapperSettings ( GXVoid* handler );
		static GXVoid GXCALL OnMouseButton ( GXVoid* handler, GXInputMouseFlags mouseflags );
		static GXVoid GXCALL OnObject ( GXVoid* handler, GXVoid* object );
		static GXVoid GXCALL OnViewerTransformChanged ( GXVoid* handler );
		static GXVoid GXCALL OnOpenFile ( const GXWChar* filePath );
};


#endif //EM_MAIN
