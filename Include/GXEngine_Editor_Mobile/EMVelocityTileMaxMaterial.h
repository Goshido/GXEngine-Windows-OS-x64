#ifndef EM_VELOCITY_TILE_MAX_MATERIAL
#define EM_VELOCITY_TILE_MAX_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMVelocityTileMaxMaterial : public GXMaterial
{
	private:
		GXTexture*		velocityBlurTexture;

		GXInt			maxBlurSamples;
		GLint			maxBlurSamplesLocation;

		GXVec2			inverseScreenResolution;
		GLint			inverseScreenResolutionLocation;

	public:
		EMVelocityTileMaxMaterial ();
		~EMVelocityTileMaxMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) const override;
		GXVoid Unbind () const override;

		GXVoid SetVelocityBlurTexture ( GXTexture &texture );
		GXVoid SetMaxBlurSamples ( GXUByte maxSamples );
		GXVoid SetScreenResolution ( GXUShort width, GXUShort height );
};


#endif //EM_VELOCITY_TILE_MAX_MATERIAL
