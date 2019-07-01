// version 1.10

#ifndef GX_OBJ_LOADER
#define GX_OBJ_LOADER


#include "GXMath.h"


// #define DEBUG_TO_LOG

struct GXOBJPoint final
{
    GXVec3      _vertex;
    GXVec3      _normal;
    GXVec2      _uv;
};

// Function returns total vertices.
// Note points MUST be deleted via GXMemoryInspector::Free.
GXInt GXCALL GXLoadOBJ ( const GXWChar* fileName, GXOBJPoint*& points ); 


#endif // GX_OBJ_LOADER
