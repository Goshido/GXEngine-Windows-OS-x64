#ifndef EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL
#define EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMCookTorranceDirectedLightMaterial : public GXMaterial
{
	private:
		GXTexture*		albedoTexture;
		GXTexture*		normalTexture;
		GXTexture*		emissionTexture;
		GXTexture*		parameterTexture;
		GXTexture*		depthTexture;

		GXVec3			hue;
		GLint			hueLocation;

		GXFloat			intensity;
		GXInt			intensityLocation;

		GXVec3			hdrColor;
		GLint			hdrColorLocation;

		GXVec3			ambientColor;
		GLint			ambientColorLocation;

		GXVec3			toLightDirectionView;
		GLint			toLightDirectionViewLocation;

		GLint			inverseProjectionMatrixLocation;

	public:
		EMCookTorranceDirectedLightMaterial ();
		~EMCookTorranceDirectedLightMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetAlbedoTexture ( GXTexture &texture );
		GXVoid SetNormalTexture ( GXTexture &texture );
		GXVoid SetEmissionTexture ( GXTexture &texture );
		GXVoid SetParameterTexture ( GXTexture &texture );
		GXVoid SetDepthTexture ( GXTexture &texture );

		GXVoid SetLightDirectionView ( const GXVec3 &direction );
		GXVoid SetHue ( GXUByte red, GXUByte green, GXUByte blue );
		GXVoid SetIntencity ( GXFloat intencity );
		GXVoid SetAmbientColor ( const GXVec3 &color );
};


#endif //EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL
