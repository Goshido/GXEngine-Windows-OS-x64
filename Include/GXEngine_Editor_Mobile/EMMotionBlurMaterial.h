#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMMotionBlurMaterial : public GXMaterial
{
	private:
		GXTexture*			velocityNeighborMaxTexture;
		GXTexture*			velocityTexture;
		GXTexture*			depthTexture;
		GXTexture*			imageTexture;

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

		GXVoid SetVelocityNeighborMaxTexture ( GXTexture &texture );
		GXVoid SetVelocityTexture ( GXTexture &texture );
		GXVoid SetDepthTexture ( GXTexture &texture );
		GXVoid SetImageTexture ( GXTexture &texture );

		GXVoid SetDepthLimit ( GXFloat meters );
		GXFloat GetDepthLimit () const;

		GXVoid SetMaxBlurSamples ( GXUByte samples );
		GXUByte GetMaxBlurSamples () const;

		GXVoid SetScreenResolution ( GXUShort width, GXUShort height );
};
