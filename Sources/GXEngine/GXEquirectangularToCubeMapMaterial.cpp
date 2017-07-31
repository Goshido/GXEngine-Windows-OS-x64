#include <GXEngine/GXEquirectangularToCubeMapMaterial.h>


#define VERTEX_SHADER			L"Shaders/System/EquirectangularToCubeMap_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/System/EquirectangularToCubeMap_fs.txt"

#define TEXTURE_SLOT			0

#define QUAD_ASPECT_RATIO		1.0f
#define Z_NEAR					0.1f
#define Z_FAR					777.777f
#define PROJECTION_FOV_Y		GX_MATH_HALFPI


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

	viewProjectionMatrixLocation = shaderProgram.GetUniform ( "viewProjectionMatrix" );

	texture = nullptr;
	SetSide ( eGXCubeMapFace::PositiveX );
}

GXEquirectangularToCubeMapMaterial::~GXEquirectangularToCubeMapMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid GXEquirectangularToCubeMapMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !texture ) return;

	glUseProgram ( shaderProgram.GetProgram () );
	glUniformMatrix4fv ( viewProjectionMatrixLocation, 1, GL_FALSE, viewProjectionMatrix.arr );
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

GXVoid GXEquirectangularToCubeMapMaterial::SetSide ( eGXCubeMapFace side )
{
	GXMat4 projectionMatrix;
	GXSetMat4Perspective ( projectionMatrix, PROJECTION_FOV_Y, QUAD_ASPECT_RATIO, Z_NEAR, Z_FAR );

	GXMat4 viewMatrix;

	switch ( side )
	{
		case eGXCubeMapFace::PositiveX:
			GXSetMat4RotationXYZ ( viewMatrix, 0.0f, -GX_MATH_HALFPI, 0.0f );
		break;

		case eGXCubeMapFace::NegativeX:
			GXSetMat4RotationXYZ ( viewMatrix, 0.0f, GX_MATH_HALFPI, 0.0f );
		break;

		case eGXCubeMapFace::PositiveY:
			GXSetMat4RotationXYZ ( viewMatrix, -GX_MATH_HALFPI, 0.0f, 0.0f );
		break;

		case eGXCubeMapFace::NegativeY:
			GXSetMat4RotationXYZ ( viewMatrix, GX_MATH_HALFPI, 0.0f, 0.0f );
		break;

		case eGXCubeMapFace::PositiveZ:
			GXSetMat4RotationXYZ ( viewMatrix, 0.0f, 0.0f, 0.0f );
		break;

		case eGXCubeMapFace::NegativeZ:
			GXSetMat4RotationXYZ ( viewMatrix, 0.0f, GX_MATH_PI, 0.0f );
		break;

		default:
			//NOTHING
		break;
	}

	GXMulMat4Mat4 ( viewProjectionMatrix, viewMatrix, projectionMatrix );
}
