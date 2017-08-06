#ifndef EM_TONE_MAPPER_MATERIAL
#define EM_TONE_MAPPER_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>


class EMToneMapperMaterial : public GXMaterial
{
	private:
		GXTexture2D*		linearSpaceTexture;

		GXFloat				gamma;
		GXFloat				inverseGamma;
		GLint				inverseGammaLocation;

	public:
		EMToneMapperMaterial ();
		~EMToneMapperMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetLinearSpaceTexture ( GXTexture2D &texture );
		GXVoid SetGamma ( GXFloat gamma );
		GXFloat GetGamma () const;
};


#endif //EM_TONE_MAPPER_MATERIAL
