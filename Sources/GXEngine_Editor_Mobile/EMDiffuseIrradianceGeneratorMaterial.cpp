#include <GXEngine_Editor_Mobile/EMDiffuseIrradianceGeneratorMaterial.h>


#define VERTEX_SHADER			L"Shaders/System/VertexPass_vs.txt"
#define GEOMETRY_SHADER			L"Shaders/System/CubeMapSplitter_gs.txt"
#define FRAGMENT_SHADER			L"Shaders/Editor Mobile/DiffuseIrradianceGenerator_fs.txt"

#define TEXTURE_SLOT			0
#define DEFAULT_ANGLE_STEP		0.025f	//4.363323e-4f degrees

#define QUAD_ASPECT_RATIO		1.0f
#define Z_NEAR					0.1f
#define Z_FAR					777.777f
#define PROJECTION_FOV_Y		GX_MATH_HALF_PI


EMDiffuseIrradianceGeneratorMaterial::EMDiffuseIrradianceGeneratorMaterial ()
{
	environmentMap = nullptr;

	static const GLchar* samplerNames[ 1 ] = { "environmentSampler" };
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
	//angleStepLocation = shaderProgram.GetUniform ( "angleStep" );

	SetAngleStep ( DEFAULT_ANGLE_STEP );

	GXMat4 projectionMatrix;
	GXSetMat4Perspective ( projectionMatrix, PROJECTION_FOV_Y, QUAD_ASPECT_RATIO, Z_NEAR, Z_FAR );

	GXMat4 modelMatrix;
	GXMat4 v;
	GXSetMat4RotationXYZ ( modelMatrix, 0.0f, -GX_MATH_HALF_PI, 0.0f );
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

EMDiffuseIrradianceGeneratorMaterial::~EMDiffuseIrradianceGeneratorMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMDiffuseIrradianceGeneratorMaterial::Bind ( const GXTransform &transform )
{
	if ( !environmentMap ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	glUniformMatrix4fv ( modelProjectionMatricesLocation, 6, GL_FALSE, (const GLfloat*)modelProjectionMatrices );
	//glUniform1f ( angleStepLocation, angleStep );

	environmentMap->Bind ( TEXTURE_SLOT );
}

GXVoid EMDiffuseIrradianceGeneratorMaterial::Unbind ()
{
	if ( !environmentMap ) return;

	glUseProgram ( 0 );
	environmentMap->Unbind ();
}

GXVoid EMDiffuseIrradianceGeneratorMaterial::SetEnvironmentMap ( GXTextureCubeMap &cubeMap )
{
	environmentMap = &cubeMap;
}

GXUInt EMDiffuseIrradianceGeneratorMaterial::SetAngleStep ( GXFloat radians )
{
	angleStep = radians;
	GXFloat invAngleStep = 1.0f / angleStep;
	return (GXUInt)( floorf ( GX_MATH_HALF_PI * invAngleStep ) + floorf ( GX_MATH_DOUBLE_PI * invAngleStep ) );
}
