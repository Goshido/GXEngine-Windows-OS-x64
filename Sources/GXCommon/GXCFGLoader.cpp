// version 1.4

#include <GXCommon/GXCFGLoader.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


#define CONFIG_FILE_PATH    "../../Config/GXEngine.cfg"

//---------------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadCFG ( GXEngineConfiguration &out )
{
    GXFile file ( CONFIG_FILE_PATH );

    GXUByte* content = nullptr;
    GXUBigInt size = 0u;

    if ( !file.LoadContent ( content, size, eGXFileContentOwner::GXFile, GX_TRUE ) ) return;

    memcpy ( &out, content, size );
}
