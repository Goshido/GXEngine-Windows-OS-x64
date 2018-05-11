// version 1.2

#include <GXEngine/GXEquirectangularToCubeMapMaterial.h>
#include <GXEngine/GXCameraPerspective.h>


#define VERTEX_SHADER					L"Shaders/System/VertexPass_vs.txt"
#define GEOMETRY_SHADER					L"Shaders/System/CubeMapSplitter_gs.txt"
#define FRAGMENT_SHADER					L"Shaders/System/EquirectangularToCubeMap_fs.txt"

#define TEXTURE_SLOT					0u

#define SQUARE_ASPECT_RATIO				1.0f
#define Z_NEAR							0.1f
#define Z_FAR							777.777f
#define PROJECTION_FOV_Y				GX_MATH_HALF_PI

#define ENABLE_GAMMA_CORRECTION_VALUE	2.2f
#define DISABLE_GAMMA_CORRECTION_VALUE	1.0f


GXEquirectangularToCubeMapMaterial::GXEquirectangularToCubeMapMaterial ():
	equirectangularTexture ( nullptr ),
	sampler ( GL_REPEAT, eGXResampling::Linear, 1.0f )
{
	static const GLchar* samplerNames[ 1 ] = { "equirectangularSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 1;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.transformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );

	viewProjectionMatricesLocation = shaderProgram.GetUniform ( "viewProjectionMatrices" );
	gammaLocation = shaderProgram.GetUniform ( "gamma" );

	GXCameraPerspective camera ( PROJECTION_FOV_Y, SQUARE_ASPECT_RATIO, Z_NEAR, Z_FAR );
	camera.SetRotation ( 0.0f, GX_MATH_HALF_PI, 0.0f );
	viewProjectionMatrices[ 0 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( 0.0f, -GX_MATH_HALF_PI, 0.0f );
	viewProjectionMatrices[ 1 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( GX_MATH_HALF_PI, 0.0f, 0.0f );
	viewProjectionMatrices[ 2 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( -GX_MATH_HALF_PI, 0.0f, 0.0f );
	viewProjectionMatrices[ 3 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( 0.0f, 0.0f, 0.0f );
	viewProjectionMatrices[ 4 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	camera.SetRotation ( 0.0f, GX_MATH_PI, 0.0f );
	viewProjectionMatrices[ 5 ] = camera.GetCurrentFrameViewProjectionMatrix ();

	EnableGammaCorrection ();
}

GXEquirectangularToCubeMapMaterial::~GXEquirectangularToCubeMapMaterial ()
{
	// NOTHING
}

GXVoid GXEquirectangularToCubeMapMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !equirectangularTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );
	glUniformMatrix4fv ( viewProjectionMatricesLocation, 6, GL_FALSE, reinterpret_cast<const GLfloat*> ( viewProjectionMatrices ) );
	glUniform1f ( gammaLocation, gamma );

	equirectangularTexture->Bind ( static_cast<GXUByte> ( TEXTURE_SLOT ) );
	sampler.Bind ( TEXTURE_SLOT );
}

GXVoid GXEquirectangularToCubeMapMaterial::Unbind ()
{
	if ( !equirectangularTexture ) return;

	sampler.Unbind ( TEXTURE_SLOT );
	equirectangularTexture->Unbind ();

	glUseProgram ( 0u );
}

GXVoid GXEquirectangularToCubeMapMaterial::SetEquirectangularTexture ( GXTexture2D &texture )
{
	equirectangularTexture = &texture;
}

GXVoid GXEquirectangularToCubeMapMaterial::EnableGammaCorrection ()
{
	gamma = ENABLE_GAMMA_CORRECTION_VALUE;
}

GXVoid GXEquirectangularToCubeMapMaterial::DisableGammaCorrection ()
{
	gamma = DISABLE_GAMMA_CORRECTION_VALUE;
}
