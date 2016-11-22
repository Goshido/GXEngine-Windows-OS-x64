//version 1.1

#ifndef GX_ARRAY_TEXTURE_OUTER
#define GX_ARRAY_TEXTURE_OUTER


#include "GXOpenGL.h"
#include "GXVAOStorage.h"
#include <GXCommon/GXMTRStructs.h>


class GXArrayTextureOuter
{
	private:
		GXVAOInfo		vaoInfo;
		GXMaterialInfo	matInfo;

	public:
		GXArrayTextureOuter ();
		~GXArrayTextureOuter ();

		GXVoid Draw ( GLuint arrayTexture );
};


#endif //GX_ARRAY_TEXTURE_OUTER