#ifndef EM_RENDERER
#define EM_RENDERER


#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXEngine/GXShaderStorage.h>


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
		GLuint						diffuseTexture;
		GLuint						normalTexture;
		GLuint						specularTexture;
		GLuint						emissionTexture;
		GLuint						objectTextures[ 2 ];
		GLuint						depthStencilTexture;
		GLuint						outTexture;
		GLuint						combineHudTarget;

		GLuint						fbo;
		GLuint						sourceFbo;

		GLuint						screenSampler;

		GXUByte						objectMask[ 8 ];

		GXVAOInfo					screenQuadVAO;
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

		static EMRenderer* GetInstance ();

	private:
		explicit EMRenderer ();

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
