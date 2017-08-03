#include <GXEngine/GXEquirectangularToCubeMapMaterial.h>
#include <GXEngine/GXCameraPerspective.h>


#define VERTEX_SHADER			L"Shaders/System/VertexPass_vs.txt"
#define GEOMETRY_SHADER			L"Shaders/System/CubeMapSplitter_gs.txt"
#define FRAGMENT_SHADER			L"Shaders/System/EquirectangularToCubeMap_fs.txt"

#define TEXTURE_SLOT			0

#define SQUARE_ASPECT_RATIO		1.0f
#define Z_NEAR					0.1f
#define Z_FAR					777.777f
#define PROJECTION_FOV_Y		GX_MATH_HALF_PI


GXEquirectangularToCubeMapMaterial::GXEquirectangularToCubeMapMaterial ()
{
	static const GLchar* samplerNames[ 1 ] = { "equirectangularSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 1;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	viewProjectionMatricesLocation = shaderProgram.GetUniform ( "viewProjectionMatrices" );

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

	texture = nullptr;
}

GXEquirectangularToCubeMapMaterial::~GXEquirectangularToCubeMapMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid GXEquirectangularToCubeMapMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !texture ) return;

	glUseProgram ( shaderProgram.GetProgram () );
	glUniformMatrix4fv ( viewProjectionMatricesLocation, 6, GL_FALSE, (const GLfloat*)viewProjectionMatrices );
	texture->Bind ( TEXTURE_SLOT );
}

GXVoid GXEquirectangularToCubeMapMaterial::Unbind ()
{
	if ( !texture ) return;

	glUseProgram ( 0 );
	texture->Unbind ();
}

GXVoid GXEquirectangularToCubeMapMaterial::SetEquirectangularTexture ( GXTexture2D &texture )
{
	this->texture = &texture;
}
