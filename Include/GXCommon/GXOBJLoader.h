// version 1.8

#ifndef GX_OBJLOADER
#define GX_OBJLOADER


#include "GXMath.h"


// #define DEBUG_TO_LOG


struct GXOBJIndex final
{
    GXInt               v;
    GXInt               vn;
    GXInt               vt;
};

struct GXOBJVertex final
{
    GXVec3              position;
    GXOBJVertex*        next;
};

struct GXOBJNormals final
{
    GXVec3              normal;
    GXOBJNormals*       next;
};

struct GXOBJUV_s final
{
    GXVec2              uv;
    GXOBJUV_s*          next;
};

struct GXOBJTriangle final
{
    GXOBJIndex          dat[ 3u ];
    GXOBJTriangle*      next;
};

struct GXOBJPoint final
{
    GXVec3      vertex;
    GXVec3      normal;
    GXVec2      uv;
};

// Function returns total vertices.
GXInt GXCALL GXLoadOBJ ( const GXWChar* fileName, GXOBJPoint** points ); 


#endif // GX_OBJLOADER
