//version 1.0

#ifndef GX_NATIVE_STATIC_MESH_STRUCTS
#define GX_NATIVE_STATIC_MESH_STRUCTS


#include "GXMath.h"


struct GXNativeStaticMeshHeader
{
	GXAABB		bounds;

	GXUInt		numVertices;
	GXUInt		numUVs;
	GXUInt		numNormals;
	GXUInt		numTBPairs;		//Tangent and Bitangent pairs
	GXUInt		numElements;

	GXUInt		elementOffset;
};


#endif //GX_NATIVE_STATIC_MESH_STRUCTS