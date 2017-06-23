#ifndef EM_SSAO_APPLY_MATERIAL
#define EM_SSAO_APPLY_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMSSAOApplyMaterial : public GXMaterial
{
	private:
		GXTexture*	ssaoTexture;
		GXTexture*	imageTexture;

	public:
		EMSSAOApplyMaterial ();
		~EMSSAOApplyMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetSSAOTexture ( GXTexture &texture );
		GXVoid SetImageTexture ( GXTexture &texture );
};


#endif //EM_SSAO_APPLY_MATERIAL
