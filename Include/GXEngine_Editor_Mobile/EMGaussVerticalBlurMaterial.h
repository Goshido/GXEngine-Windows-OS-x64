#ifndef EM_GAUSS_VERTICAL_BLUR_MATERIAL
#define EM_GAUSS_VERTICAL_BLUR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>


enum class eEMGaussVerticalBlurKernelType
{
	ONE_CHANNEL_FIVE_PIXEL_KERNEL
};

class EMGaussVerticalBlurMaterial : public GXMaterial
{
	private:
		GXTexture2D*						imageTexture;
		eEMGaussVerticalBlurKernelType		kernelType;

	public:
		EMGaussVerticalBlurMaterial ( eEMGaussVerticalBlurKernelType kernelType );
		~EMGaussVerticalBlurMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetImageTexture ( GXTexture2D &texture );
};


#endif // EM_GAUSS_VERTICAL_BLUR_MATERIAL
