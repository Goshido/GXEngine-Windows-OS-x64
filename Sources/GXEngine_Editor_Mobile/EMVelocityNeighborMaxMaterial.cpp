#include <GXEngine_Editor_Mobile/EMVelocityNeighborMaxMaterial.h>


#define VELOCITY_TILE_MAX_SLOT				0
#define VELOCITY_TILE_MAX_TEXTURE_WIDTH		85
#define VELOCITY_TILE_MAX_TEXTURE_HEIGHT	48

#define VERTEX_SHADER				L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER				nullptr
#define FRAGMENT_SHADER				L"Shaders/Editor Mobile/VelocityNeighborMax_fs.txt"


EMVelocityNeighborMaxMaterial::EMVelocityNeighborMaxMaterial ()
{
	velocityTileMaxTexture = nullptr;

	static const GLchar* samplerNames[ 1 ] = { "velocityTileMaxSampler" };
	static const GLuint samplerLocations[ 1 ] = { VELOCITY_TILE_MAX_SLOT };

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

	inverseVelocityTileMaxTextureResolutionLocation = shaderProgram.GetUniform ( "inverseVelocityTileMaxTextureResolution" );

	SetVelocityTileMaxTextureResolution ( VELOCITY_TILE_MAX_TEXTURE_WIDTH, VELOCITY_TILE_MAX_TEXTURE_HEIGHT );
}

EMVelocityNeighborMaxMaterial::~EMVelocityNeighborMaxMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
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

	glUseProgram ( 0 );
	velocityTileMaxTexture->Unbind ();
}

GXVoid EMVelocityNeighborMaxMaterial::SetVelocityTileMaxTexture ( GXTexture2D &texture )
{
	velocityTileMaxTexture = &texture;
}

GXVoid EMVelocityNeighborMaxMaterial::SetVelocityTileMaxTextureResolution ( GXUShort width, GXUShort height )
{
	inverseVelocityTileMaxTextureResolution.Init ( 1.0f / (GXFloat)width, 1.0f / (GXFloat)height );
}
