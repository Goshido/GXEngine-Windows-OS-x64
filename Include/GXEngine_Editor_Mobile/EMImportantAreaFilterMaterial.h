#ifndef EM_IMPORTANT_AREA_FILTER_MATERIAL
#define EM_IMPORTANT_AREA_FILTER_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


class EMImportantAreaFilterMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _imageTexture;
        GXSampler       _sampler;

        GXTexture2D     _retinaFilterTexture;

    public:
        EMImportantAreaFilterMaterial ();
        ~EMImportantAreaFilterMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetImageTexture ( GXTexture2D &texture );

    private:
        GXVoid GenerateRetinaFilterTexture ( GXUShort effectiveLength );

        EMImportantAreaFilterMaterial ( const EMImportantAreaFilterMaterial &other ) = delete;
        EMImportantAreaFilterMaterial& operator = ( const EMImportantAreaFilterMaterial &other ) = delete;
};


#endif // EM_IMPORTANT_AREA_FILTER_MATERIAL
