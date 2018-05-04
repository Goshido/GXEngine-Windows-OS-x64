// version 1.1

#ifndef GX_TEXTURE_DUPLICATOR
#define GX_TEXTURE_DUPLICATOR


#include "GXOpenGL.h"


class GXTextureDuplicator
{
	private:
		GLuint							fbo;
		static GXTextureDuplicator*		instance;

	public:
		static GXTextureDuplicator* GetInstance ();
		~GXTextureDuplicator ();

		GXVoid Duplicate ( GLuint out, GXUInt source );

	private:
		GXTextureDuplicator ();

		GXTextureDuplicator ( const GXTextureDuplicator &other ) = delete;
		GXTextureDuplicator& operator = ( const GXTextureDuplicator &other ) = delete;
};


#endif // GX_TEXTURE_DUPLICATOR
