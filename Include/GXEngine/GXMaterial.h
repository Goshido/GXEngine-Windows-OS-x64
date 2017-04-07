//version 1.0

#ifndef GX_MATERIAL
#define GX_MATERIAL


#include "GXShaderProgram.h"
#include "GXTransform.h"


class GXMaterial
{
	protected:
		GXShaderProgram		shaderProgram;

	public:
		GXMaterial ();
		virtual ~GXMaterial ();

		virtual GXVoid Bind ( const GXTransform &transform ) const = 0;
		virtual GXVoid Unbind () const = 0;
};


#endif //GX_MATERIAL
