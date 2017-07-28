#ifndef EM_VELOCITY_NEIGHBOR_MAX_MATERIAL
#define EM_VELOCITY_NEIGHBOR_MAX_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>


class EMVelocityNeighborMaxMaterial : public GXMaterial
{
	private:
		GXTexture2D*	velocityTileMaxTexture;

		GXVec2			inverseVelocityTileMaxTextureResolution;
		GLint			inverseVelocityTileMaxTextureResolutionLocation;

	public:
		EMVelocityNeighborMaxMaterial ();
		~EMVelocityNeighborMaxMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetVelocityTileMaxTexture ( GXTexture2D &texture );
		GXVoid SetVelocityTileMaxTextureResolution ( GXUShort width, GXUShort height );
};


#endif //EM_VELOCITY_NEIGHBOR_MAX_MATERIAL
