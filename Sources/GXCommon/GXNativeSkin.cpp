// version 1.0

#include <GXCommon/GXNativeSkin.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


GXSkinInfo::GXSkinInfo ()
{
	totalVertices = 0;
	vboData = nullptr;
}

GXVoid GXSkinInfo::Cleanup ()
{
	GXSafeFree ( vboData );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkin ( GXSkinInfo &info, const GXWChar* fileName )
{
	GXUByte* data;
	GXUBigInt fileSize;

	if ( !GXLoadFile ( fileName, (GXVoid**)&data, fileSize, GX_TRUE ) )
	{
		GXLogW ( L"GXLoadNativeSkin::Error - Can't load file %s\n", fileName );
		return;
	}

	GXNativeSkinHeader* h = (GXNativeSkinHeader*)data;

	info.totalVertices = h->totalVertices;

	GXUPointer size = info.totalVertices * ( sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
	info.vboData = (GXFloat*)malloc ( size );
	memcpy ( info.vboData, data + h->vboOffset, size );

	free ( data );
}
