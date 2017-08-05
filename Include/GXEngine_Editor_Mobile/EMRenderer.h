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
#include <GXEngine/GXUnlitTexture2DMaterial.h>
#include <GXEngine/GXCameraOrthographic.h>


typedef GXVoid ( GXCALL* PFNEMRENDERERONOBJECTPROC ) ( GXUPointer object );

enum class eEMRenderTarget
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

class EMRenderer
{
	private:
		GXTexture2D							albedoTexture;
		GXTexture2D							normalTexture;
		GXTexture2D							emissionTexture;
		GXTexture2D							parameterTexture;
		GXTexture2D							velocityBlurTexture;
		GXTexture2D							velocityTileMaxTexture;
		GXTexture2D							velocityNeighborMaxTexture;
		GXTexture2D							ssaoOmegaTexture;
		GXTexture2D							ssaoYottaTexture;
		GXTexture2D							objectTextures[ 2 ];
		GXTexture2D							depthStencilTexture;
		GXTexture2D							omegaTexture;
		GXTexture2D							yottaTexture;

		GLuint								fbo;

		GXUByte								objectMask[ 8 ];

		EMMesh								screenQuadMesh;

		EMCookTorranceDirectedLightMaterial	directedLightMaterial;
		EMLightProbeMaterial				lightProbeMaterial;
		EMVelocityTileMaxMaterial			velocityTileMaxMaterial;
		EMVelocityNeighborMaxMaterial		velocityNeighborMaxMaterial;
		EMMotionBlurMaterial				motionBlurMaterial;
		EMGaussHorizontalBlurMaterial		gaussHorizontalBlurMaterial;
		EMGaussVerticalBlurMaterial			gaussVerticalBlurMaterial;
		EMSSAOSharpMaterial					ssaoSharpMaterial;
		EMSSAOApplyMaterial					ssaoApplyMaterial;
		GXUnlitTexture2DMaterial			unlitMaterial;

		GXCameraOrthographic				outCamera;

		GXInt								mouseX;
		GXInt								mouseY;
		PFNEMRENDERERONOBJECTPROC			OnObject;

		GXBool								isMotionBlurSettingsChanged;
		GXUByte								newMaxMotionBlurSamples;
		GXFloat								newMotionBlurDepthLimit;
		GXFloat								motionBlurExposure;
		GXFloat								newMotionBlurExposure;

		GXBool								isSSAOSettingsChanged;
		GXFloat								newSSAOCheckRadius;
		GXUByte								newSSAOSamples;
		GXUShort							newSSAONoiseTextureResolution;
		GXFloat								newSSAOMaxDistance;

		static EMRenderer*					instance;

	public:
		~EMRenderer ();

		GXVoid StartCommonPass ();
		GXVoid StartEnvironmentPass ();
		GXVoid StartLightPass ();
		GXVoid StartHudColorPass ();
		GXVoid StartHudMaskPass ();

		GXVoid SetObjectMask ( GXUPointer object );

		GXVoid ApplySSAO ();
		GXVoid ApplyMotionBlur ( GXFloat deltaTime );

		GXVoid PresentFrame ( eEMRenderTarget target );

		GXVoid SetOnObjectCallback ( PFNEMRENDERERONOBJECTPROC callback );
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

		GXTexture2D& GetDepthTexture ();

		static EMRenderer& GXCALL GetInstance ();

	private:
		EMRenderer ();

		GXVoid CreateFBO ();

		GXVoid UpdateMotionBlurSettings ();
		GXVoid UpdateSSAOSettings ();

		GXVoid LightUp ();
		GXVoid LightUpByDirected ( EMDirectedLight* light );
		GXVoid LightUpBySpot ( EMSpotlight* light );
		GXVoid LightUpByBulp ( EMBulp* light );

		GXUPointer SampleObject ();
};


#endif //EM_RENDERER
