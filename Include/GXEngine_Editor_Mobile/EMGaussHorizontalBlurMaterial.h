#ifndef EM_GAUSS_HORIZONTAL_BLUR_MATERIAL
#define EM_GAUSS_HORIZONTAL_BLUR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>


enum class eEMGaussHorizontalBlurKernelType : GXUShort
{
	OneChannelFivePixel
};

class EMGaussHorizontalBlurMaterial : public GXMaterial
{
	private:
		eEMGaussHorizontalBlurKernelType	kernelType;
		GXTexture2D*						imageTexture;

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
