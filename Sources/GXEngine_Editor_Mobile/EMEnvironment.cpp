#include <GXEngine_Editor_Mobile/EMEnvironment.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine_Editor_Mobile/EMDiffuseIrradianceGeneratorMaterial.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXLogger.h>


#define CUBE_SCALE				7.77f
#define INVALID_TEXTURE_OBJECT	0


EMEnvironment* EMEnvironment::instance = nullptr;

EMEnvironment& GXCALL EMEnvironment::GetInstance ()
{
	if ( !instance )
		instance = new EMEnvironment ();

	return *instance;
}

EMEnvironment::~EMEnvironment ()
{
	if ( environment.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		GXTextureCubeMap::RemoveTexture ( environment );

	instance = nullptr;
}

GXVoid EMEnvironment::SetEnvironmentMap ( const GXWChar* cubeMap )
{
	if ( environment.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		GXTextureCubeMap::RemoveTexture ( environment );

	//environment = GXTextureCubeMap::LoadEquirectangularTexture ( cubeMap, GX_TRUE );
	//return;
	GXOpenGLState state;
	state.Save ();

	GXTextureCubeMap map = GXTextureCubeMap::LoadEquirectangularTexture ( cubeMap, GX_FALSE );
	GXUShort length = map.GetFaceLength ();
	environment.InitResources ( length, GL_RGB8, GL_FALSE );

	GLuint fbo;
	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, environment.GetTextureObject (), 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, 0, 0 );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0 );

	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );
	glDisable ( GL_BLEND );

	glDepthMask ( GL_FALSE );
	glStencilMask ( 0x00 );
	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE );

	glViewport ( 0, 0, (GLsizei)length, (GLsizei)length );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMEnvironment::SetEnvironmentMap::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	EMDiffuseIrradianceGeneratorMaterial material;
	material.SetEnvironmentMap ( map );

	GXLogW ( L"Сэмлов %u\n", material.SetAngleStep ( 0.025f ) );

	material.Bind ( cube );
	cube.Render ();
	material.Unbind ();

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	state.Restore ();

	GXTextureCubeMap::RemoveTexture ( map );

}

GXVoid EMEnvironment::SetEnvironmentQuasiDistance ( GXFloat meters )
{
	environmentMapMaterial.SetEnvironmentQuasiDistance ( meters );
}

GXVoid EMEnvironment::Render ( GXFloat deltaTime )
{
	if ( environment.GetTextureObject () == INVALID_TEXTURE_OBJECT ) return;

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

EMEnvironment::EMEnvironment () :
cube ( L"3D Models/System/Unit Cube.stm" )
{
	GXRenderer& renderer = GXRenderer::GetInstance ();

	environmentMapMaterial.SetEnvironmentMap ( environment );
	environmentMapMaterial.SetDepthTexture ( EMRenderer::GetInstance ().GetDepthTexture () );
	environmentMapMaterial.SetScreenResolution ( (GXUShort)renderer.GetWidth (), (GXUShort)renderer.GetHeight () );

	cube.SetScale ( CUBE_SCALE, CUBE_SCALE, CUBE_SCALE );

	OnViewerLocationChanged ();
}
