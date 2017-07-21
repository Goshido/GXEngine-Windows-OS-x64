//version 1.2

#ifndef GX_SHADER_PROGRAM
#define GX_SHADER_PROGRAM


#include "GXOpenGL.h"
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXAVLTree.h>


class GXPrecompiledShaderProgramNode : public GXAVLTreeNode
{
	friend class GXPrecompiledShaderProgramFinder;

	private:
		GXWChar*	vs;
		GXWChar*	gs;
		GXWChar*	fs;
		GXWChar*	binaryPath;
		GLenum		binaryFormat;

	private:
		GXPrecompiledShaderProgramNode ();
		explicit GXPrecompiledShaderProgramNode ( const GXUTF8* vs, const GXUTF8* fs, const GXUTF8* gs, const GXUTF8* binaryPath, GLenum binaryFormat );
		explicit GXPrecompiledShaderProgramNode ( const GXWChar* vs, const GXWChar* fs, const GXWChar* gs, const GXWChar* binaryPath, GLenum binaryFormat );
		~GXPrecompiledShaderProgramNode () override;

		static GXInt GXCALL Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );
		static GXVoid GXCALL InitFinderNode ( GXPrecompiledShaderProgramNode& node, const GXWChar* vs, const GXWChar* fs, const GXWChar* gs );
		static GXVoid GXCALL DestroyFinderNode ( GXPrecompiledShaderProgramNode& node );
};

class GXPrecompiledShaderProgramFinder : public GXAVLTree
{
	friend class GXShaderProgram;

	private:
		GXUBigInt						counter;

	private:
		GXPrecompiledShaderProgramFinder ();
		~GXPrecompiledShaderProgramFinder () override;

		GXBool FindProgram ( const GXWChar** binaryPath, GLenum &binaryFormat, const GXWChar* vs, const GXWChar* gs, const GXWChar* fs ) const;
		GXVoid AddProgram ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs, const GXWChar* binaryPath, GLenum binaryFormat );
		GXUBigInt GetCounter ();

		static GXVoid GXCALL GetDictionarySize ( const GXAVLTreeNode &node, GXVoid* args );
		static GXVoid GXCALL SaveDictionary ( const GXAVLTreeNode &node, GXVoid* args );
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
};


#endif //GX_SHADER_PROGRAM
