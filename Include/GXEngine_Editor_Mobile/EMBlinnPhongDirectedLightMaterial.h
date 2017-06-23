#ifndef EM_BLINN_PHONG_DIRECTED_LIGHT_MATERIAL
#define EM_BLINN_PHONG_DIRECTED_LIGHT_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMBlinnPhongDirectedLightMaterial : public GXMaterial
{
	private:
		GXTexture*			diffuseTexture;
		GXTexture*			normalTexture;
		GXTexture*			specularTexture;
		GXTexture*			emissionTexture;
		GXTexture*			depthTexture;

		GXVec3				toLightDirectionView;
		GLint				toLightDirectionViewLocation;

		GXVec3				color;
		GLint				colorLocation;

		GXVec3				ambientColor;
		GLint				ambientColorLocation;

		GLint				inv_proj_matLocation;

	public:
		EMBlinnPhongDirectedLightMaterial ();
		~EMBlinnPhongDirectedLightMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetDiffuseTexture ( GXTexture &texture );
		GXVoid SetNormalTexture ( GXTexture &texture );
		GXVoid SetSpecularTexture ( GXTexture &texture );
		GXVoid SetEmissionTexture ( GXTexture &texture );
		GXVoid SetDepthTexture ( GXTexture &texture );

		GXVoid SetToLightDirectionView ( const GXVec3 &toLightDirectionView );
		GXVoid SetColor ( const GXVec3 &color );
		GXVoid SetAmbientColor ( const GXVec3 &color );
};


#endif //EM_BLINN_PHONG_DIRECTED_LIGHT_MATERIAL
