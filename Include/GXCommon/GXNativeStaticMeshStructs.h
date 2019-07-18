// version 1.2

#ifndef GX_NATIVE_STATIC_MESH_STRUCTS
#define GX_NATIVE_STATIC_MESH_STRUCTS


#include "GXMath.h"


struct GXNativeStaticMeshHeader final
{
    GXAABB      _bounds;

    GXUInt      _numVertices;
    GXUInt      _numUVs;
    GXUInt      _numNormals;
    GXUInt      _numTBPairs;    // Tangent and Bitangent pairs
    GXUInt      _numElements;

    GXUInt      _elementOffset;
};


#endif // GX_NATIVE_STATIC_MESH_STRUCTS
