#ifndef EM_BLINN_PHONG_COMMON_PASS_MATERIAL
#define EM_BLINN_PHONG_COMMON_PASS_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture.h>


class EMBlinnPhongCommonPassMaterial : public GXMaterial
{
	private:
		GXTexture*			diffuseTexture;
		GXVec4				diffuseTextureScaleOffset;
		GLint				diffuseTextureScaleOffsetLocation;
		GXVec4				diffuseColor;
		GLint				diffuseColorLocation;

		GXTexture*			normalTexture;
		GXVec4				normalTextureScaleOffset;
		GLint				normalTextureScaleOffsetLocation;

		GXTexture*			specularTexture;
		GXVec4				specularTextureScaleOffset;
		GLint				specularTextureScaleOffsetLocation;

		GXTexture*			emissionTexture;
		GXVec4				emissionTextureScaleOffset;
		GLint				emissionTextureScaleOffsetLocation;
		GXVec3				emissionColor;
		GLint				emissionColorLocation;

		GLint				currentModelViewProjectionMatrixLocation;
		GLint				currentRotationViewMatrixLocation;
		GLint				currentModelViewMatrixLocation;
		GLint				lastFrameModelViewMatrixLocation;

	public:
		EMBlinnPhongCommonPassMaterial ();
		~EMBlinnPhongCommonPassMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) const override;
		GXVoid Unbind () const override;

		GXVoid SetDiffuseTexture ( GXTexture &texture );
		GXVoid SetDiffuseTextureScale ( GXFloat x, GXFloat y );
		GXVoid SetDiffuseTextureOffset ( GXFloat x, GXFloat y );
		GXVoid SetDiffuseTextureColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha );

		GXVoid SetNormalTexture ( GXTexture &texture );
		GXVoid SetNormalTextureScale ( GXFloat x, GXFloat y );
		GXVoid SetNormalTextureOffset ( GXFloat x, GXFloat y );

		GXVoid SetSpecularTexture ( GXTexture &texture );
		GXVoid SetSpecularTextureScale ( GXFloat x, GXFloat y );
		GXVoid SetSpecularTextureOffset ( GXFloat x, GXFloat y );

		GXVoid SetEmissionTexture ( GXTexture &texture );
		GXVoid SetEmissionTextureScale ( GXFloat x, GXFloat y );
		GXVoid SetEmissionTextureOffset ( GXFloat x, GXFloat y );
		GXVoid SetEmissionTextureColor ( GXUByte red, GXUByte green, GXUByte blue );
};


#endif //EM_BLINN_PHONG_COMMON_PASS_MATERIAL
