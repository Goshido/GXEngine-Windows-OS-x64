#ifndef EM_COOK_TORRANCE_COMMON_PASS_MATERIAL
#define EM_COOK_TORRANCE_COMMON_PASS_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>


class EMCookTorranceCommonPassMaterial : public GXMaterial
{
	private:
		GXTexture2D*		albedoTexture;
		GXTexture2D*		normalTexture;
		GXTexture2D*		emissionTexture;

		// texture components:
		// red - roughness. Remapping to [0.0f, 1.0f] and take square.
		// green - index of refraction. Remapping [0.0f, 5.0f]
		// blue - specular intensity. Remapping to [0.0f, 1.0f]
		// alpha - metallic. Remapping rule: 1.0f if value > 129, 0.0f otherwise
		GXTexture2D*		parameterTexture;

		GXVec4				albedoTextureScaleOffset;
		GLint				albedoTextureScaleOffsetLocation;

		GXColorRGB			albedoColor;
		GLint				albedoColorLocation;

		GXVec4				normalTextureScaleOffset;
		GLint				normalTextureScaleOffsetLocation;

		GXVec4				emissionTextureScaleOffset;
		GLint				emissionTextureScaleOffsetLocation;

		GXColorRGB			emissionColor;
		GXFloat				emissionColorScale;
		GXColorRGB			emissionScaledColor;
		GLint				emissionScaledColorLocation;

		GXVec4				parameterTextureScaleOffset;
		GLint				parameterTextureScaleOffsetLocation;

		// x - roughness scale
		// y - index of refraction scale
		// z - specular intencity scale
		// w - metallic scale
		GXVec4				parameterScale;
		GLint				parameterScaleLocation;

		GXFloat				inverseDeltaTime;
		GLint				inverseDeltaTimeLocation;

		GXFloat				exposure;
		GLint				exposureLocation;

		GXVec2				screenResolution;
		GLint				screenResolutionLocation;

		GXFloat				maximumBlurSamples;
		GLint				maximumBlurSamplesLocation;

		GXFloat				inverseMaximumBlurSamples;
		GLint				inverseMaximumBlurSamplesLocation;

		GLint				currentFrameModelViewProjectionMatrixLocation;
		GLint				currentFrameRotationViewMatrixLocation;
		GLint				lastFrameModelViewProjectionMatrixLocation;

	public:
		EMCookTorranceCommonPassMaterial ();
		~EMCookTorranceCommonPassMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetAlbedoTexture ( GXTexture2D &texture );
		GXTexture2D* GetAlbedoTexture ();
		GXVoid SetAlbedoTextureScale ( GXFloat scaleU, GXFloat scaleV );
		GXVoid GetAlbedoTextureScale ( GXVec2 &scaleUV ) const;
		GXVoid SetAlbedoTextureOffset ( GXFloat offsetU, GXFloat offsetV );
		GXVoid GetAlbedoTextureOffset ( GXVec2 &offsetUV ) const;
		GXVoid SetAlbedoColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );
		GXVoid GetAlbedoColor ( GXUByte &red, GXUByte &green, GXUByte &blue, GXUByte &alpha ) const;

		GXVoid SetNormalTexture ( GXTexture2D &texture );
		GXTexture2D* GetNormalTexture ();
		GXVoid SetNormalTextureScale ( GXFloat scaleU, GXFloat scaleV );
		GXVoid GetNormalTextureScale ( GXVec2 &scaleUV ) const;
		GXVoid SetNormalTextureOffset ( GXFloat offsetU, GXFloat offsetV );
		GXVoid GetNormalTextureOffset ( GXVec2 &offsetUV ) const;

		GXVoid SetEmissionTexture ( GXTexture2D &texture );
		GXTexture2D* GetEmissionTexture ();
		GXVoid SetEmissionTextureScale ( GXFloat scaleU, GXFloat scaleV );
		GXVoid GetEmissionTextureScale ( GXVec2 &scaleUV ) const;
		GXVoid SetEmissionTextureOffset ( GXFloat offsetU, GXFloat offsetV );
		GXVoid GetEmissionTextureOffset ( GXVec2 &offsetUV ) const;
		GXVoid SetEmissionColor ( GXUByte red, GXUByte green, GXUByte blue );
		GXVoid GetEmissionColor ( GXUByte &red, GXUByte &green, GXUByte &blue ) const;
		GXVoid SetEmissionColorScale ( GXFloat scale );
		GXFloat GetEmissionColorScale () const;

		GXVoid SetParameterTexture ( GXTexture2D &texture );
		GXTexture2D* GetParameterTexture ();
		GXVoid SetParameterTextureScale ( GXFloat scaleU, GXFloat scaleV );
		GXVoid GetParameterTextureScale ( GXVec2 &scaleUV ) const;
		GXVoid SetParameterTextureOffset ( GXFloat offsetU, GXFloat offsetV );
		GXVoid GetParameterTextureOffset ( GXVec2 &offsetUV ) const;
		GXVoid SetRoughnessScale ( GXFloat scale );
		GXFloat GetRoughnessScale () const;
		GXVoid SetIndexOfRefractionScale ( GXFloat scale );
		GXFloat GetIndexOfRefractionScale () const;
		GXVoid SetSpecularIntensityScale ( GXFloat scale );
		GXFloat GetSpecularIntensityScale () const;
		GXVoid SetMetallicScale ( GXFloat scale );
		GXFloat GetMetallicScale () const;

		GXVoid SetDeltaTime ( GXFloat seconds );
		GXVoid SetExposure ( GXFloat seconds );
		GXVoid SetScreenResolution ( GXUShort width, GXUShort height );
		GXVoid SetMaximumBlurSamples ( GXUByte samples );
};


#endif // EM_COOK_TORRANCE_COMMON_PASS_MATERIAL
