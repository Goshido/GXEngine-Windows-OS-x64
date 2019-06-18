// version 1.3

#include <GXEngine/GXTextureDuplicator.h>


GXTextureDuplicator* GXTextureDuplicator::_instance = nullptr;

GXTextureDuplicator* GXTextureDuplicator::GetInstance ()
{
    if ( !_instance )
        _instance = new GXTextureDuplicator ();

    return _instance;
}

GXTextureDuplicator::~GXTextureDuplicator ()
{
    if ( _fbo == 0u ) return;

    glDeleteFramebuffers ( 1, &_fbo );
}

GXVoid GXTextureDuplicator::Duplicate ( GLuint out, GXUInt /*source*/ )
{
    if ( _fbo == 0u )
        glGenFramebuffers ( 1, &_fbo );

    glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, out, 0 );
    const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );
}

GXTextureDuplicator::GXTextureDuplicator ():
    _fbo ( 0u )
{
    // NOTHING
}
