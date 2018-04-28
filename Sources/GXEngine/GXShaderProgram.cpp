// version 1.3

#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXLocale.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


#define PRECOMPILED_SHADER_PROGRAM_DICTIONARY		L"Shaders/Precompiled shader program dictionary.bin"
#define PRECOMPILED_SHADER_PROGRAM_INFO				L"Shaders/Precompiled shader programs.info"
#define PRECOMPILED_SHADER_PROGRAM_DIRECTORY		L"Shaders/Precompiled shader programs"
#define PRECOMPILED_SHADER_PROGRAM_EXTENSION		L"spr"

#define NULL_STRING_OFFSET							0u
#define STRING_BUFFER_SYMBOLS						260u

#define INVALID_SHADER								0u
#define SHADER_NOT_NEEDED							0u
#define INVALID_SHADER_PROGRAM						0u

//-------------------------------------------------------------------------------

class GXShaderProgramEntry;
static GXShaderProgramEntry* gx_ShaderProgramHead = nullptr;

class GXShaderProgramEntry
{
	private:
		GXShaderProgram*		program;
		GXInt					refs;

	public:
		GXShaderProgramEntry*	prev;
		GXShaderProgramEntry*	next;

	public:
		explicit GXShaderProgramEntry ( GXShaderProgram &program );

		GXShaderProgram& GetProgram () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXShaderProgramEntry ();

		GXShaderProgramEntry () = delete;
		GXShaderProgramEntry ( const GXShaderProgramEntry &other ) = delete;
		GXShaderProgramEntry& operator = ( const GXShaderProgramEntry &other ) = delete;
};

GXShaderProgramEntry::GXShaderProgramEntry ( GXShaderProgram &program ):
	program ( &program ),
	refs ( 1 ),
	prev ( nullptr )
{
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

	if ( refs > 0 ) return;

	delete this;
}

GXShaderProgramEntry::~GXShaderProgramEntry ()
{
	delete program;

	if ( gx_ShaderProgramHead == this )
		gx_ShaderProgramHead = gx_ShaderProgramHead->next;
	else
		prev->next = next;

	if ( !next ) return;

	next->prev = prev;
}

//-------------------------------------------------------------------------------

class GXPrecompiledShaderProgramFinder;
class GXPrecompiledShaderProgramNode : public GXAVLTreeNode
{
	friend class GXPrecompiledShaderProgramFinder;

	private:
		GLenum		binaryFormat;
		GXWChar*	binaryPath;
		GXWChar*	vs;
		GXWChar*	gs;
		GXWChar*	fs;

	private:
		GXPrecompiledShaderProgramNode ();
		explicit GXPrecompiledShaderProgramNode ( const GXUTF8* vs, const GXUTF8* fs, const GXUTF8* gs, const GXUTF8* binaryPath, GLenum binaryFormat );
		explicit GXPrecompiledShaderProgramNode ( const GXWChar* vs, const GXWChar* fs, const GXWChar* gs, const GXWChar* binaryPath, GLenum binaryFormat );
		~GXPrecompiledShaderProgramNode () override;

		static GXInt GXCALL Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );
		static GXVoid GXCALL InitFinderNode ( GXPrecompiledShaderProgramNode& node, const GXWChar* vs, const GXWChar* fs, const GXWChar* gs );
		static GXVoid GXCALL DestroyFinderNode ( GXPrecompiledShaderProgramNode& node );

		GXPrecompiledShaderProgramNode ( const GXPrecompiledShaderProgramNode &other ) = delete;
		GXPrecompiledShaderProgramNode& operator = ( const GXPrecompiledShaderProgramNode &other ) = delete;
};

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ():
	binaryPath ( nullptr ),
	binaryFormat ( GL_INVALID_ENUM ),
	vs ( nullptr ),
	gs ( nullptr ),
	fs ( nullptr )
{
	// NOTHING
}

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXUTF8* vs, const GXUTF8* gs, const GXUTF8* fs, const GXUTF8* binaryPath, GLenum binaryFormat ):
	binaryFormat ( binaryFormat )
{
	GXToWcs ( &this->binaryPath, binaryPath );
	GXToWcs ( &this->vs, vs );

	if ( gs )
		GXToWcs ( &this->gs, gs );
	else
		this->gs = nullptr;

	if ( fs )
		GXToWcs ( &this->fs, fs );
	else
		this->fs = nullptr;
}

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs, const GXWChar* binaryPath, GLenum binaryFormat ):
	binaryFormat ( binaryFormat )
{
	GXWcsclone ( &this->binaryPath, binaryPath );
	GXWcsclone ( &this->vs, vs );

	if ( gs )
		GXWcsclone ( &this->gs, gs );
	else
		this->gs = nullptr;

	if ( fs )
		GXWcsclone ( &this->fs, fs );
	else
		this->fs = nullptr;
}

GXPrecompiledShaderProgramNode::~GXPrecompiledShaderProgramNode ()
{
	free ( binaryPath );
	free ( vs );
	GXSafeFree ( gs );
	GXSafeFree ( fs );
}

GXInt GXCALL GXPrecompiledShaderProgramNode::Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b )
{
	const GXPrecompiledShaderProgramNode& aNode = static_cast<const GXPrecompiledShaderProgramNode&> ( a );
	const GXPrecompiledShaderProgramNode& bNode = static_cast<const GXPrecompiledShaderProgramNode&> ( b );

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
	node.vs = const_cast<GXWChar*> ( vs );
	node.gs = const_cast<GXWChar*> ( gs );
	node.fs = const_cast<GXWChar*> ( fs );
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

#pragma pack ( push )
#pragma pack ( 1 )

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

#pragma pack ( pop )

//-------------------------------------------------------------------------------

class GXSaveState
{
	public:
		GXWriteFileStream	info;

		GXUByte*			content;
		GXUBigInt			chunckOffset;
		GXUBigInt			utf8StringOffset;

		explicit GXSaveState ( const GXWChar* infoFile );
		~GXSaveState ();

	private:
		GXSaveState () = delete;
		GXSaveState ( const GXSaveState &other ) = delete;
		GXSaveState& operator = ( const GXSaveState &other ) = delete;
};

GXSaveState::GXSaveState ( const GXWChar* infoFile ):
	info ( infoFile )
{
	// NOTHING
}

GXSaveState::~GXSaveState ()
{
	info.Close ();
}

//-------------------------------------------------------------------------------

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

		GXPrecompiledShaderProgramFinder ( const GXPrecompiledShaderProgramFinder &other ) = delete;
		GXPrecompiledShaderProgramFinder& operator = ( const GXPrecompiledShaderProgramFinder &other ) = delete;
};

GXPrecompiledShaderProgramFinder::GXPrecompiledShaderProgramFinder ():
	GXAVLTree ( &GXPrecompiledShaderProgramNode::Compare, GX_TRUE )
{
	GXUByte* data = nullptr;
	GXUBigInt size = 0u;

	if ( !GXLoadFile ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY, reinterpret_cast<GXVoid**> ( &data ), size, GX_FALSE ) )
	{
		counter = 0u;
		return;
	}

	GXDictionaryHeader* header = reinterpret_cast<GXDictionaryHeader*> ( data );
	GXChunk* chuncks = reinterpret_cast<GXChunk*> ( data + header->chunkOffset );
	counter = header->counter;

	for ( GXUInt i = 0u; i < header->totalPrecompiledPrograms; i++ )
	{
		const GXChunk& chunk = chuncks[ i ];

		const GXUTF8* vs = chunk.vsOffset == NULL_STRING_OFFSET ? nullptr : reinterpret_cast<const GXUTF8*> ( data + chunk.vsOffset );
		const GXUTF8* gs = chunk.gsOffset == NULL_STRING_OFFSET ? nullptr : reinterpret_cast<const GXUTF8*> ( data + chunk.gsOffset );
		const GXUTF8* fs = chunk.fsOffset == NULL_STRING_OFFSET ? nullptr : reinterpret_cast<const GXUTF8*> ( data + chunk.fsOffset );
		const GXUTF8* binaryPath = reinterpret_cast<const GXUTF8*> ( data + chunk.binaryPathOffset );

		GXPrecompiledShaderProgramNode* node = new GXPrecompiledShaderProgramNode ( vs, gs, fs, binaryPath, chunk.binaryFormat );
		Add ( *node );
	}

	free ( data );
}

GXPrecompiledShaderProgramFinder::~GXPrecompiledShaderProgramFinder ()
{
	// NOTHING
}

GXBool GXPrecompiledShaderProgramFinder::FindProgram ( const GXWChar** binaryPath, GLenum &binaryFormat, const GXWChar* vs, const GXWChar* fs, const GXWChar* gs ) const
{
	GXPrecompiledShaderProgramNode finderNode;
	GXPrecompiledShaderProgramNode::InitFinderNode ( finderNode, vs, gs, fs );
	const GXPrecompiledShaderProgramNode* program = static_cast<const GXPrecompiledShaderProgramNode*> ( Find ( finderNode ) );
	GXPrecompiledShaderProgramNode::DestroyFinderNode ( finderNode );

	if ( program )
	{
		*binaryPath = program->binaryPath;
		binaryFormat = program->binaryFormat;
		return GX_TRUE;
	}
	else
	{
		*binaryPath = nullptr;
		binaryFormat = GL_INVALID_ENUM;
		return GX_FALSE;
	}
}

GXVoid GXPrecompiledShaderProgramFinder::AddProgram ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs, const GXWChar* binaryPath, GLenum binaryFormat )
{
	GXPrecompiledShaderProgramNode* node = new GXPrecompiledShaderProgramNode ( vs, gs, fs, binaryPath, binaryFormat );
	Add ( *node );

	GXUBigInt size = 0u;
	DoPostfix ( root, &GXPrecompiledShaderProgramFinder::GetDictionarySize, &size );

	size += sizeof ( GXDictionaryHeader );

	GXUByte* dictionary = static_cast<GXUByte*> ( malloc ( size ) );
	GXDictionaryHeader* header = reinterpret_cast<GXDictionaryHeader*> ( dictionary );
	header->counter = counter;
	header->totalPrecompiledPrograms = totalNodes;
	header->chunkOffset = sizeof ( GXDictionaryHeader );

	GXSaveState state ( PRECOMPILED_SHADER_PROGRAM_INFO );
	state.content = dictionary;
	state.chunckOffset = header->chunkOffset;
	state.utf8StringOffset = sizeof ( GXDictionaryHeader ) + header->totalPrecompiledPrograms * sizeof ( GXChunk );

	DoPostfix ( root, &GXPrecompiledShaderProgramFinder::SaveDictionary, &state );

	if ( !GXWriteToFile ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY, reinterpret_cast<const GXVoid*> ( dictionary ), static_cast<GXUInt> ( size ) ) )
		GXLogW ( L"GXPrecompiledShaderProgramFinder::AddProgram::Error - Не могу сохранить файл словаря прекомпилированных шейдерных программ.\n" );

	free ( dictionary );
}

GXUBigInt GXPrecompiledShaderProgramFinder::GetCounter ()
{
	GXUBigInt c = counter;
	counter++;
	return c;
}

GXVoid GXCALL GXPrecompiledShaderProgramFinder::GetDictionarySize ( const GXAVLTreeNode &node, GXVoid* args )
{
	GXUBigInt* size = static_cast<GXUBigInt*> ( args );
	const GXPrecompiledShaderProgramNode& item = static_cast<const GXPrecompiledShaderProgramNode&> ( node );
	
	*size += sizeof ( GXChunk );

	GXUTF8* stringU = nullptr;

	*size += GXToUTF8 ( &stringU, item.vs );;
	free ( stringU );

	if ( item.gs )
	{
		*size += GXToUTF8 ( &stringU, item.gs );
		free ( stringU );
	}

	if ( item.fs )
	{
		*size += GXToUTF8 ( &stringU, item.fs );
		free ( stringU );
	}

	*size += GXToUTF8 ( &stringU, item.binaryPath );
	free ( stringU );
};

GXVoid GXCALL GXPrecompiledShaderProgramFinder::SaveDictionary ( const GXAVLTreeNode &node, GXVoid* args )
{
	static const GXUTF8 newLineU[] = "\n";
	static const GXUPointer newLineSize = GXUTF8size ( newLineU ) - 1;

	GXLocale& locale = GXLocale::GetInstance ();

	const GXPrecompiledShaderProgramNode& item = static_cast<const GXPrecompiledShaderProgramNode&> ( node );
	GXSaveState* state = static_cast<GXSaveState*> ( args );
	GXChunk* chunk = reinterpret_cast<GXChunk*> ( state->content + state->chunckOffset );

	GXUTF8* stringU = nullptr;
	GXUPointer size = 0u;

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Vertex shader: " ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
	free ( stringU );

	size = GXToUTF8 ( &stringU, item.vs );
	memcpy ( state->content + state->utf8StringOffset, stringU, size );
	chunk->vsOffset = state->utf8StringOffset;
	state->utf8StringOffset += size;

	state->info.Write ( stringU, size - 1u );
	free ( stringU );

	state->info.Write ( newLineU, newLineSize );

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Geometry Shader: " ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
	free ( stringU );

	if ( item.gs )
	{
		size = GXToUTF8 ( &stringU, item.gs );
		memcpy ( state->content + state->utf8StringOffset, stringU, size );
		chunk->gsOffset = state->utf8StringOffset;
		state->utf8StringOffset += size;

		state->info.Write ( stringU, size - 1u );
		free ( stringU );
	}
	else
	{
		chunk->gsOffset = NULL_STRING_OFFSET;

		GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" ) );
		state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
		free ( stringU );
	}

	state->info.Write ( newLineU, newLineSize );

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Fragment Shader: " ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
	free ( stringU );

	if ( item.fs )
	{
		size = GXToUTF8 ( &stringU, item.fs );
		memcpy ( state->content + state->utf8StringOffset, stringU, size );
		chunk->fsOffset = state->utf8StringOffset;
		state->utf8StringOffset += size;

		state->info.Write ( stringU, size - 1u );

		free ( stringU );
	}
	else
	{
		chunk->fsOffset = NULL_STRING_OFFSET;

		GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" ) );
		state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
		free ( stringU );
	}

	state->info.Write ( newLineU, newLineSize );

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Precompiled shader program: " ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
	free ( stringU );

	size = GXToUTF8 ( &stringU, item.binaryPath );
	memcpy ( state->content + state->utf8StringOffset, stringU, size );
	chunk->binaryPathOffset = state->utf8StringOffset;
	state->utf8StringOffset += size;

	state->info.Write ( stringU, size - 1u );

	free ( stringU );

	state->info.Write ( newLineU, newLineSize );

	chunk->binaryFormat = item.binaryFormat;

	state->chunckOffset += sizeof ( GXChunk );

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->-------------------------------------" ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1u );
	free ( stringU );

	state->info.Write ( newLineU, newLineSize );
	state->info.Write ( newLineU, newLineSize );
}

//-------------------------------------------------------------------------------

GXPrecompiledShaderProgramFinder* GXShaderProgram::precompiledShaderProgramFinder = nullptr;
GXWChar* GXShaderProgram::stringBuffer = nullptr;

GXShaderProgram::GXShaderProgram ():
	vs ( nullptr ),
	gs ( nullptr ),
	fs ( nullptr ),
	program ( 0u )
{
	// NOTHING
}

GXShaderProgram::~GXShaderProgram ()
{
	if ( program == 0u ) return;

	free ( vs );
	GXSafeFree ( gs );
	GXSafeFree ( fs );

	glUseProgram ( 0u );
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

GXVoid GXShaderProgram::InitPrecompiledShaderProgramSubsystem ()
{
	if ( !glGetProgramBinary || !glProgramBinary || !glProgramParameteri ) return;

	stringBuffer = static_cast<GXWChar*> ( malloc ( STRING_BUFFER_SYMBOLS * sizeof ( GXWChar ) ) );
	precompiledShaderProgramFinder = new GXPrecompiledShaderProgramFinder ();

	if ( GXDoesDirectoryExist ( PRECOMPILED_SHADER_PROGRAM_DIRECTORY ) ) return;

	if ( GXCreateDirectory ( PRECOMPILED_SHADER_PROGRAM_DIRECTORY ) ) return;

	GXLogW ( L"GXShaderProgram::InitPrecompiledShaderProgramSubsystem::Error - Не могу создать директорию для прекомпилированных шейдерных программ [%s]\n", PRECOMPILED_SHADER_PROGRAM_DIRECTORY );
}

GXVoid GXShaderProgram::DestroyPrecompiledShaderProgramSubsystem ()
{
	GXSafeDelete ( precompiledShaderProgramFinder );
	GXSafeFree ( stringBuffer );
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
	GXUInt total = 0u;

	for ( GXShaderProgramEntry* p = gx_ShaderProgramHead; p; p = p->next )
		total++;

	if ( total > 0u )
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

	if ( info.gs )
		GXWcsclone ( &gs, info.gs );
	else
		gs = nullptr;

	if ( info.fs )
		GXWcsclone ( &fs, info.fs );
	else
		fs = nullptr;

	GXBool doesPrecompiledShaderProgramExist = GX_FALSE;

	if ( precompiledShaderProgramFinder )
	{
		const GXWChar* binaryPath = nullptr;
		GLenum binaryFormat = GL_INVALID_ENUM;

		doesPrecompiledShaderProgramExist = precompiledShaderProgramFinder->FindProgram ( &binaryPath, binaryFormat, vs, gs, fs );

		if ( doesPrecompiledShaderProgramExist )
		{
			GXUByte* precompiledShaderProgram;
			GXUPointer size = 0u;

			if ( GXLoadFile ( binaryPath, reinterpret_cast<GXVoid**> ( &precompiledShaderProgram ), size, GX_FALSE ) )
			{
				program = glCreateProgram ();

				glProgramBinary ( program, binaryFormat, precompiledShaderProgram, static_cast<GLsizei> ( size ) );

				free ( precompiledShaderProgram );

				GLint status = 0;
				glGetProgramiv ( program, GL_LINK_STATUS, &status );

				if ( status != GL_TRUE )
				{
					GXInt logSize = 0;
					glGetShaderiv ( program, GL_INFO_LOG_LENGTH, &logSize );

					GLchar* log = static_cast<GLchar*> ( malloc ( static_cast<GXUPointer> ( logSize ) ) );

					GLsizei length;
					glGetProgramInfoLog ( program, static_cast<GLsizei> ( logSize ), &length, log );

					GXLogW ( L"GXShaderProgramStatus::Warning - Не могу использовать прекомпилированную шейдерную программу:\n%s\n%s\n%s\n[%s]\n\n", vs, gs, fs, binaryPath );
					GXLogA ( "%s\n\n", log );
					GXLogW ( L"Возможно обновился драйвер видеокарты. Пробую сделать перекомпиляцию шейдерной программы...\n\n" );

					free ( log );

					glDeleteProgram ( program );
					program = CompileShaderProgram ( info );

					if ( program == INVALID_SHADER_PROGRAM ) return;

					SavePrecompiledShaderProgram ( program, info, binaryPath );
				}
			}
			else
			{
				GXLogW ( L"GXShaderProgram::GXShaderProgram::Warning - Не могу загрузить файл с прекомпилированной шейдерной программой:\n%s\n%s\n%s\n[%s]\nПопробую сделать перекомпиляцию шейдерной программы...\n\n", vs, gs, fs, binaryPath );
				program = CompileShaderProgram ( info );

				if ( program == INVALID_SHADER_PROGRAM ) return;

				SavePrecompiledShaderProgram ( program, info, binaryPath );
			}
		}
	}

	if ( !doesPrecompiledShaderProgramExist )
	{
		program = CompileShaderProgram ( info );

		if ( program == INVALID_SHADER_PROGRAM ) return;

		if ( precompiledShaderProgramFinder )
		{
			swprintf_s ( stringBuffer, STRING_BUFFER_SYMBOLS, L"%s/%llu.%s", PRECOMPILED_SHADER_PROGRAM_DIRECTORY, precompiledShaderProgramFinder->GetCounter (), PRECOMPILED_SHADER_PROGRAM_EXTENSION );
			SavePrecompiledShaderProgram ( program, info, stringBuffer );
		}
	}

	glUseProgram ( program );

	for ( GXUInt i = 0u; i < info.numSamplers; i++ )
		glUniform1i ( GetUniform ( info.samplerNames[ i ] ), static_cast<GLint> ( info.samplerLocations[ i ] ) );

	glUseProgram ( 0u );
}

GXBool GXShaderProgram::operator == ( const GXShaderProgramInfo &other ) const
{
	if ( GXWcscmp ( other.fs, fs ) != 0 ) return GX_FALSE;
	if ( GXWcscmp ( other.vs, vs ) != 0 ) return GX_FALSE;
	if ( GXWcscmp ( other.gs, gs ) != 0 ) return GX_FALSE;

	return GX_TRUE;
}

GXBool GXShaderProgram::operator == ( const GXShaderProgram &other ) const
{
	return program == other.program;
}

GLuint GXShaderProgram::GetShader ( GLenum type, const GXWChar* fileName )
{
	if ( !fileName ) return 0u;

	GLuint shader = glCreateShader ( type );

	GXChar* shaderSource = nullptr;
	GXUPointer shaderSourceLength = 0u;

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

	if ( !GXLoadFile ( fileName, reinterpret_cast<GXVoid**> ( &shaderSource ), shaderSourceLength, GX_TRUE ) )
	{
		GXLogW ( loadError, fileName );
		glDeleteShader ( shader );
		return 0u;
	}

	GLint sourceSize = static_cast<GLint> ( shaderSourceLength );

	glShaderSource ( shader, 1, static_cast<const GLchar* const*> ( &shaderSource ), &sourceSize );
	glCompileShader ( shader );
	GXSafeFree ( shaderSource );

	GLint status;

	glGetShaderiv ( shader, GL_COMPILE_STATUS, &status );

	if ( status != GL_TRUE )
	{
		GXInt size = 0;
		glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &size );

		GLchar* log = static_cast<GLchar*> ( malloc ( static_cast<GXUPointer> ( size ) ) );
		glGetShaderInfoLog ( shader, size, &size, log );

		GXLogW ( L"GXShaderProgram::GetShader::Error - Шейдер %s\n", fileName );
		GXLogA ( "%s\n", log );

		free ( log );

		glDeleteShader ( shader );
		return 0u;
	}

	return shader;
}

GLuint GXShaderProgram::CompileShaderProgram ( const GXShaderProgramInfo &info )
{
	GLuint shaderProgram = glCreateProgram ();

	GLuint vertexShader = GetShader ( GL_VERTEX_SHADER, vs );

	GLuint geometryShader = INVALID_SHADER;

	if ( gs )
	{
		geometryShader = GetShader ( GL_GEOMETRY_SHADER, gs );

		if ( geometryShader == INVALID_SHADER )
		{
			glDeleteProgram ( shaderProgram );
			return INVALID_SHADER_PROGRAM;
		}
	}
	else
	{
		geometryShader = SHADER_NOT_NEEDED;
	}

	GLuint fragmentShader = INVALID_SHADER;

	if ( fs )
	{
		fragmentShader = GetShader ( GL_FRAGMENT_SHADER, fs );

		if ( fragmentShader == INVALID_SHADER )
		{
			glDeleteProgram ( shaderProgram );
			return INVALID_SHADER_PROGRAM;
		}
	}
	else
	{
		fragmentShader = SHADER_NOT_NEEDED;
	}

	glAttachShader ( shaderProgram, vertexShader );
	glDeleteShader ( vertexShader );

	if ( geometryShader )
	{
		glAttachShader ( shaderProgram, geometryShader );
		glDeleteShader ( geometryShader );
	}

	if ( fragmentShader )
	{
		glAttachShader ( shaderProgram, fragmentShader );
		glDeleteShader ( fragmentShader );
	}

	if ( info.numTransformFeedbackOutputs > 0 )
		glTransformFeedbackVaryings ( shaderProgram, info.numTransformFeedbackOutputs, info.transformFeedbackOutputNames, GL_INTERLEAVED_ATTRIBS );

	glLinkProgram ( shaderProgram );

	GLint status = 0;
	glGetProgramiv ( shaderProgram, GL_LINK_STATUS, &status );

	if ( status != GL_TRUE )
	{
		GXInt size = 0;
		glGetShaderiv ( shaderProgram, GL_INFO_LOG_LENGTH, &size );

		GLchar* log = static_cast<GLchar*> ( malloc ( static_cast<GXUPointer> ( size ) ) );
		glGetProgramInfoLog ( shaderProgram, size, &size, log );

		GXLogW ( L"GXShaderProgram::CompileShaderProgram::Error - Не могу слинковать шейдерную программу %s + %s + %s\n", vs, gs, fs );
		GXLogA ( "%s\n", log );

		free ( log );

		glDeleteProgram ( shaderProgram );

		return INVALID_SHADER_PROGRAM;
	}

	return shaderProgram;
}

GXVoid GXShaderProgram::SavePrecompiledShaderProgram ( GLuint shaderProgram, const GXShaderProgramInfo &info, const GXWChar* binaryPath )
{
	GLint value = static_cast<GLint> ( GL_TRUE );
	glProgramParameteri ( shaderProgram, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, value );

	GLint precompiledShaderProgramSize = 0;
	glGetProgramiv ( shaderProgram, GL_PROGRAM_BINARY_LENGTH, &precompiledShaderProgramSize );

	GXUByte* precompiledShaderProgram = static_cast<GXUByte*> ( malloc ( static_cast<GXUPointer> ( precompiledShaderProgramSize ) ) );

	GLsizei length;
	GLenum binaryFormat;
	glGetProgramBinary ( shaderProgram, static_cast<GLsizei> ( precompiledShaderProgramSize ), &length, &binaryFormat, precompiledShaderProgram );

	GXWriteToFile ( binaryPath, precompiledShaderProgram, static_cast<GXUPointer> ( length ) );

	precompiledShaderProgramFinder->AddProgram ( info.vs, info.gs, info.fs, binaryPath, binaryFormat );

	free ( precompiledShaderProgram );
}
