#ifndef EM_SSAO_APPLY_MATERIAL
#define EM_SSAO_APPLY_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>


class EMSSAOApplyMaterial : public GXMaterial
{
	private:
		GXTexture2D*	ssaoTexture;
		GXTexture2D*	imageTexture;

	public:
		EMSSAOApplyMaterial ();
		~EMSSAOApplyMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetSSAOTexture ( GXTexture2D &texture );
		GXVoid SetImageTexture ( GXTexture2D &texture );
};


#endif // EM_SSAO_APPLY_MATERIAL
