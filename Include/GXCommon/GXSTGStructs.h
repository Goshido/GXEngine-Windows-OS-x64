// version 1.2

#ifndef GX_STG_STRUCTS
#define GX_STG_STRUCTS


#include "GXTypes.h"


/*
    Вершина
    Нормаль
    Тангент     (опционально)
    UV          (опционально)
*/

struct GXStaticGeometryHeader final
{
    GXUInt      _numVertices;
    GXBool      _isUVs;
    GXBool      _isNormals;
    GXBool      _isTangents;
};


#endif // GX_STG_STRUCTS
