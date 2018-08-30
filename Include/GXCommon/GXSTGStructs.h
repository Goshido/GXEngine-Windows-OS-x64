// version 1.1

#ifndef GX_STG_STRUCTS
#define GX_STG_STRUCTS


#include "GXTypes.h"


/*
�������
�������
������� (�����������)
UV		(�����������)
*/

struct GXStaticGeometryHeader final
{
	GXUInt		numVertices;
	GXBool		isUVs;
	GXBool		isNormals;
	GXBool		isTangents;
};


#endif // GX_STG_STRUCTS
