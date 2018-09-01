// version 1.2

#include <GXEngine/GXSampler.h>
#include <GXCommon/GXUIntAtomic.h>


#define INVALID_TEXTURE_UNIT				0xFFu

//---------------------------------------------------------------------------------------------------------------------

class GXSamplerEntry final
{
	private:
		GXSamplerEntry*			previous;
		GXSamplerEntry*			next;
		GXUIntAtomic			references;

		GLint					wrapMode;
		eGXResampling			resampling;
		GXFloat					anisotropy;

		GLuint					samplerObject;

		static GXSamplerEntry*	top;

	public:
		explicit GXSamplerEntry ( GLint wrapMode, eGXResampling resampling, GXFloat anisotrophy );

		GLuint GetSamplerObject () const;

		GXVoid AddReference ();
		GXVoid Release ();

		// Method returns valid pointer if sampler resource was found.
		// Method returns nullptr if sampler resource was not found.
		static GXSamplerEntry* GXCALL Find ( GLint matchWrapMode, eGXResampling matchResampling, GXFloat matchAnisotrophy );

		static GXUInt GXCALL GetTotalSamplers ( GLint &lastWrapMode, eGXResampling &lastResampling, GXFloat &lastAnisotropy );

	private:
		~GXSamplerEntry ();

		GXSamplerEntry () = delete;
		GXSamplerEntry ( const GXSamplerEntry &other ) = delete;
		GXSamplerEntry& operator = ( const GXSamplerEntry &other ) = delete;
};

GXSamplerEntry* GXSamplerEntry::top = nullptr;

GXSamplerEntry::GXSamplerEntry ( GLint wrapMode, eGXResampling resampling, GXFloat anisotropy ):
	previous ( nullptr ),
	next ( top ),
	references ( 1 ),
	wrapMode ( wrapMode ),
	resampling ( resampling ),
	anisotropy ( anisotropy )
{
	if ( top )
		top->previous = this;

	top = this;

	glGenSamplers ( 1, &samplerObject );

	switch ( resampling )
	{
		case eGXResampling::Linear:
			glSamplerParameteri ( samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glSamplerParameteri ( samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		break;

		case eGXResampling::Bilinear:
			glSamplerParameteri ( samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glSamplerParameteri ( samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		break;

		case eGXResampling::Trilinear:
			glSamplerParameteri ( samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glSamplerParameteri ( samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		break;

		case eGXResampling::None:
		default:
			glSamplerParameteri ( samplerObject, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glSamplerParameteri ( samplerObject, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		break;
	}

	glSamplerParameteri ( samplerObject, GL_TEXTURE_WRAP_S, wrapMode );
	glSamplerParameteri ( samplerObject, GL_TEXTURE_WRAP_T, wrapMode );
	glSamplerParameteri ( samplerObject, GL_TEXTURE_WRAP_R, wrapMode );

	GLfloat maxAnisotropy = 1.0f;
	glGetFloatv ( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
	GXFloat supportedAnisotropy = ( anisotropy > maxAnisotropy ) ? maxAnisotropy : anisotropy;
	glSamplerParameterf ( samplerObject, GL_TEXTURE_MAX_ANISOTROPY_EXT, supportedAnisotropy );
}

GLuint GXSamplerEntry::GetSamplerObject () const
{
	return samplerObject;
}

GXVoid GXSamplerEntry::AddReference ()
{
	++references;
}

GXVoid GXSamplerEntry::Release ()
{
	--references;

	if ( references > 0u ) return;

	delete this;
}

GXSamplerEntry* GXCALL GXSamplerEntry::Find ( GLint matchWrapMode, eGXResampling matchResampling, GXFloat matchAnisotrophy )
{
	for ( GXSamplerEntry* p = top; p; p = p->next )
	{
		if ( p->wrapMode != matchWrapMode || p->resampling != matchResampling || p->anisotropy != matchAnisotrophy ) continue;

		return p;
	}

	return nullptr;
}

GXUInt GXCALL GXSamplerEntry::GetTotalSamplers ( GLint &lastWrapMode, eGXResampling &lastResampling, GXFloat &lastAnisotropy )
{
	GXUInt total = 0u;

	for ( GXSamplerEntry* p = top; p; p = p->next )
		++total;

	if ( total == 0u ) return 0u;

	lastWrapMode = top->wrapMode;
	lastResampling = top->resampling;
	lastAnisotropy = top->anisotropy;

	return total;
}

GXSamplerEntry::~GXSamplerEntry ()
{
	if ( top == this )
		top = top->next;

	if ( previous )
		previous->next = next;

	if ( next )
		next->previous = previous;

	glDeleteSamplers ( 1, &samplerObject );
}

//---------------------------------------------------------------------------------------------------------------------

GXSampler::GXSampler ( GLint wrapMode, eGXResampling resampling, GXFloat anisotropy )
{
	samplerEntry = GXSamplerEntry::Find ( wrapMode, resampling, anisotropy );

	if ( samplerEntry )
		samplerEntry->AddReference ();
	else
		samplerEntry = new GXSamplerEntry ( wrapMode, resampling, anisotropy );
}

GXSampler::~GXSampler ()
{
	samplerEntry->Release ();
}

GXVoid GXSampler::Bind ( GXUByte unit )
{
	glBindSampler ( unit, samplerEntry->GetSamplerObject () );
}

GXVoid GXSampler::Unbind ( GXUByte unit )
{
	glBindSampler ( unit, 0u );
}

GXUInt GXCALL GXSampler::GetTotalSamplers ( GLint &lastWrapMode, eGXResampling &lastResampling, GXFloat &lastAnisotropy )
{
	return GXSamplerEntry::GetTotalSamplers ( lastWrapMode, lastResampling, lastAnisotropy );
}
