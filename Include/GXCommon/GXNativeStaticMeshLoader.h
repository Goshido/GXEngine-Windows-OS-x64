// version 1.4

#ifndef GX_NATIVE_STATIC_MESH_LOADER
#define GX_NATIVE_STATIC_MESH_LOADER


#include "GXNativeStaticMeshStructs.h"


struct GXNativeStaticMeshInfo final
{
    GXAABB      _bounds;

    GXUInt      _numVertices;
    GXUInt      _numUVs;
    GXUInt      _numNormals;
    GXUInt      _numTBPairs;
    GXUInt      _numElements;

    GXUByte*    _vboData;
    GXUByte*    _eboData;

    GXNativeStaticMeshInfo ();
    GXVoid Cleanup ();
};


GXVoid GXCALL GXLoadNativeStaticMesh ( const GXWChar* fileName, GXNativeStaticMeshInfo &info );


#endif // GX_NATIVE_STATIC_MESH_LOADER
