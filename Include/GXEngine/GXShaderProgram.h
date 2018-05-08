// version 1.5

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
	GXUInt				numSamplers;

	const GLchar**		transformFeedbackOutputNames;
	GLsizei				numTransformFeedbackOutputs;
};

// Class handles lazy loading reference counting shader program resource creation.

class GXShaderProgramEntry;
class GXShaderProgram
{
	private:
		GXShaderProgramEntry*		shaderProgramEntry;

	public:
		GXShaderProgram ();
		~GXShaderProgram ();

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
