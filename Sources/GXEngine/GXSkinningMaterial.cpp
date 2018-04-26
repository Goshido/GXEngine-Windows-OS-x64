// version 1.1

#include <GXEngine/GXSkinningMaterial.h>


#define VERTEX_SHADER		L"Shaders/System/Skinning_vs.txt"
#define GEOMETRY_SHADER		nullptr
#define FRAGMENT_SHADER		nullptr


GXSkinningMaterial::GXSkinningMaterial ():
	skeletonObject ( nullptr )
{
	static const GLchar* transformFeedbackOutputNames[ 5 ] = { "v_vertex", "v_uv", "v_normal", "v_tangent", "v_bitangent" };

	GXShaderProgramInfo si;
	si.vs = VERTEX_SHADER;
	si.gs = GEOMETRY_SHADER;
	si.fs = FRAGMENT_SHADER;
	si.numSamplers = 0;
	si.samplerNames = nullptr;
	si.samplerLocations = nullptr;
	si.numTransformFeedbackOutputs = 5;
	si.transformFeedbackOutputNames = transformFeedbackOutputNames;

	shaderProgram = GXShaderProgram::GetShaderProgram ( si );

	bonesLocation = shaderProgram.GetUniform ( "bones" );
}

GXSkinningMaterial::~GXSkinningMaterial ()
{
	GXShaderProgram::RemoveShaderProgram ( shaderProgram );
}

GXVoid GXSkinningMaterial::Bind ( const GXTransform& /*transform*/ )
{
	if ( !skeletonObject ) return;

	glUseProgram ( shaderProgram.GetProgram () );
	
	GLsizei floats = static_cast<GLsizei> ( skeletonObject->GetTotalBones () * GX_FLOATS_PER_BONE );
	glUniform1fv ( bonesLocation, floats, reinterpret_cast<const GLfloat*> ( skeletonObject->GetSkinTransform () ) );
}

GXVoid GXSkinningMaterial::Unbind ()
{
	if ( !skeletonObject ) return;

	glUseProgram ( 0u );
}

GXVoid GXSkinningMaterial::SetSkeleton ( const GXSkeleton &skeleton )
{
	skeletonObject = &skeleton;
}