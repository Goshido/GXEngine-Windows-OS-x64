// version 1.5

#include <GXEngine/GXSkinningMaterial.h>


#define VERTEX_SHADER       L"Shaders/System/Skinning_vs.txt"
#define GEOMETRY_SHADER     nullptr
#define FRAGMENT_SHADER     nullptr

//---------------------------------------------------------------------------------------------------------------------

GXSkinningMaterial::GXSkinningMaterial ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXSkinningMaterial" )
    _skeletonObject ( nullptr )
{
    constexpr GLchar* transformFeedbackOutputNames[ 5u ] = { "v_vertex", "v_uv", "v_normal", "v_tangent", "v_bitangent" };

    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 0u;
    si._samplerNames = nullptr;
    si._samplerLocations = nullptr;
    si._transformFeedbackOutputs = 5;
    si._transformFeedbackOutputNames = transformFeedbackOutputNames;

    _shaderProgram.Init ( si );

    _bonesLocation = _shaderProgram.GetUniform ( "bones" );
}

GXSkinningMaterial::~GXSkinningMaterial ()
{
    // NOTHING
}

GXVoid GXSkinningMaterial::Bind ( const GXTransform& /*transform*/ )
{
    if ( !_skeletonObject ) return;

    glUseProgram ( _shaderProgram.GetProgram () );
    
    GLsizei floats = static_cast<GLsizei> ( _skeletonObject->GetTotalBones () * GX_FLOATS_PER_BONE );
    glUniform1fv ( _bonesLocation, floats, reinterpret_cast<const GLfloat*> ( _skeletonObject->GetSkinTransform () ) );
}

GXVoid GXSkinningMaterial::Unbind ()
{
    if ( !_skeletonObject ) return;

    glUseProgram ( 0u );
}

GXVoid GXSkinningMaterial::SetSkeleton ( const GXSkeleton &skeleton )
{
    _skeletonObject = &skeleton;
}
