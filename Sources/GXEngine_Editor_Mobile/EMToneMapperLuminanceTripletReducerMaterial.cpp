#include <GXEngine_Editor_Mobile/EMToneMapperLuminanceTripletReducerMaterial.h>


#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/LuminanceTripletReducer_fs.txt"

#define TEXTURE_SLOT						0

#define DEFAULT_LEVEL_OF_DETAIL_TO_REDUCE	0


EMToneMapperLuminanceTripletReducerMaterial::EMToneMapperLuminanceTripletReducerMaterial ()
{
	static const GLchar* samplerNames[ 1 ] = { "luminanceTripletSampler" };
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
	levelOfDetailLocation = shaderProgram.GetUniform ( "levelOfDetail" );

	luminanceTripletTexture = nullptr;
	SetLevelOfDetailToReduce ( DEFAULT_LEVEL_OF_DETAIL_TO_REDUCE );
}

EMToneMapperLuminanceTripletReducerMaterial::~EMToneMapperLuminanceTripletReducerMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
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

	glUseProgram ( 0 );
	luminanceTripletTexture->Unbind ();
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::SetLuminanceTripletTexture ( GXTexture2D &texture )
{
	luminanceTripletTexture = &texture;
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::SetLevelOfDetailToReduce ( GXUByte newLevelOfDetail )
{
	this->levelOfDetail = (GXFloat)newLevelOfDetail;
}
