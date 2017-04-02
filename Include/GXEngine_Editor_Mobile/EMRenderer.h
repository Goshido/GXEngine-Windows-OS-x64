#ifndef EM_RENDERER
#define EM_RENDERER


#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXTexture.h>


typedef GXVoid ( GXCALL* PFNEMRENDERERONOBJECTPROC ) ( GXUPointer object );

enum class eEMRenderPass : GXUByte
{
	Common				= 0,
	Light				= 1,
	HudDepthDependent	= 2,
	HudDepthIndependent	= 3
};

enum class eEMRenderTarget
{
	Albedo,
	Normal,
	Specular,
	Emission,
	Depth,
	Combine
};

class EMRenderer
{
	private:
		GXTexture					diffuseTexture;
		GXTexture					normalTexture;
		GXTexture					specularTexture;
		GXTexture					emissionTexture;
		GXTexture					objectTextures[ 2 ];
		GXTexture					depthStencilTexture;
		GXTexture					outTexture;
		GXTexture					combineHudTarget;

		GLuint						fbo;
		GLuint						sourceFbo;

		GLuint						screenSampler;

		GXUByte						objectMask[ 8 ];

		GXMeshGeometry				screenQuadMesh;
		GXShaderProgram				screenQuadProgram;

		GXMat4						inv_proj_mat;

		GLint						dl_lightDirectionViewLocation;
		GLint						dl_inv_proj_matLocation;
		GLint						dl_colorLocation;
		GLint						dl_ambientColorLocation;
		GXShaderProgram				directedLightProgram;

		GXInt						mouseX;
		GXInt						mouseY;
		PFNEMRENDERERONOBJECTPROC	OnObject;

		static EMRenderer*			instance;

	public:
		~EMRenderer ();

		GXVoid StartCommonPass ();
		GXVoid StartLightPass ();
		GXVoid StartHudColorPass ();
		GXVoid StartHudMaskPass ();

		GXVoid SetObjectMask ( GXUPointer object );
		
		GXVoid CombineHudWithTarget ( eEMRenderTarget target = eEMRenderTarget::Combine );
		GXVoid PresentFrame ( eEMRenderTarget target = eEMRenderTarget::Combine );

		GXVoid SetOnObjectCallback ( PFNEMRENDERERONOBJECTPROC callback );
		GXVoid GetObject ( GXUShort x, GXUShort y );

		static EMRenderer* GXCALL GetInstance ();

	private:
		EMRenderer ();

		GXVoid CreateFBO ();
		GXVoid CreateScreenQuad ();
		GXVoid InitDirectedLightShader ();

		GXVoid LightUp ();
		GXVoid LightUpByDirected ( EMDirectedLight* light );
		GXVoid LightUpBySpot ( EMSpotlight* light );
		GXVoid LightUpByBulp ( EMBulp* light );

		GXVoid CopyTexureToCombineTexture ( GLuint texture );

		GXUPointer SampleObject ();
};


#endif //EM_RENDERER
