//version 1.2

#include <GXEngine/GXPSKLoader.h>
#include <GXCommon/GXFileSystem.h>


GXVoid GXCALL FlipYZPointsRoutine ( VPoint* pointsdata, GXUInt NumPoints  )
{
	for ( GXUInt i = 0; i < NumPoints; i++ )
	{
		GXFloat Y = pointsdata[ i ].Point.y;
		pointsdata[ i ].Point.y = pointsdata[ i ].Point.z;
		pointsdata[ i ].Point.z = Y;
	}
}

GXVoid GXCALL FlipYZBonesRoutine ( VBone* bonesdata, GXUInt NumBones )
{
	for ( GXUInt i = 0; i < NumBones; i++ )
	{
		GXFloat Y = bonesdata[ i ].BonePos.Position.y;
		bonesdata[ i ].BonePos.Position.y = bonesdata[ i ].BonePos.Position.z;
		bonesdata[ i ].BonePos.Position.z = Y;
		Y = bonesdata[ i ].BonePos.Orientation.y;
		bonesdata[ i ].BonePos.Orientation.y = bonesdata[ i ].BonePos.Orientation.z;
		bonesdata[ i ].BonePos.Orientation.z = Y;
		bonesdata[ i ].BonePos.Orientation.w = -bonesdata[ i ].BonePos.Orientation.w;
	}
}

GXVoid GXCALL GXLoadPSK ( const GXWChar* file_name, GXSkeletalMeshInfo* skminfo )
{
	GXChar*	buffer;
	GXUInt	size;
	GXUInt	offset;

	VChunkHeader pointsheader;
	VChunkHeader wedgesheader;
	VChunkHeader facesheader;
	VChunkHeader materialsheader;
	VChunkHeader bonesheader;
	VChunkHeader influencesheader;

	VPoint*				pointsdata;
	VVertex*			wedgesdata;
	VTriangle*			facesdata;
	VMaterial*			materialsdata;			//≈сть дублирурующиес€ записи, имена нормальные
	VBone*				bonesdata;					//«аполн€ет им€ пробелами до name [ 30 ], затем шарашит \0
	VRawBoneInfluence*	influencesdata;
//---------------------------------------------------------------------------------------------------------

	if ( !GXLoadFile ( file_name, (GXVoid**)&buffer, size, GX_TRUE ) )
		GXDebugBox ( L"GXLoadPSK::Error - не могу загрузить файл" );

	offset = sizeof ( VChunkHeader );
	memcpy ( pointsheader.v, buffer + offset, sizeof ( VChunkHeader ) );
	pointsdata = (VPoint*)malloc ( pointsheader.DataCount * sizeof ( VPoint ) );
	offset += sizeof ( VChunkHeader );
	memcpy ( pointsdata, buffer + offset, pointsheader.DataCount * sizeof ( VPoint ) );
	FlipYZPointsRoutine ( pointsdata, pointsheader.DataCount  );

	offset += pointsheader.DataCount * pointsheader.DataSize;
	memcpy ( wedgesheader.v, buffer + offset, sizeof ( VChunkHeader ) );
	wedgesdata = (VVertex*)malloc ( wedgesheader.DataCount * sizeof ( VVertex ) );
	offset += sizeof ( VChunkHeader );
	memcpy ( wedgesdata, buffer + offset, wedgesheader.DataCount * sizeof ( VVertex ) );

	offset += wedgesheader.DataCount * wedgesheader.DataSize;
	memcpy ( facesheader.v, buffer + offset, sizeof ( VChunkHeader ) );
	facesdata = (VTriangle*)malloc ( facesheader.DataCount * sizeof ( VTriangle ) );
	offset += sizeof ( VChunkHeader );
	memcpy ( facesdata, buffer + offset, facesheader.DataCount * sizeof ( VPoint ) );

	offset += facesheader.DataCount * facesheader.DataSize;
	memcpy ( materialsheader.v, buffer + offset, sizeof ( VChunkHeader ) );
	materialsdata = (VMaterial*)malloc ( materialsheader.DataCount * sizeof ( VMaterial ) );
	offset += sizeof ( VChunkHeader );
	memcpy ( materialsdata, buffer + offset, materialsheader.DataCount * sizeof ( VMaterial ) );

	offset += materialsheader.DataCount * materialsheader.DataSize;
	memcpy ( bonesheader.v, buffer + offset, sizeof ( VChunkHeader ) );
	bonesdata = (VBone*)malloc ( bonesheader.DataCount * sizeof ( VBone ) );
	offset += sizeof ( VChunkHeader );
	memcpy ( bonesdata, buffer + offset, bonesheader.DataCount * sizeof ( VBone ) );
	FlipYZBonesRoutine ( bonesdata, bonesheader.DataCount );

	offset += bonesheader.DataCount * bonesheader.DataSize;
	memcpy ( influencesheader.v, buffer + offset, sizeof ( VChunkHeader ) );
	influencesdata = (VRawBoneInfluence*)malloc ( influencesheader.DataCount * sizeof ( VRawBoneInfluence ) );
	offset += sizeof ( VChunkHeader );
	memcpy ( influencesdata, buffer + offset, influencesheader.DataCount * sizeof ( VRawBoneInfluence ) );
//---------------------------------------------------------------------------------------------------------
	
	skminfo->VBOxyz = (GXFloat*)malloc ( wedgesheader.DataCount * sizeof ( VPoint ) );
	skminfo->totalVBOxyz = wedgesheader.DataCount;
	skminfo->VBOuv = (GXFloat*)malloc ( wedgesheader.DataCount * sizeof ( GXVec2 ) );
	skminfo->totalVBOuv = wedgesheader.DataCount;
	for ( GXUInt i = 0; i < wedgesheader.DataCount; i++ )
	{
		memcpy ( skminfo->VBOxyz + i * 3, pointsdata + wedgesdata[ i ].PointIndex, sizeof ( VPoint ) );
		memcpy ( skminfo->VBOuv + i * 2, wedgesdata[ i ].uv._v, sizeof ( GXVec2 ) );
	}
//---------------------------------------------------------------------------------------------------------

	skminfo->ElementArray = (GXUShort*)malloc ( facesheader.DataCount * 3 * sizeof ( GXUShort ) );
	skminfo->totalelements = facesheader.DataCount * 3;
	for ( GXUInt i = 0; i < facesheader.DataCount; i++ )
	{
		memcpy ( skminfo->ElementArray + i * 3, (GXUShort*)facesdata[ i ].WedgeIndex, 3 * sizeof ( GXUShort ) );
	}
//---------------------------------------------------------------------------------------------------------

	skminfo->totalBones = ( GXUShort )bonesheader.DataCount;
	skminfo->Bones = (GXLightBone*)malloc ( sizeof ( GXLightBone ) * bonesheader.DataCount );
	for ( GXUInt i = 0; i < bonesheader.DataCount; i++ )
	{
		memcpy ( skminfo->Bones[ i ].Name, bonesdata[ i ].Name, 64 );
		memcpy ( skminfo->Bones[ i ].BonePos.Orientation.v, bonesdata[ i ].BonePos.Orientation.v, sizeof ( GXQuat ) );
		memcpy ( skminfo->Bones[ i ].BonePos.Position.v, bonesdata[ i ].BonePos.Position.v, sizeof ( GXVec3 ) );
		skminfo->Bones[ i ].ParentIndex = ( bonesdata + i )->ParentIndex;
	};
//---------------------------------------------------------------------------------------------------------

	skminfo->totalVBOskin = ( GXUShort )wedgesheader.DataCount;
	skminfo->VBOskin = (GXVBOSkinStruct*)malloc ( sizeof ( GXVBOSkinStruct ) * wedgesheader.DataCount );

	#define MOVE_INDEX() \
		skminfo->VBOskin[ i ].index.w = skminfo->VBOskin [ i ].index.z; \
		skminfo->VBOskin[ i ].index.z = skminfo->VBOskin [ i ].index.y; \
		skminfo->VBOskin[ i ].index.y = skminfo->VBOskin [ i ].index.x

	#define MOVE_WEIGHT() \
		skminfo->VBOskin[ i ].weight.w = skminfo->VBOskin [ i ].weight.z; \
		skminfo->VBOskin[ i ].weight.z = skminfo->VBOskin [ i ].weight.y; \
		skminfo->VBOskin[ i ].weight.y = skminfo->VBOskin [ i ].weight.x

	#define CLEAR_INDEX() \
		skminfo->VBOskin[ i ].index.x = skminfo->VBOskin[ i ].index.y = skminfo->VBOskin[ i ].index.z = skminfo->VBOskin[ i ].index.w = 0.0f

	#define CLEAR_WEIGHT() \
		skminfo->VBOskin[ i ].weight.x = skminfo->VBOskin[ i ].weight.y = skminfo->VBOskin[ i ].weight.z = skminfo->VBOskin[ i ].weight.w = 0.0f
	
	for ( GXUInt i = 0; i < wedgesheader.DataCount; i++ )
	{
		CLEAR_INDEX ();
		CLEAR_WEIGHT ();
		for ( GXUInt j = 0; j < influencesheader.DataCount; j++ )
		{
			if ( wedgesdata[ i ].PointIndex == influencesdata[ j ].PointIndex )
			{
				MOVE_INDEX ();
				skminfo->VBOskin[ i ].index.x = GXFloat ( influencesdata[ j ].BoneIndex );
				MOVE_WEIGHT ();
				skminfo->VBOskin[ i ].weight.x = influencesdata[ j ].Weight;
			}
		}
	}

	#undef MOVE_INDEX
	#undef MOVE_WEIGHT
	#undef CLEAR_INDEX
	#undef CLEAR_WEIGHT
//---------------------------------------------------------------------------------------------------------

	free ( influencesdata );
	free ( bonesdata );
	free ( materialsdata );
	free ( facesdata );
	free ( wedgesdata );
	free ( pointsdata );
	GXSafeFree ( buffer );
}