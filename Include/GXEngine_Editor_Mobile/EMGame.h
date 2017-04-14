#ifndef EM_MAIN
#define EM_MAIN


#include "EMUIButton.h"
#include "EMUIMenu.h"
#include "EMUIOpenFile.h"
#include "EMUIPopup.h"
#include "EMUIEditBox.h"
#include "EMDirectedLightActor.h"
#include "EMUnitActor.h"
#include "EMMoveTool.h"
#include "EMPhysicsDrivenActor.h"
#include "EMFluttershy.h"
#include <GXEngine/GXGame.h>
#include <GXEngine/GXCameraOrthographic.h>


class EMGame : public GXGame
{
	private:
		GXCameraOrthographic*	hudCamera;
		EMUIButton*				button1;
		EMUIButton*				button2;
		EMUIButton*				button3;
		EMUIMenu*				menu;
		EMUIOpenFile*			openFile;
		EMUIDraggableArea*		draggableArea;
		EMUIPopup*				filePopup;
		EMUIPopup*				createPopup;
		EMUIPopup*				toolsPopup;
		EMUIPopup*				utilityPopup;
		EMUIPopup*				languagePopup;
		EMUIEditBox*			editBox;
		EMDirectedLightActor*	directedLight;
		EMUnitActor*			unitActor;
		EMMoveTool*				moveTool;
		EMPhysicsDrivenActor*	physicsBoxActor;
		EMPhysicsDrivenActor*	physicsPlaneActor;
		EMFluttershy*			fluttershy;
		GXUIInput*				uiInput;

	public:
		EMGame ();
		~EMGame () override;

		GXVoid OnInit () override;
		GXVoid OnResize ( GXInt width, GXInt height ) override;
		GXVoid OnFrame ( GXFloat deltaTime ) override;
		GXVoid OnDestroy () override;

	private:
		static GXVoid GXCALL OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state );
		static GXVoid GXCALL OnExit ( GXVoid* handler );
		static GXVoid GXCALL OnMouseButton ( GXVoid* handler, GXInputMouseFlags mouseflags );
		static GXVoid GXCALL OnObject ( GXUPointer object );
		static GXVoid GXCALL OnViewerTransformChanged ( GXVoid* handler );
		static GXVoid GXCALL OnOpenFile ( const GXWChar* filePath );
		static GXVoid GXCALL StartBoxFallingSimulation ( GXVoid* handler );
};


#endif //EM_MAIN
