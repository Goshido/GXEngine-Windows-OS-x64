//version 1.1

#include <GXEngine/GXSkeletalMesh.h>


GXSkeletalMesh::GXSkeletalMesh ()
{
	skmaniminfo = 0;
	bShowSkeleton = GX_FALSE;
	bNeedUpdateSkeleton = GX_FALSE;
}

GXVoid GXSkeletalMesh::ShowSkeleton ( GXBool bShow ) 
{ 	
	bNeedUpdateSkeleton = bShow;
	bShowSkeleton = bShow;
};