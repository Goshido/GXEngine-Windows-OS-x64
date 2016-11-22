//version 1.2

#include <GXEngine/GXPSALoader.h>
#include <GXCommon/GXFileSystem.h>


GXVoid GXCALL FlipYZBonesRoutine ( VQuatAnimKey* rawkeys, GXUInt NumRawKeys )
{
	for ( GXUInt i = 0; i < NumRawKeys; i++ )
	{
		GXFloat Y = rawkeys [ i ].Position.y;
		rawkeys [ i ].Position.y = rawkeys [ i ].Position.z;
		rawkeys [ i ].Position.z = Y;
		Y = rawkeys [ i ].Orientation.y;
		rawkeys [ i ].Orientation.y = rawkeys [ i ].Orientation.z;
		rawkeys [ i ].Orientation.z = Y;
		rawkeys [ i ].Orientation.w = -rawkeys [ i ].Orientation.w;
	}
}

GXVoid GXCALL GXLoadPSA ( const GXWChar* file_name, GXSkeletalMeshAnimationInfo* skmaniminfo )
{
	GXChar*	buffer;
	GXUInt	size;
	GXUInt	offset;

	VChunkHeader bonesheader;
	VChunkHeader animationsheader;
	VChunkHeader rawkeysheader;

	FNamedBoneBinary* bonesdata;
	AnimInfoBinary* animationsdata;
	VQuatAnimKey* rawkeysdata;
//-----------------------------------------------------------------------------------------

	if ( !GXLoadFile ( file_name, (GXVoid**)&buffer, size, GX_TRUE ) )
		GXDebugBox ( L"GXLoadPSA::Error - Не могу загрузить файл" );

	offset = sizeof ( VChunkHeader );
	memcpy ( bonesheader.v, buffer + offset, sizeof ( VChunkHeader ) );
	bonesdata = (FNamedBoneBinary*)malloc ( bonesheader.DataCount * sizeof ( FNamedBoneBinary ) );
	offset += sizeof ( VChunkHeader );
	memcpy ( bonesdata, buffer + offset, bonesheader.DataCount * sizeof ( FNamedBoneBinary ) );

	offset += bonesheader.DataCount * bonesheader.DataSize;
	memcpy ( animationsheader.v, buffer + offset, sizeof ( VChunkHeader ) );
	animationsdata = (AnimInfoBinary*)malloc ( animationsheader.DataCount * sizeof ( AnimInfoBinary ) );
	offset += sizeof ( VChunkHeader );
	memcpy ( animationsdata, buffer + offset, animationsheader.DataCount * sizeof ( AnimInfoBinary ) );

	offset += animationsheader.DataCount * animationsheader.DataSize;
	memcpy ( rawkeysheader.v, buffer + offset, sizeof ( VChunkHeader ) );
	rawkeysdata = (VQuatAnimKey*)malloc ( rawkeysheader.DataCount * sizeof ( VQuatAnimKey ) );
	offset += sizeof ( VChunkHeader );
	memcpy ( rawkeysdata, buffer + offset, rawkeysheader.DataCount * sizeof ( VQuatAnimKey ) );
	FlipYZBonesRoutine ( rawkeysdata, rawkeysheader.DataCount );
//-----------------------------------------------------------------------------------------
	
	skmaniminfo->totalbones = bonesheader.DataCount;
	skmaniminfo->Bones = (GXBoneNames*)malloc ( sizeof ( GXBoneNames ) * bonesheader.DataCount );
	for ( GXUInt i = 0; i < bonesheader.DataCount; i++ )
	{
		memcpy ( skmaniminfo->Bones[ i ].Name, bonesdata [ i ].Name, 64 );
	}
//-----------------------------------------------------------------------------------------

	skmaniminfo->totalanimations = animationsheader.DataCount;
	skmaniminfo->Animations = (GXAnimSeq*)malloc ( sizeof ( GXAnimSeq ) * animationsheader.DataCount );
	for ( GXUInt i = 0; i < animationsheader.DataCount; i++ )
	{
		memcpy ( skmaniminfo->Animations[ i ].Name, animationsdata [ i ].Name, 64 );
		skmaniminfo->Animations[ i ].FPS = animationsdata [ i ].AnimRate;
		skmaniminfo->Animations[ i ].FirstRawFrame = animationsdata [ i ].FirstRawFrame;
		skmaniminfo->Animations[ i ].NumRawFrames = animationsdata [ i ].NumRawFrames;
	}
//-----------------------------------------------------------------------------------------

	skmaniminfo->totalrawkeys = rawkeysheader.DataCount;
	skmaniminfo->RawKeys = (VQuatAnimKey*)malloc ( sizeof ( VQuatAnimKey ) * rawkeysheader.DataCount );
	memcpy ( skmaniminfo->RawKeys, rawkeysdata, rawkeysheader.DataCount * sizeof ( VQuatAnimKey ) );
//-----------------------------------------------------------------------------------------

	free ( bonesdata );
	free ( animationsdata );
	free ( rawkeysdata );
	GXSafeFree ( buffer );
}