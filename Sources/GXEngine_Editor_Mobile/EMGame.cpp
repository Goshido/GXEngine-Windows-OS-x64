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
#include <GXPhysics/GXSphereShape.h>
#include <GXPhysics/GXBoxShape.h>


#define EM_WINDOW_NAME                  L"GXEditor Mobile"
#define ENVIRONMENT_QUASI_DISTANCE      7.77f
#define FLUTTERSHY_SCALE                2.0e-2f

//---------------------------------------------------------------------------------------------------------------------

GXBool EMGame::isContact;
GXMat4 EMGame::contactNormalTransform;

EMGame::EMGame ():
    hudCamera ( nullptr ),
    openFile ( nullptr ),
    menu ( nullptr ),
    filePopup ( nullptr ),
    createPopup ( nullptr ),
    toolsPopup ( nullptr ),
    utilityPopup ( nullptr ),
    effectsPopup ( nullptr ),
    physicsInfo ( nullptr ),
    physicsContactPointMesh ( nullptr ),
    physicsContactPointMaterial ( nullptr ),
    gravity ( GXVec3 ( 0.0f, -9.81f, 0.0f ) ),
    directedLight ( nullptr ),
    unitActor ( nullptr ),
    colliderOne ( nullptr ),
    colliderTwo ( nullptr ),
    kinematicPlane ( nullptr ),
    plasticSphere ( nullptr ),
    goldSphere ( nullptr ),
    silverSphere ( nullptr ),
    moveTool ( nullptr ),
    fluttershy ( nullptr ),
    contactLocationMesh ( nullptr ),
    contactLocationMaterial ( nullptr ),
    contactNormalMesh ( nullptr ),
    contactNormalMaterial ( nullptr ),
    environmentMap ( nullptr ),
    lightProbeSourceTexture ( nullptr ),
    lightProbe ( nullptr ),
    uiInput ( nullptr ),
    physicsInfoFont ( nullptr )
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
    locale.LoadLanguage ( L"Locale/Editor Mobile/RU.lng", eGXLanguage::Russian );
    locale.LoadLanguage ( L"Locale/Editor Mobile/EN.lng", eGXLanguage::English );
    locale.SetLanguage ( eGXLanguage::Russian );

    GXInput& input = GXInput::GetInstance ();
    input.BindKeyCallback ( this, &EMGame::OnExit, VK_ESCAPE, eGXInputButtonState::Up );
    input.BindMouseButtonCallback ( this, &EMGame::OnMouseButton );

    uiInput = new GXUIInput ( nullptr, GX_FALSE );
    GXTouchSurface::GetInstance ().SetDefaultWidget ( uiInput );

    GXFloat w = static_cast<GXFloat> ( coreRenderer.GetWidth () );
    GXFloat h = static_cast<GXFloat> ( coreRenderer.GetHeight () );

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

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    physicsInfo = new GXHudSurface ( static_cast<GXUShort> ( physicsInfoWidth ), static_cast<GXUShort> ( physicsInfoHeight ) );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXFont" );
    physicsInfoFont = new GXFont ( L"Fonts/trebuc.ttf", static_cast<GXUShort> ( 0.4f * gx_ui_Scale ) );

    physicsInfo->SetLocation ( 0.5f * ( physicsInfoWidth - w ), 0.5f * ( physicsInfoHeight - h ), 7.0f );

    physicsInfoBackgroundTexture.LoadImage ( L"Textures/System/Default_Diffuse.tga", GX_FALSE, GX_FALSE );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMeshGeometry" );
    physicsContactPointMesh = new GXMeshGeometry ();
    physicsContactPointMesh->LoadMesh ( L"Meshes/System/Unit Sphere.obj" );
    physicsContactPointMaterial = new GXUnlitColorMaterial ();
    physicsContactPointMaterial->SetColor ( 255u, 0u, 0u, 255u );

    GXPhysicsEngine::GetInstance ().SetTimeMultiplier ( 1.0f );
    GXPhysicsEngine& physicsEngine = GXPhysicsEngine::GetInstance ();
    GXWorld& world = physicsEngine.GetWorld ();

    GXTransform transform;
    unitActor = new EMUnitActor ( L"Unit actor 01", transform );

    //transform.SetLocation ( 1.0f, 4.0f, 0.0f );
    transform.SetLocation ( 1.0f, 4.0f, 0.0f );
    transform.SetRotation ( 0.5f, 0.0f, 0.3f );
    transform.SetScale ( 1.0f, 1.0f, 1.0f );
    colliderOne = new EMPhysicsDrivenActor ( L"Collider One", transform );
    GXBoxShape* colliderOneShape = new GXBoxShape ( &( colliderOne->GetRigidBody () ), 1.0f, 1.0f, 1.0f );
    colliderOneShape->SetRestitution ( 0.5f );
    colliderOne->GetRigidBody ().SetMass ( 1.0f );
    colliderOne->GetRigidBody ().SetShape ( *colliderOneShape );
    //colliderOne->GetRigidBody ().SetCanSleep ( GX_FALSE );
    //colliderOne->GetRigidBody ().EnableKinematic ();
    //colliderOne->GetRigidBody ().SetLinearVelocity ( GXVec3 ( 0.0, -1.0f, 1.0f ) );
    colliderOne->GetRigidBody ().SetAngularVelocity ( GXVec3 ( 0.0f, 10.0f, -5.0f ) );
    colliderOne->SetMesh ( L"Meshes/System/Unit Cube.stm" );
    EMCookTorranceCommonPassMaterial& colliderOneMaterial = colliderOne->GetMaterial ();
    colliderOneMaterial.SetAlbedoColor ( 253u, 180u, 17u, 255u );
    colliderOneMaterial.SetRoughnessScale ( 0.25f );
    colliderOneMaterial.SetIndexOfRefractionScale ( 0.094f );
    colliderOneMaterial.SetSpecularIntensityScale ( 0.998f );
    colliderOneMaterial.SetMetallicScale ( 1.0f );
    colliderOneMaterial.SetEmissionColorScale ( 0.0f );
    colliderOne->EnablePhysicsDebug ();

    world.RegisterForceGenerator ( colliderOne->GetRigidBody (), gravity );

    transform.SetLocation ( 3.0f, 0.5f, 0.0f );
    //transform.SetLocation ( 1.0f, 1.0f, 0.0f );
    transform.SetRotation ( 0.0f, 0.0f, 0.0f );
    transform.SetScale ( 1.0f, 1.0f, 1.0f );
    colliderTwo = new EMPhysicsDrivenActor ( L"Collider Two", transform );
    GXSphereShape* colliderTwoShape = new GXSphereShape ( &( colliderTwo->GetRigidBody () ), 0.5f );
    colliderTwoShape->SetRestitution ( 0.8f );
    colliderTwo->GetRigidBody ().SetMass ( 1.0f );
    colliderTwo->GetRigidBody ().SetShape ( *colliderTwoShape );
    //colliderTwo->GetRigidBody ().SetCanSleep ( GX_FALSE );
    //colliderTwo->GetRigidBody ().EnableKinematic ();
    colliderTwo->GetRigidBody ().SetAngularVelocity ( GXVec3 ( 0.0f, 0.0f, 35.0f ) );
    colliderTwo->SetMesh ( L"Meshes/System/Unit Sphere.obj" );
    EMCookTorranceCommonPassMaterial& colliderTwoMaterial = colliderTwo->GetMaterial ();
    colliderTwoMaterial.SetAlbedoColor ( 247u, 244u, 233u, 255u );
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
    kinematicPlane->GetRigidBody ().SetMass ( 777.777e+7f );
    kinematicPlane->GetRigidBody ().SetShape ( *kinematicPlaneShape );
    kinematicPlane->GetRigidBody ().EnableKinematic ();
    //kinematicPlane->GetRigidBody ().SetLinearVelocity ( GXVec3 ( 0.0, 0.1f, 0.0f ) );
    //kinematicPlane->GetRigidBody ().SetAngularVelocity ( GXVec3 ( 0.0f, 0.0f, 0.2f ) );
    kinematicPlaneShape->SetRestitution ( 0.95f );
    kinematicPlane->SetMesh ( L"Meshes/System/Unit Cube.stm" );
    EMCookTorranceCommonPassMaterial& kinematicPlaneMaterial = kinematicPlane->GetMaterial ();
    kinematicPlaneMaterial.SetRoughnessScale ( 0.07f );
    kinematicPlaneMaterial.SetSpecularIntensityScale ( 0.15f );
    kinematicPlaneMaterial.SetMetallicScale ( 0.0f );
    kinematicPlaneMaterial.SetIndexOfRefractionScale ( 0.292f );
    GXTexture2D* texture = kinematicPlaneMaterial.GetAlbedoTexture ();
    texture->FreeResources ();
    texture->LoadImage ( L"Textures/System/GXEngine Logo 4k.png", GX_TRUE, GX_TRUE );
    kinematicPlane->EnablePhysicsDebug ();

    transform.SetLocation ( -2.0f, 0.0f, 0.0f );
    transform.SetScale ( 1.0f, 1.0, 1.0f );
    plasticSphere = new EMMeshActor ( L"Plastic sphere", transform );
    plasticSphere->SetMesh ( L"Meshes/System/Unit Sphere.obj" );
    EMCookTorranceCommonPassMaterial& plasticSphereMaterial = plasticSphere->GetMaterial ();
    plasticSphereMaterial.SetAlbedoColor ( 115u, 185u, 0u, 255u );
    plasticSphereMaterial.SetRoughnessScale ( 0.5f );
    plasticSphereMaterial.SetIndexOfRefractionScale ( 0.292f );
    plasticSphereMaterial.SetSpecularIntensityScale ( 0.75f );
    plasticSphereMaterial.SetMetallicScale ( 1.0f );
    plasticSphereMaterial.SetEmissionColorScale ( 0.0f );

    transform.SetLocation ( 4.0f, 0.0f, 0.0f );
    goldSphere = new EMMeshActor ( L"Gold sphere", transform );
    goldSphere->SetMesh ( L"Meshes/System/Unit Sphere.obj" );
    EMCookTorranceCommonPassMaterial& goldSphereMaterial = goldSphere->GetMaterial ();
    goldSphereMaterial.SetAlbedoColor ( 253u, 180u, 17u, 255u );
    goldSphereMaterial.SetRoughnessScale ( 0.25f );
    goldSphereMaterial.SetIndexOfRefractionScale ( 0.094f );
    goldSphereMaterial.SetSpecularIntensityScale ( 0.998f );
    goldSphereMaterial.SetMetallicScale ( 1.0f );
    goldSphereMaterial.SetEmissionColorScale ( 0.0f );

    transform.SetLocation ( 6.0f, 0.0f, 0.0f );
    silverSphere = new EMMeshActor ( L"Silver sphere", transform );
    silverSphere->SetMesh ( L"Meshes/System/Unit Sphere.obj" );
    EMCookTorranceCommonPassMaterial& silverSphereMaterial = silverSphere->GetMaterial ();
    silverSphereMaterial.SetAlbedoColor ( 247u, 244u, 233u, 255u );
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

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXCameraOrthographic" );
    hudCamera = new GXCameraOrthographic ( w, h, EM_UI_HUD_CAMERA_NEAR_Z, EM_UI_HUD_CAMERA_FAR_Z );

    EMViewer* viewer = EMViewer::GetInstance ();
    viewer->SetOnViewerTransformChangedCallback ( this, &EMGame::OnViewerTransformChanged );
    viewer->SetInputWidget ( *uiInput );
    viewer->SetTarget ( unitActor );
    GXCamera::SetActiveCamera ( &viewer->GetCamera () );

    EMTool::SetActiveTool ( moveTool );

    fluttershy = new EMFluttershy ();
    fluttershy->SetScale ( FLUTTERSHY_SCALE, FLUTTERSHY_SCALE, FLUTTERSHY_SCALE );
    // fluttershy->SetRotation ( -GX_MATH_HALF_PI, 0.0f, 0.0f );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMeshGeometry" );
    contactLocationMesh = new GXMeshGeometry ();
    contactLocationMesh->LoadMesh ( L"Meshes/System/Unit Sphere.obj" );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMeshGeometry" );
    contactNormalMesh = new GXMeshGeometry ();
    contactNormalMesh->LoadMesh ( L"Meshes/Editor Mobile/Move gismo Z axis.stm" );

    contactLocationMaterial = new GXUnlitColorMaterial ();
    contactLocationMaterial->SetColor ( 255u, 255u, 255u, 255u );

    contactNormalMaterial = new GXUnlitColorMaterial ();
    contactNormalMaterial->SetColor ( 0u, 0u, 255u, 255u );

    environmentMap = new GXTextureCubeMap ();
    environmentMap->LoadEquirectangularImage ( L"Textures/Editor Mobile/Default LDR environment map.jpg", GX_FALSE, GX_TRUE );
    //environmentMap->LoadEquirectangularImage ( L"Textures/Editor Mobile/Default HDR environment map.hdr", GX_TRUE, GX_FALSE );

    lightProbeSourceTexture = new GXTextureCubeMap ();
    lightProbeSourceTexture->LoadEquirectangularImage ( L"Textures/Editor Mobile/Default HDR environment map.hdr", GX_TRUE, GX_FALSE );

    lightProbe = new EMLightProbe ();
    lightProbe->SetEnvironmentMap ( *lightProbeSourceTexture );

    EMEnvironment& environment = EMEnvironment::GetInstance ();
    environment.SetEnvironmentMap ( *environmentMap );
    environment.SetEnvironmentQuasiDistance ( ENVIRONMENT_QUASI_DISTANCE );

    EMUIFPSCounter::GetInstance ();
    EMUIColorPicker::GetInstance ();

    ShowCursor ( 1 );
    SetCursor ( LoadCursorW ( 0, IDC_ARROW ) );

    physicsEngine.Start ();
}

GXVoid EMGame::OnResize ( GXInt width, GXInt height )
{
    GXLogW ( L"EMGame::OnResize::Info - Новый размер %i x %i\n", width, height );
}

GXVoid EMGame::OnFrame ( GXFloat deltaTime )
{
    GXTouchSurface::GetInstance ().ExecuteMessages ();

    /*if ( deltaTime < 0.2f )
        GXPhysicsEngine::GetInstance ().RunSimulateLoop ( deltaTime );*/

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

    GXOpenGLState state;
    GXTransform transform;

    if ( isContact )
    {
        state.Save ();

        transform.SetCurrentFrameModelMatrix ( contactNormalTransform );
        transform.SetScale ( 0.1f, 0.1f, 0.1f );

        contactLocationMaterial->Bind ( transform );
        contactLocationMesh->Render ();
        contactLocationMaterial->Unbind ();

        transform.SetScale ( 1.0f, 1.0f, 1.0f );

        contactNormalMaterial->Bind ( transform );
        contactNormalMesh->Render ();
        contactNormalMaterial->Unbind ();

        glDisable ( GL_DEPTH_TEST );

        state.Restore ();
    }

    GXCollisionDetector& collisionDetector = GXCollisionDetector::GetInstance ();
    const GXCollisionData& collisionData = GXPhysicsEngine::GetInstance ().GetWorld ().GetCollisionData ();
    GXContact* contact = collisionData.GetAllContacts ();

    if ( collisionData.GetTotalContacts () > 0 )
    {
        state.Save ();

        glDisable ( GL_DEPTH_TEST );

        transform.SetScale ( 0.1f, 0.1f, 0.1f );

        for ( GXUInt i = 0u; i < collisionData.GetTotalContacts (); ++i )
        {
            transform.SetLocation ( contact[ i ].GetContactPoint () );

            physicsContactPointMaterial->Bind ( transform );
            physicsContactPointMesh->Render ();
            physicsContactPointMaterial->Unbind ();
        }

        state.Restore ();
    }

    GXCamera::SetActiveCamera ( hudCamera );

    EMDrawUI ();

    physicsInfo->Reset ();

    GXImageInfo ii;
    ii.color.From ( 0u, 0u, 0u, 180u );
    ii.texture = &physicsInfoBackgroundTexture;
    ii.insertX = 0.0f;
    ii.insertY = 0.0f;
    ii.insertWidth = static_cast<GXFloat> ( physicsInfo->GetWidth () );
    ii.insertHeight = static_cast<GXFloat> ( physicsInfo->GetHeight () );
    ii.overlayType = eGXImageOverlayType::SimpleReplace;

    physicsInfo->AddImage ( ii );

    GXPenInfo pi;
    pi.color.From ( 115u, 185u, 0u, 255u );
    pi.font = physicsInfoFont;
    pi.insertX = 0.5f * gx_ui_Scale;
    pi.insertY = 0.5f * gx_ui_Scale;
    pi.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

    physicsInfo->AddText ( pi, 64, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Contacts: %i" ), collisionData.GetTotalContacts () );

    GXFloat offset = static_cast<GXFloat> ( physicsInfoFont->GetSize () );
    pi.insertY += offset;
    physicsInfo->AddText ( pi, 128u, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Allocated support points: %i" ), collisionDetector.GetAllocatedSupportPoints () );

    pi.insertY += offset;
    physicsInfo->AddText ( pi, 128u, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Allocated edges: %i" ), collisionDetector.GetAllocatedEdges () );

    pi.insertY += offset;
    physicsInfo->AddText ( pi, 128u, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Allocated faces: %i" ), collisionDetector.GetAllocatedFaces () );

    if ( collisionData.GetTotalContacts () > 0 )
    {
        pi.insertY += offset;
        physicsInfo->AddText ( pi, 128u, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Penetration depth: %f" ), contact->GetPenetration () );

        pi.insertY += offset;
        physicsInfo->AddText ( pi, 128u, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->GJK iterations: %i" ), contact->GetGJKIterations () );

        pi.insertY += offset;
        physicsInfo->AddText ( pi, 128u, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->EPA iterations: %i" ), contact->GetEPAIterations () );

        pi.insertY += offset;
        physicsInfo->AddText ( pi, 128u, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Used support points: %i" ), contact->GetSupportPoints () );

        pi.insertY += offset;
        physicsInfo->AddText ( pi, 128u, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Used edges: %i" ), contact->GetEdges () );

        pi.insertY += offset;
        physicsInfo->AddText ( pi, 128u, GXLocale::GetInstance ().GetString ( L"EMGame->Physics info->Used faces: %i" ), contact->GetFaces () );
    }

    physicsInfo->Render ();

    EMUIFPSCounter::GetInstance ().Render ();

    renderer.StartHudMaskPass ();

    GXCamera::SetActiveCamera ( &viewerCamera );

    moveTool->OnDrawHudMaskPass ();

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

    delete lightProbeSourceTexture;
    delete environmentMap;

    GXSafeDelete ( contactNormalMaterial );
    GXSafeDelete ( contactNormalMesh );

    GXSafeDelete ( contactLocationMaterial );
    GXSafeDelete ( contactLocationMesh );

    GXSafeDelete ( fluttershy );
    GXSafeDelete ( moveTool );

    GXSafeDelete ( silverSphere );
    GXSafeDelete ( goldSphere );
    GXSafeDelete ( plasticSphere );

    GXWorld& world = GXPhysicsEngine::GetInstance ().GetWorld ();

    // world.UnregisterForceGenerator ( kinematicPlane->GetRigidBody (), gravity );
    world.UnregisterForceGenerator ( colliderOne->GetRigidBody (), gravity );
    world.UnregisterForceGenerator ( colliderTwo->GetRigidBody (), gravity );

    GXSafeDelete ( kinematicPlane );
    GXSafeDelete ( colliderOne );
    GXSafeDelete ( colliderTwo );
    GXSafeDelete ( unitActor );

    GXSafeDelete ( directedLight );

    GXSafeDelete ( physicsContactPointMaterial );
    GXSafeDelete ( physicsContactPointMesh );
    physicsInfoBackgroundTexture.FreeResources ();
    delete physicsInfoFont;
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

    GXUShort x = (GXUShort)mouse.GetX ();
    GXUShort y = (GXUShort)mouse.GetY ();

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

    const GXFloat rayLength = 10000.0f;
    GXVec3 contactLocation;
    GXVec3 contactNormal;
    const GXShape* contactShape;

    isContact = GXPhysicsEngine::GetInstance ().GetWorld ().Raycast ( rayOrigin, rayDirection, rayLength, contactLocation, contactNormal, &contactShape );
    if ( !isContact ) return;

    contactNormalTransform.From ( contactNormal, contactLocation );

    const GXFloat impulseModule = 3.0f;
    GXVec3 impulseWorld;
    impulseWorld.Multiply ( rayDirection, impulseModule );

    contactShape->GetRigidBody ().AddImpulseAtPointWorld ( impulseWorld, contactLocation );
}

GXVoid GXCALL EMGame::OnObject ( GXVoid* handler, GXVoid* object )
{
    EMGame* game = static_cast<EMGame*> ( handler );
    if ( game->moveTool->OnObject ( object ) ) return;

    EMActor* actor = static_cast<EMActor*> ( object );

    if ( actor )
        GXLogW ( L"EMOnObject::Info - Объект %s\n", actor->GetName () );

    game->moveTool->SetActor ( actor );
    EMViewer::GetInstance ()->SetTarget ( actor );
}

GXVoid GXCALL EMGame::OnViewerTransformChanged ( GXVoid* handler )
{
    EMGame* game = static_cast<EMGame*> ( handler );
    game->moveTool->OnViewerTransformChanged ();
    EMEnvironment::GetInstance ().OnViewerLocationChanged ();
}

GXVoid GXCALL EMGame::OnOpenFile ( const GXWChar* filePath )
{
    GXLogW ( L"EMOnOpenFile::Info - Файл %s\n", filePath );
}
