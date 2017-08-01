//version 1.2

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

#define NULL_STRING_OFFSET							0
#define STRING_BUFFER_SYMBOLS						260


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

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXUTF8* vs, const GXUTF8* gs, const GXUTF8* fs, const GXUTF8* binaryPath, GLenum binaryFormat )
{
	GXToWcs ( &this->vs, vs );

	if ( gs )
		GXToWcs ( &this->gs, gs );
	else
		this->gs = nullptr;

	if ( fs )
		GXToWcs ( &this->fs, fs );
	else
		this->fs = nullptr;
	
	GXToWcs ( &this->binaryPath, binaryPath );
	this->binaryFormat = binaryFormat;
}

GXPrecompiledShaderProgramNode::GXPrecompiledShaderProgramNode ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs, const GXWChar* binaryPath, GLenum binaryFormat )
{
	GXWcsclone ( &this->vs, vs );

	if ( gs )
		GXWcsclone ( &this->gs, gs );
	else
		this->gs = nullptr;

	if ( fs )
		GXWcsclone ( &this->fs, fs );
	else
		this->fs = nullptr;

	GXWcsclone ( &this->binaryPath, binaryPath );
	this->binaryFormat = binaryFormat;
}

GXPrecompiledShaderProgramNode::~GXPrecompiledShaderProgramNode ()
{
	free ( vs );
	GXSafeFree ( gs );
	GXSafeFree ( fs );
	free ( binaryPath );
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

//-------------------------------------------------------------------------------

struct GXSaveState
{
	GXUByte*			content;
	GXUBigInt			chunckOffset;
	GXUBigInt			utf8StringOffset;
	GXWriteFileStream	info;

	GXSaveState ( const GXWChar* infoFile );
	~GXSaveState ();
};

GXSaveState::GXSaveState ( const GXWChar* infoFile ) :
info ( infoFile )
{
	// NOTHING
}

GXSaveState::~GXSaveState ()
{
	info.Close ();
}

//-------------------------------------------------------------------------------

GXPrecompiledShaderProgramFinder::GXPrecompiledShaderProgramFinder () :
GXAVLTree ( &GXPrecompiledShaderProgramNode::Compare, GX_TRUE )
{
	GXUByte* data = nullptr;
	GXUBigInt size = 0;

	if ( !GXLoadFile ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY, (GXVoid**)&data, size, GX_FALSE ) )
	{
		counter = 0;
		return;
	}

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
	GXPrecompiledShaderProgramNode* program = (GXPrecompiledShaderProgramNode*)Find ( finderNode );
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

	GXUBigInt size = 0;
	DoPostfix ( root, &GXPrecompiledShaderProgramFinder::GetDictionarySize, &size );

	size += sizeof ( GXDictionaryHeader );

	GXUByte* dictionary = (GXUByte*)malloc ( size );
	GXDictionaryHeader* header = (GXDictionaryHeader*)dictionary;
	header->counter = counter;
	header->totalPrecompiledPrograms = totalNodes;
	header->chunkOffset = sizeof ( GXDictionaryHeader );

	GXSaveState state ( PRECOMPILED_SHADER_PROGRAM_INFO );
	state.content = dictionary;
	state.chunckOffset = header->chunkOffset;
	state.utf8StringOffset = sizeof ( GXDictionaryHeader ) + header->totalPrecompiledPrograms * sizeof ( GXChunk );

	DoPostfix ( root, &GXPrecompiledShaderProgramFinder::SaveDictionary, &state );

	if ( !GXWriteToFile ( PRECOMPILED_SHADER_PROGRAM_DICTIONARY, (const GXVoid*)dictionary, (GXUInt)size ) )
		GXLogW ( L"GXPrecompiledShaderProgramFinder::AddProgram::Error - Не могу сохранить файл словаря прекомпилированных шейдерных программ." );

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
	GXUBigInt* size = (GXUBigInt*)args;
	const GXPrecompiledShaderProgramNode& item = (const GXPrecompiledShaderProgramNode&)node;
	
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

	GXUInt s = GXToUTF8 ( &stringU, item.binaryPath );
	*size += s;
	free ( stringU );
};

GXVoid GXCALL GXPrecompiledShaderProgramFinder::SaveDictionary ( const GXAVLTreeNode &node, GXVoid* args )
{
	static const GXUTF8 newLineU[] = "\n";
	static const GXUPointer newLineSize = GXUTF8size ( newLineU ) - 1;

	GXLocale& locale = GXLocale::GetInstance ();
	const GXWChar* delimeterW = locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->-------------------------------------" );

	const GXPrecompiledShaderProgramNode& item = (const GXPrecompiledShaderProgramNode&)node;
	GXSaveState* state = (GXSaveState*)args;
	GXChunk* chunk = (GXChunk*)( state->content + state->chunckOffset );

	GXUTF8* stringU = nullptr;
	GXUInt size = 0;

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Vertex shader: " ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1 );
	free ( stringU );

	size = GXToUTF8 ( &stringU, item.vs );
	memcpy ( state->content + state->utf8StringOffset, stringU, size );
	chunk->vsOffset = state->utf8StringOffset;
	state->utf8StringOffset += size;

	state->info.Write ( stringU, size - 1 );
	free ( stringU );

	state->info.Write ( newLineU, newLineSize );

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Geometry Shader: " ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1 );
	free ( stringU );

	if ( item.gs )
	{
		size = GXToUTF8 ( &stringU, item.gs );
		memcpy ( state->content + state->utf8StringOffset, stringU, size );
		chunk->gsOffset = state->utf8StringOffset;
		state->utf8StringOffset += size;

		state->info.Write ( stringU, size - 1 );
		free ( stringU );
	}
	else
	{
		chunk->gsOffset = NULL_STRING_OFFSET;

		GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" ) );
		state->info.Write ( stringU, GXUTF8size ( stringU ) - 1 );
		free ( stringU );
	}

	state->info.Write ( newLineU, newLineSize );

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Fragment Shader: " ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1 );
	free ( stringU );

	if ( item.fs )
	{
		size = GXToUTF8 ( &stringU, item.fs );
		memcpy ( state->content + state->utf8StringOffset, stringU, size );
		chunk->fsOffset = state->utf8StringOffset;
		state->utf8StringOffset += size;

		state->info.Write ( stringU, size - 1 );

		free ( stringU );
	}
	else
	{
		chunk->fsOffset = NULL_STRING_OFFSET;

		GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Not needed" ) );
		state->info.Write ( stringU, GXUTF8size ( stringU ) - 1 );
		free ( stringU );
	}

	state->info.Write ( newLineU, newLineSize );

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->Precompiled shader program: " ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1 );
	free ( stringU );

	size = GXToUTF8 ( &stringU, item.binaryPath );
	memcpy ( state->content + state->utf8StringOffset, stringU, size );
	chunk->binaryPathOffset = state->utf8StringOffset;
	state->utf8StringOffset += size;

	state->info.Write ( stringU, size - 1 );

	free ( stringU );

	state->info.Write ( newLineU, newLineSize );

	chunk->binaryFormat = item.binaryFormat;

	state->chunckOffset += sizeof ( GXChunk );

	GXToUTF8 ( &stringU, locale.GetString ( L"GXShaderProgram->GXPrecompiledShaderProgramFinder::SaveDictionary->-------------------------------------" ) );
	state->info.Write ( stringU, GXUTF8size ( stringU ) - 1 );
	free ( stringU );

	state->info.Write ( newLineU, newLineSize );
	state->info.Write ( newLineU, newLineSize );
}

//-------------------------------------------------------------------------------

GXPrecompiledShaderProgramFinder* GXShaderProgram::precompiledShaderProgramFinder = nullptr;
GXWChar* GXShaderProgram::stringBuffer = nullptr;

GXShaderProgram::GXShaderProgram ()
{
	vs = gs = fs = nullptr;
	program = 0;
}

GXShaderProgram::~GXShaderProgram ()
{
	free ( vs );
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

GXVoid GXShaderProgram::InitPrecompiledShaderProgramSubsystem ()
{
	if ( !glGetProgramBinary || !glProgramBinary || !glProgramParameteri ) return;

	stringBuffer = (GXWChar*)malloc ( STRING_BUFFER_SYMBOLS * sizeof ( GXWChar ) );
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
			GXUPointer size = 0;

			if ( GXLoadFile ( binaryPath, (GXVoid**)&precompiledShaderProgram, size, GX_FALSE ) )
			{
				program = glCreateProgram ();

				glProgramBinary ( program, binaryFormat, precompiledShaderProgram, (GLsizei)size );

				free ( precompiledShaderProgram );

				GLint status = 0;
				glGetProgramiv ( program, GL_LINK_STATUS, &status );

				if ( status != GL_TRUE )
				{
					GXInt size = 0;
					glGetShaderiv ( program, GL_INFO_LOG_LENGTH, &size );

					GLchar* log = (GLchar*)malloc ( size );
					glGetProgramInfoLog ( program, size, &size, log );

					GXLogW ( L"GXShaderProgramStatus::Warning - Не могу использовать прекомпилированную шейдерную программу:\n%s\n%s\n%s\n[%s]\n\n", vs, gs, fs, binaryPath );
					GXLogA ( "%s\n\n", log );
					GXLogW ( L"Возможно обновился драйвер видеокарты. Пробую сделать перекомпиляцию шейдерной программы...\n\n" );

					free ( log );

					glDeleteProgram ( program );
					program = CompileShaderProgram ( info );
					SavePrecompiledShaderProgram ( program, info, binaryPath );
				}
			}
			else
			{
				GXLogW ( L"GXShaderProgram::GXShaderProgram::Warning - Не могу загрузить файл с прекомпилированной шейдерной программой:\n%s\n%s\n%s\n[%s]\nПопробую сделать перекомпиляцию шейдерной программы...\n\n", vs, gs, fs, binaryPath );
				program = CompileShaderProgram ( info );
				SavePrecompiledShaderProgram ( program, info, binaryPath );
			}
		}
	}

	if ( !doesPrecompiledShaderProgramExist )
	{
		program = CompileShaderProgram ( info );

		if ( precompiledShaderProgramFinder )
		{
			swprintf_s ( stringBuffer, STRING_BUFFER_SYMBOLS, L"%s/%llu.%s", PRECOMPILED_SHADER_PROGRAM_DIRECTORY, precompiledShaderProgramFinder->GetCounter (), PRECOMPILED_SHADER_PROGRAM_EXTENSION );
			SavePrecompiledShaderProgram ( program, info, stringBuffer );
		}
	}

	glUseProgram ( program );

	for ( GXInt i = 0; i < info.numSamplers; i++ )
		glUniform1i ( GetUniform ( info.samplerNames[ i ] ), info.samplerLocations[ i ] );

	glUseProgram ( 0 );
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
	if ( !fileName ) return 0;

	GLuint shader = glCreateShader ( type );

	GXChar* shaderSource = nullptr;
	GXUPointer shaderSourceLength = 0;

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

GLuint GXShaderProgram::CompileShaderProgram ( const GXShaderProgramInfo &info )
{
	GLuint shaderProgram = glCreateProgram ();

	GLuint vertexShader = GetShader ( GL_VERTEX_SHADER, vs );
	GLuint geometryShader = GetShader ( GL_GEOMETRY_SHADER, gs );
	GLuint fragmentShader = GetShader ( GL_FRAGMENT_SHADER, fs );

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

		GLchar* log = (GLchar*)malloc ( size );
		glGetProgramInfoLog ( shaderProgram, size, &size, log );

		GXLogW ( L"GXShaderProgram::CompileShaderProgram::Error - Не могу слинковать шейдерную программу %s + %s + %s\n", vs, gs, fs );
		GXLogA ( "%s\n", log );

		free ( log );

		glDeleteProgram ( shaderProgram );

		return 0;
	}

	return shaderProgram;
}

GXVoid GXShaderProgram::SavePrecompiledShaderProgram ( GLuint shaderProgram, const GXShaderProgramInfo &info, const GXWChar* binaryPath )
{
	GLint value = (GLint)GL_TRUE;
	glProgramParameteri ( shaderProgram, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, value );

	GLint precompiledShaderProgramSize = 0;
	glGetProgramiv ( shaderProgram, GL_PROGRAM_BINARY_LENGTH, &precompiledShaderProgramSize );

	GXUByte* precompiledShaderProgram = (GXUByte*)malloc ( (size_t)precompiledShaderProgramSize );

	GLsizei length;
	GLenum binaryFormat;
	glGetProgramBinary ( shaderProgram, (GLsizei)precompiledShaderProgramSize, &length, &binaryFormat, precompiledShaderProgram );

	GXWriteToFile ( binaryPath, precompiledShaderProgram, (GXUPointer)length );

	precompiledShaderProgramFinder->AddProgram ( info.vs, info.gs, info.fs, binaryPath, binaryFormat );

	free ( precompiledShaderProgram );
}
