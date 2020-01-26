#include <GXEngine_Editor_Mobile/EMGame.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine_Editor_Mobile/EMUIMotionBlurSettings.h>
#include <GXEngine_Editor_Mobile/EMUISSAOSettings.h>
#include <GXEngine_Editor_Mobile/EMUIToneMapperSettings.h>
#include <GXEngine_Editor_Mobile/EMUIFPSCounter.h>
#include <GXEngine_Editor_Mobile/EMUIColorPicker.h>
#include <GXEngine_Editor_Mobile/EMEnvironment.h>
#include <GXEngine/GXCore.h>
#include <GXEngine/GXDesktopInput.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXSplashScreen.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXPhysics/GXBoxShape.h>
#include <GXPhysics/GXPhysicsEngine.h>
#include <GXPhysics/GXSphereShape.h>


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

    BindDesktopInput ();

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
    GXLogA ( "EMGame::OnResize::Info - ����� ������ %i x %i\n", width, height );
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

    UnbindDesktopInput ();

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

GXVoid EMGame::BindDesktopInput ()
{
    GXDesktopInput& input = GXDesktopInput::GetInstance ();

    input.BindKeyboardKey ( nullptr, &EMGame::OnLShiftDown, eGXKeyboardKey::LeftShift, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnLShiftUp, eGXKeyboardKey::LeftShift, eGXButtonState::Up );
    input.BindKeyboardKey ( nullptr, &EMGame::OnRShiftDown, eGXKeyboardKey::RightShift, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnRShiftUp, eGXKeyboardKey::RightShift, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnLCtrlDown, eGXKeyboardKey::LeftCtrl, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnLCtrlUp, eGXKeyboardKey::LeftCtrl, eGXButtonState::Up );
    input.BindKeyboardKey ( nullptr, &EMGame::OnRCtrlDown, eGXKeyboardKey::RightCtrl, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnRCtrlUp, eGXKeyboardKey::RightCtrl, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnLAltDown, eGXKeyboardKey::LeftAlt, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnLAltUp, eGXKeyboardKey::LeftAlt, eGXButtonState::Up );
    input.BindKeyboardKey ( nullptr, &EMGame::OnRAltDown, eGXKeyboardKey::RightAlt, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnRAltUp, eGXKeyboardKey::RightAlt, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnTabDown, eGXKeyboardKey::Tab, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnTabUp, eGXKeyboardKey::Tab, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnCapsLockDown, eGXKeyboardKey::CapsLock, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnCapsLockUp, eGXKeyboardKey::CapsLock, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad0Down, eGXKeyboardKey::Numpad0, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad0Up, eGXKeyboardKey::Numpad0, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad1Down, eGXKeyboardKey::Numpad1, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad1Up, eGXKeyboardKey::Numpad1, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad2Down , eGXKeyboardKey::Numpad2, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad2Up, eGXKeyboardKey::Numpad2, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad3Down, eGXKeyboardKey::Numpad3, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad3Up, eGXKeyboardKey::Numpad3, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad4Down, eGXKeyboardKey::Numpad4, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad4Up, eGXKeyboardKey::Numpad4, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad5Down, eGXKeyboardKey::Numpad5, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad5Up, eGXKeyboardKey::Numpad5, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad6Down, eGXKeyboardKey::Numpad6, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad6Up, eGXKeyboardKey::Numpad6, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad7Down, eGXKeyboardKey::Numpad7, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad7Up, eGXKeyboardKey::Numpad7, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad8Down, eGXKeyboardKey::Numpad8, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad8Up, eGXKeyboardKey::Numpad8, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad9Down, eGXKeyboardKey::Numpad9, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpad9Up, eGXKeyboardKey::Numpad9, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadDotDown, eGXKeyboardKey::NumpadDot, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadDotUp, eGXKeyboardKey::NumpadDot, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadEnterDown, eGXKeyboardKey::NumpadEnter, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadEnterUp, eGXKeyboardKey::NumpadEnter, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadPlusDown, eGXKeyboardKey::NumpadPlus, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadPlusUp, eGXKeyboardKey::NumpadPlus, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadMinusDown, eGXKeyboardKey::NumpadMinus, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadMinusUp, eGXKeyboardKey::NumpadMinus, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadMultiplyDown, eGXKeyboardKey::NumpadMultiply, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadMultiplyUp, eGXKeyboardKey::NumpadMultiply, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadDivideDown, eGXKeyboardKey::NumpadDivide, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumpadDivideUp, eGXKeyboardKey::NumpadDivide, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNumLockDown, eGXKeyboardKey::NumLock, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNumLockUp, eGXKeyboardKey::NumLock, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnContextMenuDown, eGXKeyboardKey::ContextMenu, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnContextMenuUp, eGXKeyboardKey::ContextMenu, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF1Down, eGXKeyboardKey::F1, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF1Up, eGXKeyboardKey::F1, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF2Down, eGXKeyboardKey::F2, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF2Up, eGXKeyboardKey::F2, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF3Down, eGXKeyboardKey::F3, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF3Up, eGXKeyboardKey::F3, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF4Down, eGXKeyboardKey::F4, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF4Up, eGXKeyboardKey::F4, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF5Down, eGXKeyboardKey::F5, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF5Up, eGXKeyboardKey::F5, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF6Down, eGXKeyboardKey::F6, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF6Up, eGXKeyboardKey::F6, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF7Down, eGXKeyboardKey::F7, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF7Up, eGXKeyboardKey::F7, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF8Down, eGXKeyboardKey::F8, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF8Up, eGXKeyboardKey::F8, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF9Down, eGXKeyboardKey::F9, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF9Up, eGXKeyboardKey::F9, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF10Down, eGXKeyboardKey::F10, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF10Up, eGXKeyboardKey::F10, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF11Down, eGXKeyboardKey::F11, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF11Up, eGXKeyboardKey::F11, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnF12Down, eGXKeyboardKey::F12, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnF12Up, eGXKeyboardKey::F12, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnZeroDown, eGXKeyboardKey::Zero, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnZeroUp, eGXKeyboardKey::Zero, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnOneDown, eGXKeyboardKey::One, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnOneUp, eGXKeyboardKey::One, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnTwoDown, eGXKeyboardKey::Two, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnTwoUp, eGXKeyboardKey::Two, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnThreeDown, eGXKeyboardKey::Three, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnThreeUp, eGXKeyboardKey::Three, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnFourDown, eGXKeyboardKey::Four, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnFourUp, eGXKeyboardKey::Four, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnFiveDown, eGXKeyboardKey::Five, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnFiveUp, eGXKeyboardKey::Five, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnSixDown, eGXKeyboardKey::Six, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnSixUp, eGXKeyboardKey::Six, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnSevenDown, eGXKeyboardKey::Seven, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnSevenUp, eGXKeyboardKey::Seven, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnEightDown, eGXKeyboardKey::Eight, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnEightUp, eGXKeyboardKey::Eight, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNineDown, eGXKeyboardKey::Nine, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNineUp, eGXKeyboardKey::Nine, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnMinusDown, eGXKeyboardKey::Minus, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnMinusUp, eGXKeyboardKey::Minus, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnEqualDown, eGXKeyboardKey::Equal, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnEqualUp, eGXKeyboardKey::Equal, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnLeftDown, eGXKeyboardKey::Left, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnLeftUp, eGXKeyboardKey::Left, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnRightDown, eGXKeyboardKey::Right, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnRightUp, eGXKeyboardKey::Right, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnUpDown, eGXKeyboardKey::Up, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnUpUp, eGXKeyboardKey::Up, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnDownDown, eGXKeyboardKey::Down, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnDownUp, eGXKeyboardKey::Down, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnDelDown, eGXKeyboardKey::Delete, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnDelUp, eGXKeyboardKey::Delete, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnInsDown, eGXKeyboardKey::Insert, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnInsUp, eGXKeyboardKey::Insert, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnHomeDown, eGXKeyboardKey::Home, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnHomeUp, eGXKeyboardKey::Home, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnEndDown, eGXKeyboardKey::End, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnEndUp, eGXKeyboardKey::End, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnPageUpDown, eGXKeyboardKey::PageUp, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnPageUpUp, eGXKeyboardKey::PageUp, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnPageDownDown, eGXKeyboardKey::PageDown, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnPageDownUp, eGXKeyboardKey::PageDown, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnScrollLockDown, eGXKeyboardKey::ScrollLock, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnScrollLockUp, eGXKeyboardKey::ScrollLock, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnPauseDown, eGXKeyboardKey::Pause, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnPauseUp, eGXKeyboardKey::Pause, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnQDown, eGXKeyboardKey::Q, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnQUp, eGXKeyboardKey::Q, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnWDown, eGXKeyboardKey::W, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnWUp, eGXKeyboardKey::W, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnEDown, eGXKeyboardKey::E, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnEUp, eGXKeyboardKey::E, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnRDown, eGXKeyboardKey::R, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnRUp, eGXKeyboardKey::R, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnTDown, eGXKeyboardKey::T, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnTUp, eGXKeyboardKey::T, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnYDown, eGXKeyboardKey::Y, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnYUp, eGXKeyboardKey::Y, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnUDown, eGXKeyboardKey::U, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnUUp, eGXKeyboardKey::U, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnIDown, eGXKeyboardKey::I, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnIUp, eGXKeyboardKey::I, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnODown, eGXKeyboardKey::O, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnOUp, eGXKeyboardKey::O, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnPDown, eGXKeyboardKey::P, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnPUp, eGXKeyboardKey::P, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnADown, eGXKeyboardKey::A, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnAUp, eGXKeyboardKey::A, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnSDown, eGXKeyboardKey::S, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnSUp, eGXKeyboardKey::S, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnDDown, eGXKeyboardKey::D, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnDUp, eGXKeyboardKey::D, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnFDown, eGXKeyboardKey::F, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnFUp, eGXKeyboardKey::F, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnGDown, eGXKeyboardKey::G, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnGUp, eGXKeyboardKey::G, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnHDown, eGXKeyboardKey::H, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnHUp, eGXKeyboardKey::H, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnJDown, eGXKeyboardKey::J, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnJUp, eGXKeyboardKey::J, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnKDown, eGXKeyboardKey::K, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnKUp, eGXKeyboardKey::K, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnLDown, eGXKeyboardKey::L, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnLUp, eGXKeyboardKey::L, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnZDown, eGXKeyboardKey::Z, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnZUp, eGXKeyboardKey::Z, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnXDown, eGXKeyboardKey::X, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnXUp, eGXKeyboardKey::X, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnCDown, eGXKeyboardKey::C, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnCUp, eGXKeyboardKey::C, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnVDown, eGXKeyboardKey::V, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnVUp, eGXKeyboardKey::V, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnBDown, eGXKeyboardKey::B, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnBUp, eGXKeyboardKey::B, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnNDown, eGXKeyboardKey::N, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnNUp, eGXKeyboardKey::N, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnMDown, eGXKeyboardKey::M, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnMUp, eGXKeyboardKey::M, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnSpaceDown, eGXKeyboardKey::Space, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnSpaceUp, eGXKeyboardKey::Space, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnEscapeDown, eGXKeyboardKey::Esc, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnEscapeUp, eGXKeyboardKey::Esc, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnBackspaceDown, eGXKeyboardKey::Backspace, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnBackspaceUp, eGXKeyboardKey::Backspace, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnEnterDown, eGXKeyboardKey::Enter, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnEnterUp, eGXKeyboardKey::Enter, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnLessDown, eGXKeyboardKey::Less, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnLessUp, eGXKeyboardKey::Less, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnGreaterDown, eGXKeyboardKey::Greater, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnGreaterUp, eGXKeyboardKey::Greater, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnSlashDown, eGXKeyboardKey::Slash, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnSlashUp, eGXKeyboardKey::Slash, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnSemicolonDown, eGXKeyboardKey::Semicolon, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnSemicolonUp, eGXKeyboardKey::Semicolon, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnQuoteDown, eGXKeyboardKey::Quote, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnQuoteUp, eGXKeyboardKey::Quote, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnOpeningSquareBracketDown, eGXKeyboardKey::OpeningSquareBracket, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnOpeningSquareBracketUp, eGXKeyboardKey::OpeningSquareBracket, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnClosingSquareBracketDown, eGXKeyboardKey::ClosingSquareBracket, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnClosingSquareBracketUp, eGXKeyboardKey::ClosingSquareBracket, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnBackslashDown, eGXKeyboardKey::Backslash, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnBackslashUp, eGXKeyboardKey::Backslash, eGXButtonState::Up );

    input.BindKeyboardKey ( nullptr, &EMGame::OnTildeDown, eGXKeyboardKey::Tilde, eGXButtonState::Down );
    input.BindKeyboardKey ( nullptr, &EMGame::OnTildeUp, eGXKeyboardKey::Tilde, eGXButtonState::Up );
}

GXVoid EMGame::UnbindDesktopInput ()
{
    GXDesktopInput& input = GXDesktopInput::GetInstance ();

    input.UnbindKeyboardKey ( eGXKeyboardKey::LeftShift, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::LeftShift, eGXButtonState::Up );
    input.UnbindKeyboardKey ( eGXKeyboardKey::RightShift, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::RightShift, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::LeftCtrl, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::LeftCtrl, eGXButtonState::Up );
    input.UnbindKeyboardKey ( eGXKeyboardKey::RightCtrl, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::RightCtrl, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::LeftAlt, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::LeftAlt, eGXButtonState::Up );
    input.UnbindKeyboardKey ( eGXKeyboardKey::RightAlt, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::RightAlt, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Tab, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Tab, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::CapsLock, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::CapsLock, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad0, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad0, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad1, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad1, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad2, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad2, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad3, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad3, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad4, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad4, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad5, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad5, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad6, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad6, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad7, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad7, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad8, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad8, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad9, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Numpad9, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadDot, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadDot, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadEnter, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadEnter, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadPlus, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadPlus, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadMinus, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadMinus, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadMultiply, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadMultiply, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadDivide, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::NumpadDivide, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::NumLock, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::NumLock, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::ContextMenu, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::ContextMenu, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F1, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F1, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F2, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F2, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F3, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F3, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F4, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F4, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F5, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F5, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F6, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F6, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F7, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F7, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F8, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F8, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F9, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F9, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F10, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F10, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F11, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F11, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F12, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F12, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Zero, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Zero, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::One, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::One, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Two, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Two, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Three, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Three, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Four, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Four, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Five, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Five, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Six, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Six, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Seven, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Seven, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Eight, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Eight, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Nine, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Nine, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Minus, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Minus, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Equal, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Equal, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Left, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Left, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Right, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Right, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Up, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Up, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Down, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Down, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Delete, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Delete, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Insert, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Insert, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Home, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Home, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::End, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::End, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::PageUp, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::PageUp, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::PageDown, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::PageDown, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::ScrollLock, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::ScrollLock, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Pause, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Pause, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Q, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Q, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::W, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::W, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::E, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::E, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::R, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::R, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::T, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::T, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Y, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Y, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::U, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::U, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::I, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::I, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::O, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::O, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::P, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::P, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::A, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::A, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::S, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::S, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::D, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::D, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::F, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::F, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::G, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::G, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::H, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::H, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::J, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::J, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::K, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::K, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::L, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::L, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Z, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Z, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::X, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::X, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::C, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::C, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::V, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::V, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::B, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::B, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::N, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::N, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::M, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::M, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Space, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Space, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Esc, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Esc, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Backspace, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Backspace, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Enter, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Enter, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Less, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Less, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Greater, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Greater, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Slash, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Slash, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Semicolon, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Semicolon, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Quote, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Quote, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::OpeningSquareBracket, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::OpeningSquareBracket, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::ClosingSquareBracket, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::ClosingSquareBracket, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Backslash, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Backslash, eGXButtonState::Up );

    input.UnbindKeyboardKey ( eGXKeyboardKey::Tilde, eGXButtonState::Down );
    input.UnbindKeyboardKey ( eGXKeyboardKey::Tilde, eGXButtonState::Up );
}

GXVoid GXCALL EMGame::OnExit ( GXVoid* context )
{
    EMGame* game = static_cast<EMGame*> ( context );
    game->_openFile->Browse ( &EMGame::OnOpenFile );
    /*GXCore::GetInstance ().Exit ();
    GXLogA ( "����������\n" );*/
}

GXVoid GXCALL EMGame::OnColorPicker ( GXVoid* /*context*/ )
{
    EMUIColorPicker::GetInstance ().PickColor ( nullptr, &EMGame::OnPickRGBUByte, 115, 185, 0, 255 );
}

GXVoid GXCALL EMGame::OnPickRGBUByte ( GXVoid* /*context*/, GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    GXLogA ( "EMGame::OnPickRGBUByte::Info - ������ ���� RGBA:\n\t������� - %hhu\n\t������ - %hhu\n\t����� - %hhu\n\t������������ - %hhu\n", red, green, blue, alpha );
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
        GXLogA ( "EMOnObject::Info - ������ %S\n", actor->GetName () );

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
    GXLogA ( "EMOnOpenFile::Info - ���� %s\n", static_cast<const GXMBChar*> ( filePath ) );
}

GXVoid GXCALL EMGame::OnLShiftDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLShiftDown\n" );
}

GXVoid GXCALL EMGame::OnLShiftUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLShiftUp\n" );
}

GXVoid GXCALL EMGame::OnRShiftDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRShiftDown\n" );
}

GXVoid GXCALL EMGame::OnRShiftUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRShiftUp\n" );
}

GXVoid GXCALL EMGame::OnLCtrlDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLCtrlDown\n" );
}

GXVoid GXCALL EMGame::OnLCtrlUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLCtrlUp\n" );
}

GXVoid GXCALL EMGame::OnRCtrlDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRCtrlDown\n" );
}

GXVoid GXCALL EMGame::OnRCtrlUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRCtrlUp\n" );
}

GXVoid GXCALL EMGame::OnLAltDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLAltDown\n" );
}

GXVoid GXCALL EMGame::OnLAltUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLAltUp\n" );
}

GXVoid GXCALL EMGame::OnRAltDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRAltDown\n" );
}

GXVoid GXCALL EMGame::OnRAltUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRAltUp\n" );
}

GXVoid GXCALL EMGame::OnTabDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnTabDown\n" );
}

GXVoid GXCALL EMGame::OnTabUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnTabUp\n" );
}

GXVoid GXCALL EMGame::OnCapsLockDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnCapsLockDown\n" );
}

GXVoid GXCALL EMGame::OnCapsLockUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnCapsLockUp\n" );
}

GXVoid GXCALL EMGame::OnNumpad0Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad0Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad0Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad0Up\n" );
}

GXVoid GXCALL EMGame::OnNumpad1Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad1Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad1Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad1Up\n" );
}

GXVoid GXCALL EMGame::OnNumpad2Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad2Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad2Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad2Up\n" );
}

GXVoid GXCALL EMGame::OnNumpad3Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad3Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad3Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad3Up\n" );
}

GXVoid GXCALL EMGame::OnNumpad4Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad4Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad4Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad4Up\n" );
}

GXVoid GXCALL EMGame::OnNumpad5Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad5Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad5Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad5Up\n" );
}

GXVoid GXCALL EMGame::OnNumpad6Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad6Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad6Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad6Up\n" );
}

GXVoid GXCALL EMGame::OnNumpad7Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad7Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad7Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad7Up\n" );
}

GXVoid GXCALL EMGame::OnNumpad8Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad8Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad8Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad8Up\n" );
}

GXVoid GXCALL EMGame::OnNumpad9Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad9Down\n" );
}

GXVoid GXCALL EMGame::OnNumpad9Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpad9Up\n" );
}

GXVoid GXCALL EMGame::OnNumpadDotDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadDotDown\n" );
}

GXVoid GXCALL EMGame::OnNumpadDotUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadDotUp\n" );
}

GXVoid GXCALL EMGame::OnNumpadEnterDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadEnterDown\n" );
}

GXVoid GXCALL EMGame::OnNumpadEnterUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadEnterUp\n" );
}

GXVoid GXCALL EMGame::OnNumpadPlusDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadPlusDown\n" );
}

GXVoid GXCALL EMGame::OnNumpadPlusUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadPlusUp\n" );
}

GXVoid GXCALL EMGame::OnNumpadMinusDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadMinusDown\n" );
}

GXVoid GXCALL EMGame::OnNumpadMinusUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadMinusUp\n" );
}

GXVoid GXCALL EMGame::OnNumpadMultiplyDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadMultiplyDown\n" );
}

GXVoid GXCALL EMGame::OnNumpadMultiplyUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadMultiplyUp\n" );
}

GXVoid GXCALL EMGame::OnNumpadDivideDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadDivideDown\n" );
}

GXVoid GXCALL EMGame::OnNumpadDivideUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumpadDivideUp\n" );
}

GXVoid GXCALL EMGame::OnNumLockDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumLockDown\n" );
}

GXVoid GXCALL EMGame::OnNumLockUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNumLockUp\n" );
}

GXVoid GXCALL EMGame::OnContextMenuDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnContextMenuDown\n" );
}

GXVoid GXCALL EMGame::OnContextMenuUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnContextMenuUp\n" );
}

GXVoid GXCALL EMGame::OnF1Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF1Down\n" );
}

GXVoid GXCALL EMGame::OnF1Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF1Up\n" );
}

GXVoid GXCALL EMGame::OnF2Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF2Down\n" );
}

GXVoid GXCALL EMGame::OnF2Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF2Up\n" );
}

GXVoid GXCALL EMGame::OnF3Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF3Down\n" );
}

GXVoid GXCALL EMGame::OnF3Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF3Up\n" );
}

GXVoid GXCALL EMGame::OnF4Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF4Down\n" );
}

GXVoid GXCALL EMGame::OnF4Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF4Up\n" );
}

GXVoid GXCALL EMGame::OnF5Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF5Down\n" );
}

GXVoid GXCALL EMGame::OnF5Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF5Up\n" );
}

GXVoid GXCALL EMGame::OnF6Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF6Down\n" );
}

GXVoid GXCALL EMGame::OnF6Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF6Up\n" );
}

GXVoid GXCALL EMGame::OnF7Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF7Down\n" );
}

GXVoid GXCALL EMGame::OnF7Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF7Up\n" );
}

GXVoid GXCALL EMGame::OnF8Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF8Down\n" );
}

GXVoid GXCALL EMGame::OnF8Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF8Up\n" );
}

GXVoid GXCALL EMGame::OnF9Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF9Down\n" );
}

GXVoid GXCALL EMGame::OnF9Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF9Up\n" );
}

GXVoid GXCALL EMGame::OnF10Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF10Down\n" );
}

GXVoid GXCALL EMGame::OnF10Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF10Up\n" );
}

GXVoid GXCALL EMGame::OnF11Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF11Down\n" );
}

GXVoid GXCALL EMGame::OnF11Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF11Up\n" );
}

GXVoid GXCALL EMGame::OnF12Down ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF12Down\n" );
}

GXVoid GXCALL EMGame::OnF12Up ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnF12Up\n" );
}

GXVoid GXCALL EMGame::OnZeroDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnZeroDown\n" );
}

GXVoid GXCALL EMGame::OnZeroUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnZeroUp\n" );
}

GXVoid GXCALL EMGame::OnOneDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnOneDown\n" );
}

GXVoid GXCALL EMGame::OnOneUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnOneUp\n" );
}

GXVoid GXCALL EMGame::OnTwoDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnTwoDown\n" );
}

GXVoid GXCALL EMGame::OnTwoUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnTwoUp\n" );
}

GXVoid GXCALL EMGame::OnThreeDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnThreeDown\n" );
}

GXVoid GXCALL EMGame::OnThreeUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnThreeUp\n" );
}

GXVoid GXCALL EMGame::OnFourDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnFourDown\n" );
}

GXVoid GXCALL EMGame::OnFourUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnFourUp\n" );
}

GXVoid GXCALL EMGame::OnFiveDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnFiveDown\n" );
}

GXVoid GXCALL EMGame::OnFiveUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnFiveUp\n" );
}

GXVoid GXCALL EMGame::OnSixDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSixDown\n" );
}

GXVoid GXCALL EMGame::OnSixUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSixUp\n" );
}

GXVoid GXCALL EMGame::OnSevenDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSevenDown\n" );
}

GXVoid GXCALL EMGame::OnSevenUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSevenUp\n" );
}

GXVoid GXCALL EMGame::OnEightDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEightDown\n" );
}

GXVoid GXCALL EMGame::OnEightUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEightUp\n" );
}

GXVoid GXCALL EMGame::OnNineDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNineDown\n" );
}

GXVoid GXCALL EMGame::OnNineUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNineUp\n" );
}

GXVoid GXCALL EMGame::OnMinusDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnMinusDown\n" );
}

GXVoid GXCALL EMGame::OnMinusUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnMinusUp\n" );
}

GXVoid GXCALL EMGame::OnEqualDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEqualDown\n" );
}

GXVoid GXCALL EMGame::OnEqualUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEqualUp\n" );
}

GXVoid GXCALL EMGame::OnLeftDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLeftDown\n" );
}

GXVoid GXCALL EMGame::OnLeftUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLeftUp\n" );
}

GXVoid GXCALL EMGame::OnRightDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRightDown\n" );
}

GXVoid GXCALL EMGame::OnRightUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRightUp\n" );
}

GXVoid GXCALL EMGame::OnUpDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnUpDown\n" );
}

GXVoid GXCALL EMGame::OnUpUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnUpUp\n" );
}

GXVoid GXCALL EMGame::OnDownDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnDownDown\n" );
}

GXVoid GXCALL EMGame::OnDownUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnDownUp\n" );
}

GXVoid GXCALL EMGame::OnDelDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnDelDown\n" );
}

GXVoid GXCALL EMGame::OnDelUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnDelUp\n" );
}

GXVoid GXCALL EMGame::OnInsDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnInsDown\n" );
}

GXVoid GXCALL EMGame::OnInsUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnInsUp\n" );
}

GXVoid GXCALL EMGame::OnHomeDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnHomeDown\n" );
}

GXVoid GXCALL EMGame::OnHomeUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnHomeUp\n" );
}

GXVoid GXCALL EMGame::OnEndDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEndDown\n" );
}

GXVoid GXCALL EMGame::OnEndUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEndUp\n" );
}

GXVoid GXCALL EMGame::OnPageUpDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnPageUpDown\n" );
}

GXVoid GXCALL EMGame::OnPageUpUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnPageUpUp\n" );
}

GXVoid GXCALL EMGame::OnPageDownDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnPageDownDown\n" );
}

GXVoid GXCALL EMGame::OnPageDownUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnPageDownUp\n" );
}

GXVoid GXCALL EMGame::OnScrollLockDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnScrollLockDown\n" );
}

GXVoid GXCALL EMGame::OnScrollLockUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnScrollLockUp\n" );
}

GXVoid GXCALL EMGame::OnPauseDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnPauseDown\n" );
}

GXVoid GXCALL EMGame::OnPauseUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnPauseUp\n" );
}

GXVoid GXCALL EMGame::OnQDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnQDown\n" );
}

GXVoid GXCALL EMGame::OnQUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnQUp\n" );
}

GXVoid GXCALL EMGame::OnWDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnWDown\n" );
}

GXVoid GXCALL EMGame::OnWUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnWUp\n" );
}

GXVoid GXCALL EMGame::OnEDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEDown\n" );
}

GXVoid GXCALL EMGame::OnEUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEUp\n" );
}

GXVoid GXCALL EMGame::OnRDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRDown\n" );
}

GXVoid GXCALL EMGame::OnRUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnRUp\n" );
}

GXVoid GXCALL EMGame::OnTDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnTDown\n" );
}

GXVoid GXCALL EMGame::OnTUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnTUp\n" );
}

GXVoid GXCALL EMGame::OnYDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnYDown\n" );
}

GXVoid GXCALL EMGame::OnYUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnYUp\n" );
}

GXVoid GXCALL EMGame::OnUDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnUDown\n" );
}

GXVoid GXCALL EMGame::OnUUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnUUp\n" );
}

GXVoid GXCALL EMGame::OnIDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnIDown\n" );
}

GXVoid GXCALL EMGame::OnIUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnIUp\n" );
}

GXVoid GXCALL EMGame::OnODown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnODown\n" );
}

GXVoid GXCALL EMGame::OnOUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnOUp\n" );
}

GXVoid GXCALL EMGame::OnPDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnPDown\n" );
}

GXVoid GXCALL EMGame::OnPUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnPUp\n" );
}

GXVoid GXCALL EMGame::OnADown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnADown\n" );
}

GXVoid GXCALL EMGame::OnAUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnAUp\n" );
}

GXVoid GXCALL EMGame::OnSDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSDown\n" );
}

GXVoid GXCALL EMGame::OnSUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSUp\n" );
}

GXVoid GXCALL EMGame::OnDDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnDDown\n" );
}

GXVoid GXCALL EMGame::OnDUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnDUp\n" );
}

GXVoid GXCALL EMGame::OnFDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnFDown\n" );
}

GXVoid GXCALL EMGame::OnFUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnFUp\n" );
}

GXVoid GXCALL EMGame::OnGDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnGDown\n" );
}

GXVoid GXCALL EMGame::OnGUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnGUp\n" );
}

GXVoid GXCALL EMGame::OnHDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnHDown\n" );
}

GXVoid GXCALL EMGame::OnHUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnHUp\n" );
}

GXVoid GXCALL EMGame::OnJDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnJDown\n" );
}

GXVoid GXCALL EMGame::OnJUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnJUp\n" );
}

GXVoid GXCALL EMGame::OnKDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnKDown\n" );
}

GXVoid GXCALL EMGame::OnKUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnKUp\n" );
}

GXVoid GXCALL EMGame::OnLDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLDown\n" );
}

GXVoid GXCALL EMGame::OnLUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLUp\n" );
}

GXVoid GXCALL EMGame::OnZDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnZDown\n" );
}

GXVoid GXCALL EMGame::OnZUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnZUp\n" );
}

GXVoid GXCALL EMGame::OnXDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnXDown\n" );
}

GXVoid GXCALL EMGame::OnXUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnXUp\n" );
}

GXVoid GXCALL EMGame::OnCDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnCDown\n" );
}

GXVoid GXCALL EMGame::OnCUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnCUp\n" );
}

GXVoid GXCALL EMGame::OnVDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnVDown\n" );
}

GXVoid GXCALL EMGame::OnVUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnVUp\n" );
}

GXVoid GXCALL EMGame::OnBDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnBDown\n" );
}

GXVoid GXCALL EMGame::OnBUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnBUp\n" );
}

GXVoid GXCALL EMGame::OnNDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNDown\n" );
}

GXVoid GXCALL EMGame::OnNUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnNUp\n" );
}

GXVoid GXCALL EMGame::OnMDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnMDown\n" );
}

GXVoid GXCALL EMGame::OnMUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnMUp\n" );
}

GXVoid GXCALL EMGame::OnSpaceDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSpaceDown\n" );
}

GXVoid GXCALL EMGame::OnSpaceUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSpaceUp\n" );
}

GXVoid GXCALL EMGame::OnEscapeDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEscapeDown\n" );
}

GXVoid GXCALL EMGame::OnEscapeUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEscapeUp\n" );
}

GXVoid GXCALL EMGame::OnBackspaceDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnBackspaceDown\n" );
}

GXVoid GXCALL EMGame::OnBackspaceUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnBackspaceUp\n" );
}

GXVoid GXCALL EMGame::OnEnterDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEnterDown\n" );
}

GXVoid GXCALL EMGame::OnEnterUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnEnterUp\n" );
}

GXVoid GXCALL EMGame::OnLessDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLessDown\n" );
}

GXVoid GXCALL EMGame::OnLessUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnLessUp\n" );
}

GXVoid GXCALL EMGame::OnGreaterDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnGreaterDown\n" );
}

GXVoid GXCALL EMGame::OnGreaterUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnGreaterUp\n" );
}

GXVoid GXCALL EMGame::OnSlashDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSlashDown\n" );
}

GXVoid GXCALL EMGame::OnSlashUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSlashUp\n" );
}

GXVoid GXCALL EMGame::OnSemicolonDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSemicolonDown\n" );
}

GXVoid GXCALL EMGame::OnSemicolonUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnSemicolonUp\n" );
}

GXVoid GXCALL EMGame::OnQuoteDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnQuoteDown\n" );
}

GXVoid GXCALL EMGame::OnQuoteUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnQuoteUp\n" );
}

GXVoid GXCALL EMGame::OnOpeningSquareBracketDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnOpeningSquareBracketDown\n" );
}

GXVoid GXCALL EMGame::OnOpeningSquareBracketUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnOpeningSquareBracketUp\n" );
}

GXVoid GXCALL EMGame::OnClosingSquareBracketDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnClosingSquareBracketDown\n" );
}

GXVoid GXCALL EMGame::OnClosingSquareBracketUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnClosingSquareBracketUp\n" );
}

GXVoid GXCALL EMGame::OnBackslashDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnBackslashDown\n" );
}

GXVoid GXCALL EMGame::OnBackslashUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnBackslashUp\n" );
}

GXVoid GXCALL EMGame::OnTildeDown ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnTildeDown\n" );
}

GXVoid GXCALL EMGame::OnTildeUp ( GXVoid* /*context*/ )
{
    GXLogA ( "EMGame::OnTildeUp\n" );
}
