//version 1.0

#ifndef GX_SKELETON_EXPORTER
#define GX_SKELETON_EXPORTER


#include "GXExporter.h"
#include "GXSkeleton.h"


class GXSkeletonExporter : public GXExporter
{
	private:
		GXSkeleton		skeleton;

	public:
		explicit GXSkeletonExporter ( INode &selection, const GXUTF8* fileName );
		~GXSkeletonExporter () override;

		GXUTF8* GetWarnings () const override;
		GXUTF8* GetReport () const override;

	protected:
		GXBool Export () override;
		
	private:
		GXBool Init ();

		GXSkeletonExporter ( const GXSkeletonExporter &other ) = delete;
		GXSkeletonExporter& operator = ( const GXSkeletonExporter &other ) = delete;
};


#endif //GX_SKELETON_EXPORTER
