//version 1.0

#include "GXOpenGL.h"

struct GXShaderProgramInfo 
{
	const GXWChar* vs;
	const GXWChar* gs;
	const GXWChar* fs;

	const GLchar** samplerNames;
	const GLuint* samplerLocations;
	GXInt numSamplers;
};

class GXShaderProgram
{
	friend class GXShaderProgramEntry;

	private:
		GXWChar*						vs;
		GXWChar*						gs;
		GXWChar*						fs;

		GLuint							program;

	public:
		GXShaderProgram ();

		GLuint GetProgram () const;
		GLint GetUniform ( const GLchar* name ) const;

		static GXShaderProgram& GXCALL GetShaderProgram ( const GXShaderProgramInfo &info );
		static GXVoid GXCALL RemoveShaderProgram ( GXShaderProgram& program );

		static GXUInt GXCALL GetTotalPrograms ( const GXWChar* lastVS, const GXWChar* lastGS, const GXWChar* lastFS );

	private:
		explicit GXShaderProgram ( const GXShaderProgramInfo &info );
		~GXShaderProgram ();

		GXBool operator == ( const GXShaderProgramInfo &other );
		GXBool operator == ( const GXShaderProgram &other );

		GLuint GetShader ( GLenum type, const GXWChar* fileName );
};