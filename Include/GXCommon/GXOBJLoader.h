// version 1.9

#ifndef GX_OBJ_LOADER
#define GX_OBJ_LOADER


#include "GXMath.h"


// #define DEBUG_TO_LOG

struct GXOBJPoint final
{
    GXVec3      vertex;
    GXVec3      normal;
    GXVec2      uv;
};

// Function returns total vertices.
// Note points MUST be deleted via GXMemoryInspector::Free.
GXInt GXCALL GXLoadOBJ ( const GXWChar* fileName, GXOBJPoint*& points ); 


#endif // GX_OBJ_LOADER
