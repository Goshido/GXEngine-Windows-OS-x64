//version 1.1

#ifndef GX_SHADER_PROGRAM
#define GX_SHADER_PROGRAM


#include "GXOpenGL.h"
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXAVLTree.h>


class GXPrecompiledShaderProgramNode : public GXAVLTreeNode
{
	friend class GXPrecompiledShaderProgramFinder;

	private:
		GXWChar*	name;
		GXWChar*	binaryPath;
		GLenum		binaryFormat;

	public:
		GXPrecompiledShaderProgramNode ( const GXWChar* vs, const GXWChar* fs, const GXWChar* gs, const GXWChar* binaryPath, GLenum binaryFormat );
		~GXPrecompiledShaderProgramNode () override;

		const GXVoid* GetKey () const override;

	private:
		static GXInt GXCALL Compare ( const GXVoid* a, const GXVoid* b );
};

class GXPrecompiledShaderProgramFinder : public GXAVLTree
{
	public:
		GXPrecompiledShaderProgramFinder ();
		~GXPrecompiledShaderProgramFinder () override;

		GXVoid FindProgram ( const GXWChar** binaryPath, GLenum &binaryFormat, const GXWChar* vs, const GXWChar* fs, const GXWChar* gs ) const;
		GXVoid AddProgram ( GXPrecompiledShaderProgramNode &program );
};

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

class GXShaderProgram
{
	private:
		GXWChar*					vs;
		GXWChar*					gs;
		GXWChar*					fs;

		GLuint						program;
		static GXDynamicArray		stringArray;

	public:
		GXShaderProgram ();
		~GXShaderProgram ();

		GLuint GetProgram () const;
		GLint GetUniform ( const GLchar* name ) const;

		static GXVoid InitShaderProgramCache ();
		static GXVoid DestroyShaderProgramCache ();

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
