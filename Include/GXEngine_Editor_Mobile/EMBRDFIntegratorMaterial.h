#ifndef EM_BRDF_INTEGRATOR_MATERIAL
#define EM_BRDF_INTEGRATOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMBRDFIntegratorMaterial : public GXMaterial
{
	private:
		GXInt		samples;
		GLint		samplesLocation;

		GXFloat		inverseSamples;
		GLint		inverseSamplesLocation;

	public:
		EMBRDFIntegratorMaterial ();
		~EMBRDFIntegratorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetSamplesPerPixel ( GXUShort samplesPerPixel );
};


#endif //EM_BRDF_INTEGRATOR_MATERIAL
