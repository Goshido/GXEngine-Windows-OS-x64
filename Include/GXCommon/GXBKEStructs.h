// version 1.2

#ifndef GX_BKE_STRUCTS
#define GX_BKE_STRUCTS


#include "GXTypes.h"


struct GXBakeHeader final
{
    GXUShort    _fileNameOffset;
    GXUShort    _cacheFileNameOffset;
};

struct GXBakeInfo final
{
    GXWChar*    _fileName;
    GXWChar*    _cacheFileName;

    GXBakeInfo ();
    GXVoid GXCALL Cleanup ();
};


#endif	// GX_BKE_STRUCTS
