//version 1.0

#include <GXCommon/GXBKEStructs.h>


GXBakeInfo::GXBakeInfo ()
{
	fileName = cacheFileName = 0;
}

GXVoid GXCALL GXBakeInfo::Cleanup ()
{
	if ( fileName ) free ( fileName );
	if ( cacheFileName ) free ( cacheFileName );
}