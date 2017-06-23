#ifndef EM_GAUSS_HORIZONTAL_BLUR_MATERIAL
#define EM_GAUSS_HORIZONTAL_BLUR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


enum class eEMGaussHorizontalBlurKernelType
{
	ONE_CHANNEL_FIVE_PIXEL_KERNEL
};

class EMGaussHorizontalBlurMaterial : public GXMaterial
{
	private:
		GXTexture*							imageTexture;
		eEMGaussHorizontalBlurKernelType	kernelType;

	public:
		EMGaussHorizontalBlurMaterial ( eEMGaussHorizontalBlurKernelType kernelType );
		~EMGaussHorizontalBlurMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetImageTexture ( GXTexture &texture );
};


#endif //EM_GAUSS_HORIZONTAL_BLUR_MATERIAL
