#include <GXEngine_Editor_Mobile/EMGame.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXCore.h>
#include <GXPhysics/GXPhysicsEngine.h>


#define EM_WINDOW_NAME L"GXEditor Mobile"


EMGame::EMGame ()
{
	hudCamera = nullptr;

	button1 = nullptr;
	button2 = nullptr;
	button3 = nullptr;

	openFile = nullptr;
	draggableArea = nullptr;
	filePopup = nullptr;

	menu = nullptr;
	createPopup = nullptr;
	toolsPopup = nullptr;
	utilityPopup = nullptr;
	languagePopup = nullptr;

	editBox = nullptr;

	directedLight = nullptr;
	unitActor = nullptr;
	moveTool = nullptr;

	physicsBoxActor = nullptr;
	physicsPlaneActor = nullptr;

	fluttershy = nullptr;

	uiInput = nullptr;
}

EMGame::~EMGame ()
{
	//NOTHING
}

GXVoid EMGame::OnInit ()
{
	GXRenderer* renderer = GXRenderer::GetInstance ();
	renderer->SetWindowName ( EM_WINDOW_NAME );

	GXLocale* locale = GXLocale::GetInstance ();
	locale->LoadLanguage ( L"Locale/Editor Mobile/RU.lng", eGXLanguage::Russian );
	locale->LoadLanguage ( L"Locale/Editor Mobile/EN.lng", eGXLanguage::English );
	locale->SetLanguage ( eGXLanguage::Russian );

	GXInput* input = GXInput::GetInstance ();
	input->BindKeyCallback ( this, &EMGame::OnExit, VK_ESCAPE, eGXInputButtonState::Up );
	input->BindMouseButtonCallback ( this, &EMGame::OnMouseButton );

	uiInput = new GXUIInput ( nullptr, GX_FALSE );
	GXTouchSurface::GetInstance ()->SetDefaultWidget ( uiInput );

	GXFloat w = (GXFloat)renderer->GetWidth ();
	GXFloat h = (GXFloat)renderer->GetHeight ();

	EMRenderer* editorRenderer = EMRenderer::GetInstance ();
	editorRenderer->SetOnObjectCallback ( &EMGame::OnObject );

	locale->SetLanguage ( eGXLanguage::Russian );

	button1 = new EMUIButton ( nullptr );
	button1->SetOnLeftMouseButtonCallback ( this, &EMGame::OnButton );
	button1->Disable ();
	button1->Resize ( 100.0f, 100.0f, 4.0f * gx_ui_Scale, gx_ui_Scale );
	button1->SetCaption ( locale->GetString ( L"russian" ) );

	button2 = new EMUIButton ( nullptr );
	button2->SetOnLeftMouseButtonCallback ( this, &EMGame::OnButton );
	button2->Enable ();
	button2->Resize ( 100.0f + 4.2f * gx_ui_Scale, 100.0f, 4.0f * gx_ui_Scale, 0.5f * gx_ui_Scale );
	button2->SetCaption ( locale->GetString ( L"english" ) );

	filePopup = new EMUIPopup ( nullptr );
	filePopup->AddItem ( locale->GetString ( L"File->New" ), nullptr, nullptr );
	filePopup->AddItem ( locale->GetString ( L"File->Open" ), nullptr, nullptr );
	filePopup->AddItem ( locale->GetString ( L"File->Close" ), nullptr, nullptr );
	filePopup->AddItem ( locale->GetString ( L"File->Save" ), nullptr, nullptr );
	filePopup->AddItem ( locale->GetString ( L"File->Exit" ), this, &EMGame::OnExit );

	createPopup = new EMUIPopup ( nullptr );
	createPopup->AddItem ( locale->GetString ( L"Create->Unit Actor" ), nullptr, nullptr );
	createPopup->AddItem ( locale->GetString ( L"Create->Box falling" ), this, &EMGame::StartBoxFallingSimulation );
	createPopup->AddItem ( locale->GetString ( L"Create->Skeletal mesh" ), nullptr, nullptr );
	createPopup->AddItem ( locale->GetString ( L"Create->Directed light" ), nullptr, nullptr );
	createPopup->AddItem ( locale->GetString ( L"Create->Spot" ), nullptr, nullptr );
	createPopup->AddItem ( locale->GetString ( L"Create->Bulp" ), nullptr, nullptr );

	toolsPopup = new EMUIPopup ( nullptr );
	toolsPopup->AddItem ( locale->GetString ( L"Tools->Select" ), nullptr, nullptr );
	toolsPopup->AddItem ( locale->GetString ( L"Tools->Move" ), nullptr, nullptr );
	toolsPopup->AddItem ( locale->GetString ( L"Tools->Rotate" ), nullptr, nullptr );
	toolsPopup->AddItem ( locale->GetString ( L"Tools->Scale" ), nullptr, nullptr );

	utilityPopup = new EMUIPopup ( nullptr );
	utilityPopup->AddItem ( locale->GetString ( L"Utility->Particle system" ), nullptr, nullptr );
	utilityPopup->AddItem ( locale->GetString ( L"Utility->Animation graph" ), nullptr, nullptr );

	languagePopup = new EMUIPopup ( nullptr );
	languagePopup->AddItem ( locale->GetString ( L"Language->Русский" ), nullptr, nullptr );
	languagePopup->AddItem ( locale->GetString ( L"Language->English" ), nullptr, nullptr );

	menu = new EMUIMenu ( nullptr );
	menu->SetLocation ( 0.0f, h - 1.0f - menu->GetHeight () );
	menu->AddItem ( locale->GetString ( L"Main menu->File" ), filePopup );
	menu->AddItem ( locale->GetString ( L"Main menu->Create" ), createPopup );
	menu->AddItem ( locale->GetString ( L"Main menu->Tools" ), toolsPopup );
	menu->AddItem ( locale->GetString ( L"Main menu->Utility" ), utilityPopup );
	menu->AddItem ( locale->GetString ( L"Main menu->Language" ), languagePopup );

	draggableArea = new EMUIDraggableArea ( nullptr );
	draggableArea->SetHeaderHeight ( gx_ui_Scale * 2.0f );
	draggableArea->Resize ( 10.0f * gx_ui_Scale, 10.0f * gx_ui_Scale, 5.0f * gx_ui_Scale, 5.0f * gx_ui_Scale );

	button3 = new EMUIButton ( draggableArea );
	button3->Enable ();
	button3->Resize ( 0.2f * gx_ui_Scale, 0.2f * gx_ui_Scale, 3.0f * gx_ui_Scale, 0.8f * gx_ui_Scale );
	button3->SetOnLeftMouseButtonCallback ( this, &EMGame::OnButton );
	button3->SetCaption ( locale->GetString ( L"TestButton" ) );

	editBox = new EMUIEditBox ( draggableArea );
	editBox->SetText ( L"Привет, Сиськи" );
	editBox->Resize ( 10.0f, 60.0f, 4.5f * gx_ui_Scale, 0.6f * gx_ui_Scale );

	openFile = new EMUIOpenFile ();

	GXMat4 transfrom;
	GXSetMat4Identity ( transfrom );
	unitActor = new EMUnitActor ( L"Unit actor 01", transfrom );

	GXSetMat4RotationXY ( transfrom, GXDegToRad ( 30.0f ), GXDegToRad ( 30.0f ) );
	directedLight = new EMDirectedLightActor ( L"Directed light 01", transfrom );

	moveTool = new EMMoveTool ();
	moveTool->Bind ();
	moveTool->SetActor ( unitActor );
	moveTool->SetLocalMode ();

	hudCamera = new GXCameraOrthographic ( w, h, EM_UI_HUD_CAMERA_NEAR_Z, EM_UI_HUD_CAMERA_FAR_Z );

	EMViewer* viewer = EMViewer::GetInstance ();
	viewer->SetOnViewerTransformChangedCallback ( this, &EMGame::OnViewerTransformChanged );
	viewer->SetInputWidget ( *uiInput );
	viewer->SetTarget ( unitActor );

	EMTool::SetActiveTool ( moveTool );

	physicsBoxActor = new EMPhysicsDrivenActor ( eGXShapeType::Sphere );
	GXRigidBody& body1 = physicsBoxActor->GetRigidBody ();
	body1.SetLocation ( 0.0f, 10.0f, 15.0f );

	physicsPlaneActor = new EMPhysicsDrivenActor ( eGXShapeType::Polygon );
	GXRigidBody& body2 = physicsPlaneActor->GetRigidBody ();
	body2.SetLocation ( 0.0f, -10.0f, 15.0f );

	GXWorld& world = GXPhysicsEngine::GetInstance ()->GetWorld ();
	world.RegisterRigidBody ( physicsBoxActor->GetRigidBody () );
	world.RegisterRigidBody ( physicsPlaneActor->GetRigidBody () );

	fluttershy = new EMFluttershy ();

	ShowCursor ( 1 );
}

GXVoid EMGame::OnResize ( GXInt width, GXInt height )
{
	GXLogW ( L"EMGame::OnResize::Info - Новый размер %i x %i\n", width, height );
}

GXVoid EMGame::OnFrame ( GXFloat deltaTime )
{
	GXTouchSurface::GetInstance ()->ExecuteMessages ();

	GXPhysicsEngine::GetInstance ()->RunSimulateLoop ( deltaTime );

	//fluttershy->UpdatePose ( deltaTime );

	GXCamera& viewerCamera = EMViewer::GetInstance ()->GetCamera ();
	GXCamera::SetActiveCamera ( &viewerCamera );

	EMRenderer* renderer = EMRenderer::GetInstance ();
	renderer->StartCommonPass ();

	unitActor->OnDrawCommonPass ( deltaTime );
	physicsBoxActor->Draw ( deltaTime );
	physicsPlaneActor->Draw ( deltaTime );
	fluttershy->UpdatePose ( deltaTime );
	fluttershy->Render ( deltaTime );

	renderer->StartLightPass ();

	renderer->ApplyMotionBlur ( deltaTime );

	renderer->StartHudColorPass ();

	moveTool->OnDrawHudColorPass ();

	GXCamera::SetActiveCamera ( hudCamera );

	EMDrawUI ();

	renderer->StartHudMaskPass ();

	GXCamera::SetActiveCamera ( &viewerCamera );

	moveTool->OnDrawHudMaskPass ();

	renderer->PresentFrame ( eEMRenderTarget::Combine );

	viewerCamera.UpdateLastFrameViewMatrix ();
}

GXVoid EMGame::OnDestroy ()
{
	GXInput* input = GXInput::GetInstance ();
	input->UnbindMouseMoveCallback ();
	input->UnbindMouseButtonCallback ();
	input->UnbindKeyCallback ( VK_ESCAPE, eGXInputButtonState::Up );

	moveTool->UnBind ();
	GXSafeDelete ( moveTool );

	GXSafeDelete ( physicsBoxActor );
	GXSafeDelete ( physicsPlaneActor );

	GXSafeDelete ( unitActor );
	GXSafeDelete ( directedLight );

	GXSafeDelete ( menu );
	GXSafeDelete ( button1 );
	GXSafeDelete ( button2 );
	GXSafeDelete ( button3 );
	GXSafeDelete ( editBox );
	GXSafeDelete ( draggableArea );
	GXSafeDelete ( openFile );
	GXSafeDelete ( filePopup );
	GXSafeDelete ( createPopup );
	GXSafeDelete ( toolsPopup );
	GXSafeDelete ( utilityPopup );
	GXSafeDelete ( languagePopup );
	GXSafeDelete ( hudCamera );

	delete EMViewer::GetInstance ();

	GXSafeDelete ( fluttershy );

	GXTouchSurface::GetInstance ()->SetDefaultWidget ( nullptr );
	GXSafeDelete ( uiInput );

	delete EMRenderer::GetInstance ();
	delete GXPhysicsEngine::GetInstance ();
}

GXVoid GXCALL EMGame::OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	EMGame* game = (EMGame*)handler;

	if ( state == eGXMouseButtonState::Down ) return;

	if ( button == game->button1->GetWidget () )
	{
		GXLocale* locale = GXLocale::GetInstance ();
		locale->SetLanguage ( eGXLanguage::Russian );

		game->button1->SetCaption ( locale->GetString ( L"russian" ) );
		game->button2->SetCaption ( locale->GetString ( L"english" ) );

		game->button1->Disable ();
		game->button2->Enable ();
	}
	else if ( button == game->button2->GetWidget () )
	{
		GXLocale* locale = GXLocale::GetInstance ();
		locale->SetLanguage ( eGXLanguage::English );

		game->button1->SetCaption ( locale->GetString ( L"russian" ) );
		game->button2->SetCaption ( locale->GetString ( L"english" ) );

		game->button2->Disable ();
		game->button1->Enable ();
	}
	else if ( button == game->button3->GetWidget () )
	{
		game->openFile->Browse ( &EMGame::OnOpenFile );
	}
}

GXVoid GXCALL EMGame::OnExit ( GXVoid* /*handler*/ )
{
	GXCore::GetInstance ()->Exit ();
	GXLogA ( "Завершение\n" );
}

GXVoid GXCALL EMGame::OnMouseButton ( GXVoid* /*handler*/, GXInputMouseFlags mouseflags )
{
	if ( !mouseflags.lmb ) return;

	const GXVec2& mouse = GXTouchSurface::GetInstance ()->GetMousePosition ();
	EMRenderer::GetInstance ()->GetObject ( (GXUShort)mouse.x, (GXUShort)mouse.y );
}

GXVoid GXCALL EMGame::OnObject ( GXUPointer object )
{
	if ( !object ) return;

	EMActor* actor = (EMActor*)object;
	GXLogW ( L"EMOnObject::Info - Объект%s (адрес 0x%016X)\n", actor->GetName (), object );
}

GXVoid GXCALL EMGame::OnViewerTransformChanged ( GXVoid* handler )
{
	EMGame* game = (EMGame*)handler;
	game->moveTool->OnViewerTransformChanged ();
}

GXVoid GXCALL EMGame::OnOpenFile ( const GXWChar* filePath )
{
	GXLogW ( L"EMOnOpenFile::Info - Файл %s\n", filePath );
}

GXVoid GXCALL EMGame::StartBoxFallingSimulation ( GXVoid* handler )
{
	GXLogW ( L"EMStartBoxFallingSimulation::Info - Старт\n" );

	EMGame* game = (EMGame*)handler;

	GXRigidBody& body = game->physicsBoxActor->GetRigidBody ();
	body.SetLocation ( 0.0f, 10.0f, 15.0f );
	GXQuat rot;
	GXSetQuatFromAxisAngle ( rot, GXCreateVec3 ( 0.0f, 0.0f, 1.0f ), 0.0f );
	body.SetRotaton ( rot );
	body.SetLinearVelocity ( GXCreateVec3 ( 0.0f, 0.0f, 0.0f ) );
	body.SetAngularVelocity ( GXCreateVec3 ( 0.0f, 0.0f, 10.0f ) );
	body.ClearAccumulators ();
	body.SetAwake ();
}
