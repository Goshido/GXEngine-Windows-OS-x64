#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMMotionBlurMaterial : public GXMaterial
{
	private:
		GXTexture*			velocityNeighborMaxTexture;
		GXTexture*			velocityTexture;
		GXTexture*			depthTexture;
		GXTexture*			imageTexture;

		GXFloat				inverseDepthLimit;
		GLint				inverseDepthLimitLocation;

		GXInt				maxBlurSamples;
		GLint				maxBlurSamplesLocation;

		GXVec2				inverseScreenResolution;
		GLint				inverseScreenResolutionLocation;

		GLint				inverseProjectionMatrixLocation;

	public:
		EMMotionBlurMaterial ();
		~EMMotionBlurMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) const override;
		GXVoid Unbind () const override;

		GXVoid SetVelocityNeighborMaxTexture ( GXTexture &texture );
		GXVoid SetVelocityTexture ( GXTexture &texture );
		GXVoid SetDepthTexture ( GXTexture &texture );
		GXVoid SetImageTexture ( GXTexture &texture );

		GXVoid SetDepthLimit ( GXFloat limit );
		GXVoid SetMaxBlurSamples ( GXUByte maxSamples );
		GXVoid SetScreenResolution ( GXUShort width, GXUShort height );
};
