#include <GXEngine_Editor_Mobile/EMMain.h>
#include <GXEngine_Editor_Mobile/EMUIButton.h>
#include <GXEngine_Editor_Mobile/EMUIMenu.h>
#include <GXEngine_Editor_Mobile/EMUIDraggableArea.h>
#include <GXEngine_Editor_Mobile/EMUIOpenFile.h>
#include <GXEngine_Editor_Mobile/EMUIPopup.h>
#include <GXEngine_Editor_Mobile/EMUIEditBox.h>
#include <GXEngine_Editor_Mobile/EMDirectedLightActor.h>
#include <GXEngine_Editor_Mobile/EMUnitActor.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMMoveTool.h>
#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine/GXCore.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXEngine/GXInput.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXCameraOrthographic.h>


#define EM_WINDOW_NAME L"GXEditor Mobile"


GXCameraOrthographic*	em_HudCamera = nullptr;
EMUIButton*				em_Button1 = nullptr;
EMUIButton*				em_Button2 = nullptr;
EMUIButton*				em_Button3 = nullptr;
EMUIMenu*				em_Menu = nullptr;
EMUIOpenFile*			em_OpenFile = nullptr;
EMUIDraggableArea*		em_DraggableArea = nullptr;
EMUIPopup*				em_Popup = nullptr;
EMUIEditBox*			em_EditBox = nullptr;
EMDirectedLightActor*	em_DirectedLight = nullptr;
EMUnitActor*			em_UnitActor = nullptr;
EMMoveTool*				em_MoveTool = nullptr;
GXInt					em_MouseX = 0;
GXInt					em_MouseY = 0;


GXVoid GXCALL EMOnOpenFile ( const GXWChar* filePath );
GXVoid GXCALL EMOnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state == GX_MOUSE_BUTTON_DOWN ) return;

	if ( button == em_Button1->GetWidget () )
	{
		GXLocale* locale = GXLocale::GetInstance ();
		locale->SetLanguage ( GX_LANGUAGE_RU );

		em_Button1->SetCaption ( locale->GetString ( L"russian" ) );
		em_Button2->SetCaption ( locale->GetString ( L"english" ) );

		em_Button1->Disable ();
		em_Button2->Enable ();

		em_Popup->Show ();
	}
	else if ( button == em_Button2->GetWidget () )
	{
		GXLocale* locale = GXLocale::GetInstance ();
		locale->SetLanguage ( GX_LANGUAGE_EN );

		em_Button1->SetCaption ( locale->GetString ( L"russian" ) );
		em_Button2->SetCaption ( locale->GetString ( L"english" ) );

		em_Button2->Disable ();
		em_Button1->Enable ();
	}
	else if ( button == em_Button3->GetWidget () )
	{
		em_OpenFile->Browse ( &EMOnOpenFile );
	}
}

GXVoid GXCALL EMOnSave ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state == GX_MOUSE_BUTTON_UP )
		GXLogW ( L"EMOnSave::Info - Just worked\n" );
}

GXVoid GXCALL EMOnExit ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state != GX_MOUSE_BUTTON_UP ) return;

	GXCore::GetInstance ()->Exit ();
	GXLogA ( "Завершение\n" );
}

GXVoid GXCALL EMOnDummy ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	//NOTHING
}


GXVoid GXCALL EMOnMoveTool ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	//TODO
}

//-----------------------------------------------------------------------------

GXVoid GXCALL EMOnMouseMove ( GXInt win_x, GXInt win_y )
{
	em_MouseX = win_x;
	em_MouseY = GXRenderer::GetInstance ()->GetHeight () - win_y;
}

GXVoid GXCALL EMOnMouseButton ( EGXInputMouseFlags mouseflags )
{
	if ( mouseflags.lmb )
		EMRenderer::GetInstance ()->GetObject ( em_MouseX, em_MouseY );
}

GXVoid GXCALL EMOnObject ( GXUPointer object )
{
	if ( !object ) return;

	EMActor* actor = (EMActor*)object;
	GXLogW ( L"EMOnObject::Info - Объект %s (адрес 0x%016X)\n", actor->GetName (), object );
}

//-----------------------------------------------------------------------------

GXVoid GXCALL EMOnViewerTransformChanged ()
{
	em_MoveTool->OnViewerTransformChanged ();
}

//-----------------------------------------------------------------------------

GXVoid GXCALL EMOnOpenFile ( const GXWChar* filePath )
{
	GXLogW ( L"EMOnOpenFile::Info - Файл: %s\n", filePath );
}

//-----------------------------------------------------------------------------

GXVoid GXCALL EMOnPopupItem ()
{
	GXLogW ( L"EMOnPopupItem::Info - Атата\n" );
}

//-----------------------------------------------------------------------------

GXBool GXCALL EMOnFrame ( GXFloat deltatime )
{
	GXTouchSurface::GetInstance ()->ExecuteMessages ();

	GXCamera* viewerCamera = EMViewer::GetInstance ()->GetCamera ();
	GXCamera::SetActiveCamera ( viewerCamera );

	EMRenderer* renderer = EMRenderer::GetInstance ();
	renderer->StartCommonPass ();
	
	em_UnitActor->OnDrawCommonPass ();
	
	renderer->StartLightPass ();
	
	renderer->StartHudColorPass ();
	em_MoveTool->OnDrawHudColorPass ();

	GXCamera::SetActiveCamera ( em_HudCamera );
	
	EMDrawUI ();

	renderer->StartHudMaskPass ();
	EMDrawUIMasks ();

	GXCamera::SetActiveCamera ( viewerCamera );

	em_MoveTool->OnDrawHudMaskPass ();

	renderer->PresentFrame ();

	return GX_TRUE;
}

GXVoid GXCALL EMOnInitRenderableObjects ()
{
	GXRenderer* engineRenderer = GXRenderer::GetInstance ();
	GXFloat w = (GXFloat)engineRenderer->GetWidth ();
	GXFloat h = (GXFloat)engineRenderer->GetHeight ();

	EMRenderer* editorRenderer = EMRenderer::GetInstance ();
	editorRenderer->SetOnObjectCallback ( &EMOnObject );

	GXLocale* locale = GXLocale::GetInstance ();
	locale->SetLanguage ( GX_LANGUAGE_EN );

	em_Button1 = new EMUIButton ( nullptr );
	em_Button1->SetOnLeftMouseButtonCallback ( 0, &EMOnButton );
	em_Button1->Disable ();
	em_Button1->Resize ( 100.0f, 100.0f, 4.0f * gx_ui_Scale, gx_ui_Scale );
	em_Button1->SetLayer ( EMGetNextGUIForegroundZ () );
	em_Button1->SetCaption ( locale->GetString ( L"russian" ) );

	em_Button2 = new EMUIButton ( nullptr );
	em_Button2->SetOnLeftMouseButtonCallback ( 0, &EMOnButton );
	em_Button2->Enable ();
	em_Button2->Resize ( 100.0f + 4.2f * gx_ui_Scale, 100.0f, 4.0f * gx_ui_Scale, 0.5f * gx_ui_Scale );
	em_Button2->SetLayer ( EMGetNextGUIForegroundZ () );
	em_Button2->SetCaption ( locale->GetString ( L"english" ) );
	
	em_Menu = new EMUIMenu ();
	em_Menu->AddItem ( locale->GetString ( L"Main menu->File" ) );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->New" ), locale->GetString ( L"File->New (Hotkey)" ), &EMOnSave );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->Open" ), locale->GetString ( L"File->Open (Hotkey)" ), &EMOnExit );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->Close" ), locale->GetString ( L"File->Close (Hotkey)" ), &EMOnExit );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->Save" ), locale->GetString ( L"File->Save (Hotkey)" ), &EMOnSave );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->Exit" ), locale->GetString ( L"File->Exit (Hotkey)" ), &EMOnExit );

	em_Menu->AddItem ( locale->GetString ( L"Main menu->Create" ) );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Unit Actor" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Static mesh" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Skeletal mesh" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Directed light" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Spot" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Bulp" ), 0, &EMOnDummy );

	em_Menu->AddItem ( locale->GetString ( L"Main menu->Tools" ) );
	em_Menu->AddSubitem ( 2, locale->GetString ( L"Tools->Select" ), locale->GetString ( L"Tools->Select (Hotkey)" ), &EMOnDummy );
	em_Menu->AddSubitem ( 2, locale->GetString ( L"Tools->Move" ), locale->GetString ( L"Tools->Move (Hotkey)" ), &EMOnMoveTool );
	em_Menu->AddSubitem ( 2, locale->GetString ( L"Tools->Rotate" ), locale->GetString ( L"Tools->Rotate (Hotkey)" ), &EMOnDummy );
	em_Menu->AddSubitem ( 2, locale->GetString ( L"Tools->Scale" ), locale->GetString ( L"Tools->Scale (Hotkey)" ), &EMOnDummy );

	em_Menu->AddItem ( locale->GetString ( L"Main menu->Utility" ) );
	em_Menu->AddSubitem ( 3, locale->GetString ( L"Utility->Particle system" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 3, locale->GetString ( L"Utility->Animation graph" ), 0, &EMOnDummy );

	em_Menu->AddItem ( locale->GetString ( L"Main menu->Language" ) );
	em_Menu->AddSubitem ( 4, locale->GetString ( L"Language->Русский" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 4, locale->GetString ( L"Language->English" ), 0, &EMOnDummy );

	em_Menu->SetLayer ( EMGetNextGUIForegroundZ () );
	em_Menu->SetLocation ( 0.0f, h - gx_ui_Scale * 0.5f );
	
	em_DraggableArea = new EMUIDraggableArea ( nullptr );
	em_DraggableArea->SetHeaderHeight ( gx_ui_Scale * 2.0f );
	em_DraggableArea->Resize ( 10.0f * gx_ui_Scale, 10.0f * gx_ui_Scale, 5.0f * gx_ui_Scale, 5.0f * gx_ui_Scale );

	em_Button3 = new EMUIButton ( em_DraggableArea );
	em_Button3->Enable ();
	em_Button3->Resize ( 0.2f * gx_ui_Scale, 0.2f * gx_ui_Scale, 3.0f * gx_ui_Scale, 0.8f * gx_ui_Scale );
	em_Button3->SetOnLeftMouseButtonCallback ( 0, &EMOnButton );
	em_Button3->SetLayer ( EMGetNextGUIForegroundZ () );
	em_Button3->SetCaption ( locale->GetString ( L"TestButton" ) );

	em_EditBox = new EMUIEditBox ( em_DraggableArea );
	em_EditBox->SetText ( L"Привет Сиськи" );
	em_EditBox->Resize ( 10.0f, 60.0f, 4.5f * gx_ui_Scale, 0.6f * gx_ui_Scale );

	em_OpenFile = new EMUIOpenFile ();

	em_Popup = new EMUIPopup ( nullptr );
	em_Popup->AddItem ( L"New", nullptr );
	em_Popup->AddItem ( L"Open", nullptr );
	em_Popup->AddItem ( L"Close", nullptr );
	em_Popup->AddItem ( L"Save", nullptr );
	em_Popup->AddItem ( L"Save as", &EMOnPopupItem );
	em_Popup->AddItem ( L"Exit", nullptr );
	em_Popup->SetLocation ( 20.0f, 500.0f );

	GXMat4 transfrom;
	GXSetMat4Identity ( transfrom );
	em_UnitActor = new EMUnitActor ( L"Unit actor 01", transfrom );

	GXSetMat4RotationXY ( transfrom, GXDegToRad ( 30.0f ), GXDegToRad ( 30.0f ) );
	em_DirectedLight = new EMDirectedLightActor ( L"Directed light 01", transfrom );

	em_MoveTool = new EMMoveTool ();
	em_MoveTool->Bind ();
	em_MoveTool->SetActor ( em_UnitActor );
	em_MoveTool->SetLocalMode ();

	em_HudCamera = new GXCameraOrthographic ( w, h, EM_UI_HUD_CAMERA_NEAR_Z, EM_UI_HUD_CAMERA_FAR_Z );
	
	EMViewer* viewer = EMViewer::GetInstance ();
	viewer->SetOnViewerTransformChangedCallback ( &EMOnViewerTransformChanged );
	viewer->SetTarget ( em_UnitActor );
	viewer->CaptureInput ();

	EMTool::SetActiveTool ( em_MoveTool );

	ShowCursor ( 1 );
}

GXVoid GXCALL EMOnDeleteRenderableObjects ()
{
	GXSafeDelete ( em_UnitActor );
	GXSafeDelete ( em_DirectedLight );

	GXSafeDelete ( em_Menu );
	GXSafeDelete ( em_Button1 );
	GXSafeDelete ( em_Button2 );
	GXSafeDelete ( em_Button3 );
	GXSafeDelete ( em_EditBox );
	GXSafeDelete ( em_DraggableArea );
	GXSafeDelete ( em_OpenFile );
	GXSafeDelete ( em_Popup );
	GXSafeDelete ( em_HudCamera );

	em_MoveTool->UnBind ();
	em_MoveTool->Delete ();
	em_MoveTool->OnDrawHudColorPass ();

	delete EMViewer::GetInstance ();
	delete EMRenderer::GetInstance ();
}

//-------------------------------------------------------------------

GXVoid GXCALL EMExit ( GXVoid* handler )
{
	GXCore::GetInstance ()->Exit ();
	GXLogW ( L"Завершение\n" );
}

//-------------------------------------------------------------------

GXVoid GXCALL EMOnInit ()
{
	GXRenderer* renderer = GXRenderer::GetInstance ();
	renderer->SetOnInitRenderableObjectsFunc ( &EMOnInitRenderableObjects );
	renderer->SetOnFrameFunc ( &EMOnFrame );
	renderer->SetOnDeleteRenderableObjectsFunc ( &EMOnDeleteRenderableObjects );
	renderer->SetWindowName ( EM_WINDOW_NAME );

	GXLocale* locale = GXLocale::GetInstance ();
	locale->LoadLanguage ( L"Locale/Editor Mobile/RU.lng", GX_LANGUAGE_RU );
	locale->LoadLanguage ( L"Locale/Editor Mobile/EN.lng", GX_LANGUAGE_EN );
	locale->SetLanguage ( GX_LANGUAGE_RU );

	GXInput* input = GXInput::GetInstance ();
	input->BindKeyFunc ( &EMExit, 0, VK_ESCAPE, INPUT_UP );
	input->BindMouseMoveFunc ( &EMOnMouseMove );
	input->BindMouseButtonsFunc ( &EMOnMouseButton );
}

GXVoid GXCALL EMOnClose ()
{
	GXInput* input = GXInput::GetInstance ();
	input->UnBindMouseMoveFunc ();
	input->UnBindMouseButtonsFunc ();
	input->UnBindKeyFunc ( VK_ESCAPE, INPUT_UP );
}
