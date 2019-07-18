#ifndef EM_GAUSS_HORIZONTAL_BLUR_MATERIAL
#define EM_GAUSS_HORIZONTAL_BLUR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


enum class eEMGaussHorizontalBlurKernelType : GXUShort
{
    OneChannelFivePixel
};

class EMGaussHorizontalBlurMaterial final : public GXMaterial
{
    private:
        eEMGaussHorizontalBlurKernelType    _kernelType;
        GXTexture2D*                        _imageTexture;
        GXSampler                           _sampler;

    public:
        EMGaussHorizontalBlurMaterial ( eEMGaussHorizontalBlurKernelType kernelType );
        ~EMGaussHorizontalBlurMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetImageTexture ( GXTexture2D &texture );

    private:
        EMGaussHorizontalBlurMaterial () = delete;
        EMGaussHorizontalBlurMaterial ( const EMGaussHorizontalBlurMaterial &other ) = delete;
        EMGaussHorizontalBlurMaterial& operator = ( const EMGaussHorizontalBlurMaterial &other ) = delete;
};


#endif // EM_GAUSS_HORIZONTAL_BLUR_MATERIAL
