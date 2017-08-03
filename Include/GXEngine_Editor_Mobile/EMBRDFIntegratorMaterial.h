#ifndef EM_BRDF_INTEGRATOR_MATERIAL
#define EM_BRDF_INTEGRATOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMBRDFIntegratorMaterial : public GXMaterial
{
	private:
		GXInt		totalSamples;
		GLint		totalSamplesLocation;

		GXFloat		inverseTotalSamples;
		GLint		inverseTotalSamplesLocation;

	public:
		EMBRDFIntegratorMaterial ();
		~EMBRDFIntegratorMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetTotalSamples ( GXUShort samples );
};


#endif //EM_BRDF_INTEGRATOR_MATERIAL
