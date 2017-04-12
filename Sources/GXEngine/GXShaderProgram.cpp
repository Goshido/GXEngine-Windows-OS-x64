//version 1.0

#include <GXEngine/GXShaderProgram.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


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
		GXLogW ( L"GXShaderProgram::GetUniform::Warning - �� ���� ����� �������� %s � ��������� %s + %s + %s\n", nameW, vs, gs, fs );
		free ( nameW );
	}

	return uniform;
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

		GXLogW ( L"GXShaderProgramStatus::Error - �� ���� ���������� ������ %s + %s + %s\n", vs, gs, fs );
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
	GXUInt shaderSourceLength = 0;

	static const GXWChar* vertexShaderLoadError = L"GXShaderProgram::GetShader::Error - �� ���� ��������� ��������� ������ %s\n";
	static const GXWChar* geometryShaderLoadError = L"GXShaderProgram::GetShader::Error - �� ���� ��������� �������������� ������ %s\n";
	static const GXWChar* fragmentShaderLoadError = L"GXShaderProgram::GetShader::Error - �� ���� ��������� ����������� ������ %s\n";

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

		GXLogW ( L"GXShaderProgram::GetShader::Error - ������ %s\n", fileName );
		GXLogA ( "%s\n", log );

		free ( log );

		glDeleteShader ( shader );
		return 0;
	}

	return shader;
}
