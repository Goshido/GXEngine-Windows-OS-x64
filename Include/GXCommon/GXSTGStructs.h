//version 1.0

#ifndef GX_STG_STRUCTS
#define GX_STG_STRUCTS


#include "GXTypes.h"


/*
�������
�������
������� (�����������)
UV		(�����������)
*/

struct GXStaticGeometryHeader
{
	GXUInt		numVertices;
	GXBool		isUVs;
	GXBool		isNormals;
	GXBool		isTangents;
};


#endif //GX_STG_STRUCTS
