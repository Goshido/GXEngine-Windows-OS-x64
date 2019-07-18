#ifndef EM_OBJECT_MASK_MATERIAL
#define EM_OBJECT_MASK_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMObjectMaskMaterial final : public GXMaterial
{
    private:
        GLint   _mod_view_proj_matLocation;

    public:
        EMObjectMaskMaterial ();
        ~EMObjectMaskMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

    private:
        EMObjectMaskMaterial ( const EMObjectMaskMaterial &other ) = delete;
        EMObjectMaskMaterial& operator = ( const EMObjectMaskMaterial &other ) = delete;
};


#endif // EM_OBJECT_MASK_MATERIAL
