#include <GXEngine_Editor_Mobile/EMGame.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine_Editor_Mobile/EMUIMotionBlurSettings.h>
#include <GXEngine_Editor_Mobile/EMUISSAOSettings.h>
#include <GXEngine_Editor_Mobile/EMUIFPSCounter.h>
#include <GXEngine_Editor_Mobile/EMUIColorPicker.h>
#include <GXEngine_Editor_Mobile/EMEnvironment.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXCore.h>
#include <GXEngine/GXSplashScreen.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXPhysics/GXPhysicsEngine.h>


#define EM_WINDOW_NAME					L"GXEditor Mobile"
#define ENVIRONMENT_QUASI_DISTANCE		7.77f


EMGame::EMGame ()
{
	hudCamera = nullptr;

	openFile = nullptr;
	filePopup = nullptr;

	menu = nullptr;
	createPopup = nullptr;
	toolsPopup = nullptr;
	utilityPopup = nullptr;
	effectsPopup = nullptr;

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
	GXRenderer& coreRenderer = GXRenderer::GetInstance ();
	coreRenderer.SetWindowName ( EM_WINDOW_NAME );

	GXLocale& locale = GXLocale::GetInstance ();
	locale.LoadLanguage ( L"Locale/Editor Mobile/RU.lng", eGXLanguage::Russian );
	locale.LoadLanguage ( L"Locale/Editor Mobile/EN.lng", eGXLanguage::English );
	locale.SetLanguage ( eGXLanguage::Russian );

	GXInput& input = GXInput::GetInstance ();
	input.BindKeyCallback ( this, &EMGame::OnExit, VK_ESCAPE, eGXInputButtonState::Up );
	input.BindMouseButtonCallback ( this, &EMGame::OnMouseButton );

	uiInput = new GXUIInput ( nullptr, GX_FALSE );
	GXTouchSurface::GetInstance ().SetDefaultWidget ( uiInput );

	GXFloat w = (GXFloat)coreRenderer.GetWidth ();
	GXFloat h = (GXFloat)coreRenderer.GetHeight ();

	EMRenderer& editorRenderer = EMRenderer::GetInstance ();
	editorRenderer.SetOnObjectCallback ( &EMGame::OnObject );

	filePopup = new EMUIPopup ( nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->New" ), nullptr, nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->Open" ), nullptr, nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->Close" ), nullptr, nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->Save" ), nullptr, nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->Exit" ), this, &EMGame::OnExit );

	createPopup = new EMUIPopup ( nullptr );
	createPopup->AddItem ( locale.GetString ( L"Main menu->Create->Unit Actor" ), nullptr, nullptr );
	createPopup->AddItem ( locale.GetString ( L"Main menu->Create->Box falling" ), this, &EMGame::StartBoxFallingSimulation );
	createPopup->AddItem ( locale.GetString ( L"Main menu->Create->Skeletal mesh" ), nullptr, nullptr );
	createPopup->AddItem ( locale.GetString ( L"Main menu->Create->Directed light" ), nullptr, nullptr );
	createPopup->AddItem ( locale.GetString ( L"Main menu->Create->Spot" ), nullptr, nullptr );
	createPopup->AddItem ( locale.GetString ( L"Main menu->Create->Bulp" ), nullptr, nullptr );

	toolsPopup = new EMUIPopup ( nullptr );
	toolsPopup->AddItem ( locale.GetString ( L"Main menu->Tools->Select" ), nullptr, &EMGame::OnColorPicker );
	toolsPopup->AddItem ( locale.GetString ( L"Main menu->Tools->Move" ), nullptr, nullptr );
	toolsPopup->AddItem ( locale.GetString ( L"Main menu->Tools->Rotate" ), nullptr, nullptr );
	toolsPopup->AddItem ( locale.GetString ( L"Main menu->Tools->Scale" ), nullptr, nullptr );

	utilityPopup = new EMUIPopup ( nullptr );
	utilityPopup->AddItem ( locale.GetString ( L"Main menu->Utility->Particle system" ), nullptr, nullptr );
	utilityPopup->AddItem ( locale.GetString ( L"Main menu->Utility->Animation graph" ), nullptr, nullptr );

	effectsPopup = new EMUIPopup ( nullptr );
	effectsPopup->AddItem ( locale.GetString ( L"Main menu->Effects->Motion blur" ), this, &EMGame::OnShowMotionBlurSettings );
	effectsPopup->AddItem ( locale.GetString ( L"Main menu->Effects->SSAO" ), this, &EMGame::OnShowSSAOSettings );

	menu = new EMUIMenu ( nullptr );
	menu->SetLocation ( 0.0f, h - menu->GetHeight () );
	menu->AddItem ( locale.GetString ( L"Main menu->File" ), filePopup );
	menu->AddItem ( locale.GetString ( L"Main menu->Create" ), createPopup );
	menu->AddItem ( locale.GetString ( L"Main menu->Tools" ), toolsPopup );
	menu->AddItem ( locale.GetString ( L"Main menu->Utility" ), utilityPopup );
	menu->AddItem ( locale.GetString ( L"Main menu->Effects" ), effectsPopup );

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
	GXCamera::SetActiveCamera ( &viewer->GetCamera () );

	EMTool::SetActiveTool ( moveTool );

	physicsBoxActor = new EMPhysicsDrivenActor ( eGXShapeType::Sphere );
	GXRigidBody& body1 = physicsBoxActor->GetRigidBody ();
	body1.SetLocation ( 0.0f, 10.0f, 15.0f );

	physicsPlaneActor = new EMPhysicsDrivenActor ( eGXShapeType::Polygon );
	GXRigidBody& body2 = physicsPlaneActor->GetRigidBody ();
	body2.SetLocation ( 0.0f, -10.0f, 15.0f );

	GXWorld& world = GXPhysicsEngine::GetInstance ().GetWorld ();
	world.RegisterRigidBody ( physicsBoxActor->GetRigidBody () );
	world.RegisterRigidBody ( physicsPlaneActor->GetRigidBody () );

	fluttershy = new EMFluttershy ();

	environmentMap = new GXTextureCubeMap ();
	*environmentMap = GXTextureCubeMap::LoadEquirectangularTexture ( L"Textures/Editor Mobile/Default LDR environment map.jpg", GX_FALSE );

	lightProbeSourceTexture = new GXTextureCubeMap ();
	*lightProbeSourceTexture = GXTextureCubeMap::LoadEquirectangularTexture ( L"Textures/Editor Mobile/Default HDR environment map.hdr", GX_TRUE );

	lightProbe = new EMLightProbe ();
	lightProbe->SetEnvironmentMap ( *lightProbeSourceTexture );

	EMEnvironment& environment = EMEnvironment::GetInstance ();
	environment.SetEnvironmentMap ( *environmentMap );
	environment.SetEnvironmentQuasiDistance ( ENVIRONMENT_QUASI_DISTANCE );

	EMCookTorranceCommonPassMaterial& m = unitActor->GetMaterial ();
	m.SetAlbedoColor ( 253, 190, 2, 255 );
	m.SetRoughnessScale ( 0.2f );
	m.SetIndexOfRefractionScale ( 0.094f );
	m.SetSpecularIntencityScale ( 0.75f );
	m.SetMetallicScale ( 1.0f );

	EMUIFPSCounter::GetInstance ();
	EMUIColorPicker::GetInstance ();

	ShowCursor ( 1 );
	SetCursor ( LoadCursorW ( 0, IDC_ARROW ) );
}

GXVoid EMGame::OnResize ( GXInt width, GXInt height )
{
	GXLogW ( L"EMGame::OnResize::Info - Новый размер %i x %i\n", width, height );
}

GXVoid EMGame::OnFrame ( GXFloat deltaTime )
{
	GXTouchSurface::GetInstance ().ExecuteMessages ();

	GXPhysicsEngine::GetInstance ().RunSimulateLoop ( deltaTime );

	fluttershy->UpdatePose ( deltaTime );

	GXCamera& viewerCamera = EMViewer::GetInstance ()->GetCamera ();
	GXCamera::SetActiveCamera ( &viewerCamera );

	EMRenderer& renderer = EMRenderer::GetInstance ();
	renderer.StartCommonPass ();

	unitActor->OnDrawCommonPass ( deltaTime );
	physicsBoxActor->Draw ( deltaTime );
	physicsPlaneActor->Draw ( deltaTime );
	fluttershy->Render ( deltaTime );

	renderer.StartEnvironmentPass ();

	EMEnvironment::GetInstance ().Render ( deltaTime );

	renderer.StartLightPass ();

	renderer.ApplySSAO ();
	renderer.ApplyMotionBlur ( deltaTime );

	renderer.StartHudColorPass ();

	moveTool->OnDrawHudColorPass ();

	GXCamera::SetActiveCamera ( hudCamera );

	EMDrawUI ();

	EMUIFPSCounter::GetInstance ().Render ();

	renderer.StartHudMaskPass ();

	GXCamera::SetActiveCamera ( &viewerCamera );

	moveTool->OnDrawHudMaskPass ();

	renderer.PresentFrame ( eEMRenderTarget::Combine );

	viewerCamera.UpdateLastFrameMatrices ();

	GXRenderer& coreRenderer = GXRenderer::GetInstance ();

	if ( coreRenderer.IsVisible () ) return;

	coreRenderer.Show ();
	GXSplashScreen::GetInstance ().Hide ();
}

GXVoid EMGame::OnDestroy ()
{
	GXSplashScreen::GetInstance ().Show ();
	GXRenderer::GetInstance ().Hide ();

	GXInput& input = GXInput::GetInstance ();
	input.UnbindMouseMoveCallback ();
	input.UnbindMouseButtonCallback ();
	input.UnbindKeyCallback ( VK_ESCAPE, eGXInputButtonState::Up );

	moveTool->UnBind ();
	GXSafeDelete ( moveTool );

	GXSafeDelete ( physicsBoxActor );
	GXSafeDelete ( physicsPlaneActor );

	GXSafeDelete ( unitActor );
	GXSafeDelete ( directedLight );

	GXSafeDelete ( menu );
	GXSafeDelete ( openFile );
	GXSafeDelete ( filePopup );
	GXSafeDelete ( createPopup );
	GXSafeDelete ( toolsPopup );
	GXSafeDelete ( utilityPopup );
	GXSafeDelete ( effectsPopup );
	GXSafeDelete ( hudCamera );

	delete EMViewer::GetInstance ();

	GXSafeDelete ( fluttershy );
	GXSafeDelete ( lightProbe );

	delete &( EMEnvironment::GetInstance () );
	delete &( EMUIFPSCounter::GetInstance () );
	delete &( EMUIColorPicker::GetInstance () );

	GXTextureCubeMap::RemoveTexture ( *environmentMap );
	GXSafeDelete ( environmentMap );

	GXTextureCubeMap::RemoveTexture ( *lightProbeSourceTexture );
	GXSafeDelete ( lightProbeSourceTexture );

	GXTouchSurface::GetInstance ().SetDefaultWidget ( nullptr );
	GXSafeDelete ( uiInput );

	delete &( EMRenderer::GetInstance () );
	delete &( GXPhysicsEngine::GetInstance () );
}

GXVoid GXCALL EMGame::OnExit ( GXVoid* /*handler*/ )
{
	GXCore::GetInstance ().Exit ();
	GXLogW ( L"Завершение\n" );
}

GXVoid GXCALL EMGame::OnColorPicker ( GXVoid* /*handler*/ )
{
	EMUIColorPicker::GetInstance ().PickRGBAColor ( nullptr, &EMGame::OnPickRGBA, 115, 185, 0, 255 );
}

GXVoid GXCALL EMGame::OnPickRGBA ( GXVoid* /*handler*/, GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXLogW ( L"EMGame::OnPickRGBA::Info - Выбран цвет RGBA:\n\tКрасный - %hhu\n\tЗелёный - %hhu\n\tСиний - %hhu\n\tПрозрачность - %hhu\n", red, green, blue, alpha );
}

GXVoid GXCALL EMGame::OnShowMotionBlurSettings ( GXVoid* /*handler*/ )
{
	EMUIMotionBlurSettings::GetInstance ().Show ();
}

GXVoid GXCALL EMGame::OnShowSSAOSettings ( GXVoid* /*handler*/ )
{
	EMUISSAOSettings::GetInstance ().Show ();
}

GXVoid GXCALL EMGame::OnMouseButton ( GXVoid* /*handler*/, GXInputMouseFlags mouseflags )
{
	if ( !mouseflags.lmb ) return;

	const GXVec2& mouse = GXTouchSurface::GetInstance ().GetMousePosition ();
	EMRenderer::GetInstance ().GetObject ( (GXUShort)mouse.x, (GXUShort)mouse.y );
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
	EMEnvironment::GetInstance ().OnViewerLocationChanged ();
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
