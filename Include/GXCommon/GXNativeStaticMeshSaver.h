//version 1.1

#ifndef GX_NATIVE_STATIC_MESH_SAVER
#define GX_NATIVE_STATIC_MESH_SAVER


#include "GXMath.h"


struct GXNativeStaticMeshDesc
{
	GXUInt		numVertices;
	GXUInt		numUVs;
	GXUInt		numNormals;
	GXUInt		numTBPairs;		//Tangent and Bitangent pairs
	GXUInt		numElements;

	GXVec3*		vertices;
	GXVec2*		uvs;
	GXVec3*		normals;
	GXVec3*		tangents;
	GXVec3*		bitangents;

	GXUInt*		elements;
};


GXVoid GXCALL GXExportNativeStaticMesh ( const GXWChar* fileName, const GXNativeStaticMeshDesc &descriptor );


#endif //GX_NATIVE_STATIC_MESH_SAVER
