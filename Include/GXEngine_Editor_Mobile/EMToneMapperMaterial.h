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

		GXFloat				prescaleFactor;
		GLint				prescaleFactorLocation;

		GXFloat				absoluteWhiteIntensity;
		GXFloat				inverseAbsoluteWhiteSquareIntensity;
		GLint				inverseAbsoluteWhiteSquareIntensityLocation;

		GXFloat				eyeSensitivity;

	public:
		EMToneMapperMaterial ();
		~EMToneMapperMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetLinearSpaceTexture ( GXTexture2D &texture );
		GXVoid SetLuminanceTriplet ( GXFloat averageLuminance, GXFloat minimumLuminance, GXFloat maximumLuminance );

		GXVoid SetGamma ( GXFloat gamma );
		GXFloat GetGamma () const;

		// Artistic purpose parameter. There is no physical explanation.
		GXVoid SetEyeSensitivity ( GXFloat sensitivity );
		GXFloat GetEyeSensitivity () const;

		GXVoid SetAbsoluteWhiteIntensity ( GXFloat intensity );
		GXFloat GetAbsoluteWhiteIntensity () const;
};


#endif //EM_TONE_MAPPER_MATERIAL
