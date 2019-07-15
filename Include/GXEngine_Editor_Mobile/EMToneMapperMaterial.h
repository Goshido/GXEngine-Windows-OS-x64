#ifndef EM_TONE_MAPPER_MATERIAL
#define EM_TONE_MAPPER_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


class EMToneMapperMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _linearSpaceTexture;
        GXSampler       _sampler;

        GXFloat         _gamma;
        GXFloat         _inverseGamma;
        GXFloat         _eyeSensitivity;

        GXFloat         _absoluteWhiteIntensity;
        GXFloat         _inverseAbsoluteWhiteSquareIntensity;
        GXFloat         _prescaleFactor;

        GLint           _inverseGammaLocation;
        GLint           _prescaleFactorLocation;
        GLint           _inverseAbsoluteWhiteSquareIntensityLocation;

    public:
        EMToneMapperMaterial ();
        ~EMToneMapperMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetLinearSpaceTexture ( GXTexture2D &texture );
        GXVoid SetLuminanceTriplet ( GXFloat averageLuminance, GXFloat minimumLuminance, GXFloat maximumLuminance );

        GXVoid SetGamma ( GXFloat newGamma );
        GXFloat GetGamma () const;

        // Artistic purpose parameter. There is no physical explanation.
        GXVoid SetEyeSensitivity ( GXFloat sensitivity );
        GXFloat GetEyeSensitivity () const;

        GXVoid SetAbsoluteWhiteIntensity ( GXFloat intensity );
        GXFloat GetAbsoluteWhiteIntensity () const;

    private:
        EMToneMapperMaterial ( const EMToneMapperMaterial &other ) = delete;
        EMToneMapperMaterial& operator = ( const EMToneMapperMaterial &other ) = delete;
};


#endif // EM_TONE_MAPPER_MATERIAL
