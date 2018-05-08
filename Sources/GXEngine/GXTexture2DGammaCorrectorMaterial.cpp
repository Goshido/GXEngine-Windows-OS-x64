// version 1.1

#include <GXEngine/GXTexture2DGammaCorrectorMaterial.h>


#define VERTEX_SHADER			L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/System/Texture2DGammaCorrector_fs.txt"

#define TEXTURE_SLOT			0u

#define GAMMA					2.2f

//---------------------------------------------------------------------------------------------------------------------

GXTexture2DGammaCorrectorMaterial::GXTexture2DGammaCorrectorMaterial ():
	sRGBTexture ( nullptr )
{
	static const GLchar* samplerNames[ 1 ] = { "sRGBSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 1u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.numTransformFeedbackOutputs = 0;
	si.transformFeedbackOutputNames = nullptr;

	shaderProgram.Init ( si );
	gammaLocation = shaderProgram.GetUniform ( "gamma" );
}

GXTexture2DGammaCorrectorMaterial::~GXTexture2DGammaCorrectorMaterial ()
{
	// NOTHING
}

GXVoid GXTexture2DGammaCorrectorMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !sRGBTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );

	glUniform1f ( gammaLocation, GAMMA );
	sRGBTexture->Bind ( TEXTURE_SLOT );
}

GXVoid GXTexture2DGammaCorrectorMaterial::Unbind ()
{
	if ( !sRGBTexture ) return;

	glUseProgram ( 0u );
	sRGBTexture->Unbind ();
}

GXVoid GXTexture2DGammaCorrectorMaterial::SetsRGBTexture ( GXTexture2D &texture )
{
	sRGBTexture = &texture;
}
