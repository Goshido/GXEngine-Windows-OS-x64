#include <GXEngine_Editor_Mobile/EMToneMapperMaterial.h>


#define VERTEX_SHADER			L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/Editor Mobile/ToneMapper_fs.txt"

#define TEXTURE_SLOT			0

#define DEFAULT_GAMMA			2.2f


EMToneMapperMaterial::EMToneMapperMaterial ()
{
	static const GLchar* samplerNames[ 1 ] = { "linearSpaceSampler" };
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

	inverseGammaLocation = shaderProgram.GetUniform ( "inverseGamma" );

	linearSpaceTexture = nullptr;
	SetGamma ( DEFAULT_GAMMA );
}

EMToneMapperMaterial::~EMToneMapperMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMToneMapperMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !linearSpaceTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	glUniform1f ( inverseGammaLocation, inverseGamma );
	linearSpaceTexture->Bind ( TEXTURE_SLOT );
}

GXVoid EMToneMapperMaterial::Unbind ()
{
	if ( !linearSpaceTexture ) return;

	glUseProgram ( 0 );
	linearSpaceTexture->Unbind ();
}

GXVoid EMToneMapperMaterial::SetLinearSpaceTexture ( GXTexture2D &texture )
{
	linearSpaceTexture = &texture;
}

GXVoid EMToneMapperMaterial::SetGamma ( GXFloat gamma )
{
	this->gamma = gamma;
	inverseGamma = 1.0f / gamma;
}

GXFloat EMToneMapperMaterial::GetGamma () const
{
	return gamma;
}
