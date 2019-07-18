// version 1.3

#include <GXCommon/GXCFGSaver.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>


GXVoid GXCALL GXExportCFG ( const GXEngineConfiguration &config )
{
	GXWriteToFile ( GXString ( "../Config/GXEngine.cfg" ), &config, sizeof ( GXEngineConfiguration) );
}
