//version 1.0

#ifndef GX_TEXTURE_DUPLICATOR
#define GX_TEXTURE_DUPLICATOR


#include "GXOpenGL.h"


class GXTextureDuplicator
{
private:
	GLuint	fbo;
	static GXTextureDuplicator*		instance;

public:
	~GXTextureDuplicator ();
	GXVoid Duplicate ( GLuint out, GXUInt source );

	static GXTextureDuplicator* GetInstance ();

private:
	GXTextureDuplicator ();
};


#endif GX_TEXTURE_DUPLICATOR
