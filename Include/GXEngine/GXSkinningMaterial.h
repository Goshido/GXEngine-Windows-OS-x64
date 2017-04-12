//version 1.0

#ifndef GX_SKINNING_MATERIAL
#define GX_SKINNING_MATERIAL


#include "GXMaterial.h"
#include "GXSkeleton.h"


class GXSkinningMaterial : public GXMaterial
{
	private:
		const GXSkeleton*	skeleton;
		GLint				bonesLocation;

	public:
		GXSkinningMaterial ();
		~GXSkinningMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) const override;
		GXVoid Unbind () const override;

		GXVoid SetSkeleton ( const GXSkeleton &skeleton );
};


#endif //GX_SKINNING_MATERIAL
