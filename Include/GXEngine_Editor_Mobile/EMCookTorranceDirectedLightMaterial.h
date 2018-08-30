#ifndef EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL
#define EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXSampler.h>


class EMCookTorranceDirectedLightMaterial final : public GXMaterial
{
	private:
		GXTexture2D*	albedoTexture;
		GXTexture2D*	normalTexture;
		GXTexture2D*	emissionTexture;
		GXTexture2D*	parameterTexture;
		GXTexture2D*	depthTexture;

		GXSampler		sampler;

		GXColorRGB		hue;
		GXFloat			intensity;
		GXColorRGB		hdrColor;
		GXColorRGB		ambientColor;
		GXVec3			toLightDirectionView;

		GLint			hueLocation;
		GLint			intensityLocation;
		GLint			hdrColorLocation;
		GLint			ambientColorLocation;
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

	private:
		EMCookTorranceDirectedLightMaterial ( const EMCookTorranceDirectedLightMaterial &other ) = delete;
		EMCookTorranceDirectedLightMaterial& operator = ( const EMCookTorranceDirectedLightMaterial &other ) = delete;
};


#endif // EM_COOK_TORRANCE_DIRECTED_LIGHT_MATERIAL
