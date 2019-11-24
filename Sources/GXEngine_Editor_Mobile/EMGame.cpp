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
#include <GXEngine/GXCore.h>
#include <GXEngine/GXSplashScreen.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXPhysics/GXPhysicsEngine.h>
#include <GXPhysics/GXSphereShape.h>
#include <GXPhysics/GXBoxShape.h>


#define EM_WINDOW_NAME                  L"GXEditor Mobile"
#define ENVIRONMENT_QUASI_DISTANCE      7.77f
#define FLUTTERSHY_SCALE                2.0e-2f

//---------------------------------------------------------------------------------------------------------------------

GXBool EMGame::_isContact;
GXMat4 EMGame::_contactNormalTransform;

EMGame::EMGame ():
    _hudCamera ( nullptr ),
    _openFile ( nullptr ),
    _menu ( nullptr ),
    _filePopup ( nullptr ),
    _createPopup ( nullptr ),
    _toolsPopup ( nullptr ),
    _utilityPopup ( nullptr ),
    _effectsPopup ( nullptr ),
    _physicsInfo ( nullptr ),
    _physicsContactPointMesh ( nullptr ),
    _physicsContactPointMaterial ( nullptr ),
    _gravity ( GXVec3 ( 0.0f, -9.81f, 0.0f ) ),
    _directedLight ( nullptr ),
    _unitActor ( nullptr ),
    _colliderOne ( nullptr ),
    _colliderTwo ( nullptr ),
    _kinematicPlane ( nullptr ),
    _moveTool ( nullptr ),
    _contactLocationMesh ( nullptr ),
    _contactLocationMaterial ( nullptr ),
    _contactNormalMesh ( nullptr ),
    _contactNormalMaterial ( nullptr ),
    _environmentMap ( nullptr ),
    _lightProbeSourceTexture ( nullptr ),
    _lightProbe ( nullptr ),
    _uiInput ( nullptr ),
    _physicsInfoFont ( nullptr )
{
    // NOTHING
}

EMGame::~EMGame ()
{
    // NOTHING
}

GXVoid EMGame::OnInit ()
{
    GXRenderer& coreRenderer = GXRenderer::GetInstance ();
    coreRenderer.SetWindowName ( EM_WINDOW_NAME );

    GXLocale& locale = GXLocale::GetInstance ();
    locale.LoadLanguage ( L"Locale/Editor Mobile/EN.lng", eGXLanguage::English );
    locale.LoadLanguage ( L"Locale/Editor Mobile/RU.lng", eGXLanguage::Russian );
    locale.SetLanguage ( eGXLanguage::English );

    InitStrings ( locale );

    GXInput& input = GXInput::GetInstance ();
    input.BindKeyCallback ( this, &EMGame::OnExit, VK_ESCAPE, eGXInputButtonState::Up );
    input.BindMouseButtonCallback ( this, &EMGame::OnMouseButton );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXUIInput" )
    _uiInput = new GXUIInput ( nullptr, GX_FALSE );
    GXTouchSurface::GetInstance ().SetDefaultWidget ( _uiInput );

    GXFloat w = static_cast<GXFloat> ( coreRenderer.GetWidth () );
    GXFloat h = static_cast<GXFloat> ( coreRenderer.GetHeight () );

    EMRenderer& editorRenderer = EMRenderer::GetInstance ();
    editorRenderer.SetOnObjectCallback ( this, &EMGame::OnObject );

    GXCollisionDetector::GetInstance ().EnableDebugData ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIOpenFile" )
    _openFile = new EMUIOpenFile ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIPopup" )
    _filePopup = new EMUIPopup ( nullptr );
    _filePopup->AddItem ( locale.GetString ( "Main menu->File->New" ), nullptr, nullptr );
    _filePopup->AddItem ( locale.GetString ( "Main menu->File->Open" ), nullptr, nullptr );
    _filePopup->AddItem ( locale.GetString ( "Main menu->File->Close" ), nullptr, nullptr );
    _filePopup->AddItem ( locale.GetString ( "Main menu->File->Save" ), nullptr, nullptr );
    _filePopup->AddItem ( locale.GetString ( "Main menu->File->Exit" ), this, &EMGame::OnExit );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIPopup" )
    _createPopup = new EMUIPopup ( nullptr );
    _createPopup->AddItem ( locale.GetString ( "Main menu->Create->Unit Actor" ), nullptr, nullptr );
    _createPopup->AddItem ( locale.GetString ( "Main menu->Create->Box falling" ), this, nullptr );
    _createPopup->AddItem ( locale.GetString ( "Main menu->Create->Skeletal mesh" ), nullptr, nullptr );
    _createPopup->AddItem ( locale.GetString ( "Main menu->Create->Directed light" ), nullptr, nullptr );
    _createPopup->AddItem ( locale.GetString ( "Main menu->Create->Spot" ), nullptr, nullptr );
    _createPopup->AddItem ( locale.GetString ( "Main menu->Create->Bulp" ), nullptr, nullptr );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIPopup" )
    _toolsPopup = new EMUIPopup ( nullptr );
    _toolsPopup->AddItem ( locale.GetString ( "Main menu->Tools->Select" ), nullptr, &EMGame::OnColorPicker );
    _toolsPopup->AddItem ( locale.GetString ( "Main menu->Tools->Move" ), nullptr, nullptr );
    _toolsPopup->AddItem ( locale.GetString ( "Main menu->Tools->Rotate" ), nullptr, nullptr );
    _toolsPopup->AddItem ( locale.GetString ( "Main menu->Tools->Scale" ), nullptr, nullptr );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIPopup" )
    _utilityPopup = new EMUIPopup ( nullptr );
    _utilityPopup->AddItem ( locale.GetString ( "Main menu->Utility->Particle system" ), nullptr, nullptr );
    _utilityPopup->AddItem ( locale.GetString ( "Main menu->Utility->Animation graph" ), nullptr, nullptr );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIPopup" )
    _effectsPopup = new EMUIPopup ( nullptr );
    _effectsPopup->AddItem ( locale.GetString ( "Main menu->Effects->Motion blur" ), this, &EMGame::OnShowMotionBlurSettings );
    _effectsPopup->AddItem ( locale.GetString ( "Main menu->Effects->SSAO" ), this, &EMGame::OnShowSSAOSettings );
    _effectsPopup->AddItem ( locale.GetString ( "Main menu->Effects->HDR tone mapper" ), this, &EMGame::OnShowToneMapperSettings );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIMenu" )
    _menu = new EMUIMenu ( nullptr );
    _menu->SetLocation ( 0.0f, h - _menu->GetHeight () );
    _menu->AddItem ( locale.GetString ( "Main menu->File" ), _filePopup );
    _menu->AddItem ( locale.GetString ( "Main menu->Create" ), _createPopup );
    _menu->AddItem ( locale.GetString ( "Main menu->Tools" ), _toolsPopup );
    _menu->AddItem ( locale.GetString ( "Main menu->Utility" ), _utilityPopup );
    _menu->AddItem ( locale.GetString ( "Main menu->Effects" ), _effectsPopup );

    GXFloat physicsInfoWidth = 10.0f * gx_ui_Scale;
    GXFloat physicsInfoHeight = 5.0f * gx_ui_Scale;

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _physicsInfo = new GXHudSurface ( static_cast<GXUShort> ( physicsInfoWidth ), static_cast<GXUShort> ( physicsInfoHeight ) );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXFont" )
    _physicsInfoFont = new GXFont ( L"Fonts/trebuc.ttf", static_cast<GXUShort> ( 0.4f * gx_ui_Scale ) );

    _physicsInfo->SetLocation ( 0.5f * ( physicsInfoWidth - w ), 0.5f * ( physicsInfoHeight - h ), 7.0f );

    _physicsInfoBackgroundTexture.LoadImage ( L"Textures/System/Default_Diffuse.tga", GX_FALSE, GX_FALSE );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMeshGeometry" )
    _physicsContactPointMesh = new GXMeshGeometry ();
    _physicsContactPointMesh->LoadMesh ( L"Meshes/System/Unit Sphere.obj" );
    _physicsContactPointMaterial = new GXUnlitColorMaterial ();
    _physicsContactPointMaterial->SetColor ( 255u, 0u, 0u, 255u );

    GXPhysicsEngine::GetInstance ().SetTimeMultiplier ( 0.5f );
    GXPhysicsEngine& physicsEngine = GXPhysicsEngine::GetInstance ();
    GXWorld& world = physicsEngine.GetWorld ();

    GXTransform transform;
    _unitActor = new EMUnitActor ( L"Unit actor 01", transform );

    //transform.SetLocation ( 1.0f, 4.0f, 0.0f );
    transform.SetLocation ( 1.0f, 4.0f, 0.0f );
    transform.SetRotation ( 0.5f, 0.0f, 0.3f );
    transform.SetScale ( 1.0f, 1.0f, 1.0f );
    _colliderOne = new EMPhysicsDrivenActor ( L"Collider One", transform );
    GXBoxShape* colliderOneShape = new GXBoxShape ( &( _colliderOne->GetRigidBody () ), 1.0f, 1.0f, 1.0f );
    colliderOneShape->SetRestitution ( 0.5f );
    _colliderOne->GetRigidBody ().SetMass ( 1.0f );
    _colliderOne->GetRigidBody ().SetShape ( *colliderOneShape );
    //_colliderOne->GetRigidBody ().SetCanSleep ( GX_FALSE );
    _colliderOne->GetRigidBody ().EnableKinematic ();
    //_colliderOne->GetRigidBody ().SetLinearVelocity ( GXVec3 ( 0.0, -1.0f, 1.0f ) );
    _colliderOne->GetRigidBody ().SetAngularVelocity ( GXVec3 ( 0.0f, 10.0f, -5.0f ) );
    _colliderOne->SetMesh ( L"Meshes/System/Unit Cube.stm" );
    EMCookTorranceCommonPassMaterial& colliderOneMaterial = _colliderOne->GetMaterial ();
    colliderOneMaterial.SetAlbedoColor ( 253u, 180u, 17u, 255u );
    colliderOneMaterial.SetRoughnessScale ( 0.25f );
    colliderOneMaterial.SetIndexOfRefractionScale ( 0.094f );
    colliderOneMaterial.SetSpecularIntensityScale ( 0.998f );
    colliderOneMaterial.SetMetallicScale ( 1.0f );
    colliderOneMaterial.SetEmissionColorScale ( 0.0f );
    _colliderOne->EnablePhysicsDebug ();

    world.RegisterForceGenerator ( _colliderOne->GetRigidBody (), _gravity );

    transform.SetLocation ( 3.0f, 0.5f, 0.0f );
    //transform.SetLocation ( 1.0f, 1.0f, 0.0f );
    transform.SetRotation ( 0.0f, 0.0f, 0.0f );
    transform.SetScale ( 1.0f, 1.0f, 1.0f );
    _colliderTwo = new EMPhysicsDrivenActor ( L"Collider Two", transform );
    GXSphereShape* colliderTwoShape = new GXSphereShape ( &( _colliderTwo->GetRigidBody () ), 0.5f );
    colliderTwoShape->SetRestitution ( 0.8f );
    _colliderTwo->GetRigidBody ().SetMass ( 1.0f );
    _colliderTwo->GetRigidBody ().SetShape ( *colliderTwoShape );
    //_colliderTwo->GetRigidBody ().SetCanSleep ( GX_FALSE );
    //_colliderTwo->GetRigidBody ().EnableKinematic ();
    _colliderTwo->GetRigidBody ().SetAngularVelocity ( GXVec3 ( 0.0f, 0.0f, 35.0f ) );
    _colliderTwo->SetMesh ( L"Meshes/System/Unit Sphere.obj" );
    EMCookTorranceCommonPassMaterial& colliderTwoMaterial = _colliderTwo->GetMaterial ();
    colliderTwoMaterial.SetAlbedoColor ( 247u, 244u, 233u, 255u );
    colliderTwoMaterial.SetRoughnessScale ( 0.19f );
    colliderTwoMaterial.SetIndexOfRefractionScale ( 0.1f );
    colliderTwoMaterial.SetSpecularIntensityScale ( 0.998f );
    colliderTwoMaterial.SetMetallicScale ( 1.0f );
    colliderTwoMaterial.SetEmissionColorScale ( 0.0f );
    _colliderTwo->EnablePhysicsDebug ();

    world.RegisterForceGenerator ( _colliderTwo->GetRigidBody (), _gravity );

    transform.SetLocation ( 0.0f, -3.0f, 0.0f );
    transform.SetRotation ( 0.0f, GX_MATH_PI, 0.0f );
    transform.SetScale ( 50.0f, 1.0, 50.0f );
    _kinematicPlane = new EMPhysicsDrivenActor ( L"Kinematic Plane", transform );
    GXBoxShape* kinematicPlaneShape = new GXBoxShape ( &( _kinematicPlane->GetRigidBody () ), 50.0f, 1.0f, 50.0f );
    _kinematicPlane->GetRigidBody ().SetMass ( 777.777e+3f );
    _kinematicPlane->GetRigidBody ().SetShape ( *kinematicPlaneShape );
    //kinematicPlane->GetRigidBody ().EnableKinematic ();
    //kinematicPlane->GetRigidBody ().SetLinearVelocity ( GXVec3 ( 0.0, 0.1f, 0.0f ) );
    //kinematicPlane->GetRigidBody ().SetAngularVelocity ( GXVec3 ( 0.0f, 0.0f, 0.2f ) );
    kinematicPlaneShape->SetRestitution ( 0.95f );
    _kinematicPlane->SetMesh ( L"Meshes/System/Unit Cube.stm" );
    EMCookTorranceCommonPassMaterial& kinematicPlaneMaterial = _kinematicPlane->GetMaterial ();
    kinematicPlaneMaterial.SetRoughnessScale ( 0.07f );
    kinematicPlaneMaterial.SetSpecularIntensityScale ( 0.15f );
    kinematicPlaneMaterial.SetMetallicScale ( 0.0f );
    kinematicPlaneMaterial.SetIndexOfRefractionScale ( 0.292f );
    GXTexture2D* texture = kinematicPlaneMaterial.GetAlbedoTexture ();
    texture->FreeResources ();
    texture->LoadImage ( "Textures/System/GXEngine Logo 4k.png", GX_TRUE, GX_TRUE );
    _kinematicPlane->EnablePhysicsDebug ();

    transform.SetLocation ( 6.0f, 0.0f, 0.0f );
    transform.SetScale ( 1.0f, 1.0f, 1.0f );
    transform.SetRotation ( GXDegToRad ( 30.0f ), GXDegToRad ( 30.0f ), 0.0f );
    _directedLight = new EMDirectedLightActor ( L"Directed light 01", transform );

    _moveTool = new EMMoveTool ();
    _moveTool->Bind ();
    _moveTool->SetWorldMode ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXCameraOrthographic" )
    _hudCamera = new GXCameraOrthographic ( w, h, EM_UI_HUD_CAMERA_NEAR_Z, EM_UI_HUD_CAMERA_FAR_Z );

    EMViewer* viewer = EMViewer::GetInstance ();
    viewer->SetOnViewerTransformChangedCallback ( this, &EMGame::OnViewerTransformChanged );
    viewer->SetInputWidget ( *_uiInput );
    viewer->SetTarget ( _unitActor );
    GXCamera::SetActiveCamera ( &viewer->GetCamera () );

    EMTool::SetActiveTool ( _moveTool );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMeshGeometry" )
    _contactLocationMesh = new GXMeshGeometry ();
    _contactLocationMesh->LoadMesh ( L"Meshes/System/Unit Sphere.obj" );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMeshGeometry" )
    _contactNormalMesh = new GXMeshGeometry ();
    _contactNormalMesh->LoadMesh ( L"Meshes/Editor Mobile/Move gismo Z axis.stm" );

    _contactLocationMaterial = new GXUnlitColorMaterial ();
    _contactLocationMaterial->SetColor ( 255u, 255u, 255u, 255u );

    _contactNormalMaterial = new GXUnlitColorMaterial ();
    _contactNormalMaterial->SetColor ( 0u, 0u, 255u, 255u );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTextureCubeMap" )
    _environmentMap = new GXTextureCubeMap ();
    _environmentMap->LoadEquirectangularImage ( L"Textures/Editor Mobile/Default LDR environment map.jpg", GX_FALSE, GX_TRUE );
    //environmentMap->LoadEquirectangularImage ( L"Textures/Editor Mobile/Default HDR environment map.hdr", GX_TRUE, GX_FALSE );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXTextureCubeMap" )
    _lightProbeSourceTexture = new GXTextureCubeMap ();
    _lightProbeSourceTexture->LoadEquirectangularImage ( L"Textures/Editor Mobile/Default HDR environment map.hdr", GX_TRUE, GX_FALSE );

    _lightProbe = new EMLightProbe ();
    _lightProbe->SetEnvironmentMap ( *_lightProbeSourceTexture );

    EMEnvironment& environment = EMEnvironment::GetInstance ();
    environment.SetEnvironmentMap ( *_environmentMap );
    environment.SetEnvironmentQuasiDistance ( ENVIRONMENT_QUASI_DISTANCE );

    EMUIFPSCounter::GetInstance ();
    EMUIColorPicker::GetInstance ();

    ShowCursor ( TRUE );
    SetCursor ( LoadCursorW ( nullptr, IDC_ARROW ) );

    physicsEngine.Start ();
}

GXVoid EMGame::OnResize ( GXInt width, GXInt height )
{
    GXLogA ( "EMGame::OnResize::Info - Новый размер %i x %i\n", width, height );
}

GXVoid EMGame::OnFrame ( GXFloat deltaTime )
{
    GXTouchSurface::GetInstance ().ExecuteMessages ();

    /*if ( deltaTime < 0.2f )
        GXPhysicsEngine::GetInstance ().RunSimulateLoop ( deltaTime );*/

    GXCamera& viewerCamera = EMViewer::GetInstance ()->GetCamera ();
    GXCamera::SetActiveCamera ( &viewerCamera );

    EMRenderer& renderer = EMRenderer::GetInstance ();
    renderer.StartCommonPass ();

    _unitActor->OnDrawCommonPass ( deltaTime );
    _colliderOne->OnDrawCommonPass ( deltaTime );
    _colliderTwo->OnDrawCommonPass ( deltaTime );
    _kinematicPlane->OnDrawCommonPass ( deltaTime );

    renderer.StartEnvironmentPass ();

    EMEnvironment::GetInstance ().Render ( deltaTime );

    renderer.StartLightPass ();

    renderer.ApplySSAO ();
    renderer.ApplyMotionBlur ();
    renderer.ApplyToneMapping ( deltaTime );

    renderer.StartHudColorPass ();

    _colliderOne->OnDrawHudColorPass ();
    _colliderTwo->OnDrawHudColorPass ();
    _kinematicPlane->OnDrawHudColorPass ();
    _moveTool->OnDrawHudColorPass ();

    GXOpenGLState state;
    GXTransform transform;

    if ( _isContact )
    {
        state.Save ();

        transform.SetCurrentFrameModelMatrix ( _contactNormalTransform );
        transform.SetScale ( 0.1f, 0.1f, 0.1f );

        _contactLocationMaterial->Bind ( transform );
        _contactLocationMesh->Render ();
        _contactLocationMaterial->Unbind ();

        transform.SetScale ( 1.0f, 1.0f, 1.0f );

        _contactNormalMaterial->Bind ( transform );
        _contactNormalMesh->Render ();
        _contactNormalMaterial->Unbind ();

        glDisable ( GL_DEPTH_TEST );

        state.Restore ();
    }

    GXCollisionDetector& collisionDetector = GXCollisionDetector::GetInstance ();
    const GXCollisionData& collisionData = GXPhysicsEngine::GetInstance ().GetWorld ().GetCollisionData ();
    GXContact* contact = collisionData.GetAllContacts ();

    if ( collisionData.GetTotalContacts () > 0u )
    {
        state.Save ();

        glDisable ( GL_DEPTH_TEST );

        transform.SetScale ( 0.1f, 0.1f, 0.1f );

        for ( GXUInt i = 0u; i < collisionData.GetTotalContacts (); ++i )
        {
            transform.SetLocation ( contact[ i ].GetContactPoint () );

            _physicsContactPointMaterial->Bind ( transform );
            _physicsContactPointMesh->Render ();
            _physicsContactPointMaterial->Unbind ();
        }

        state.Restore ();
    }

    GXCamera::SetActiveCamera ( _hudCamera );

    EMDrawUI ();

    _physicsInfo->Reset ();

    GXImageInfo ii;
    ii._color.From ( 0u, 0u, 0u, 180u );
    ii._texture = &_physicsInfoBackgroundTexture;
    ii._insertX = 0.0f;
    ii._insertY = 0.0f;
    ii._insertWidth = static_cast<GXFloat> ( _physicsInfo->GetWidth () );
    ii._insertHeight = static_cast<GXFloat> ( _physicsInfo->GetHeight () );
    ii._overlayType = eGXImageOverlayType::SimpleReplace;

    _physicsInfo->AddImage ( ii );

    GXPenInfo pi;
    pi._color.From ( 115u, 185u, 0u, 255u );
    pi._font = _physicsInfoFont;
    pi._insertX = 0.5f * gx_ui_Scale;
    pi._insertY = 0.5f * gx_ui_Scale;
    pi._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

    _buffer.Format ( _contactFormat, collisionData.GetTotalContacts () );
    _physicsInfo->AddText ( pi, 0u, _buffer );

    GXFloat offset = static_cast<GXFloat> ( _physicsInfoFont->GetSize () );
    pi._insertY += offset;
    _buffer.Format ( _allocatedSupportPointFormat, collisionDetector.GetAllocatedSupportPoints () );
    _physicsInfo->AddText ( pi, 0u, _buffer );

    pi._insertY += offset;
    _buffer.Format ( _allocatedEdgeFormat, collisionDetector.GetAllocatedEdges () );
    _physicsInfo->AddText ( pi, 0u, _buffer );

    pi._insertY += offset;
    _buffer.Format ( _allocatedFaceFormat, collisionDetector.GetAllocatedFaces () );
    _physicsInfo->AddText ( pi, 0u, _buffer );

    if ( collisionData.GetTotalContacts () > 0 )
    {
        pi._insertY += offset;
        _buffer.Format ( _penetrationDepthFormat, contact->GetPenetration () );
        _physicsInfo->AddText ( pi, 0u, _buffer );

        pi._insertY += offset;
        _buffer.Format ( _gjkIterationFormat, contact->GetGJKIterations () );
        _physicsInfo->AddText ( pi, 0u, _buffer );

        pi._insertY += offset;
        _buffer.Format ( _epaIterationFormat, contact->GetEPAIterations () );
        _physicsInfo->AddText ( pi, 0u, _buffer );

        pi._insertY += offset;
        _buffer.Format ( _usedSupportPointFormat, contact->GetSupportPoints () );
        _physicsInfo->AddText ( pi, 0u, _buffer );

        pi._insertY += offset;
        _buffer.Format ( _usedEdgeFormat, contact->GetEdges () );
        _physicsInfo->AddText ( pi, 0u, _buffer );

        pi._insertY += offset;
        _buffer.Format ( _usedFaceFormat, contact->GetFaces () );
        _physicsInfo->AddText ( pi, 0u, _buffer );
    }

    _physicsInfo->Render ();

    EMUIFPSCounter::GetInstance ().Render ();

    renderer.StartHudMaskPass ();

    GXCamera::SetActiveCamera ( &viewerCamera );

    _moveTool->OnDrawHudMaskPass ();

    renderer.PresentFrame ( eEMRenderTarget::Combine );

    viewerCamera.UpdateLastFrameMatrices ();

    GXCheckOpenGLError ();

    GXRenderer& coreRenderer = GXRenderer::GetInstance ();

    if ( coreRenderer.IsVisible () ) return;

    coreRenderer.Show ();
    GXSplashScreen::GetInstance ().Hide ();
}

GXVoid EMGame::OnDestroy ()
{
    _moveTool->UnBind ();

    GXSplashScreen::GetInstance ().Show ();
    GXRenderer::GetInstance ().Hide ();

    GXInput& input = GXInput::GetInstance ();
    input.UnbindMouseMoveCallback ();
    input.UnbindMouseButtonCallback ();
    input.UnbindKeyCallback ( VK_ESCAPE, eGXInputButtonState::Up );

    GXTouchSurface::GetInstance ().SetDefaultWidget ( nullptr );
    GXSafeDelete ( _uiInput );

    GXSafeDelete ( _lightProbe );

    delete _lightProbeSourceTexture;
    delete _environmentMap;

    GXSafeDelete ( _contactNormalMaterial );
    GXSafeDelete ( _contactNormalMesh );

    GXSafeDelete ( _contactLocationMaterial );
    GXSafeDelete ( _contactLocationMesh );

    GXSafeDelete ( _moveTool );

    GXWorld& world = GXPhysicsEngine::GetInstance ().GetWorld ();

    // world.UnregisterForceGenerator ( kinematicPlane->GetRigidBody (), gravity );
    world.UnregisterForceGenerator ( _colliderOne->GetRigidBody (), _gravity );
    world.UnregisterForceGenerator ( _colliderTwo->GetRigidBody (), _gravity );

    GXSafeDelete ( _kinematicPlane );
    GXSafeDelete ( _colliderOne );
    GXSafeDelete ( _colliderTwo );
    GXSafeDelete ( _unitActor );

    GXSafeDelete ( _directedLight );

    GXSafeDelete ( _physicsContactPointMaterial );
    GXSafeDelete ( _physicsContactPointMesh );
    _physicsInfoBackgroundTexture.FreeResources ();
    delete _physicsInfoFont;
    GXSafeDelete ( _physicsInfo );

    GXSafeDelete ( _effectsPopup );
    GXSafeDelete ( _utilityPopup );
    GXSafeDelete ( _toolsPopup );
    GXSafeDelete ( _createPopup );
    GXSafeDelete ( _filePopup );
    GXSafeDelete ( _openFile );
    GXSafeDelete ( _menu );

    GXSafeDelete ( _hudCamera );

    delete EMViewer::GetInstance ();
    delete &( EMEnvironment::GetInstance () );
    delete &( EMUIFPSCounter::GetInstance () );
    delete &( EMUIColorPicker::GetInstance () );

    delete &( EMRenderer::GetInstance () );
    delete &( GXPhysicsEngine::GetInstance () );
}

GXVoid EMGame::InitStrings ( const GXLocale &locale )
{
    _allocatedEdgeFormat = locale.GetString ( "EMGame->Physics info->Allocated edges: %i" );
    _allocatedFaceFormat = locale.GetString ( "EMGame->Physics info->Allocated faces: %i" );
    _allocatedSupportPointFormat = locale.GetString ( "EMGame->Physics info->Allocated support points: %i" );
    _contactFormat = locale.GetString ( "EMGame->Physics info->Contacts: %i" );
    _epaIterationFormat = locale.GetString ( "EMGame->Physics info->EPA iterations: %i" );
    _gjkIterationFormat = locale.GetString ( "EMGame->Physics info->GJK iterations: %i" );
    _penetrationDepthFormat = locale.GetString ( "EMGame->Physics info->Penetration depth: %f" );
    _usedEdgeFormat = locale.GetString ( "EMGame->Physics info->Used edges: %i" );
    _usedFaceFormat = locale.GetString ( "EMGame->Physics info->Used faces: %i" );
    _usedSupportPointFormat = locale.GetString ( "EMGame->Physics info->Used support points: %i" );
}

GXVoid GXCALL EMGame::OnExit ( GXVoid* context )
{
    EMGame* game = static_cast<EMGame*> ( context );
    game->_openFile->Browse ( &EMGame::OnOpenFile );
    /*GXCore::GetInstance ().Exit ();
    GXLogA ( "Завершение\n" );*/
}

GXVoid GXCALL EMGame::OnColorPicker ( GXVoid* /*context*/ )
{
    EMUIColorPicker::GetInstance ().PickColor ( nullptr, &EMGame::OnPickRGBUByte, 115, 185, 0, 255 );
}

GXVoid GXCALL EMGame::OnPickRGBUByte ( GXVoid* /*context*/, GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    GXLogA ( "EMGame::OnPickRGBUByte::Info - Выбран цвет RGBA:\n\tКрасный - %hhu\n\tЗелёный - %hhu\n\tСиний - %hhu\n\tПрозрачность - %hhu\n", red, green, blue, alpha );
}

GXVoid GXCALL EMGame::OnShowMotionBlurSettings ( GXVoid* /*context*/ )
{
    EMUIMotionBlurSettings::GetInstance ().Show ();
}

GXVoid GXCALL EMGame::OnShowSSAOSettings ( GXVoid* /*context*/ )
{
    EMUISSAOSettings::GetInstance ().Show ();
}

GXVoid GXCALL EMGame::OnShowToneMapperSettings ( GXVoid* /*context*/ )
{
    EMUIToneMapperSettings::GetInstance ().Show ();
}

GXVoid GXCALL EMGame::OnMouseButton ( GXVoid* /*context*/, GXInputMouseFlags mouseflags )
{
    if ( !mouseflags._leftMouseButton ) return;

    const GXVec2& mouse = GXTouchSurface::GetInstance ().GetMousePosition ();

    const GXUShort x = static_cast<GXUShort> ( mouse.GetX () );
    const GXUShort y = static_cast<GXUShort> ( mouse.GetY () );

    EMRenderer::GetInstance ().GetObject ( x, y );

    EMViewer* viewer = EMViewer::GetInstance ();

    if ( !viewer ) return;

    GXCamera& camera = viewer->GetCamera ();
    GXVec3 viewerLocation;
    camera.GetLocation ( viewerLocation );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    
    GXVec3 rayOrigin;
    GXVec3 rayDirection;

    GXGetRayFromViewer ( rayOrigin, rayDirection, x, y, static_cast<GXUShort> ( renderer.GetWidth () ), static_cast<GXUShort> ( renderer.GetHeight () ), viewerLocation, camera.GetCurrentFrameViewProjectionMatrix () );

    constexpr GXFloat rayLength = 10000.0f;
    GXVec3 contactLocation;
    GXVec3 contactNormal;
    const GXShape* contactShape;

    _isContact = GXPhysicsEngine::GetInstance ().GetWorld ().Raycast ( rayOrigin, rayDirection, rayLength, contactLocation, contactNormal, &contactShape );

    if ( !_isContact ) return;

    _contactNormalTransform.From ( contactNormal, contactLocation );

    constexpr GXFloat impulseModule = 3.0f;
    GXVec3 impulseWorld;
    impulseWorld.Multiply ( rayDirection, impulseModule );

    contactShape->GetRigidBody ().AddImpulseAtPointWorld ( impulseWorld, contactLocation );
}

GXVoid GXCALL EMGame::OnObject ( GXVoid* context, GXVoid* object )
{
    EMGame* game = static_cast<EMGame*> ( context );

    if ( game->_moveTool->OnObject ( object ) ) return;

    EMActor* actor = static_cast<EMActor*> ( object );

    if ( actor )
        GXLogA ( "EMOnObject::Info - Объект %S\n", actor->GetName () );

    game->_moveTool->SetActor ( actor );
    EMViewer::GetInstance ()->SetTarget ( actor );
}

GXVoid GXCALL EMGame::OnViewerTransformChanged ( GXVoid* context )
{
    EMGame* game = static_cast<EMGame*> ( context );
    game->_moveTool->OnViewerTransformChanged ();
    EMEnvironment::GetInstance ().OnViewerLocationChanged ();
}

GXVoid GXCALL EMGame::OnOpenFile ( const GXString &filePath )
{
    GXLogA ( "EMOnOpenFile::Info - Файл %s\n", static_cast<const GXMBChar*> ( filePath ) );
}
