#ifndef EM_SSAO_SHARP_MATERIAL
#define EM_SSAO_SHARP_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


#define EM_MAX_SSAO_SAMPLES        64u


class EMSSAOSharpMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _depthTexture;
        GXTexture2D*    _normalTexture;

        GXSampler       _sampler;

        GXFloat         _maxDistance;
        GXFloat         _checkRadius;

        GXTexture2D     _noiseTexture;

        GXInt           _samples;
        GXFloat         _inverseSamples;

        GXVec2          _noiseScale;
        GXVec3          _kernel[ EM_MAX_SSAO_SAMPLES ];

        GLint           _checkRadiusLocation;
        GLint           _kernelLocation;
        GLint           _samplesLocation;
        GLint           _inverseSamplesLocation;
        GLint           _noiseScaleLocation;
        GLint           _maxDistanceLocation;
        GLint           _projectionMatrixLocation;
        GLint           _inverseProjectionMatrixLocation;

    public:
        EMSSAOSharpMaterial ();
        ~EMSSAOSharpMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetDepthTexture ( GXTexture2D &texture );
        GXVoid SetNormalTexture ( GXTexture2D &texture );

        GXVoid SetCheckRadius ( GXFloat meters );
        GXFloat GetCheckRadius () const;

        GXVoid SetSampleNumber ( GXUByte samplesPerPixel );
        GXUByte GetSampleNumber () const;

        GXVoid SetNoiseTextureResolution ( GXUShort resolution );
        GXUShort GetNoiseTextureResolution () const;

        GXVoid SetMaximumDistance ( GXFloat meters );
        GXFloat GetMaximumDistance () const;

    private:
        EMSSAOSharpMaterial ( const EMSSAOSharpMaterial &other ) = delete;
        EMSSAOSharpMaterial& operator = ( const EMSSAOSharpMaterial &other ) = delete;
};


#endif // EM_SSAO_SHARP_MATERIAL
