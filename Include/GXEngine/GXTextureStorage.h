//version 1.8

#ifndef GX_TEXTURE_STORAGE
#define GX_TEXTURE_STORAGE


#include "GXOpenGL.h"


struct GXTexture
{
	GXUInt		width;
	GXUInt		height;
	GXUByte		channels;
	GLuint		texObj;
};


GXVoid GXCALL GXLoadTexture ( const GXWChar* fileName, GXTexture &texture );
GXVoid GXCALL GXAddRefTexture ( const GXTexture &texture );
GXVoid GXCALL GXRemoveTexture ( const GXTexture &texture );

GXUInt GXCALL GXGetTotalTextureStorageObjects ( const GXWChar** lastTexture );


#endif // GX_TEXTURE_STORAGE
