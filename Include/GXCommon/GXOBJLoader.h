//version 1.2

#ifndef GX_OBJLOADER
#define GX_OBJLOADER


#include "GXMath.h"


//#define DEBUG_TO_LOG


struct GXOBJIndex
{
	GXInt v;
	GXInt vn;
	GXInt vt;
};

struct GXOBJVertex
{
	GXVec3 position;
	GXOBJVertex *next;
};

struct GXOBJNormals
{
	GXVec3 normal;
	GXOBJNormals *next;
};

struct GXOBJUV_s
{
	GXVec2 uv;
	GXOBJUV_s *next;
};

struct GXOBJTriangle
{
	GXOBJIndex dat[ 3 ];
	GXOBJTriangle *next;
};

struct GXOBJPoint
{
	GXVec3	vertex;
	GXVec3	normal;
	GXVec2	uv;
};


GXInt GXCALL GXLoadOBJ ( const GXWChar* fileName, GXOBJPoint** points ); //retruns total vertices


#endif //GX_OBJLOADER
