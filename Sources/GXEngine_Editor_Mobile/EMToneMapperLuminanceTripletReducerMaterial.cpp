#include <GXEngine_Editor_Mobile/EMToneMapperLuminanceTripletReducerMaterial.h>


#define DEFAULT_LEVEL_OF_DETAIL_TO_REDUCE	0u

#define TEXTURE_SLOT						0u

#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/LuminanceTripletReducer_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMToneMapperLuminanceTripletReducerMaterial::EMToneMapperLuminanceTripletReducerMaterial ():
	luminanceTripletTexture ( nullptr ),
	sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
	levelOfDetail ( static_cast<GXFloat> ( DEFAULT_LEVEL_OF_DETAIL_TO_REDUCE ) )
{
	static const GLchar* samplerNames[ 1 ] = { "luminanceTripletSampler" };
	static const GLuint samplerLocations[ 1 ] = { TEXTURE_SLOT };

	GXShaderProgramInfo si;
	si._vertexShader = VERTEX_SHADER;
	si._geometryShader = GEOMETRY_SHADER;
	si._fragmentShader = FRAGMENT_SHADER;
	si._samplers = 1u;
	si._samplerNames = samplerNames;
	si._samplerLocations = samplerLocations;
	si._transformFeedbackOutputs = 0;
	si._transformFeedbackOutputNames = nullptr;

	_shaderProgram.Init ( si );
	levelOfDetailLocation = _shaderProgram.GetUniform ( "levelOfDetail" );
}

EMToneMapperLuminanceTripletReducerMaterial::~EMToneMapperLuminanceTripletReducerMaterial ()
{
	// NOTHING
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !luminanceTripletTexture ) return;

	glUseProgram ( _shaderProgram.GetProgram () );
	glUniform1f ( levelOfDetailLocation, levelOfDetail );

	luminanceTripletTexture->Bind ( TEXTURE_SLOT );
	sampler.Bind ( TEXTURE_SLOT );
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::Unbind ()
{
	if ( !luminanceTripletTexture ) return;

	sampler.Unbind ( TEXTURE_SLOT );
	luminanceTripletTexture->Unbind ();

	glUseProgram ( 0u );
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::SetLuminanceTripletTexture ( GXTexture2D &texture )
{
	luminanceTripletTexture = &texture;
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::SetLevelOfDetailToReduce ( GXUByte newLevelOfDetail )
{
	levelOfDetail = static_cast<GXFloat> ( newLevelOfDetail );
}
