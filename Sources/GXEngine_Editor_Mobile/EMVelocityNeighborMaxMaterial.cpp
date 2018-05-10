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
	inverseVelocityTileMaxTextureResolution( 1.0f / static_cast<GXFloat> ( VELOCITY_TILE_MAX_TEXTURE_WIDTH ), 1.0f / static_cast<GXFloat> ( VELOCITY_TILE_MAX_TEXTURE_HEIGHT ) )
{
	static const GLchar* samplerNames[ 1 ] = { "velocityTileMaxSampler" };
	static const GLuint samplerLocations[ 1 ] = { VELOCITY_TILE_MAX_SLOT };

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

	inverseVelocityTileMaxTextureResolutionLocation = shaderProgram.GetUniform ( "inverseVelocityTileMaxTextureResolution" );
}

EMVelocityNeighborMaxMaterial::~EMVelocityNeighborMaxMaterial ()
{
	// NOTHING
}

GXVoid EMVelocityNeighborMaxMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !velocityTileMaxTexture ) return;

	glUseProgram ( shaderProgram.GetProgram () );
	glUniform2fv ( inverseVelocityTileMaxTextureResolutionLocation, 1, inverseVelocityTileMaxTextureResolution.data );
	velocityTileMaxTexture->Bind ( VELOCITY_TILE_MAX_SLOT );
}

GXVoid EMVelocityNeighborMaxMaterial::Unbind ()
{
	if ( !velocityTileMaxTexture ) return;

	glUseProgram ( 0u );
	velocityTileMaxTexture->Unbind ();
}

GXVoid EMVelocityNeighborMaxMaterial::SetVelocityTileMaxTexture ( GXTexture2D &texture )
{
	velocityTileMaxTexture = &texture;
}

GXVoid EMVelocityNeighborMaxMaterial::SetVelocityTileMaxTextureResolution ( GXUShort width, GXUShort height )
{
	inverseVelocityTileMaxTextureResolution.Init ( 1.0f / static_cast<GXFloat> ( width ), 1.0f / static_cast<GXFloat> ( height ) );
}
