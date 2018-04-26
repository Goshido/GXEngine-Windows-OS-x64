// version 1.3

#ifndef GX_SHADER_PROGRAM
#define GX_SHADER_PROGRAM


#include "GXOpenGL.h"
#include <GXCommon/GXAVLTree.h>




struct GXShaderProgramInfo 
{
	const GXWChar*		vs;
	const GXWChar*		gs;
	const GXWChar*		fs;

	const GLchar**		samplerNames;
	const GLuint*		samplerLocations;
	GXInt				numSamplers;

	const GLchar**		transformFeedbackOutputNames;
	GLsizei				numTransformFeedbackOutputs;
};

class GXPrecompiledShaderProgramFinder;
class GXShaderProgram
{
	private:
		GXWChar*									vs;
		GXWChar*									gs;
		GXWChar*									fs;

		GLuint										program;

		static GXPrecompiledShaderProgramFinder*	precompiledShaderProgramFinder;
		static GXWChar*								stringBuffer;

	public:
		GXShaderProgram ();
		~GXShaderProgram ();

		GLuint GetProgram () const;
		GLint GetUniform ( const GLchar* name ) const;

		static GXVoid InitPrecompiledShaderProgramSubsystem ();
		static GXVoid DestroyPrecompiledShaderProgramSubsystem ();

		static GXShaderProgram& GXCALL GetShaderProgram ( const GXShaderProgramInfo &info );
		static GXVoid GXCALL RemoveShaderProgram ( GXShaderProgram &program );
		static GXUInt GXCALL GetTotalLoadedShaderPrograms ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS );

		GXVoid operator = ( const GXShaderProgram &shadepProgram );

	private:
		explicit GXShaderProgram ( const GXShaderProgramInfo &info );

		GXBool operator == ( const GXShaderProgramInfo &other ) const;
		GXBool operator == ( const GXShaderProgram &other ) const;

		GLuint GetShader ( GLenum type, const GXWChar* fileName );
		GLuint CompileShaderProgram ( const GXShaderProgramInfo &info );
		GXVoid SavePrecompiledShaderProgram ( GLuint shaderProgram, const GXShaderProgramInfo &info, const GXWChar* binaryPath );
};


#endif // GX_SHADER_PROGRAM
