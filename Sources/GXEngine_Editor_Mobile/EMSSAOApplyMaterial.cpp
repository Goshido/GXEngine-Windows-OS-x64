#include <GXEngine_Editor_Mobile/EMSSAOApplyMaterial.h>


#define SSAO_SLOT			0u
#define IMAGE_SLOT			1u

#define VERTEX_SHADER		L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/SSAOApply_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMSSAOApplyMaterial::EMSSAOApplyMaterial ():
	ssaoTexture ( nullptr ),
	imageTexture ( nullptr ),
	sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f )
{
	static const GLchar* samplerNames[ 2 ] = { "ssaoSampler", "imageSampler" };
	static const GLuint samplerLocations[ 2 ] = { SSAO_SLOT, IMAGE_SLOT };

	GXShaderProgramInfo si;
	si._vertexShader = VERTEX_SHADER;
	si._geometryShader = GEOMETRY_SHADER;
	si._fragmentShader = FRAGMENT_SHADER;
	si._samplers = 2u;
	si._samplerNames = samplerNames;
	si._samplerLocations = samplerLocations;
	si._transformFeedbackOutputs = 0;
	si._transformFeedbackOutputNames = nullptr;

	_shaderProgram.Init ( si );
}

EMSSAOApplyMaterial::~EMSSAOApplyMaterial ()
{
	// NOTHING
}

GXVoid EMSSAOApplyMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !ssaoTexture || !imageTexture ) return;

	glUseProgram ( _shaderProgram.GetProgram () );

	ssaoTexture->Bind ( SSAO_SLOT );
	sampler.Bind ( SSAO_SLOT );

	imageTexture->Bind ( IMAGE_SLOT );
	sampler.Bind ( IMAGE_SLOT );
}

GXVoid EMSSAOApplyMaterial::Unbind ()
{
	if ( !ssaoTexture || !imageTexture ) return;

	sampler.Unbind ( SSAO_SLOT );
	ssaoTexture->Unbind ();

	sampler.Unbind ( IMAGE_SLOT );
	imageTexture->Unbind ();

	glUseProgram ( 0u );
}

GXVoid EMSSAOApplyMaterial::SetSSAOTexture ( GXTexture2D &texture )
{
	ssaoTexture = &texture;
}

GXVoid EMSSAOApplyMaterial::SetImageTexture ( GXTexture2D &texture )
{
	imageTexture = &texture;
}
