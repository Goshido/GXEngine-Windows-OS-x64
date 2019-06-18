#include <GXEngine_Editor_Mobile/EMVelocityNeighborMaxMaterial.h>


#define VELOCITY_TILE_MAX_SLOT				0u
#define VELOCITY_TILE_MAX_TEXTURE_WIDTH		85u
#define VELOCITY_TILE_MAX_TEXTURE_HEIGHT	48u

#define VERTEX_SHADER						L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER						nullptr
#define FRAGMENT_SHADER						L"Shaders/Editor Mobile/VelocityNeighborMax_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMVelocityNeighborMaxMaterial::EMVelocityNeighborMaxMaterial ():
	velocityTileMaxTexture ( nullptr ),
	sampler ( GL_CLAMP_TO_EDGE, eGXResampling::None, 1.0f ),
	inverseVelocityTileMaxTextureResolution( 1.0f / static_cast<GXFloat> ( VELOCITY_TILE_MAX_TEXTURE_WIDTH ), 1.0f / static_cast<GXFloat> ( VELOCITY_TILE_MAX_TEXTURE_HEIGHT ) )
{
	static const GLchar* samplerNames[ 1 ] = { "velocityTileMaxSampler" };
	static const GLuint samplerLocations[ 1 ] = { VELOCITY_TILE_MAX_SLOT };

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

	inverseVelocityTileMaxTextureResolutionLocation = _shaderProgram.GetUniform ( "inverseVelocityTileMaxTextureResolution" );
}

EMVelocityNeighborMaxMaterial::~EMVelocityNeighborMaxMaterial ()
{
	// NOTHING
}

GXVoid EMVelocityNeighborMaxMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !velocityTileMaxTexture ) return;

	glUseProgram ( _shaderProgram.GetProgram () );
	glUniform2fv ( inverseVelocityTileMaxTextureResolutionLocation, 1, inverseVelocityTileMaxTextureResolution.data );

	velocityTileMaxTexture->Bind ( VELOCITY_TILE_MAX_SLOT );
	sampler.Bind ( VELOCITY_TILE_MAX_SLOT );
}

GXVoid EMVelocityNeighborMaxMaterial::Unbind ()
{
	if ( !velocityTileMaxTexture ) return;

	sampler.Unbind ( VELOCITY_TILE_MAX_SLOT );
	velocityTileMaxTexture->Unbind ();

	glUseProgram ( 0u );
}

GXVoid EMVelocityNeighborMaxMaterial::SetVelocityTileMaxTexture ( GXTexture2D &texture )
{
	velocityTileMaxTexture = &texture;
}

GXVoid EMVelocityNeighborMaxMaterial::SetVelocityTileMaxTextureResolution ( GXUShort width, GXUShort height )
{
	inverseVelocityTileMaxTextureResolution.Init ( 1.0f / static_cast<GXFloat> ( width ), 1.0f / static_cast<GXFloat> ( height ) );
}
