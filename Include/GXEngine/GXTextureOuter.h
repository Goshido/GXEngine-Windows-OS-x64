//version 1.6

#ifndef GX_TEXTURE_OUTER
#define GX_TEXTURE_OUTER


#include "GXMesh.h"


class GXTextureOuter : public GXMesh
{
	private:
		GLuint				texture;

	public:
		GXTextureOuter ();
		virtual ~GXTextureOuter ();
		virtual GXVoid Draw ();
		GXVoid SetTexture ( GLuint texture );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //GX_TEXTURE_OUTER