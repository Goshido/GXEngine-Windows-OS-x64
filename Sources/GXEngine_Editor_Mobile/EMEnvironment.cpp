#include <GXEngine_Editor_Mobile/EMEnvironment.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine/GXRenderer.h>


#define CUBE_SCALE	7.77f


EMEnvironment::EMEnvironment ( const GXWChar* environmentMap ) :
cube ( L"3D Models/System/Unit Cube.stm" )
{
	environment = GXTextureCubeMap::LoadEquirectangularTexture ( environmentMap, GX_TRUE );

	GXRenderer& renderer = GXRenderer::GetInstance ();

	environmentMapMaterial.SetEnvironmentMap ( environment );
	environmentMapMaterial.SetDepthTexture ( EMRenderer::GetInstance ().GetDepthTexture () );
	environmentMapMaterial.SetScreenResolution ( (GXUShort)renderer.GetWidth (), (GXUShort)renderer.GetHeight () );

	cube.SetScale ( CUBE_SCALE, CUBE_SCALE, CUBE_SCALE );

	OnViewerLocationChanged ();
}

EMEnvironment::~EMEnvironment ()
{
	GXTextureCubeMap::RemoveTexture ( environment );
}

GXVoid EMEnvironment::Render ()
{
	GXOpenGLState state;
	state.Save ();

	glEnable ( GL_CULL_FACE );
	glCullFace ( GL_BACK );

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
