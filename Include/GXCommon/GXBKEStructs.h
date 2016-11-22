//version 1.0

#ifndef GX_BKE_STRUCTS
#define GX_BKE_STRUCTS


#include "GXCommon.h"


struct GXBakeHeader
{
	GXUShort	us_FileNameOffset;
	GXUShort	us_CacheFileNameOffset;
};

struct GXBakeInfo
{
	GXWChar*	fileName;
	GXWChar*	cacheFileName;

	GXBakeInfo ();
	GXVoid GXCALL Cleanup ();
};


#endif	//GX_BKE_STRUCTS