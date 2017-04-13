#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMMotionBlurMaterial : public GXMaterial
{
	private:
		GXTexture*			velocityTexture;
		GXTexture*			depthTexture;
		GXTexture*			imageTexture;

		GXFloat				blurStrength;
		GLint				blurStrengthLocation;

		GXFloat				blurMinimumVelocity;
		GLint				blurMinimumvelocityLocation;

		GXVec2				inverseScreenResolution;
		GLint				inverseScreenResolutionLocation;

		GLint				inverseProjectionMatrixLocation;

	public:
		EMMotionBlurMaterial ();
		~EMMotionBlurMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) const override;
		GXVoid Unbind () const override;

		GXVoid SetVelocityTexture ( GXTexture &texture );
		GXVoid SetDepthTexture ( GXTexture &texture );
		GXVoid SetImageTexture ( GXTexture &texture );

		GXVoid SetBlurStrength ( GXFloat strength );
		GXVoid SetBlurMinimumVelocity ( GXFloat velocity );
		GXVoid SetScreenResolution ( GXUShort width, GXUShort height );
};
