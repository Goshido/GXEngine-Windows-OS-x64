#ifndef EM_IMPORTANT_AREA_FILTER_MATERIAL
#define EM_IMPORTANT_AREA_FILTER_MATERIAL


#include <GXEngine/GXMaterial.h>
#include <GXEngine/GXTexture2D.h>


class EMImportantAreaFilterMaterial : public GXMaterial
{
	private:
		GXTexture2D*		imageTexture;
		GXTexture2D			retinaFilterTexture;

	public:
		EMImportantAreaFilterMaterial ();
		~EMImportantAreaFilterMaterial () override;

		GXVoid Bind ( const GXTransform &transform ) override;
		GXVoid Unbind () override;

		GXVoid SetImageTexture ( GXTexture2D &texture );

	private:
		GXVoid GenerateRetinaFilterTexture ( GXUShort effectiveLength );
};


#endif //EM_IMPORTANT_AREA_FILTER_MATERIAL
