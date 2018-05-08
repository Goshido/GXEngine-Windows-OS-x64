#include <GXEngine_Editor_Mobile/EMSSAOApplyMaterial.h>


#define SSAO_SLOT			0u
#define IMAGE_SLOT			1u

#define VERTEX_SHADER		L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		L"Shaders/Editor Mobile/SSAOApply_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMSSAOApplyMaterial::EMSSAOApplyMaterial ():
	ssaoTexture ( nullptr ),
	imageTexture ( nullptr )
{
	static const GLchar* samplerNames[ 2 ] = { "ssaoSampler", "imageSampler" };
	static const GLuint samplerLocations[ 2 ] = { SSAO_SLOT, IMAGE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 2u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );
}

EMSSAOApplyMaterial::~EMSSAOApplyMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMSSAOApplyMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !ssaoTexture || !imageTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	ssaoTexture->Bind ( SSAO_SLOT );
	imageTexture->Bind ( IMAGE_SLOT );
}

GXVoid EMSSAOApplyMaterial::Unbind ()
{
	if ( !ssaoTexture || !imageTexture ) return;

	glUseProgram ( 0u );

	ssaoTexture->Unbind ();
	imageTexture->Unbind ();
}

GXVoid EMSSAOApplyMaterial::SetSSAOTexture ( GXTexture2D &texture )
{
	ssaoTexture = &texture;
}

GXVoid EMSSAOApplyMaterial::SetImageTexture ( GXTexture2D &texture )
{
	imageTexture = &texture;
}
