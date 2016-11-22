//version 1.6

#ifndef GX_MOTION_BLUR_EFFECT
#define GX_MOTION_BLUR_EFFECT


#include "GXMesh.h"


class GXMotionBlurEffect : public GXMesh
{
	private:
		GLuint				motionBlurTexture;
		GLuint				motionBlurFBO;
		GXMat4				lastFrameViewProjMatrix;
		GLuint				invViewProjMatrixLocation;
		GLuint				lastFrameViewProjMatrixLocation;

		GLuint				preoutTexture;
		GLuint				depthTexture;

	public:
		GXMotionBlurEffect ();
		virtual ~GXMotionBlurEffect ();

		virtual GXVoid Draw ();

		GLuint GetOutTexture ();
		GXVoid SetTextures ( GLuint preoutTexture, GLuint depthTexture );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif	//GX_MOTION_BLUR_EFFECT