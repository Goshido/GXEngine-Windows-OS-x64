//version 1.0

#ifndef GX_MESH_EXPORTER
#define GX_MESH_EXPORTER


#include "GXExporter.h"


class GXMeshExporter : public GXExporter
{
	private:
		GXUInt					totalVertices;
		GXUByte*				vboData;

	public:
		explicit GXMeshExporter ( INode &selection, const GXUTF8* fileName, GXBool isExportInCurrentPose );
		~GXMeshExporter () override;

		GXUTF8* GetWarnings () const override;
		GXUTF8* GetReport () const override;

	protected:
		GXBool Export () override;

	private:
		GXBool Init ( GXBool isExportInCurrentPose );
		GXBool ExtractGeometryData ( IGameObject &meshToExport, GXBool isExportInCurrentPose );

		GXMeshExporter ( const GXMeshExporter &other ) = delete;
		GXMeshExporter& operator = ( const GXMeshExporter &other ) = delete;
};


#endif //GX_MESH_EXPORTER
