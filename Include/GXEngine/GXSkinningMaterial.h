// version 1.4

#ifndef GX_SKINNING_MATERIAL
#define GX_SKINNING_MATERIAL


#include "GXMaterial.h"
#include "GXSkeleton.h"


class GXSkinningMaterial final : public GXMemoryInspector, public GXMaterial
{
    private:
        const GXSkeleton*       skeletonObject;
        GLint                   bonesLocation;

    public:
        GXSkinningMaterial ();
        ~GXSkinningMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetSkeleton ( const GXSkeleton &skeleton );

    private:
        GXSkinningMaterial ( const GXSkinningMaterial &other ) = delete;
        GXSkinningMaterial& operator = ( const GXSkinningMaterial &other ) = delete;
};


#endif // GX_SKINNING_MATERIAL
