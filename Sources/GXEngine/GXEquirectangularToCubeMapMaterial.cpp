#include <GXEngine/GXEquirectangularToCubeMapMaterial.h>


#define VERTEX_SHADER			L"Shaders/System/VertexPass_vs.txt"
#define GEOMETRY_SHADER			L"Shaders/System/CubeMapSplitter_gs.txt"
#define FRAGMENT_SHADER			L"Shaders/System/EquirectangularToCubeMap_fs.txt"

#define TEXTURE_SLOT			0

#define QUAD_ASPECT_RATIO		1.0f
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

	modelProjectionMatricesLocation = shaderProgram.GetUniform ( "modelProjectionMatrices" );

	texture = nullptr;

	GXMat4 projectionMatrix;
	GXSetMat4Perspective ( projectionMatrix, PROJECTION_FOV_Y, QUAD_ASPECT_RATIO, Z_NEAR, Z_FAR );
	
	GXMat4 modelMatrix;
	GXSetMat4RotationXYZ( modelMatrix, 0.0f, -GX_MATH_HALF_PI, 0.0f );
	GXMulMat4Mat4 ( modelProjectionMatrices[ 0 ], modelMatrix, projectionMatrix );

	GXSetMat4RotationXYZ ( modelMatrix, 0.0f, GX_MATH_HALF_PI, 0.0f );
	GXMulMat4Mat4 ( modelProjectionMatrices[ 1 ], modelMatrix, projectionMatrix );

	GXSetMat4RotationXYZ ( modelMatrix, -GX_MATH_HALF_PI, 0.0f, 0.0f );
	GXMulMat4Mat4 ( modelProjectionMatrices[ 2 ], modelMatrix, projectionMatrix );

	GXSetMat4RotationXYZ ( modelMatrix, GX_MATH_HALF_PI, 0.0f, 0.0f );
	GXMulMat4Mat4 ( modelProjectionMatrices[ 3 ], modelMatrix, projectionMatrix );

	GXSetMat4RotationXYZ ( modelMatrix, 0.0f, 0.0f, 0.0f );
	GXMulMat4Mat4 ( modelProjectionMatrices[ 4 ], modelMatrix, projectionMatrix );

	GXSetMat4RotationXYZ ( modelMatrix, 0.0f, GX_MATH_PI, 0.0f );
	GXMulMat4Mat4 ( modelProjectionMatrices[ 5 ], modelMatrix, projectionMatrix );
}

GXEquirectangularToCubeMapMaterial::~GXEquirectangularToCubeMapMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid GXEquirectangularToCubeMapMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !texture ) return;

	glUseProgram ( shaderProgram.GetProgram () );
	glUniformMatrix4fv ( modelProjectionMatricesLocation, 6, GL_FALSE, (const GLfloat*)modelProjectionMatrices );
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
