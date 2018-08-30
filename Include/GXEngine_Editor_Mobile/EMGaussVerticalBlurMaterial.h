#ifndef EM_GAUSS_VERTICAL_BLUR_MATERIAL
#define EM_GAUSS_VERTICAL_BLUR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


enum class eEMGaussVerticalBlurKernelType : GXUShort
{
	OneChannelFivePixel
};

class EMGaussVerticalBlurMaterial final : public GXMaterial
{
	private:
		eEMGaussVerticalBlurKernelType		kernelType;
		GXTexture2D*						imageTexture;
		GXSampler							sampler;

	public:
		EMGaussVerticalBlurMaterial ( eEMGaussVerticalBlurKernelType kernelType );
		~EMGaussVerticalBlurMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetImageTexture ( GXTexture2D &texture );

	private:
		EMGaussVerticalBlurMaterial () = delete;
		EMGaussVerticalBlurMaterial ( const EMGaussVerticalBlurMaterial &other ) = delete;
		EMGaussVerticalBlurMaterial& operator = ( const EMGaussVerticalBlurMaterial &other ) = delete;
};


#endif // EM_GAUSS_VERTICAL_BLUR_MATERIAL
