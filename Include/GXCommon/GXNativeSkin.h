// version 1.2

#ifndef GX_NATIVE_SKIN
#define GX_NATIVE_SKIN


#include "GXMath.h"


#pragma pack ( push )
#pragma pack ( 1 )

struct GXNativeSkinHeader
{
    GXUInt          totalVertices;
    GXUBigInt       vboOffset;      // VBO element struct: bone index (GXVec4), weight (GXVec4).
};

#pragma pack ( pop )

struct GXSkinInfo
{
    GXUInt      totalVertices;
    GXFloat*    vboData;

    GXSkinInfo ();
    GXVoid Cleanup ();
};

//------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkin ( GXSkinInfo &info, const GXWChar* fileName );


#endif // GX_NATIVE_SKIN
