//version 1.7

#ifndef GX_DOF_EFFECT
#define GX_DOF_EFFECT


#include "GXMesh.h"


class GXDoFEffect : public GXMesh
{
	private:
		GLuint				dofTexture;
		GLuint				dofFBO;

		GLuint				gaussDownsampleTextureFirstPass;
		GLuint				gaussDownsampleTextureSecondPass;
		GLuint				downsampleFBO;

		GLuint				fBlurVLocation;
		GLuint				fBlurHLocation;
		GLuint				pixelSizeHighLocation;
		GLuint				pixelSizeLowLocation;

		GLuint				preoutTexture;

		GXUShort			downsampleWidth;
		GXUShort			downsampleHeight;

		GXFloat				pixelSizeHigh [ 2 ];
		GXFloat				pixelSizeLow [ 2 ];
		GXFloat				world_space_focus_offset;
		GXFloat				world_space_focus_range;

		GXVec3				dofParams;		//x - фокальное расстояние. 
											//y - фокальный разброс. 
											//z - максимальная степень размытия. В диапазоне [0.0f, 1.0f]. 0.0f - не размывается вовсе, 1.0f размывается максимально.
	public:
		GXDoFEffect ();
		virtual ~GXDoFEffect ();

		virtual GXVoid Draw ();

		GLuint GetOutTexture ();
		GXVoid SetPreoutTexture ( GLuint preoutTexture );

		GXVoid SetDoFParams ( GXFloat world_space_focus_offset, GXFloat world_space_focus_range, GXFloat maxBlurness );
		GXVoid GetDoFDepthParams ( GXFloat& focus, GXFloat& focusRange, GXFloat& maxBlurness );
		GXVoid GetDoFDepthParams ( GXVec3& dofParams );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif	//GX_DOF_EFFECT