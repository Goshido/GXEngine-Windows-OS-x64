#ifndef EM_SSAO_APPLY_MATERIAL
#define EM_SSAO_APPLY_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


class EMSSAOApplyMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _ssaoTexture;
        GXTexture2D*    _imageTexture;

        GXSampler       _sampler;

    public:
        EMSSAOApplyMaterial ();
        ~EMSSAOApplyMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetSSAOTexture ( GXTexture2D &texture );
        GXVoid SetImageTexture ( GXTexture2D &texture );

    private:
        EMSSAOApplyMaterial ( const EMSSAOApplyMaterial &other ) = delete;
        EMSSAOApplyMaterial& operator = ( const EMSSAOApplyMaterial &other ) = delete;
};


#endif // EM_SSAO_APPLY_MATERIAL
