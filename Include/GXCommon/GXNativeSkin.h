// version 1.3

#ifndef GX_NATIVE_SKIN
#define GX_NATIVE_SKIN


#include "GXMath.h"


#pragma pack ( push )
#pragma pack ( 1 )

struct GXNativeSkinHeader
{
    GXUInt          _totalVertices;
    GXUBigInt       _vboOffset;         // VBO element struct: bone index (GXVec4), weight (GXVec4).
};

#pragma pack ( pop )

struct GXSkinInfo
{
    GXUInt      _totalVertices;
    GXFloat*    _vboData;

    GXSkinInfo ();
    GXVoid Cleanup ();
};

//------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkin ( GXSkinInfo &info, const GXWChar* fileName );


#endif // GX_NATIVE_SKIN
