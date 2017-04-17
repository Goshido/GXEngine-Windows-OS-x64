#ifndef EM_VELOCITY_MATERIAL
#define EM_VELOCITY_MATERIAL

#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMVelocityMaterial : public GXMaterial
{
	private:
		GXTexture*		velocityViewTexture;

		GXFloat			explosureTime;
		GLint			explosureTimeLocation;

		GXFloat			maxBlurSamples;
		GLint			maxBlurSamplesLocation;

	public:
		EMVelocityMaterial ();
		~EMVelocityMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) const override;
		GXVoid Unbind () const override;

		GXVoid SetVelocityViewTexture ( GXTexture &texture );
		GXVoid SetExplosureTime ( GXFloat time );
		GXVoid SetMaxBlurSamples ( GXUByte samples );
};


#endif //EM_VELOCITY_MATERIAL
