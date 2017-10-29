//version 1.0

#ifndef GX_SKIN_EXPORTER
#define GX_SKIN_EXPORTER


#include "GXExporter.h"
#include "GXSkeleton.h"


class GXSkinExporter : public GXExporter
{
	private:
		GXSkeleton				skeleton;
		GXUInt					totalVertices;
		GXUByte*				vboData;

	public:
		explicit GXSkinExporter ( INode &selection, const GXUTF8* fileName );
		~GXSkinExporter () override;

		GXUTF8* GetWarnings () const override;
		GXUTF8* GetReport () const override;

	protected:
		GXBool Export () override;

	private:
		GXBool Init ();
		GXBool ExtractGeometryData ( IGameObject &skinMesh );

		GXSkinExporter ( const GXSkinExporter &other ) = delete;
		GXSkinExporter& operator = ( const GXSkinExporter &other ) = delete;
};


#endif //GX_SKIN_EXPORTER
