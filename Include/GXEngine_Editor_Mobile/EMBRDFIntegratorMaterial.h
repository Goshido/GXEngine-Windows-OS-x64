#ifndef EM_BRDF_INTEGRATOR_MATERIAL
#define EM_BRDF_INTEGRATOR_MATERIAL


#include <GXEngine/GXMaterial.h>


class EMBRDFIntegratorMaterial final : public GXMaterial
{
    private:
        GXInt       _samples;
        GXFloat     _inverseSamples;

        GLint       _samplesLocation;
        GLint       _inverseSamplesLocation;

    public:
        EMBRDFIntegratorMaterial ();
        ~EMBRDFIntegratorMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetSamplesPerPixel ( GXUShort samplesPerPixel );

    private:
        EMBRDFIntegratorMaterial ( const EMBRDFIntegratorMaterial &other ) = delete;
        EMBRDFIntegratorMaterial& operator = ( const EMBRDFIntegratorMaterial &other ) = delete;
};


#endif // EM_BRDF_INTEGRATOR_MATERIAL
