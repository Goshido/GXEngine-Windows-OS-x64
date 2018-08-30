// version 1.1

#ifndef GX_BKE_STRUCTS
#define GX_BKE_STRUCTS


#include "GXTypes.h"


struct GXBakeHeader final
{
	GXUShort	us_FileNameOffset;
	GXUShort	us_CacheFileNameOffset;
};

struct GXBakeInfo final
{
	GXWChar*	fileName;
	GXWChar*	cacheFileName;

	GXBakeInfo ();
	GXVoid GXCALL Cleanup ();
};


#endif	// GX_BKE_STRUCTS
