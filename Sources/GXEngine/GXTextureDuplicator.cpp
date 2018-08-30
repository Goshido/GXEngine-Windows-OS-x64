// version 1.2

#include <GXEngine/GXTextureDuplicator.h>


GXTextureDuplicator* GXTextureDuplicator::instance = nullptr;

GXTextureDuplicator* GXTextureDuplicator::GetInstance ()
{
	if ( !instance )
		instance = new GXTextureDuplicator ();

	return instance;
}

GXTextureDuplicator::~GXTextureDuplicator ()
{
	if ( fbo == 0u ) return;

	glDeleteFramebuffers ( 1, &fbo );
}

GXVoid GXTextureDuplicator::Duplicate ( GLuint out, GXUInt /*source*/ )
{
	if ( fbo == 0u )
		glGenFramebuffers ( 1, &fbo );

	glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, out, 0 );
	const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );
}

GXTextureDuplicator::GXTextureDuplicator ():
	fbo ( 0u )
{
	// NOTHING
}
