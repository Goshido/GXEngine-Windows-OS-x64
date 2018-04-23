#ifndef EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL
#define EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>


class EMCookTorranceDirectedLightMaterial : public GXMaterial
{
	private:
		GXTexture2D*	albedoTexture;
		GXTexture2D*	normalTexture;
		GXTexture2D*	emissionTexture;
		GXTexture2D*	parameterTexture;
		GXTexture2D*	depthTexture;

		GXColorRGB		hue;
		GLint			hueLocation;

		GXFloat			intensity;
		GXInt			intensityLocation;

		GXColorRGB		hdrColor;
		GLint			hdrColorLocation;

		GXColorRGB		ambientColor;
		GLint			ambientColorLocation;

		GXVec3			toLightDirectionView;
		GLint			toLightDirectionViewLocation;

		GLint			inverseProjectionMatrixLocation;

	public:
		EMCookTorranceDirectedLightMaterial ();
		~EMCookTorranceDirectedLightMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetAlbedoTexture ( GXTexture2D &texture );
		GXVoid SetNormalTexture ( GXTexture2D &texture );
		GXVoid SetEmissionTexture ( GXTexture2D &texture );
		GXVoid SetParameterTexture ( GXTexture2D &texture );
		GXVoid SetDepthTexture ( GXTexture2D &texture );

		GXVoid SetLightDirectionView ( const GXVec3 &direction );
		GXVoid SetHue ( GXUByte red, GXUByte green, GXUByte blue );
		GXVoid SetIntencity ( GXFloat intencity );
		GXVoid SetAmbientColor ( const GXColorRGB &color );
};


#endif // EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL
