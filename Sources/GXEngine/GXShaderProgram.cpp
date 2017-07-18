//version 1.1

#include <GXEngine/GXShaderProgram.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


#define PRECOMPILED_SHADER_PROGRAM_DICTIONARY		L"Shaders/Precompiled shader dictionary.bin"
#define PRECOMPILED_SHADER_PROGRAM_INFO				L"Shaders/Precompiled shaders.txt"
#define PRECOMPILED_SHADER_PROGRAM_FOLDER			L"Shaders/Precompiled shaders"
#define PRECOMPILED_SHADER_PROGRAM_EXTENSION		L"spr"

#define NULL_STRING_OFFSET							0


class GXShaderProgramEntry;
static GXShaderProgramEntry* gx_ShaderProgramHead = nullptr;

class GXShaderProgramEntry
{
	public:
		GXShaderProgramEntry*	next;
		GXShaderProgramEntry*	prev;

	private:
		GXShaderProgram*		program;
		GXInt					refs;

	public:
		explicit GXShaderProgramEntry ( GXShaderProgram &program );

		GXShaderProgram& GetProgram () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXShaderProgramEntry ();
};

GXShaderProgramEntry::GXShaderProgramEntry ( GXShaderProgram &program )
{
	this->program = &program;
	refs = 1;

	prev = nullptr;

	if ( gx_ShaderProgramHead )
		gx_ShaderProgramHead->prev = this;

	next = gx_ShaderProgramHead;
	gx_ShaderProgramHead = this;
}

GXShaderProgram& GXShaderProgramEntry::GetProgram () const
{
	return *program;
}

GXVoid GXShaderProgramEntry::AddRef ()
{
	refs++;
}

GXVoid GXShaderProgramEntry::Release ()
{
	refs--;
	if ( refs <= 0 )
		delete this;
}

GXShaderProgramEntry::~GXShaderProgramEntry ()
{
	delete program;

	if ( gx_ShaderProgramHead == this )
		gx_ShaderProgramHead = gx_ShaderProgramHead->next;
	else
		prev->next = next;

	if ( next )
		next->prev = prev;
}

//-------------------------------------------------------------------------------

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ()
{
	vs = nullptr;
	gs = nullptr;
	fs = nullptr;
	binaryPath = nullptr;
	binaryFormat = GL_INVALID_ENUM;
}

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXUTF8* vs, const GXUTF8* fs, const GXUTF8* gs, const GXUTF8* binaryPath, GLenum binaryFormat )
{
	GXToWcs ( &this->vs, vs );
	GXToWcs ( &this->gs, gs );
	GXToWcs ( &this->fs, fs );
	GXToWcs ( &this->binaryPath, binaryPath );
	this->binaryFormat = binaryFormat;
}

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXWChar* vs, const GXWChar* fs, const GXWChar* gs, const GXWChar* binaryPath, GLenum binaryFormat )
{
	GXWcsclone ( &this->vs, vs );
	GXWcsclone ( &this->gs, gs );
	GXWcsclone ( &this->fs, fs );
	GXWcsclone ( &this->binaryPath, binaryPath );

	this->binaryFormat = binaryFormat;
}

GXPrecompiledShaderProgramNode::~GXPrecompiledShaderProgramNode ()
{
	GXSafeFree ( vs );
	GXSafeFree ( gs );
	GXSafeFree ( fs );
	GXSafeFree ( binaryPath );
}

GXInt GXCALL GXPrecompiledShaderProgramNode::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
	GXPrecompiledShaderProgramNode& aNode = (GXPrecompiledShaderProgramNode&)a;
	GXPrecompiledShaderProgramNode& bNode = (GXPrecompiledShaderProgramNode&)b;

	GXInt compare = GXWcscmp ( aNode.fs, bNode.fs );

	if ( compare != 0 )
		return compare;

	compare = GXWcscmp ( aNode.vs, bNode.vs );

	if ( compare != 0 )
		return compare;

	return GXWcscmp ( aNode.gs, bNode.gs );
}

GXVoid GXCALL GXPrecompiledShaderProgramNode::InitFinderNode ( GXPrecompiledShaderProgramNode& node, const GXWChar* vs, const GXWChar* fs, const GXWChar* gs )
{
	node.vs = (GXWChar*)vs;
	node.gs = (GXWChar*)gs;
	node.fs = (GXWChar*)fs;
	node.binaryPath = nullptr;
}

GXVoid GXCALL GXPrecompiledShaderProgramNode::DestroyFinderNode ( GXPrecompiledShaderProgramNode& node )
{
	node.vs = nullptr;
	node.gs = nullptr;
	node.fs = nullptr;
	node.binaryPath = nullptr;
}

//-------------------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

struct GXDictionaryHeader
{
	GXUBigInt	counter;
	GXUInt		totalPrecompiledPrograms;
	GXUBigInt	chunkOffset;
};

struct GXChunk
{
	GXUBigInt	vsOffset;
	GXUBigInt	gsOffset;
	GXUBigInt	fsOffset;
	GXUBigInt	binaryPathOffset;
	GLenum		binaryFormat;
};

#pragma pack(pop)

GXPrecompiledShaderProgramFinder::GXPrecompiledShaderProgramFinder () :
GXAVLTree ( &GXPrecompiledShaderProgramNode::Compare, GX_TRUE )
{
	GXUByte* data = nullptr;
	GXUBigInt size = 0;

	if ( !GXLoadFile ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY, (GXVoid**)&data, size, GX_FALSE ) ) return;

	GXDictionaryHeader* header = (GXDictionaryHeader*)data;
	GXChunk* chuncks = (GXChunk*)( data + header->chunkOffset );
	counter = header->counter;

	for ( GXUInt i = 0; i < header->totalPrecompiledPrograms; i++ )
	{
		const GXChunk& chunk = chuncks[ i ];

		const GXUTF8* vs = chunk.vsOffset == NULL_STRING_OFFSET ? nullptr : (const GXUTF8*)( data + chunk.vsOffset );
		const GXUTF8* gs = chunk.gsOffset == NULL_STRING_OFFSET ? nullptr : (const GXUTF8*)( data + chunk.gsOffset );
		const GXUTF8* fs = chunk.fsOffset == NULL_STRING_OFFSET ? nullptr : (const GXUTF8*)( data + chunk.fsOffset );
		const GXUTF8* binaryPath = (const GXUTF8*)( data + chunk.binaryPathOffset );

		GXPrecompiledShaderProgramNode* node = new GXPrecompiledShaderProgramNode ( vs, fs, gs, binaryPath, chunk.binaryFormat );
		Add ( *node );
	}

	free ( data );
}

GXPrecompiledShaderProgramFinder::~GXPrecompiledShaderProgramFinder ()
{
	// NOTHING
}

GXVoid GXPrecompiledShaderProgramFinder::FindProgram ( const GXWChar** binaryPath, GLenum &binaryFormat, const GXWChar* vs, const GXWChar* fs, const GXWChar* gs ) const
{
	GXPrecompiledShaderProgramNode finderNode;
	GXPrecompiledShaderProgramNode::InitFinderNode ( finderNode, vs, gs, fs );
	GXPrecompiledShaderProgramNode* program = (GXPrecompiledShaderProgramNode*)Find ( finderNode );
	GXPrecompiledShaderProgramNode::DestroyFinderNode ( finderNode );

	if ( program )
	{
		*binaryPath = program->binaryPath;
		binaryFormat = program->binaryFormat;
	}
	else
	{
		*binaryPath = nullptr;
		binaryFormat = GL_INVALID_ENUM;
	}
}

GXVoid GXPrecompiledShaderProgramFinder::AddProgram ( const GXWChar* vs, const GXWChar* fs, const GXWChar* gs, const GXWChar* binaryPath, GLenum binaryFormat )
{
	GXPrecompiledShaderProgramNode* node = new GXPrecompiledShaderProgramNode ( vs, fs, gs, binaryPath, binaryFormat );
	Add ( *node );

	//TODO Update files
}

GXVoid GXCALL GXPrecompiledShaderProgramFinder::GetDictionarySize ( const GXAVLTreeNode &node, GXVoid* args )
{
	GXUBigInt* size = (GXUBigInt*)args;
	const GXPrecompiledShaderProgramNode& item = (const GXPrecompiledShaderProgramNode&)node;
	
	if ( item.vs )
	{
		GXUTF8* stringU;
		GXToUTF8 ( &stringU, item.vs );
		*size += GXUTF8len ( stringU );
		free ( stringU );

		//TODO
	}
};

GXVoid GXCALL GXPrecompiledShaderProgramFinder::SaveDictionary ( const GXAVLTreeNode &node, GXVoid* args )
{

}

//-------------------------------------------------------------------------------

GXDynamicArray GXShaderProgram::stringArray ( sizeof ( GXWChar ) );

GXShaderProgram::GXShaderProgram ()
{
	vs = gs = fs = nullptr;
	program = 0;
}

GXShaderProgram::~GXShaderProgram ()
{
	GXSafeFree ( vs );
	GXSafeFree ( gs );
	GXSafeFree ( fs );

	if ( program == 0 ) return;

	glUseProgram ( 0 );
	glDeleteProgram ( program );
}

GLuint GXShaderProgram::GetProgram () const
{
	return program;
}

GLint GXShaderProgram::GetUniform ( const GLchar* name ) const
{
	GLint uniform = glGetUniformLocation ( program, name );


	if ( uniform == -1 )
	{
		GXWChar* nameW;
		GXToWcs ( &nameW, name );
		GXLogW ( L"GXShaderProgram::GetUniform::Warning - Не могу найти юниформу %s в программе %s + %s + %s\n", nameW, vs, gs, fs );
		free ( nameW );
	}

	return uniform;
}

GXVoid GXShaderProgram::InitShaderProgramCache ()
{
	if ( !glGetProgramBinary || !glProgramBinary || !glProgramParameteri ) return;

	GXUByte* dictionary = nullptr;
	GXUBigInt size = 0;
	if ( !GXLoadFile ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY, (GXVoid**)&dictionary, size, GX_FALSE ) )
	{
		GXLogW ( L"GXShaderProgram::InitShaderProgramCache::Error - Не могу загрузить словарь откомпилированных шейдерных программ\n" );
		return;
	}

	//TODO
}

GXVoid GXShaderProgram::DestroyShaderProgramCache ()
{
	if ( !glGetProgramBinary || !glProgramBinary || !glProgramParameteri ) return;
}

GXShaderProgram& GXCALL GXShaderProgram::GetShaderProgram ( const GXShaderProgramInfo &info )
{
	for ( GXShaderProgramEntry* p = gx_ShaderProgramHead; p; p = p->next )
	{
		if ( p->GetProgram () == info )
		{
			p->AddRef ();
			return p->GetProgram ();
		}
	}

	GXShaderProgram* shaderProgram = new GXShaderProgram ( info );
	new GXShaderProgramEntry ( *shaderProgram );

	return *shaderProgram;
}

GXVoid GXCALL GXShaderProgram::RemoveShaderProgram ( GXShaderProgram &program )
{
	for ( GXShaderProgramEntry* p = gx_ShaderProgramHead; p; p = p->next )
	{
		if ( p->GetProgram () == program )
		{
			p->Release ();
			program = GXShaderProgram ();
			return;
		}
	}
}

GXUInt GXCALL GXShaderProgram::GetTotalLoadedShaderPrograms ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS )
{
	GXUInt total = 0;
	for ( GXShaderProgramEntry* p = gx_ShaderProgramHead; p; p = p->next )
		total++;

	if ( total > 0 )
	{
		GXShaderProgram& program = gx_ShaderProgramHead->GetProgram ();
		*lastVS = program.vs;
		*lastGS = program.gs;
		*lastFS = program.fs;
	}
	else
	{
		*lastVS = nullptr;
		*lastGS = nullptr;
		*lastFS = nullptr;
	}

	return total;
}

GXVoid GXShaderProgram::operator = ( const GXShaderProgram &shadepProgram )
{
	memcpy ( this, &shadepProgram, sizeof ( GXShaderProgram ) );
}

GXShaderProgram::GXShaderProgram ( const GXShaderProgramInfo &info )
{
	GXWcsclone ( &vs, info.vs );
	GXWcsclone ( &gs, info.gs );
	GXWcsclone ( &fs, info.fs );

	GLuint vertexShader = GetShader ( GL_VERTEX_SHADER, vs );
	GLuint geometryShader = GetShader ( GL_GEOMETRY_SHADER, gs );
	GLuint fragmentShader = GetShader ( GL_FRAGMENT_SHADER, fs );

	program = glCreateProgram ();
	glAttachShader ( program, vertexShader );
	glDeleteShader ( vertexShader );

	if ( geometryShader )
	{
		glAttachShader ( program, geometryShader );
		glDeleteShader ( geometryShader );
	}

	if ( fragmentShader )
	{
		glAttachShader ( program, fragmentShader );
		glDeleteShader ( fragmentShader );
	}

	if ( info.numTransformFeedbackOutputs > 0 )
		glTransformFeedbackVaryings ( program, info.numTransformFeedbackOutputs, info.transformFeedbackOutputNames, GL_INTERLEAVED_ATTRIBS );

	glLinkProgram ( program );

	GLint status = 0;
	glGetProgramiv ( program, GL_LINK_STATUS, &status );

	if ( status != GL_TRUE )
	{
		GXInt size = 0;
		glGetShaderiv ( program, GL_INFO_LOG_LENGTH, &size );

		GLchar* log = (GLchar*)malloc ( size );
		glGetProgramInfoLog ( program, size, &size, log );

		GXLogW ( L"GXShaderProgramStatus::Error - Не могу слинковать шейдер %s + %s + %s\n", vs, gs, fs );
		GXLogA ( "%s\n", log );

		free ( log );

		glDeleteProgram ( program );
		program = 0;

		return;
	}

	glUseProgram ( program );

	for ( GXInt i = 0; i < info.numSamplers; i++ )
		glUniform1i ( GetUniform ( info.samplerNames[ i ] ), info.samplerLocations[ i ] );

	glUseProgram ( 0 );
}

GXBool GXShaderProgram::operator == ( const GXShaderProgramInfo &other ) const
{
	if ( !GXWcscmp ( other.vs, vs ) != 0 ) return GX_FALSE;
	if ( !GXWcscmp ( other.gs, gs ) != 0 ) return GX_FALSE;
	if ( !GXWcscmp ( other.fs, fs ) != 0 ) return GX_FALSE;

	return GX_TRUE;
}

GXBool GXShaderProgram::operator == ( const GXShaderProgram &other ) const
{
	return program == other.program;
}

GLuint GXShaderProgram::GetShader ( GLenum type, const GXWChar* fileName )
{
	if ( !fileName ) return 0;

	GLuint shader = glCreateShader ( type );

	GXChar* shaderSource = nullptr;
	GXUBigInt shaderSourceLength = 0;

	static const GXWChar* vertexShaderLoadError = L"GXShaderProgram::GetShader::Error - Не могу загрузить вершинный шейдер %s\n";
	static const GXWChar* geometryShaderLoadError = L"GXShaderProgram::GetShader::Error - Не могу загрузить геометрический шейдер %s\n";
	static const GXWChar* fragmentShaderLoadError = L"GXShaderProgram::GetShader::Error - Не могу загрузить фрагментный шейдер %s\n";

	const GXWChar* loadError = nullptr;

	switch ( type )
	{
		case GL_VERTEX_SHADER:
			loadError = vertexShaderLoadError;
		break;

		case GL_GEOMETRY_SHADER:
			loadError = geometryShaderLoadError;
		break;

		case GL_FRAGMENT_SHADER:
			loadError = fragmentShaderLoadError;
		break;
	}

	if ( !GXLoadFile ( fileName, (GXVoid**)&shaderSource, shaderSourceLength, GX_TRUE ) )
	{
		GXLogW ( loadError, fileName );
		glDeleteShader ( shader );
		return 0;
	}

	glShaderSource ( shader, 1, (const GLchar**)&shaderSource, (const GLint*)&shaderSourceLength );
	glCompileShader ( shader );
	GXSafeFree ( shaderSource );

	GLint status;

	glGetShaderiv ( shader, GL_COMPILE_STATUS, &status );

	if ( status != GL_TRUE )
	{
		GXInt size = 0;
		glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &size );

		GLchar* log = (GLchar*)malloc ( size );
		glGetShaderInfoLog ( shader, size, &size, log );

		GXLogW ( L"GXShaderProgram::GetShader::Error - Шейдер %s\n", fileName );
		GXLogA ( "%s\n", log );

		free ( log );

		glDeleteShader ( shader );
		return 0;
	}

	return shader;
}
