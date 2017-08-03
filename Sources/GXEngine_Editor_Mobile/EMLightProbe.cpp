#include <GXEngine_Editor_Mobile/EMLightProbe.h>
#include <GXEngine_Editor_Mobile/EMDiffuseIrradianceGeneratorMaterial.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_LOCATION_X							0.0f
#define DEFAULT_LOCATION_Y							0.0f
#define DEFAULT_LOCATION_Z							0.0f

#define DEFAULT_BOUND_RANGE_X						7.0f
#define DEFAULT_BOUND_RANGE_Y						7.0f
#define DEFAULT_BOUND_RANGE_Z						7.0f

#define INVALID_TEXTURE_OBJECT						0

#define DIFFUSE_CONVOLUTION_ANGLE_STEP				0.025f

#define DEFAULT_BRDF_INTEGRATION_MAP_RESOLUTION		512


EMLightProbe::EMLightProbe ()
{
	locationWorld = GXCreateVec3 ( DEFAULT_LOCATION_X, DEFAULT_LOCATION_Y, DEFAULT_LOCATION_Z );
	
	GXSetAABBEmpty ( boundsWorld );

	GXFloat halfRangeX = 0.5f * DEFAULT_BOUND_RANGE_X;
	GXFloat halfRangeY = 0.5f * DEFAULT_BOUND_RANGE_Y;
	GXFloat halfRangeZ = 0.5f * DEFAULT_BOUND_RANGE_Z;

	GXAddVertexToAABB ( boundsWorld, locationWorld.x - halfRangeX, locationWorld.y - halfRangeY, locationWorld.z - halfRangeZ );
	GXAddVertexToAABB ( boundsWorld, locationWorld.x + halfRangeX, locationWorld.y + halfRangeY, locationWorld.z + halfRangeZ );

	cube = GXMeshGeometry::LoadFromStm ( L"3D Models/System/Unit Cube.stm" );
	screenQuad = GXMeshGeometry::LoadFromStm ( L"3D Models/System/ScreenQuad.stm" );

	SetBRDFIntegrationMapResolution ( DEFAULT_BRDF_INTEGRATION_MAP_RESOLUTION );
}

EMLightProbe::~EMLightProbe ()
{
	GXMeshGeometry::RemoveMeshGeometry ( cube );
	GXMeshGeometry::RemoveMeshGeometry ( screenQuad );

	if ( diffuseIrradiance.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		diffuseIrradiance.FreeResources ();

	if ( prefilteredEnvironmentMap.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		prefilteredEnvironmentMap.FreeResources ();

	if ( brdfIntegrationMap.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		brdfIntegrationMap.FreeResources ();
}

GXVoid EMLightProbe::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	locationWorld = GXCreateVec3 ( x, y, z );
	SetBoundLocal ( GXGetAABBWidth ( boundsWorld ), GXGetAABBHeight ( boundsWorld ), GXGetAABBDepth ( boundsWorld ) );
}

GXVoid EMLightProbe::SetEnvironmentMap ( GXTextureCubeMap &cubeMap )
{
	environmentMap = cubeMap;
	UpdateDiffuseIrradiance ();
	UpdatePrefilteredEnvironmentMap ();
}

GXVoid EMLightProbe::SetBoundLocal ( GXFloat xRange, GXFloat yRange, GXFloat zRange )
{
	GXFloat halfRangeX = 0.5f * xRange;
	GXFloat halfRangeY = 0.5f * yRange;
	GXFloat halfRangeZ = 0.5f * zRange;

	GXSetAABBEmpty ( boundsWorld );
	GXAddVertexToAABB ( boundsWorld, locationWorld.x - halfRangeX, locationWorld.y - halfRangeY, locationWorld.z - halfRangeZ );
	GXAddVertexToAABB ( boundsWorld, locationWorld.x + halfRangeX, locationWorld.y + halfRangeY, locationWorld.z + halfRangeZ );
}

GXUInt EMLightProbe::SetDiffuseIrradianceConvolutionAngleStep ( GXFloat radians )
{
	GXUInt samples = diffuseIrradianceGeneratorMaterial.SetAngleStep ( radians );
	UpdateDiffuseIrradiance ();

	return samples;
}

GXVoid EMLightProbe::SetBRDFIntegrationMapResolution ( GXUShort length )
{
	if ( brdfIntegrationMap.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		brdfIntegrationMap.FreeResources ();

	brdfIntegrationMap.InitResources ( length, length, GL_RG8, GL_TRUE, GL_CLAMP_TO_EDGE );
	UpdateBRDFIntegrationMap ();
}

GXVoid EMLightProbe::SetBRDFIntegrationMapSamples ( GXUShort samples )
{
	brdfIntegratorMaterial.SetTotalSamples ( samples );
	UpdateBRDFIntegrationMap ();
}

GXTextureCubeMap& EMLightProbe::GetDiffuseIrradiance ()
{
	return diffuseIrradiance;
}

GXTextureCubeMap& EMLightProbe::GetPrefilteredEnvironmentMap ()
{
	return prefilteredEnvironmentMap;
}

GXTexture2D& EMLightProbe::GetBRDFIntegrationMap ()
{
	return brdfIntegrationMap;
}

GXVoid EMLightProbe::UpdateDiffuseIrradiance ()
{
	if ( environmentMap.GetTextureObject () == INVALID_TEXTURE_OBJECT ) return;

	GXOpenGLState state;
	state.Save ();

	GXUShort length = environmentMap.GetFaceLength ();

	if ( diffuseIrradiance.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		diffuseIrradiance.FreeResources ();

	diffuseIrradiance.InitResources ( length, GL_RGB16F, GL_FALSE );

	GLuint fbo;
	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diffuseIrradiance.GetTextureObject (), 0 );
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
		GXLogW ( L"EMLightProbe::UpdateDiffuseIrradiance::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	diffuseIrradianceGeneratorMaterial.SetEnvironmentMap ( environmentMap );
	diffuseIrradianceGeneratorMaterial.Bind ( GXTransform::GetNullTransform () );
	cube.Render ();
	diffuseIrradianceGeneratorMaterial.Unbind ();

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	state.Restore ();
}

GXVoid EMLightProbe::UpdatePrefilteredEnvironmentMap ()
{
	if ( environmentMap.GetTextureObject () == INVALID_TEXTURE_OBJECT ) return;

	GXOpenGLState state;
	state.Save ();

	GXUShort length = environmentMap.GetFaceLength ();

	if ( diffuseIrradiance.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		diffuseIrradiance.FreeResources ();

	diffuseIrradiance.InitResources ( length, GL_RGB16F, GL_FALSE );

	GLuint fbo;
	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diffuseIrradiance.GetTextureObject (), 0 );
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
		GXLogW ( L"EMLightProbe::UpdatePrefilteredEnvironmentMap::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	if ( prefilteredEnvironmentMap.GetTextureObject () != INVALID_TEXTURE_OBJECT )
		prefilteredEnvironmentMap.FreeResources ();

	prefilteredEnvironmentMap.InitResources ( length, GL_RGB16F, GL_TRUE );

	GXUChar mipmaps = 0;
	GXUShort currentResolution = 1;
	while ( currentResolution <= length )
	{
		mipmaps++;
		currentResolution *= 2;
	}

	prefilteredEnvironmentMapGeneratorMaterial.SetEnvironmentMap ( environmentMap );

	GXFloat roughnessStep = 1.0f / (GXFloat)( mipmaps - 1 );
	GXInt mipmapLevel = 0;
	currentResolution = length;
	for ( GXFloat rougness = 0.0f; rougness < 1.0f; rougness += roughnessStep )
	{
		glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, prefilteredEnvironmentMap.GetTextureObject (), mipmapLevel );

		glViewport ( 0, 0, (GLsizei)currentResolution, (GLsizei)currentResolution );

		status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
		if ( status != GL_FRAMEBUFFER_COMPLETE )
		{
			GXLogW ( L"EMLightProbe::UpdatePrefilteredEnvironmentMap::Error - Что-то не так на проходе генерации %i mipmap уровня FBO (ошибка 0x%08x)\n", mipmapLevel, status );
			mipmapLevel++;
			continue;
		}

		prefilteredEnvironmentMapGeneratorMaterial.SetRoughness ( rougness );
		prefilteredEnvironmentMapGeneratorMaterial.Bind ( GXTransform::GetNullTransform () );
		cube.Render ();
		prefilteredEnvironmentMapGeneratorMaterial.Unbind ();

		currentResolution /= 2;

		mipmapLevel++;
	}

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	state.Restore ();
}

GXVoid EMLightProbe::UpdateBRDFIntegrationMap ()
{
	GXOpenGLState state;
	state.Save ();

	GLuint fbo;
	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, brdfIntegrationMap.GetTextureObject (), 0 );
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
	glColorMask ( GL_TRUE, GL_TRUE, GL_FALSE, GL_FALSE );

	glViewport ( 0, 0, (GLsizei)brdfIntegrationMap.GetWidth (), (GLsizei)brdfIntegrationMap.GetHeight () );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMLightProbe::UpdateBRDFIntegrationMap::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	brdfIntegratorMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	diffuseIrradianceGeneratorMaterial.Unbind ();

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	state.Restore ();

	brdfIntegrationMap.UpdateMipmaps ();
}
