#ifndef EM_GAUSS_VERTICAL_BLUR_MATERIAL
#define EM_GAUSS_VERTICAL_BLUR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


enum class eEMGaussVerticalBlurKernelType
{
	ONE_CHANNEL_FIVE_PIXEL_KERNEL
};

class EMGaussVerticalBlurMaterial : public GXMaterial
{
	private:
		GXTexture*							imageTexture;
		eEMGaussVerticalBlurKernelType		kernelType;

	public:
		EMGaussVerticalBlurMaterial ( eEMGaussVerticalBlurKernelType kernelType );
		~EMGaussVerticalBlurMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetImageTexture ( GXTexture &texture );
};


#endif //EM_GAUSS_VERTICAL_BLUR_MATERIAL
