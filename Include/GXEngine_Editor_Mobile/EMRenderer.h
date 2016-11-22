#ifndef EM_RENDERER
#define EM_RENDERER


#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXEngine/GXShaderStorage.h>


typedef GXVoid ( GXCALL* PFNEMRENDERERONOBJECTPROC ) ( GXUPointer object );

enum eEMRenderPass : GXUByte
{
	EM_RENDER_PASS_COMMON					= 0,
	EM_RENDER_PASS_LIGHT					= 1,
	EM_RENDER_PASS_HUD_DEPTH_DEPENDENT		= 2,
	EM_RENDER_PASS_HUD_DEPTH_INDEPENDENT	= 3
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

		GLuint						fbo;
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

	public:
		EMRenderer ();
		~EMRenderer ();

		GXVoid StartCommonPass ();
		GXVoid StartLightPass ();
		GXVoid StartHudColorPass ();
		GXVoid StartHudMaskPass ();

		GXVoid SetObjectMask ( GXUPointer object );

		GXVoid PresentFrame ();

		GXVoid SetOnObjectCallback ( PFNEMRENDERERONOBJECTPROC callback );
		GXVoid GetObject ( GXUShort x, GXUShort y );

	private:
		GXVoid CreateFBO ();
		GXVoid CreateScreenQuad ();
		GXVoid InitDirectedLightShader ();

		GXVoid LightUp ();
		GXVoid LightUpByDirected ( EMDirectedLight* light );
		GXVoid LightUpBySpot ( EMSpotlight* light );
		GXVoid LightUpByBulp ( EMBulp* light );

		GXUPointer SampleObject ();
};


#endif //EM_RENDERER
