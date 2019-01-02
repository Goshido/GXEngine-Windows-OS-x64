// version 1.8

#ifndef GX_SHADER_PROGRAM
#define GX_SHADER_PROGRAM


#include "GXOpenGL.h"
#include <GXCommon/GXAVLTree.h>


struct GXShaderProgramInfo final
{
    const GXWChar*      vertexShader;
    const GXWChar*      geometryShader;
    const GXWChar*      fragmentShader;

    const GLchar**      samplerNames;
    const GLuint*       samplerLocations;
    GXUInt              samplers;

    const GLchar**      transformFeedbackOutputNames;
    GLsizei             transformFeedbackOutputs;
};

// Class handles lazy loading reference counting shader program resource creation.

class GXShaderProgramEntry;
class GXShaderProgram final : public GXMemoryInspector
{
    private:
        GXShaderProgramEntry*       shaderProgramEntry;

    public:
        GXShaderProgram ();
        ~GXShaderProgram ();

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
