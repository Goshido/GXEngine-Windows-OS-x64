#include <GXEngine_Editor_Mobile/EMToneMapperLuminanceTripletReducerMaterial.h>


#define DEFAULT_LEVEL_OF_DETAIL_TO_REDUCE       0u

#define TEXTURE_SLOT                            0u

#define VERTEX_SHADER                           L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER                         nullptr
#define FRAGMENT_SHADER                         L"Shaders/Editor Mobile/LuminanceTripletReducer_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMToneMapperLuminanceTripletReducerMaterial::EMToneMapperLuminanceTripletReducerMaterial ():
    _luminanceTripletTexture ( nullptr ),
    _sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
    _levelOfDetail ( static_cast<GXFloat> ( DEFAULT_LEVEL_OF_DETAIL_TO_REDUCE ) )
{
    constexpr GLchar* samplerNames[ 1u ] = { "luminanceTripletSampler" };
    constexpr GLuint samplerLocations[ 1u ] = { TEXTURE_SLOT };

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
    _levelOfDetailLocation = _shaderProgram.GetUniform ( "levelOfDetail" );
}

EMToneMapperLuminanceTripletReducerMaterial::~EMToneMapperLuminanceTripletReducerMaterial ()
{
    // NOTHING
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_luminanceTripletTexture ) return;

    glUseProgram ( _shaderProgram.GetProgram () );
    glUniform1f ( _levelOfDetailLocation, _levelOfDetail );

    _luminanceTripletTexture->Bind ( TEXTURE_SLOT );
    _sampler.Bind ( TEXTURE_SLOT );
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::Unbind ()
{
    if ( !_luminanceTripletTexture ) return;

    _sampler.Unbind ( TEXTURE_SLOT );
    _luminanceTripletTexture->Unbind ();

    glUseProgram ( 0u );
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::SetLuminanceTripletTexture ( GXTexture2D &texture )
{
    _luminanceTripletTexture = &texture;
}

GXVoid EMToneMapperLuminanceTripletReducerMaterial::SetLevelOfDetailToReduce ( GXUByte newLevelOfDetail )
{
    _levelOfDetail = static_cast<GXFloat> ( newLevelOfDetail );
}
