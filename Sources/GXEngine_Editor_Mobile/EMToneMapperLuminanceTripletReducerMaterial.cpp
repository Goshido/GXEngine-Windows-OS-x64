#include <GXEngine_Editor_Mobile/EMToneMapperLuminanceTripletReducerMaterial.h>


#define DEFAULT_LEVEL_OF_DETAIL_TO_REDUCE	0u

#define TEXTURE_SLOT						0u

#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/LuminanceTripletReducer_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMToneMapperLuminanceTripletReducerMaterial::EMToneMapperLuminanceTripletReducerMaterial ():
	luminanceTripletTexture ( nullptr ),
	levelOfDetail ( static_cast<GXFloat> ( DEFAULT_LEVEL_OF_DETAIL_TO_REDUCE ) )
{
	static const GLchar* samplerNames[ 1 ] = { "luminanceTripletSampler" };
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
	levelOfDetailLocation = shaderProgram.GetUniform ( "levelOfDetail" );
}

EMToneMapperLuminanceTripletReducerMaterial::~EMToneMapperLuminanceTripletReducerMaterial ()
{
	// NOTHING
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !luminanceTripletTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );
	glUniform1f ( levelOfDetailLocation, levelOfDetail );
	luminanceTripletTexture->Bind ( TEXTURE_SLOT );
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::Unbind ()
{
	if ( !luminanceTripletTexture ) return;

	glUseProgram ( 0u );
	luminanceTripletTexture->Unbind ();
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::SetLuminanceTripletTexture ( GXTexture2D &texture )
{
	luminanceTripletTexture = &texture;
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::SetLevelOfDetailToReduce ( GXUByte newLevelOfDetail )
{
	levelOfDetail = static_cast<GXFloat> ( newLevelOfDetail );
}
