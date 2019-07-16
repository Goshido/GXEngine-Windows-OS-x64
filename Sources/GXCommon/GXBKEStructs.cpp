//version 1.0

#include <GXCommon/GXBKEStructs.h>
#include <GXCommon/GXMemory.h>


GXBakeInfo::GXBakeInfo ():
    _fileName ( nullptr ),
    _cacheFileName ( nullptr )
{
    // NOTHING
}

GXVoid GXCALL GXBakeInfo::Cleanup ()
{
    GXSafeFree ( _fileName );
    GXSafeFree ( _fileName );
}
