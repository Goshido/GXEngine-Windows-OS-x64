#include <GXEngine/GXEquirectangularToCubeMapMaterial.h>


#define VERTEX_SHADER			L"Shaders/System/ScreenQuad_vs.txt"
#define GEOMETRY_SHADER			nullptr
#define FRAGMENT_SHADER			L"Shaders/System/EquirectangularToCubeMap_fs.txt"

#define TEXTURE_SLOT			0


GXEquirectangularToCubeMapMaterial::GXEquirectangularToCubeMapMaterial ()
{
	static const GLchar* samplerNames[ 1 ] = { "sampler" };
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

	sideLocation = shaderProgram.GetUniform ( "side" );

	texture = nullptr;
	SetSide ( eGXCubeMapSide::PositiveX );
}

GXEquirectangularToCubeMapMaterial::~GXEquirectangularToCubeMapMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid GXEquirectangularToCubeMapMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !texture ) return;

	glUseProgram ( shaderProgram.GetProgram () );
	glUniform1i ( sideLocation, side );
	texture->Bind ( TEXTURE_SLOT );
}

GXVoid GXEquirectangularToCubeMapMaterial::Unbind ()
{
	if ( !texture ) return;

	glUseProgram ( 0 );
	texture->Unbind ();
}

GXVoid GXEquirectangularToCubeMapMaterial::SetEquirectangularTexture ( GXTexture2D &texture )
{
	this->texture = &texture;
}

GXVoid GXEquirectangularToCubeMapMaterial::SetSide ( eGXCubeMapSide side )
{
	this->side = (GLint)side;
}
