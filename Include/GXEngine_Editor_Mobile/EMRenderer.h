#ifndef EM_RENDERER
#define EM_RENDERER


#include "EMLight.h"
#include "EMMesh.h"
#include "EMBlinnPhongDirectedLightMaterial.h"
#include "EMVelocityMaterial.h"
#include "EMVelocityTileMaxMaterial.h"
#include "EMVelocityNeighborMaxMaterial.h"
#include "EMMotionBlurMaterial.h"
#include <GXEngine/GXUnlitTexture2DMaterial.h>
#include <GXEngine/GXCameraOrthographic.h>


typedef GXVoid ( GXCALL* PFNEMRENDERERONOBJECTPROC ) ( GXUPointer object );

enum class eEMRenderTarget
{
	Albedo,
	Normal,
	Specular,
	Emission,
	VelocityView,
	VelocityBlur,
	VelocityTileMax,
	VelocityNeighborMax,
	Depth,
	Combine
};

class EMRenderer
{
	private:
		GXTexture							diffuseTexture;
		GXTexture							normalTexture;
		GXTexture							specularTexture;
		GXTexture							emissionTexture;
		GXTexture							velocityViewTexture;
		GXTexture							velocityBlurTexture;
		GXTexture							velocityTileMaxTexture;
		GXTexture							velocityNeighborMaxTexture;
		GXTexture							objectTextures[ 2 ];
		GXTexture							depthStencilTexture;
		GXTexture							outTexture;
		GXTexture							motionBlurredTexture;

		GLuint								fbo;
		GLuint								sourceFbo;

		GXUByte								objectMask[ 8 ];

		EMMesh								screenQuadMesh;
		EMBlinnPhongDirectedLightMaterial	directedLightMaterial;
		EMVelocityMaterial					velocityMaterial;
		EMVelocityTileMaxMaterial			velocityTileMaxMaterial;
		EMVelocityNeighborMaxMaterial		velocityNeighborMaxMaterial;
		EMMotionBlurMaterial				motionBlurMaterial;
		GXUnlitTexture2DMaterial			unlitMaterial;
		GXCameraOrthographic				outCamera;

		GXInt								mouseX;
		GXInt								mouseY;
		PFNEMRENDERERONOBJECTPROC			OnObject;

		static EMRenderer*					instance;

	public:
		~EMRenderer ();

		GXVoid StartCommonPass ();
		GXVoid StartLightPass ();
		GXVoid StartHudColorPass ();
		GXVoid StartHudMaskPass ();

		GXVoid SetObjectMask ( GXUPointer object );

		GXVoid ApplyMotionBlur ( GXFloat deltaTime );
		GXVoid PresentFrame ( eEMRenderTarget target );

		GXVoid SetOnObjectCallback ( PFNEMRENDERERONOBJECTPROC callback );
		GXVoid GetObject ( GXUShort x, GXUShort y );

		static EMRenderer* GXCALL GetInstance ();

	private:
		EMRenderer ();

		GXVoid CreateFBO ();

		GXVoid LightUp ();
		GXVoid LightUpByDirected ( EMDirectedLight* light );
		GXVoid LightUpBySpot ( EMSpotlight* light );
		GXVoid LightUpByBulp ( EMBulp* light );

		GXUPointer SampleObject ();
};


#endif //EM_RENDERER
