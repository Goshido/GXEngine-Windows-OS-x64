#ifndef EM_RENDERER
#define EM_RENDERER


#include "EMLight.h"
#include "EMMesh.h"
#include "EMCookTorranceDirectedLightMaterial.h"
#include "EMLightProbeMaterial.h"
#include "EMVelocityTileMaxMaterial.h"
#include "EMVelocityNeighborMaxMaterial.h"
#include "EMMotionBlurMaterial.h"
#include "EMGaussHorizontalBlurMaterial.h"
#include "EMGaussVerticalBlurMaterial.h"
#include "EMSSAOSharpMaterial.h"
#include "EMSSAOApplyMaterial.h"
#include "EMToneMapperMaterial.h"
#include "EMToneMapperLuminanceTripletReducerMaterial.h"
#include "EMImportantAreaFilterMaterial.h"
#include <GXEngine/GXUnlitTexture2DMaterial.h>
#include <GXEngine/GXCameraOrthographic.h>


typedef GXVoid ( GXCALL* EMRendererOnObjectHandler ) ( GXVoid* context, GXVoid* object );

enum class eEMRenderTarget : GXUShort
{
    Albedo,
    Normal,
    Emission,
    Parameter,
    VelocityBlur,
    VelocityTileMax,
    VelocityNeighborMax,
    Depth,
    SSAO,
    Combine
};

class EMRenderer final
{
    private:
        GXInt                                           _mouseX;
        GXInt                                           _mouseY;

        EMRendererOnObjectHandler                       _onObject;
        GXVoid*                                         _onObjectContext;

        GXUByte                                         _newMaxMotionBlurSamples;
        GXFloat                                         _newMotionBlurDepthLimit;
        GXFloat                                         _motionBlurExposure;
        GXFloat                                         _newMotionBlurExposure;
        GXBool                                          _isMotionBlurSettingsChanged;

        GXFloat                                         _newSSAOCheckRadius;
        GXUByte                                         _newSSAOSamples;
        GXUShort                                        _newSSAONoiseTextureResolution;
        GXFloat                                         _newSSAOMaxDistance;
        GXBool                                          _isSSAOSettingsChanged;

        GXTexture2D                                     _albedoTexture;
        GXTexture2D                                     _normalTexture;
        GXTexture2D                                     _emissionTexture;
        GXTexture2D                                     _parameterTexture;
        GXTexture2D                                     _velocityBlurTexture;
        GXTexture2D                                     _velocityTileMaxTexture;
        GXTexture2D                                     _velocityNeighborMaxTexture;
        GXTexture2D                                     _ssaoOmegaTexture;
        GXTexture2D                                     _ssaoYottaTexture;
        GXTexture2D                                     _objectTextures[ 2u ];
        GXTexture2D                                     _importantAreaTexture;
        GXTexture2D                                     _depthStencilTexture;
        GXTexture2D                                     _omegaTexture;
        GXTexture2D                                     _yottaTexture;

        EMGaussHorizontalBlurMaterial                   _gaussHorizontalBlurMaterial;
        EMGaussVerticalBlurMaterial                     _gaussVerticalBlurMaterial;

        GLuint                                          _fbo;

        GXUByte                                         _objectMask[ 8u ];

        EMMesh                                          _screenQuadMesh;

        EMCookTorranceDirectedLightMaterial             _directedLightMaterial;
        EMLightProbeMaterial                            _lightProbeMaterial;
        EMVelocityTileMaxMaterial                       _velocityTileMaxMaterial;
        EMVelocityNeighborMaxMaterial                   _velocityNeighborMaxMaterial;
        EMMotionBlurMaterial                            _motionBlurMaterial;
        EMSSAOSharpMaterial                             _ssaoSharpMaterial;
        EMSSAOApplyMaterial                             _ssaoApplyMaterial;
        EMToneMapperMaterial                            _toneMapperMaterial;
        EMToneMapperLuminanceTripletReducerMaterial     _toneMapperLuminanceTripletReducerMaterial;
        EMImportantAreaFilterMaterial                   _importantAreaFilterMaterial;
        GXUnlitTexture2DMaterial                        _unlitMaterial;

        GXCameraOrthographic                            _outCamera;

        GXBool                                          _isToneMapperSettingsChanged;
        GXFloat                                         _newToneMapperGamma;
        GXFloat                                         _newToneMapperEyeSensitivity;
        GXFloat                                         _newToneMapperAbsoluteWhiteIntensity;

        GXFloat                                         _toneMapperEyeAdaptationSpeed;
        GXVec3                                          _toneMapperEffectiveLuminanceTriplet;

        static EMRenderer*                              _instance;

    public:
        ~EMRenderer ();

        GXVoid StartCommonPass ();
        GXVoid StartEnvironmentPass ();
        GXVoid StartLightPass ();
        GXVoid StartHudColorPass ();
        GXVoid StartHudMaskPass ();

        GXVoid SetObjectMask ( GXVoid* object );

        GXVoid ApplySSAO ();
        GXVoid ApplyMotionBlur ();
        GXVoid ApplyToneMapping ( GXFloat deltaTime );

        GXVoid PresentFrame ( eEMRenderTarget target );

        GXVoid SetOnObjectCallback ( GXVoid* context, EMRendererOnObjectHandler callback );
        GXVoid GetObject ( GXUShort x, GXUShort y );

        GXVoid SetMaximumMotionBlurSamples ( GXUByte samples );
        GXUByte GetMaximumMotionBlurSamples () const;

        GXVoid SetMotionBlurDepthLimit ( GXFloat meters );
        GXFloat GetMotionBlurDepthLimit () const;

        GXVoid SetMotionBlurExposure ( GXFloat seconds );
        GXFloat GetMotionBlurExposure () const;

        GXVoid SetSSAOCheckRadius ( GXFloat meters );
        GXFloat GetSSAOCheckRadius () const;

        GXVoid SetSSAOSampleNumber ( GXUByte samples );
        GXUByte GetSSAOSampleNumber () const;

        GXVoid SetSSAONoiseTextureResolution ( GXUShort resolution );
        GXUShort GetSSAONoiseTextureResolution () const;

        GXVoid SetSSAOMaximumDistance ( GXFloat meters );
        GXFloat GetSSAOMaximumDistance () const;

        GXVoid SetToneMapperGamma ( GXFloat gamma );
        GXFloat GetToneMapperGamma () const;

        // Clamped to [ 0.0f, +infinite )
        GXVoid SetToneMapperEyeAdaptationSpeed ( GXFloat speed );
        GXFloat GetToneMapperEyeAdaptationSpeed () const;

        // Artistic purpose parameter. There is no physical explanation.
        GXVoid SetToneMapperEyeSensitivity ( GXFloat sensitivity );
        GXFloat GetToneMapperEyeSensitivity () const;

        GXVoid SetToneMapperAbsoluteWhiteIntensity ( GXFloat intensity );
        GXFloat GetToneMapperAbsoluteWhiteIntensity () const;

        GXTexture2D& GetDepthTexture ();

        static EMRenderer& GXCALL GetInstance ();

    private:
        EMRenderer ();

        GXVoid CreateFBO ();

        GXVoid UpdateMotionBlurSettings ();
        GXVoid UpdateSSAOSettings ();
        GXVoid UpdateToneMapperSettings ();

        GXVoid LightUp ();
        GXVoid LightUpByDirected ( EMDirectedLight* light );
        GXVoid LightUpBySpot ( EMSpotlight* light );
        GXVoid LightUpByBulp ( EMBulp* light );

        GXVoid* SampleObject ();

        EMRenderer ( const EMRenderer &other ) = delete;
        EMRenderer& operator = ( const EMRenderer &other ) = delete;
};


#endif // EM_RENDERER
