#ifndef EM_MOTION_BLUR_MATERIAL
#define EM_MOTION_BLUR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>


class EMMotionBlurMaterial : public GXMaterial
{
	private:
		GXTexture2D*			velocityNeighborMaxTexture;
		GXTexture2D*			velocityTexture;
		GXTexture2D*			depthTexture;
		GXTexture2D*			imageTexture;

		GXFloat				depthLimit;
		GXFloat				inverseDepthLimit;
		GLint				inverseDepthLimitLocation;

		GXFloat				maxBlurSamples;
		GLint				maxBlurSamplesLocation;

		GXVec2				inverseScreenResolution;
		GLint				inverseScreenResolutionLocation;

		GLint				inverseProjectionMatrixLocation;

	public:
		EMMotionBlurMaterial ();
		~EMMotionBlurMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetVelocityNeighborMaxTexture ( GXTexture2D &texture );
		GXVoid SetVelocityTexture ( GXTexture2D &texture );
		GXVoid SetDepthTexture ( GXTexture2D &texture );
		GXVoid SetImageTexture ( GXTexture2D &texture );

		GXVoid SetDepthLimit ( GXFloat meters );
		GXFloat GetDepthLimit () const;

		GXVoid SetMaxBlurSamples ( GXUByte samples );
		GXUByte GetMaxBlurSamples () const;

		GXVoid SetScreenResolution ( GXUShort width, GXUShort height );
};


#endif // EM_MOTION_BLUR_MATERIAL
