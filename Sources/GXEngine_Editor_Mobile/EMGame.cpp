#include <GXEngine_Editor_Mobile/EMGame.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine_Editor_Mobile/EMUIMotionBlurSettings.h>
#include <GXEngine_Editor_Mobile/EMUISSAOSettings.h>
#include <GXEngine_Editor_Mobile/EMUIToneMapperSettings.h>
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

	menu = nullptr;

	openFile = nullptr;

	filePopup = nullptr;
	createPopup = nullptr;
	toolsPopup = nullptr;
	utilityPopup = nullptr;
	effectsPopup = nullptr;

	directedLight = nullptr;

	unitActor = nullptr;
	colliderOne = nullptr;
	colliderTwo = nullptr;
	sphere = nullptr;
	plane = nullptr;

	moveTool = nullptr;

	fluttershy = nullptr;

	environmentMap = nullptr;
	lightProbeSourceTexture = nullptr;

	lightProbe = nullptr;

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
	editorRenderer.SetOnObjectCallback ( this, &EMGame::OnObject );

	openFile = new EMUIOpenFile ();

	filePopup = new EMUIPopup ( nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->New" ), nullptr, nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->Open" ), nullptr, nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->Close" ), nullptr, nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->Save" ), nullptr, nullptr );
	filePopup->AddItem ( locale.GetString ( L"Main menu->File->Exit" ), this, &EMGame::OnExit );

	createPopup = new EMUIPopup ( nullptr );
	createPopup->AddItem ( locale.GetString ( L"Main menu->Create->Unit Actor" ), nullptr, nullptr );
	createPopup->AddItem ( locale.GetString ( L"Main menu->Create->Box falling" ), this, nullptr );
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
	effectsPopup->AddItem ( locale.GetString ( L"Main menu->Effects->HDR tone mapper" ), this, &EMGame::OnShowToneMapperSettings );

	menu = new EMUIMenu ( nullptr );
	menu->SetLocation ( 0.0f, h - menu->GetHeight () );
	menu->AddItem ( locale.GetString ( L"Main menu->File" ), filePopup );
	menu->AddItem ( locale.GetString ( L"Main menu->Create" ), createPopup );
	menu->AddItem ( locale.GetString ( L"Main menu->Tools" ), toolsPopup );
	menu->AddItem ( locale.GetString ( L"Main menu->Utility" ), utilityPopup );
	menu->AddItem ( locale.GetString ( L"Main menu->Effects" ), effectsPopup );

	GXFloat physicsInfoWidth = 10.0f * gx_ui_Scale;
	GXFloat physicsInfoHeight = 5.0f * gx_ui_Scale;
	physicsInfo = new GXHudSurface ( (GXUShort)physicsInfoWidth, (GXUShort)physicsInfoHeight );
	physicsInfoFont = GXFont::GetFont ( L"Fonts/trebuc.ttf", (GXUShort)( 0.4f * gx_ui_Scale ) );
	physicsInfo->SetLocation ( 0.5f * ( physicsInfoWidth - w ), 0.5f * ( physicsInfoHeight - h ), 7.0f );

	physicsInfoBackgroundTexture = GXTexture2D::LoadTexture ( L"Textures/System/Default_Diffuse.tga", GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );

	physicsContactNormalMesh = GXMeshGeometry::LoadFromStm ( L"3D Models/Editor Mobile/Move gismo Z axis.stm" );
	physicsContactNormalMaterial = new GXUnlitColorMaterial ();
	physicsContactNormalMaterial->SetColor ( 255, 0, 0, 255 );

	GXTransform transform;
	unitActor = new EMUnitActor ( L"Unit actor 01", transform );

	transform.SetLocation ( -3.0f, 0.0f, 0.0f );
	colliderOne = new EMPhysicsDrivenActor ( L"Collider One", transform );
	GXSphereShape* colliderOneShape = new GXSphereShape ( &( colliderOne->GetRigidBody () ), 0.5f );
	colliderOne->GetRigidBody ().SetShape ( *colliderOneShape );
	colliderOne->SetMesh ( L"3D Models/System/Unit Sphere.obj" );
	EMCookTorranceCommonPassMaterial& colliderOneMaterial = colliderOne->GetMaterial ();
	colliderOneMaterial.SetAlbedoColor ( 253, 180, 17, 255 );
	colliderOneMaterial.SetRoughnessScale ( 0.25f );
	colliderOneMaterial.SetIndexOfRefractionScale ( 0.094f );
	colliderOneMaterial.SetSpecularIntensityScale ( 0.998f );
	colliderOneMaterial.SetMetallicScale ( 1.0f );
	colliderOneMaterial.SetEmissionColorScale ( 0.0f );
	colliderOne->EnablePhysicsDebug ();

	transform.SetLocation ( 3.0f, 0.0f, 0.0f );
	colliderTwo = new EMPhysicsDrivenActor ( L"Collider Two", transform );
	GXBoxShape* colliderTwoShape = new GXBoxShape ( &( colliderTwo->GetRigidBody () ), 1.0f, 1.0f, 1.0f );
	colliderTwo->GetRigidBody ().SetShape ( *colliderTwoShape );
	colliderTwo->SetMesh ( L"3D Models/System/Unit Cube.stm" );
	EMCookTorranceCommonPassMaterial& colliderTwoMaterial = colliderTwo->GetMaterial ();
	colliderTwoMaterial.SetAlbedoColor ( 247, 244, 233, 255 );
	colliderTwoMaterial.SetRoughnessScale ( 0.19f );
	colliderTwoMaterial.SetIndexOfRefractionScale ( 0.1f );
	colliderTwoMaterial.SetSpecularIntensityScale ( 0.998f );
	colliderTwoMaterial.SetMetallicScale ( 1.0f );
	colliderTwoMaterial.SetEmissionColorScale ( 0.0f );
	colliderTwo->EnablePhysicsDebug ();

	transform.SetLocation ( 6.0f, 0.0f, -.0f );
	sphere = new EMMeshActor ( L"Collider Three", transform );
	sphere->SetMesh ( L"3D Models/System/Unit Sphere.obj" );
	EMCookTorranceCommonPassMaterial& sphereMaterial = sphere->GetMaterial ();
	sphereMaterial.SetAlbedoColor ( 115, 185, 0, 255 );
	sphereMaterial.SetRoughnessScale ( 0.5f );
	sphereMaterial.SetIndexOfRefractionScale ( 0.292f );
	sphereMaterial.SetSpecularIntensityScale ( 0.75f );
	sphereMaterial.SetMetallicScale ( 1.0f );
	sphereMaterial.SetEmissionColorScale ( 0.0f );

	transform.SetRotation ( GXDegToRad ( 30.0f ), GXDegToRad ( 30.0f ), 0.0f );
	directedLight = new EMDirectedLightActor ( L"Directed light 01", transform );

	moveTool = new EMMoveTool ();
	moveTool->Bind ();
	moveTool->SetLocalMode ();

	hudCamera = new GXCameraOrthographic ( w, h, EM_UI_HUD_CAMERA_NEAR_Z, EM_UI_HUD_CAMERA_FAR_Z );

	EMViewer* viewer = EMViewer::GetInstance ();
	viewer->SetOnViewerTransformChangedCallback ( this, &EMGame::OnViewerTransformChanged );
	viewer->SetInputWidget ( *uiInput );
	viewer->SetTarget ( unitActor );
	GXCamera::SetActiveCamera ( &viewer->GetCamera () );

	EMTool::SetActiveTool ( moveTool );

	fluttershy = new EMFluttershy ();

	environmentMap = new GXTextureCubeMap ();
	*environmentMap = GXTextureCubeMap::LoadEquirectangularTexture ( L"Textures/Editor Mobile/Default LDR environment map.jpg", GX_FALSE, GX_TRUE );
	//*environmentMap = GXTextureCubeMap::LoadEquirectangularTexture ( L"Textures/Editor Mobile/Default HDR environment map.hdr", GX_TRUE, GX_FALSE );

	lightProbeSourceTexture = new GXTextureCubeMap ();
	*lightProbeSourceTexture = GXTextureCubeMap::LoadEquirectangularTexture ( L"Textures/Editor Mobile/Default HDR environment map.hdr", GX_TRUE, GX_FALSE );

	lightProbe = new EMLightProbe ();
	lightProbe->SetEnvironmentMap ( *lightProbeSourceTexture );

	EMEnvironment& environment = EMEnvironment::GetInstance ();
	environment.SetEnvironmentMap ( *environmentMap );
	environment.SetEnvironmentQuasiDistance ( ENVIRONMENT_QUASI_DISTANCE );

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
	colliderOne->OnDrawCommonPass ( deltaTime );
	colliderTwo->OnDrawCommonPass ( deltaTime );
	sphere->OnDrawCommonPass ( deltaTime );
	fluttershy->Render ( deltaTime );

	renderer.StartEnvironmentPass ();

	EMEnvironment::GetInstance ().Render ( deltaTime );

	renderer.StartLightPass ();

	renderer.ApplySSAO ();
	renderer.ApplyMotionBlur ( deltaTime );
	renderer.ApplyToneMapping ( deltaTime );

	renderer.StartHudColorPass ();

	colliderOne->OnDrawHudColorPass ();
	colliderTwo->OnDrawHudColorPass ();
	moveTool->OnDrawHudColorPass ();

	const GXCollisionData& collisionData = GXPhysicsEngine::GetInstance ().GetWorld ().GetCollisionData ();
	if ( collisionData.GetTotalContacts () > 0 )
	{
		static const GXVec3 absoluteUp ( 0.0f, 1.0f, 0.0f );
		GXVec3 z = collisionData.GetAllContacts ()->GetNormal ();
		
		GXVec3 x;
		GXCrossVec3Vec3 ( x, absoluteUp, z );
		GXNormalizeVec3 ( x );

		GXVec3 y;
		GXCrossVec3Vec3 ( y, z, x );

		GXMat4 rot;
		GXSetMat4Identity ( rot );
		rot.SetX ( x );
		rot.SetY ( y );
		rot.SetZ ( z );

		GXTransform transform;
		transform.SetRotation ( rot );

		static const GXVec3 offset ( 1.0f, 0.0f, 3.0f );
		GXVec3 location;
		GXMulVec3Mat4AsPoint ( location, offset, viewerCamera.GetCurrentFrameModelMatrix () );
		transform.SetLocation ( location );

		physicsContactNormalMaterial->Bind ( transform );
		physicsContactNormalMesh.Render ();
		physicsContactNormalMaterial->Unbind ();
	}

	GXCamera::SetActiveCamera ( hudCamera );

	EMDrawUI ();

	physicsInfo->Reset ();

	GXImageInfo ii;
	GXColorToVec4 ( ii.color, 0, 0, 0, 180 );
	ii.texture = &physicsInfoBackgroundTexture;
	ii.insertX = 0.0f;
	ii.insertY = 0.0f;
	ii.insertWidth = (GXFloat)physicsInfo->GetWidth ();
	ii.insertHeight = (GXFloat)physicsInfo->GetHeight ();
	ii.overlayType = eGXImageOverlayType::SimpleReplace;

	physicsInfo->AddImage ( ii );

	GXPenInfo pi;
	GXColorToVec4 ( pi.color, 115, 185, 0, 255 );
	pi.font = &physicsInfoFont;
	pi.insertX = 0.5f * gx_ui_Scale;
	pi.insertY = 0.5f * gx_ui_Scale;
	pi.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

	physicsInfo->AddText ( pi, 64, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Contacts: %i" ), collisionData.GetTotalContacts () );

	if ( collisionData.GetTotalContacts () > 0 )
	{
		pi.insertY += (GXFloat)physicsInfoFont.GetSize ();
		physicsInfo->AddText ( pi, 64, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Penetration depth: %f" ), collisionData.GetAllContacts ()->GetPenetration () );
	}

	physicsInfo->Render ();

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
	moveTool->UnBind ();

	GXSplashScreen::GetInstance ().Show ();
	GXRenderer::GetInstance ().Hide ();

	GXInput& input = GXInput::GetInstance ();
	input.UnbindMouseMoveCallback ();
	input.UnbindMouseButtonCallback ();
	input.UnbindKeyCallback ( VK_ESCAPE, eGXInputButtonState::Up );

	GXTouchSurface::GetInstance ().SetDefaultWidget ( nullptr );
	GXSafeDelete ( uiInput );

	GXSafeDelete ( lightProbe );

	GXTextureCubeMap::RemoveTexture ( *lightProbeSourceTexture );
	GXSafeDelete ( lightProbeSourceTexture );

	GXTextureCubeMap::RemoveTexture ( *environmentMap );
	GXSafeDelete ( environmentMap );

	GXSafeDelete ( fluttershy );
	GXSafeDelete ( moveTool );

	GXSafeDelete ( plane );
	GXSafeDelete ( sphere );
	GXSafeDelete ( colliderOne );
	GXSafeDelete ( colliderTwo );
	GXSafeDelete ( unitActor );

	GXSafeDelete ( directedLight );

	GXSafeDelete ( physicsContactNormalMaterial );
	GXMeshGeometry::RemoveMeshGeometry ( physicsContactNormalMesh );
	GXTexture2D::RemoveTexture ( physicsInfoBackgroundTexture );
	GXFont::RemoveFont ( physicsInfoFont );
	GXSafeDelete ( physicsInfo );

	GXSafeDelete ( effectsPopup );
	GXSafeDelete ( utilityPopup );
	GXSafeDelete ( toolsPopup );
	GXSafeDelete ( createPopup );
	GXSafeDelete ( filePopup );
	GXSafeDelete ( openFile );
	GXSafeDelete ( menu );

	GXSafeDelete ( hudCamera );

	delete EMViewer::GetInstance ();
	delete &( EMEnvironment::GetInstance () );
	delete &( EMUIFPSCounter::GetInstance () );
	delete &( EMUIColorPicker::GetInstance () );

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

GXVoid GXCALL EMGame::OnShowToneMapperSettings ( GXVoid* /*handler*/ )
{
	EMUIToneMapperSettings::GetInstance ().Show ();
}

GXVoid GXCALL EMGame::OnMouseButton ( GXVoid* /*handler*/, GXInputMouseFlags mouseflags )
{
	if ( !mouseflags.lmb ) return;

	const GXVec2& mouse = GXTouchSurface::GetInstance ().GetMousePosition ();
	EMRenderer::GetInstance ().GetObject ( (GXUShort)mouse.x, (GXUShort)mouse.y );
}

GXVoid GXCALL EMGame::OnObject ( GXVoid* handler, GXVoid* object )
{
	EMGame* game = (EMGame*)handler;
	if ( game->moveTool->OnObject ( object ) ) return;

	EMActor* actor = (EMActor*)object;

	if ( actor )
		GXLogW ( L"EMOnObject::Info - Объект %s\n", actor->GetName () );

	game->moveTool->SetActor ( actor );
	EMViewer::GetInstance ()->SetTarget ( actor );
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
