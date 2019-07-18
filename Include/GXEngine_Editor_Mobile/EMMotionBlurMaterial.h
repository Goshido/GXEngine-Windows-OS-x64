#ifndef EM_MOTION_BLUR_MATERIAL
#define EM_MOTION_BLUR_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXSampler.h>
#include <GXEngine/GXTexture2D.h>


class EMMotionBlurMaterial final : public GXMaterial
{
    private:
        GXTexture2D*    _velocityNeighborMaxTexture;
        GXTexture2D*    _velocityTexture;
        GXTexture2D*    _depthTexture;
        GXTexture2D*    _imageTexture;

        GXSampler       _sampler;

        GXFloat         _depthLimit;
        GXFloat         _inverseDepthLimit;
        GXFloat         _maxBlurSamples;
        GXVec2          _inverseScreenResolution;

        GLint           _inverseDepthLimitLocation;
        GLint           _maxBlurSamplesLocation;
        GLint           _inverseScreenResolutionLocation;
        GLint           _inverseProjectionMatrixLocation;

    public:
        EMMotionBlurMaterial ();
        ~EMMotionBlurMaterial () override;

        GXVoid Bind ( const GXTransform &transform ) override;
        GXVoid Unbind () override;

        GXVoid SetVelocityNeighborMaxTexture ( GXTexture2D &texture );
        GXVoid SetVelocityTexture ( GXTexture2D &texture );
        GXVoid SetDepthTexture ( GXTexture2D &texture );
        GXVoid SetImageTexture ( GXTexture2D &texture );

        GXVoid SetDepthLimit ( GXFloat meters );
        GXFloat GetDepthLimit () const;

        GXVoid SetMaxBlurSamples ( GXUByte samples );
        GXUByte GetMaxBlurSamples () const;

        GXVoid SetScreenResolution ( GXUShort width, GXUShort height );

    private:
        EMMotionBlurMaterial ( const EMMotionBlurMaterial &other ) = delete;
        EMMotionBlurMaterial& operator = ( const EMMotionBlurMaterial &other ) = delete;
};


#endif // EM_MOTION_BLUR_MATERIAL
