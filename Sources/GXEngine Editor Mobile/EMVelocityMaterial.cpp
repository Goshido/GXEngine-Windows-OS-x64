#include <GXEngine_Editor_Mobile/EMVelocityMaterial.h>


#define DEFAULT_EXPLOSURE_TIME		0.01667f
#define DEFAULT_MAX_BLUR_SAMPLES	15

#define VELOCITY_SLOT				0

#define VERTEX_SHADER				L"Shaders/Editor Mobile/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER				nullptr
#define FRAGMENT_SHADER				L"Shaders/Editor Mobile/Velocity_fs.txt"


EMVelocityMaterial::EMVelocityMaterial ()
{
	velocityViewTexture = nullptr;

	static const GLchar* samplerNames[ 1 ] = { "velocityViewSampler" };
	static const GLuint samplerLocations[ 1 ] = { VELOCITY_SLOT };

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

	explosureTimeLocation = shaderProgram.GetUniform ( "explosureTime" );
	maxBlurSamplesLocation = shaderProgram.GetUniform ( "maxBlurSamples" );

	SetExplosureTime ( DEFAULT_EXPLOSURE_TIME );
	SetMaxBlurSamples ( DEFAULT_MAX_BLUR_SAMPLES );
}

EMVelocityMaterial::~EMVelocityMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid EMVelocityMaterial::Bind ( const GXTransform& /*transform*/ ) const
{
	if ( !velocityViewTexture ) return;
	
	glUseProgram ( shaderProgram.GetProgram () );

	glUniform1f ( explosureTimeLocation, explosureTime );
	glUniform1f ( maxBlurSamplesLocation, maxBlurSamples );

	velocityViewTexture->Bind ( VELOCITY_SLOT );
}

GXVoid EMVelocityMaterial::Unbind () const
{
	if ( !velocityViewTexture ) return;

	glUseProgram ( 0 );
	velocityViewTexture->Unbind ();
}

GXVoid EMVelocityMaterial::SetVelocityViewTexture ( GXTexture &texture )
{
	velocityViewTexture = &texture;
}

GXVoid EMVelocityMaterial::SetExplosureTime ( GXFloat time )
{
	explosureTime = time;
}

GXVoid EMVelocityMaterial::SetMaxBlurSamples ( GXUByte samples )
{
	maxBlurSamples = (GXFloat)samples;
}
