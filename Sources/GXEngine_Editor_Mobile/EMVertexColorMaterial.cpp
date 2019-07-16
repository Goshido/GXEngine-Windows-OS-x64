#include <GXEngine_Editor_Mobile/EMVertexColorMaterial.h>
#include <GXEngine/GXCamera.h>


#define VERTEX_SHADER       L"Shaders/Editor Mobile/VertexColor_vs.txt"
#define GEOMETRY_SHADER     nullptr
#define FRAGMENT_SHADER     L"Shaders/Editor Mobile/VertexColor_fs.txt"

//---------------------------------------------------------------------------------------------------------------------

EMVertexColorMaterial::EMVertexColorMaterial ()
{
    GXShaderProgramInfo si;
    si._vertexShader = VERTEX_SHADER;
    si._geometryShader = GEOMETRY_SHADER;
    si._fragmentShader = FRAGMENT_SHADER;
    si._samplers = 0u;
    si._samplerNames = nullptr;
    si._samplerLocations = nullptr;
    si._transformFeedbackOutputs = 0;
    si._transformFeedbackOutputNames = nullptr;

    _shaderProgram.Init ( si );

    _currentFrameModelViewProjectionMatrixLocation = _shaderProgram.GetUniform ( "currentFrameModelViewProjectionMatrix" );
}

EMVertexColorMaterial::~EMVertexColorMaterial ()
{
    // NOTHING
}

GXVoid EMVertexColorMaterial::Bind ( const GXTransform &transform )
{
    glUseProgram ( _shaderProgram.GetProgram () );

    GXMat4 currentFrameModelViewProjectionMatrix;
    currentFrameModelViewProjectionMatrix.Multiply ( transform.GetCurrentFrameModelMatrix (), GXCamera::GetActiveCamera ()->GetCurrentFrameViewProjectionMatrix () );

    glUniformMatrix4fv ( _currentFrameModelViewProjectionMatrixLocation, 1, GL_FALSE, currentFrameModelViewProjectionMatrix._data );
}

GXVoid EMVertexColorMaterial::Unbind ()
{
    glUseProgram ( 0u );
}
