// version 1.12

#ifndef GX_SHADER_PROGRAM
#define GX_SHADER_PROGRAM


#include "GXOpenGL.h"
#include <GXCommon/GXAVLTree.h>


struct GXShaderProgramInfo final
{
    const GXWChar*          _vertexShader;
    const GXWChar*          _geometryShader;
    const GXWChar*          _fragmentShader;

    const GLchar* const*    _samplerNames;
    const GLuint*           _samplerLocations;
    GXUInt                  _samplers;

    const GLchar* const*    _transformFeedbackOutputNames;
    GLsizei                 _transformFeedbackOutputs;
};

// Class handles lazy loading reference counting shader program resource creation.

class GXShaderProgramEntry;
class GXShaderProgram final : public GXMemoryInspector
{
    private:
        GXShaderProgramEntry*       _shaderProgramEntry;

    public:
        GXShaderProgram ();
        ~GXShaderProgram () override;

        // Releases previous shader program resource if it exists.
        GXVoid Init ( const GXShaderProgramInfo &info );

        GLuint GetProgram () const;
        GLint GetUniform ( const GLchar* name ) const;

        static GXVoid GXCALL InitPrecompiledShaderProgramSubsystem ();
        static GXVoid GXCALL DestroyPrecompiledShaderProgramSubsystem ();

        static GXUInt GXCALL GetTotalLoadedShaderPrograms ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS );

    private:
        GXShaderProgram ( const GXShaderProgram &other ) = delete;
        GXShaderProgram& operator = ( const GXShaderProgram &other ) = delete;
};


#endif // GX_SHADER_PROGRAM
