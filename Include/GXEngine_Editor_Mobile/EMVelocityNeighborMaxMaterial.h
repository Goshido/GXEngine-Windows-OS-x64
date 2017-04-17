#ifndef EM_VELOCITY_NEIGHBOR_MAX_MATERIAL
#define EM_VELOCITY_NEIGHBOR_MAX_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMVelocityNeighborMaxMaterial : public GXMaterial
{
	private:
		GXTexture*		velocityTileMaxTexture;

		GXVec2			inverseVelocityTileMaxTextureResolution;
		GLint			inverseVelocityTileMaxTextureResolutionLocation;

	public:
		EMVelocityNeighborMaxMaterial ();
		~EMVelocityNeighborMaxMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) const override;
		GXVoid Unbind () const override;

		GXVoid SetVelocityTileMaxTexture ( GXTexture &texture );
		GXVoid SetVelocityTileMaxTextureResolution ( GXUShort width, GXUShort height );
};


#endif //EM_VELOCITY_NEIGHBOR_MAX_MATERIAL
