// version 1.2

#include <GXEngine/GXTexture2DGammaCorrectorMaterial.h>


#define VERTEX_SHADER			L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/System/Texture2DGammaCorrector_fs.txt"

#define TEXTURE_SLOT			0u

#define GAMMA					2.2f

//---------------------------------------------------------------------------------------------------------------------

GXTexture2DGammaCorrectorMaterial::GXTexture2DGammaCorrectorMaterial ():
	sRGBTexture ( nullptr ),
	sampler( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f )
{
	static const GLchar* samplerNames[ 1 ] = { "sRGBSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si.vertexShader = VERTEX_SHADER;
	si.geometryShader = GEOMETRY_SHADER;
	si.fragmentShader = FRAGMENT_SHADER;
	si.samplers = 1u;
	si.samplerNames = samplerNames;
	si.samplerLocations = samplerLocations;
	si.transformFeedbackOutputs = 0;
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
	sampler.Bind ( TEXTURE_SLOT );
}

GXVoid GXTexture2DGammaCorrectorMaterial::Unbind ()
{
	if ( !sRGBTexture ) return;

	sampler.Unbind ( TEXTURE_SLOT );
	sRGBTexture->Unbind ();

	glUseProgram ( 0u );
}

GXVoid GXTexture2DGammaCorrectorMaterial::SetSRGBTexture ( GXTexture2D &texture )
{
	sRGBTexture = &texture;
}
