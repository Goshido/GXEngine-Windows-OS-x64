// version 1.0

#include <GXEngine/GXTextureDuplicator.h>


GXTextureDuplicator* GXTextureDuplicator::instance = nullptr;

GXTextureDuplicator::~GXTextureDuplicator ()
{
	if ( fbo != 0 )
		glDeleteFramebuffers ( 1, &fbo );
}

GXVoid GXTextureDuplicator::Duplicate ( GLuint out, GXUInt /*source*/ )
{
	if ( fbo == 0 )
		glGenFramebuffers ( 1, &fbo );

	glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, out, 0 );
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );
}

GXTextureDuplicator* GXTextureDuplicator::GetInstance ()
{
	if ( !instance )
		instance = new GXTextureDuplicator ();

	return instance;
}

GXTextureDuplicator::GXTextureDuplicator ()
{
	fbo = 0;
}
