//version 1.0

#ifndef GX_NATIVE_STATIC_MESH_LOADER
#define GX_NATIVE_STATIC_MESH_LOADER


#include "GXNativeStaticMeshStructs.h"


struct GXNativeStaticMeshInfo
{
	GXAABB			bounds;

	GXUInt			numVertices;
	GXUInt			numUVs;
	GXUInt			numNormals;
	GXUInt			numTBPairs;
	GXUInt			numElements;

	GXUByte*		vboData;
	GXUByte*		eboData;

	GXNativeStaticMeshInfo ();
	GXVoid Cleanup ();
};


GXVoid GXCALL GXLoadNativeStaticMesh ( const GXWChar* fileName, GXNativeStaticMeshInfo &info );
GXVoid GXCALL GXLoadNativeStaticMesh ( const GXUByte* mappedFile, GXNativeStaticMeshInfo &info );


#endif //GX_NATIVE_STATIC_MESH_LOADER
