#ifndef EM_OBJECT_MASK_MATERIAL
#define EM_OBJECT_MASK_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMObjectMaskMaterial : public GXMaterial
{
	private:
		GLint	mod_view_proj_matLocation;

	public:
		EMObjectMaskMaterial ();
		~EMObjectMaskMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;
};


#endif // EM_OBJECT_MASK_MATERIAL
