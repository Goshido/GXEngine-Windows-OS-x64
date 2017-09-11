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


EMGame::EMGame ():
gravity ( GXVec3 ( 0.0f, -9.81f, 0.0f ) )
{
	hudCamera = nullptr;

	menu = nullptr;

	openFile = nullptr;

	filePopup = nullptr;
	createPopup = nullptr;
	toolsPopup = nullptr;
	utilityPopup = nullptr;
	effectsPopup = nullptr;

	physicsInfo = nullptr;
	physicsContactNormalMaterial = nullptr;
	physicsContactPointMaterial = nullptr;
	physicsGeometry = nullptr;
	physicsShapeAContactGeometryMaterial = nullptr;
	physicsShapeBContactGeometryMaterial = nullptr;

	directedLight = nullptr;

	unitActor = nullptr;
	colliderOne = nullptr;
	colliderTwo = nullptr;
	kinematicPlane = nullptr;
	plasticSphere = nullptr;
	goldSphere = nullptr;
	silverSphere = nullptr;

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

	GXCollisionDetector::GetInstance ().EnableDebugData ();

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

	physicsContactPointMesh = GXMeshGeometry::LoadFromObj ( L"3D Models/System/Unit Sphere.obj" );
	physicsContactPointMaterial = new GXUnlitColorMaterial ();
	physicsContactPointMaterial->SetColor ( 255, 0, 0, 255 );

	physicsGeometry = new GXMeshGeometry ();
	physicsGeometry->SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, sizeof ( GXVec3 ), 0 );

	physicsShapeAContactGeometryMaterial = new GXUnlitColorMaterial ();
	physicsShapeAContactGeometryMaterial->SetColor ( 255, 255, 0, 255 );

	physicsShapeBContactGeometryMaterial = new GXUnlitColorMaterial ();
	physicsShapeBContactGeometryMaterial->SetColor ( 0, 255, 255, 255 );

	physicsPlanarIntersectionGeometryMaterial = new GXUnlitColorMaterial ();
	physicsPlanarIntersectionGeometryMaterial->SetColor ( 255, 0, 0, 255 );

	GXTransform transform;
	unitActor = new EMUnitActor ( L"Unit actor 01", transform );

	transform.SetLocation ( -3.0f, 0.0f, 0.0f );
	transform.SetRotation ( 0.0f, 0.0f, 0.5f );
	colliderOne = new EMPhysicsDrivenActor ( L"Collider One", transform );
	GXBoxShape* colliderOneShape = new GXBoxShape ( &( colliderOne->GetRigidBody () ), 1.0f, 1.0f, 1.0f );
	colliderOne->GetRigidBody ().SetShape ( *colliderOneShape );
	//colliderOne->GetRigidBody ().SetAngularVelocity ( GXVec3 ( 0.0f, 20.0f, 0.0f ) );
	//colliderOne->GetRigidBody ().EnableKinematic ();
	colliderOne->SetMesh ( L"3D Models/System/Unit Cube.stm" );
	EMCookTorranceCommonPassMaterial& colliderOneMaterial = colliderOne->GetMaterial ();
	colliderOneMaterial.SetAlbedoColor ( 253, 180, 17, 255 );
	colliderOneMaterial.SetRoughnessScale ( 0.25f );
	colliderOneMaterial.SetIndexOfRefractionScale ( 0.094f );
	colliderOneMaterial.SetSpecularIntensityScale ( 0.998f );
	colliderOneMaterial.SetMetallicScale ( 1.0f );
	colliderOneMaterial.SetEmissionColorScale ( 0.0f );
	colliderOne->EnablePhysicsDebug ();

	GXWorld& world = GXPhysicsEngine::GetInstance ().GetWorld ();
	world.RegisterForceGenerator ( colliderOne->GetRigidBody (), gravity );

	transform.SetLocation ( 3.0f, 0.0f, 0.0f );
	transform.SetRotation ( 0.0f, 0.0f, 0.0f );
	colliderTwo = new EMPhysicsDrivenActor ( L"Collider Two", transform );
	GXSphereShape* colliderTwoShape = new GXSphereShape ( &( colliderTwo->GetRigidBody () ), 0.5f );
	colliderTwo->GetRigidBody ().SetShape ( *colliderTwoShape );
	colliderTwo->GetRigidBody ().EnableKinematic ();
	colliderTwo->SetMesh ( L"3D Models/System/Unit Sphere.obj" );
	EMCookTorranceCommonPassMaterial& colliderTwoMaterial = colliderTwo->GetMaterial ();
	colliderTwoMaterial.SetAlbedoColor ( 247, 244, 233, 255 );
	colliderTwoMaterial.SetRoughnessScale ( 0.19f );
	colliderTwoMaterial.SetIndexOfRefractionScale ( 0.1f );
	colliderTwoMaterial.SetSpecularIntensityScale ( 0.998f );
	colliderTwoMaterial.SetMetallicScale ( 1.0f );
	colliderTwoMaterial.SetEmissionColorScale ( 0.0f );
	colliderTwo->EnablePhysicsDebug ();

	world.RegisterForceGenerator ( colliderTwo->GetRigidBody (), gravity );

	transform.SetLocation ( 0.0f, -3.0f, 0.0f );
	transform.SetRotation ( 0.0f, GX_MATH_PI, 0.0f );
	transform.SetScale ( 50.0f, 1.0, 50.0f );
	kinematicPlane = new EMPhysicsDrivenActor ( L"Kinematic Plane", transform );
	GXBoxShape* kinematicPlaneShape = new GXBoxShape ( &( kinematicPlane->GetRigidBody () ), 50.0f, 1.0f, 50.0f );
	kinematicPlane->GetRigidBody ().SetShape ( *kinematicPlaneShape );
	kinematicPlane->GetRigidBody ().EnableKinematic ();
	kinematicPlane->SetMesh ( L"3D Models/System/Unit Cube.stm" );
	EMCookTorranceCommonPassMaterial& kinematicPlaneMaterial = kinematicPlane->GetMaterial ();
	kinematicPlaneMaterial.SetRoughnessScale ( 0.07f );
	kinematicPlaneMaterial.SetSpecularIntensityScale ( 0.15f );
	kinematicPlaneMaterial.SetMetallicScale ( 0.0f );
	kinematicPlaneMaterial.SetIndexOfRefractionScale ( 0.292f );
	GXTexture2D* texture = kinematicPlaneMaterial.GetAlbedoTexture ();
	GXTexture2D::RemoveTexture ( *texture );
	*texture = GXTexture2D::LoadTexture ( L"Textures/System/GXEngine Logo 4k.png", GX_TRUE, GL_REPEAT, GX_TRUE );
	kinematicPlane->EnablePhysicsDebug ();

	world.RegisterForceGenerator ( kinematicPlane->GetRigidBody (), gravity );

	transform.SetLocation ( 6.0f, 0.0f, 0.0f );
	transform.SetScale ( 1.0f, 1.0, 1.0f );
	plasticSphere = new EMMeshActor ( L"Plastic sphere", transform );
	plasticSphere->SetMesh ( L"3D Models/System/Unit Sphere.obj" );
	EMCookTorranceCommonPassMaterial& plasticSphereMaterial = plasticSphere->GetMaterial ();
	plasticSphereMaterial.SetAlbedoColor ( 115, 185, 0, 255 );
	plasticSphereMaterial.SetRoughnessScale ( 0.5f );
	plasticSphereMaterial.SetIndexOfRefractionScale ( 0.292f );
	plasticSphereMaterial.SetSpecularIntensityScale ( 0.75f );
	plasticSphereMaterial.SetMetallicScale ( 1.0f );
	plasticSphereMaterial.SetEmissionColorScale ( 0.0f );

	transform.SetLocation ( 9.0f, 0.0f, 0.0f );
	goldSphere = new EMMeshActor ( L"Gold sphere", transform );
	goldSphere->SetMesh ( L"3D Models/System/Unit Sphere.obj" );
	EMCookTorranceCommonPassMaterial& goldSphereMaterial = goldSphere->GetMaterial ();
	goldSphereMaterial.SetAlbedoColor ( 253, 180, 17, 255 );
	goldSphereMaterial.SetRoughnessScale ( 0.25f );
	goldSphereMaterial.SetIndexOfRefractionScale ( 0.094f );
	goldSphereMaterial.SetSpecularIntensityScale ( 0.998f );
	goldSphereMaterial.SetMetallicScale ( 1.0f );
	goldSphereMaterial.SetEmissionColorScale ( 0.0f );

	transform.SetLocation ( 12.0f, 0.0f, 0.0f );
	silverSphere = new EMMeshActor ( L"Silver sphere", transform );
	silverSphere->SetMesh ( L"3D Models/System/Unit Sphere.obj" );
	EMCookTorranceCommonPassMaterial& silverSphereMaterial = silverSphere->GetMaterial ();
	silverSphereMaterial.SetAlbedoColor ( 247, 244, 233, 255 );
	silverSphereMaterial.SetRoughnessScale ( 0.19f );
	silverSphereMaterial.SetIndexOfRefractionScale ( 0.1f );
	silverSphereMaterial.SetSpecularIntensityScale ( 0.998f );
	silverSphereMaterial.SetMetallicScale ( 1.0f );
	silverSphereMaterial.SetEmissionColorScale ( 0.0f );

	transform.SetScale ( 1.0f, 1.0f, 1.0f );
	transform.SetRotation ( GXDegToRad ( 30.0f ), GXDegToRad ( 30.0f ), 0.0f );
	directedLight = new EMDirectedLightActor ( L"Directed light 01", transform );

	moveTool = new EMMoveTool ();
	moveTool->Bind ();
	moveTool->SetWorldMode ();

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

	if ( deltaTime < 0.2f )
		GXPhysicsEngine::GetInstance ().RunSimulateLoop ( deltaTime * 0.1f );

	fluttershy->UpdatePose ( deltaTime );

	GXCamera& viewerCamera = EMViewer::GetInstance ()->GetCamera ();
	GXCamera::SetActiveCamera ( &viewerCamera );

	EMRenderer& renderer = EMRenderer::GetInstance ();
	renderer.StartCommonPass ();

	unitActor->OnDrawCommonPass ( deltaTime );
	colliderOne->OnDrawCommonPass ( deltaTime );
	colliderTwo->OnDrawCommonPass ( deltaTime );
	kinematicPlane->OnDrawCommonPass ( deltaTime );
	plasticSphere->OnDrawCommonPass ( deltaTime );
	goldSphere->OnDrawCommonPass ( deltaTime );
	silverSphere->OnDrawCommonPass ( deltaTime );
	fluttershy->Render ( deltaTime );

	renderer.StartEnvironmentPass ();

	EMEnvironment::GetInstance ().Render ( deltaTime );

	renderer.StartLightPass ();

	renderer.ApplySSAO ();
	renderer.ApplyMotionBlur ();
	renderer.ApplyToneMapping ( deltaTime );

	renderer.StartHudColorPass ();

	colliderOne->OnDrawHudColorPass ();
	colliderTwo->OnDrawHudColorPass ();
	kinematicPlane->OnDrawHudColorPass ();
	moveTool->OnDrawHudColorPass ();

	GXCollisionDetector& collisionDetector = GXCollisionDetector::GetInstance ();
	const GXCollisionData& collisionData = GXPhysicsEngine::GetInstance ().GetWorld ().GetCollisionData ();
	GXContact* contact = collisionData.GetAllContacts ();

	if ( collisionData.GetTotalContacts () > 0 )
	{
		GXOpenGLState state;
		state.Save ();

		glDisable ( GL_DEPTH_TEST );

		GXUShort points = (GXUShort)collisionDetector.GetTotalShapeAContactGeometryPoints ();
		physicsGeometry->FillVertexBuffer ( collisionDetector.GetShapeAContactGeometry (), (GLsizeiptr)( points * sizeof ( GXVec3 ) ), GL_DYNAMIC_DRAW );
		physicsGeometry->SetTotalVertices ( (GLsizei)points );

		if ( points == 1 )
			physicsGeometry->SetTopology ( GL_POINTS );
		else
			physicsGeometry->SetTopology ( GL_LINE_LOOP );

		physicsShapeAContactGeometryMaterial->Bind ( GXTransform::GetNullTransform () );
		physicsGeometry->Render ();
		physicsShapeAContactGeometryMaterial->Unbind ();

		points = (GXUShort)collisionDetector.GetTotalShapeBContactGeometryPoints ();
		physicsGeometry->FillVertexBuffer ( collisionDetector.GetShapeBContactGeometry (), (GLsizeiptr)( points * sizeof ( GXVec3 ) ), GL_DYNAMIC_DRAW );
		physicsGeometry->SetTotalVertices ( (GLsizei)points );

		if ( points == 1 )
			physicsGeometry->SetTopology ( GL_POINTS );
		else
			physicsGeometry->SetTopology ( GL_LINE_LOOP );

		physicsShapeBContactGeometryMaterial->Bind ( GXTransform::GetNullTransform () );
		physicsGeometry->Render ();
		physicsShapeBContactGeometryMaterial->Unbind ();

		if ( collisionDetector.GetTotalPlanarIntersectionPoints () > 0 )
		{
			points = (GXUShort)collisionDetector.GetTotalShapeAPlanarContactGeometryPoints ();
			physicsGeometry->FillVertexBuffer ( collisionDetector.GetShapeAPlanarContactGeometry (), (GLsizeiptr)( points * sizeof ( GXVec3 ) ), GL_DYNAMIC_DRAW );
			physicsGeometry->SetTotalVertices ( (GLsizei)points );

			if ( points == 1 )
				physicsGeometry->SetTopology ( GL_POINTS );
			else
				physicsGeometry->SetTopology ( GL_LINE_LOOP );

			physicsShapeAContactGeometryMaterial->Bind ( GXTransform::GetNullTransform () );
			physicsGeometry->Render ();
			physicsShapeAContactGeometryMaterial->Unbind ();

			points = (GXUShort)collisionDetector.GetTotalShapeBPlanarContactGeometryPoints ();
			physicsGeometry->FillVertexBuffer ( collisionDetector.GetShapeBPlanarContactGeometry (), (GLsizeiptr)( points * sizeof ( GXVec3 ) ), GL_DYNAMIC_DRAW );
			physicsGeometry->SetTotalVertices ( (GLsizei)points );

			if ( points == 1 )
				physicsGeometry->SetTopology ( GL_POINTS );
			else
				physicsGeometry->SetTopology ( GL_LINE_LOOP );

			physicsShapeBContactGeometryMaterial->Bind ( GXTransform::GetNullTransform () );
			physicsGeometry->Render ();
			physicsShapeBContactGeometryMaterial->Unbind ();

			points = (GXUShort)collisionDetector.GetTotalPlanarIntersectionPoints ();
			physicsGeometry->FillVertexBuffer ( collisionDetector.GetPlanarIntersectionGeometry (), (GLsizeiptr)( points * sizeof ( GXVec3 ) ), GL_DYNAMIC_DRAW );
			physicsGeometry->SetTotalVertices ( (GLsizei)points );

			if ( points == 1 )
				physicsGeometry->SetTopology ( GL_POINTS );
			else
				physicsGeometry->SetTopology ( GL_LINE_LOOP );

			physicsPlanarIntersectionGeometryMaterial->Bind ( GXTransform::GetNullTransform () );
			physicsGeometry->Render ();
			physicsPlanarIntersectionGeometryMaterial->Unbind ();
		}

		GXVec3 z = collisionData.GetAllContacts ()->GetNormal ();

		GXVec3 x;
		if ( fabsf ( z.GetY () ) > fabsf ( z.GetX () ) && fabsf ( z.GetY () ) > fabsf ( z.GetZ () ) )
			x.CrossProduct ( GXVec3::GetAbsoluteX (), z );
		else
			x.CrossProduct ( GXVec3::GetAbsoluteY (), z );

		x.Normalize ();

		GXVec3 y;
		y.CrossProduct ( z, x );

		GXMat4 rot;
		rot.Identity ();
		rot.SetX ( x );
		rot.SetY ( y );
		rot.SetZ ( z );

		GXTransform transform;
		transform.SetRotation ( rot );

		static const GXVec3 offset ( 1.0f, 0.0f, 3.0f );
		GXVec3 location;
		viewerCamera.GetCurrentFrameModelMatrix ().MultiplyAsPoint ( location, offset );
		transform.SetLocation ( location );

		physicsContactNormalMaterial->Bind ( transform );
		physicsContactNormalMesh.Render ();
		physicsContactNormalMaterial->Unbind ();

		transform.SetScale ( 0.1f, 0.1f, 0.1f );

		for ( GXUInt i = 0; i < collisionData.GetTotalContacts (); i++ )
		{
			transform.SetLocation ( contact[ i ].GetContactPoint () );

			physicsContactPointMaterial->Bind ( transform );
			physicsContactPointMesh.Render ();
			physicsContactPointMaterial->Unbind ();
		}

		state.Restore ();
	}

	GXCamera::SetActiveCamera ( hudCamera );

	EMDrawUI ();

	physicsInfo->Reset ();

	GXImageInfo ii;
	ii.color.From ( 0, 0, 0, 180 );
	ii.texture = &physicsInfoBackgroundTexture;
	ii.insertX = 0.0f;
	ii.insertY = 0.0f;
	ii.insertWidth = (GXFloat)physicsInfo->GetWidth ();
	ii.insertHeight = (GXFloat)physicsInfo->GetHeight ();
	ii.overlayType = eGXImageOverlayType::SimpleReplace;

	physicsInfo->AddImage ( ii );

	GXPenInfo pi;
	pi.color.From ( 115, 185, 0, 255 );
	pi.font = &physicsInfoFont;
	pi.insertX = 0.5f * gx_ui_Scale;
	pi.insertY = 0.5f * gx_ui_Scale;
	pi.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

	physicsInfo->AddText ( pi, 64, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Contacts: %i" ), collisionData.GetTotalContacts () );

	GXFloat offset = (GXFloat)physicsInfoFont.GetSize ();
	pi.insertY += offset;
	physicsInfo->AddText ( pi, 128, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Allocated support points: %i" ), collisionDetector.GetAllocatedSupportPoints () );

	pi.insertY += offset;
	physicsInfo->AddText ( pi, 128, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Allocated edges: %i" ), collisionDetector.GetAllocatedEdges () );

	pi.insertY += offset;
	physicsInfo->AddText ( pi, 128, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Allocated faces: %i" ), collisionDetector.GetAllocatedFaces () );

	if ( collisionData.GetTotalContacts () > 0 )
	{
		pi.insertY += offset;
		physicsInfo->AddText ( pi, 128, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Penetration depth: %f" ), contact->GetPenetration () );

		pi.insertY += offset;
		physicsInfo->AddText ( pi, 128, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->GJK iterations: %i" ), contact->GetGTKIterations () );

		pi.insertY += offset;
		physicsInfo->AddText ( pi, 128, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->EPA iterations: %i" ), contact->GetEPAIterations () );

		pi.insertY += offset;
		physicsInfo->AddText ( pi, 128, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Used support points: %i" ), contact->GetSupportPoints () );

		pi.insertY += offset;
		physicsInfo->AddText ( pi, 128, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Used edges: %i" ), contact->GetEdges () );

		pi.insertY += offset;
		physicsInfo->AddText ( pi, 128, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Used faces: %i" ), contact->GetFaces () );
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

	GXSafeDelete ( silverSphere );
	GXSafeDelete ( goldSphere );
	GXSafeDelete ( plasticSphere );

	GXWorld& world = GXPhysicsEngine::GetInstance ().GetWorld ();

	world.UnregisterForceGenerator ( kinematicPlane->GetRigidBody (), gravity );
	world.UnregisterForceGenerator ( colliderOne->GetRigidBody (), gravity );
	world.UnregisterForceGenerator ( colliderTwo->GetRigidBody (), gravity );

	GXSafeDelete ( kinematicPlane );
	GXSafeDelete ( colliderOne );
	GXSafeDelete ( colliderTwo );
	GXSafeDelete ( unitActor );

	GXSafeDelete ( directedLight );

	GXSafeDelete ( physicsPlanarIntersectionGeometryMaterial );
	GXSafeDelete ( physicsShapeBContactGeometryMaterial );
	GXSafeDelete ( physicsShapeAContactGeometryMaterial );
	GXSafeDelete ( physicsGeometry );
	GXSafeDelete ( physicsContactPointMaterial );
	GXSafeDelete ( physicsContactNormalMaterial );
	GXMeshGeometry::RemoveMeshGeometry ( physicsContactPointMesh );
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
	EMUIColorPicker::GetInstance ().PickColor ( nullptr, &EMGame::OnPickRGBUByte, 115, 185, 0, 255 );
}

GXVoid GXCALL EMGame::OnPickRGBUByte ( GXVoid* /*handler*/, GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXLogW ( L"EMGame::OnPickRGBUByte::Info - Выбран цвет RGBA:\n\tКрасный - %hhu\n\tЗелёный - %hhu\n\tСиний - %hhu\n\tПрозрачность - %hhu\n", red, green, blue, alpha );
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
	EMRenderer::GetInstance ().GetObject ( (GXUShort)mouse.GetX (), (GXUShort)mouse.GetY () );
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
