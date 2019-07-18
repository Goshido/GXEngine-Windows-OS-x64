// version 1.5

#ifndef GX_NATIVE_STATIC_MESH_SAVER
#define GX_NATIVE_STATIC_MESH_SAVER


#include "GXMath.h"


struct GXNativeStaticMeshDesc final
{
    GXUInt      _numVertices;
    GXUInt      _numUVs;
    GXUInt      _numNormals;
    GXUInt      _numTBPairs;    // Tangent and Bitangent pairs
    GXUInt      _numElements;

    GXVec3*     _vertices;
    GXVec2*     _uvs;
    GXVec3*     _normals;
    GXVec3*     _tangents;
    GXVec3*     _bitangents;

    GXUInt*     _elements;
};


GXVoid GXCALL GXExportNativeStaticMesh ( const GXWChar* fileName, const GXNativeStaticMeshDesc &descriptor );


#endif // GX_NATIVE_STATIC_MESH_SAVER
