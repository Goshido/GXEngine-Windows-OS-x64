//version 1.0

#ifndef GX_SHADER_PROGRAM
#define GX_SHADER_PROGRAM


#include "GXOpenGL.h"


struct GXShaderProgramInfo 
{
	const GXWChar*		vs;
	const GXWChar*		gs;
	const GXWChar*		fs;

	const GLchar**		samplerNames;
	const GLuint*		samplerLocations;
	GXInt				numSamplers;
};

class GXShaderProgram
{
	private:
		GXWChar*	vs;
		GXWChar*	gs;
		GXWChar*	fs;

		GLuint		program;

	public:
		GXShaderProgram ();
		~GXShaderProgram ();

		GLuint GetProgram () const;
		GLint GetUniform ( const GLchar* name ) const;

		static GXShaderProgram& GXCALL GetShaderProgram ( const GXShaderProgramInfo &info );
		static GXVoid GXCALL RemoveShaderProgram ( GXShaderProgram &program );
		static GXUInt GXCALL GetTotalLoadedShaderPrograms ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS );

		GXVoid operator = ( const GXShaderProgram &shadepProgram );

	private:
		explicit GXShaderProgram ( const GXShaderProgramInfo &info );

		GXBool operator == ( const GXShaderProgramInfo &other ) const;
		GXBool operator == ( const GXShaderProgram &other ) const;

		GLuint GetShader ( GLenum type, const GXWChar* fileName );
};


#endif //GX_SHADER_PROGRAM
