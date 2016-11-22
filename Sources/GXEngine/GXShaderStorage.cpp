//version 1.9

#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXShaderUtils.h>
#include <GXCommon/GXStrings.h>


#define	GX_ID_NOT_ASSIGNED	0xFFFFFFFF


GXShaderProgram::GXShaderProgram ()
{
	program = GX_ID_NOT_ASSIGNED;
	isSamplersTuned = GX_FALSE;
}

//------------------------------------------------------------------------------------------------

class GXShaderUnit
{
	private:
		GXShaderUnit* 	next;
		GXShaderUnit* 	prev;

		GXInt			refs;

		GXWChar*		vs;
		GXWChar*		gs;
		GXWChar*		fs;

		GLuint			program;
		GXBool			isSamplersTuned;

	public:
		GXShaderUnit ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs );
		GXShaderUnit ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs, GXUByte numVaryings, const GLchar** varyings, GLenum bufferMode );

		GXVoid AddRef ();
		GXVoid Release ();

		GXShaderUnit* Find ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs );
		GXShaderUnit* Find ( GLuint program );

		GXBool IsSamplersTuned ();
		GXVoid SetSamplersTuned ();

		GLuint GetProgram ();
		GXUInt CheckMemoryLeak ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS );

	private:
		~GXShaderUnit ();
};

GXShaderUnit* gx_strg_Shaders = 0;

GXShaderUnit::GXShaderUnit ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs )
{
	if ( !gx_strg_Shaders )
		next = prev = 0;
	else
	{
		next = gx_strg_Shaders;
		prev = 0;
		gx_strg_Shaders->prev = this;
	}

	gx_strg_Shaders = this;

	refs = 1;

	GXWcsclone ( &this->vs, vs );

	if ( gs )
		GXWcsclone ( &this->gs, gs );
	else
		this->gs = 0;

	GXWcsclone ( &this->fs, fs );

	program = GXCreateShaderProgram ( vs, gs, fs );
	isSamplersTuned = GX_FALSE;
}

GXShaderUnit::GXShaderUnit ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs, GXUByte numVaryings, const GLchar** varyings, GLenum bufferMode )
{
	if ( !gx_strg_Shaders )
		next = prev = 0;
	else
	{
		next = gx_strg_Shaders;
		prev = 0;
		gx_strg_Shaders->prev = this;
	}

	gx_strg_Shaders = this;

	refs = 1;

	GXWcsclone ( &this->vs, vs );

	if ( gs )
		GXWcsclone ( &this->gs, gs );
	else
		this->gs = 0;

	GXWcsclone ( &this->fs, fs );

	program = GXCreateTransformFeedbackShaderProgram ( vs, gs, fs, numVaryings, varyings, bufferMode );
	isSamplersTuned = GX_FALSE;
}

GXVoid GXShaderUnit::AddRef ()
{
	refs++;
}

GXVoid GXShaderUnit::Release ()
{
	refs--;

	if ( refs < 1 )
		delete this;
}

GXShaderUnit* GXShaderUnit::Find ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs )
{
	GXShaderUnit* p = this;

	while ( p )
	{
		if ( ( GXWcscmp ( p->vs, vs ) == 0 ) && ( GXWcscmp ( p->fs, fs ) == 0 ) )
		{
			if ( !gs || GXWcscmp ( p->gs, gs ) == 0 )
				return p;
		}

		p = p->next;
	}

	return 0;
}

GXShaderUnit* GXShaderUnit::Find ( GLuint program )
{
	GXShaderUnit* p = this;

	while ( p )
	{
		if ( p->program == program )
			return p;

		p = p->next;
	}

	return 0;
}

GXBool GXShaderUnit::IsSamplersTuned ()
{
	return isSamplersTuned;
}

GXVoid GXShaderUnit::SetSamplersTuned ()
{
	isSamplersTuned = GX_TRUE;
}

GLuint GXShaderUnit::GetProgram ()
{
	return program;
}

GXUInt GXShaderUnit::CheckMemoryLeak ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS )
{
	GXUInt objects = 0;
	*lastVS = *lastFS = 0;

	for ( GXShaderUnit* p = gx_strg_Shaders; p; p = p->next )
	{
		*lastVS = p->vs;
		*lastGS = p->gs;
		*lastFS = p->fs;
		objects++;
	}

	return objects;
}

GXShaderUnit::~GXShaderUnit ()
{
	if ( next ) next->prev = prev;

	if ( prev )
		prev->next = next;
	else
		gx_strg_Shaders = next;

	free ( vs );
	free ( fs );

	glUseProgram ( 0 );
	glDeleteProgram ( program );
}

//------------------------------------------------------------------------------------------------

GXVoid GXCALL GXGetShaderProgram ( GXShaderProgram &program, const GXWChar* vs, const GXWChar* gs, const GXWChar* fs )
{
	GXShaderUnit* shader;

	if ( !gx_strg_Shaders )
		shader = new GXShaderUnit ( vs, gs, fs );
	else
	{
		shader = gx_strg_Shaders->Find ( vs, gs, fs );

		if ( shader )
			shader->AddRef ();
		else
			shader = new GXShaderUnit ( vs, gs, fs );
	}

	program.isSamplersTuned = shader->IsSamplersTuned ();
	program.program = shader->GetProgram ();
}

GXVoid GXCALL GXGetTransformFeedbackShaderProgram ( GXShaderProgram &program, const GXWChar* vs, const GXWChar* gs, const GXWChar* fs, GXUByte numVaryings, const GLchar** varyings, GLenum bufferMode )
{
	GXShaderUnit* shader;

	if ( !gx_strg_Shaders )
		shader = new GXShaderUnit ( vs, gs, fs, numVaryings, varyings, bufferMode );
	else
	{
		shader = gx_strg_Shaders->Find ( vs, gs, fs );

		if ( shader )
			shader->AddRef ();
		else
			shader = new GXShaderUnit ( vs, gs, fs, numVaryings, varyings, bufferMode );
	}

	program.isSamplersTuned = shader->IsSamplersTuned ();
	program.program = shader->GetProgram ();
}

GXVoid GXCALL GXRemoveShaderProgram ( const GXShaderProgram &program )
{
	if ( !gx_strg_Shaders ) return;

	GXShaderUnit* shader = gx_strg_Shaders->Find ( program.program );

	if ( !shader ) return;

	shader->Release ();
}

GXVoid GXCALL GXTuneShaderSamplers ( GXShaderProgram &program, const GLuint* samplerIndexes, const GLchar** samplerNames, GXUByte numSamplers )
{
	if ( !gx_strg_Shaders ) return;

	GXShaderUnit* shader = gx_strg_Shaders->Find ( program.program );

	if ( !shader ) return;

	glUseProgram ( program.program );

	for ( GXUByte i = 0; i < numSamplers; i++ )
		glUniform1i ( GXGetUniformLocation ( program.program, samplerNames[ i ] ), samplerIndexes[ i ] );

	glUseProgram ( 0 );

	shader->SetSamplersTuned ();
	program.isSamplersTuned = GX_TRUE;
}

GXUInt GXCALL GXGetTotalShaderStorageObjects ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS )
{
	if ( !gx_strg_Shaders )
	{
		*lastVS = *lastGS = *lastFS = 0;
		return 0;
	}

	return gx_strg_Shaders->CheckMemoryLeak ( lastVS, lastGS, lastFS );
}
