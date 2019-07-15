#include <GXEngine_Editor_Mobile/EMEnvironment.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine_Editor_Mobile/EMDiffuseIrradianceGeneratorMaterial.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXLogger.h>


#define CUBE_SCALE      7.77f

//---------------------------------------------------------------------------------------------------------------------

EMEnvironment* EMEnvironment::_instance = nullptr;

EMEnvironment& GXCALL EMEnvironment::GetInstance ()
{
    if ( !_instance )
        _instance = new EMEnvironment ();

    return *_instance;
}

EMEnvironment::~EMEnvironment ()
{
    _instance = nullptr;
}

GXVoid EMEnvironment::SetEnvironmentMap ( GXTextureCubeMap& cubeMap )
{
    _environment = &cubeMap;
    _environmentMapMaterial.SetEnvironmentMap ( cubeMap );
}

GXVoid EMEnvironment::SetEnvironmentQuasiDistance ( GXFloat meters )
{
    _environmentMapMaterial.SetEnvironmentQuasiDistance ( meters );
}

GXTextureCubeMap& EMEnvironment::GetEnvironmentMap ()
{
    return *_environment;
}

GXVoid EMEnvironment::Render ( GXFloat deltaTime )
{
    if ( !_environment || !_environment->IsInitialized () ) return;

    GXOpenGLState state;
    state.Save ();

    glEnable ( GL_CULL_FACE );
    glCullFace ( GL_BACK );

    _environmentMapMaterial.SetDeltaTime ( deltaTime );
    _environmentMapMaterial.Bind ( _cube );
    _cube.Render ();
    _environmentMapMaterial.Unbind ();

    state.Restore ();
}

GXVoid EMEnvironment::OnViewerLocationChanged ()
{
    GXVec3 viewerLocation;
    EMViewer::GetInstance ()->GetCamera ().GetLocation ( viewerLocation );
    _cube.SetLocation ( viewerLocation );
}

EMEnvironment::EMEnvironment ():
    _cube ( L"Meshes/System/Unit Cube.stm" ),
    _environment ( nullptr )
{
    GXRenderer& renderer = GXRenderer::GetInstance ();

    _environmentMapMaterial.SetDepthTexture ( EMRenderer::GetInstance ().GetDepthTexture () );
    _environmentMapMaterial.SetScreenResolution ( static_cast<GXUShort> ( renderer.GetWidth () ), static_cast<GXUShort> ( renderer.GetHeight () ) );

    _cube.SetScale ( CUBE_SCALE, CUBE_SCALE, CUBE_SCALE );

    OnViewerLocationChanged ();
}
