#include <GXEngine_Editor_Mobile/EMEnvironment.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine_Editor_Mobile/EMDiffuseIrradianceGeneratorMaterial.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXLogger.h>


#define CUBE_SCALE				7.77f

//---------------------------------------------------------------------------------------------------------------------

EMEnvironment* EMEnvironment::instance = nullptr;

EMEnvironment& GXCALL EMEnvironment::GetInstance ()
{
	if ( !instance )
		instance = new EMEnvironment ();

	return *instance;
}

EMEnvironment::~EMEnvironment ()
{
	instance = nullptr;
}

GXVoid EMEnvironment::SetEnvironmentMap ( GXTextureCubeMap& cubeMap )
{
	environment = &cubeMap;
	environmentMapMaterial.SetEnvironmentMap ( cubeMap );
}

GXVoid EMEnvironment::SetEnvironmentQuasiDistance ( GXFloat meters )
{
	environmentMapMaterial.SetEnvironmentQuasiDistance ( meters );
}

GXTextureCubeMap& EMEnvironment::GetEnvironmentMap ()
{
	return *environment;
}

GXVoid EMEnvironment::Render ( GXFloat deltaTime )
{
	if ( !environment || !environment->IsInitialized () ) return;

	GXOpenGLState state;
	state.Save ();

	glEnable ( GL_CULL_FACE );
	glCullFace ( GL_BACK );

	environmentMapMaterial.SetDeltaTime ( deltaTime );
	environmentMapMaterial.Bind ( cube );
	cube.Render ();
	environmentMapMaterial.Unbind ();

	state.Restore ();
}

GXVoid EMEnvironment::OnViewerLocationChanged ()
{
	GXVec3 viewerLocation;
	EMViewer::GetInstance ()->GetCamera ().GetLocation ( viewerLocation );
	cube.SetLocation ( viewerLocation );
}

EMEnvironment::EMEnvironment ():
	cube ( L"Meshes/System/Unit Cube.stm" ),
	environment ( nullptr )
{
	GXRenderer& renderer = GXRenderer::GetInstance ();

	environmentMapMaterial.SetDepthTexture ( EMRenderer::GetInstance ().GetDepthTexture () );
	environmentMapMaterial.SetScreenResolution ( static_cast<GXUShort> ( renderer.GetWidth () ), static_cast<GXUShort> ( renderer.GetHeight () ) );

	cube.SetScale ( CUBE_SCALE, CUBE_SCALE, CUBE_SCALE );

	OnViewerLocationChanged ();
}
