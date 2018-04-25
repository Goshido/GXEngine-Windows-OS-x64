// version 1.2

#include <GXCommon/GXCFGSaver.h>
#include <GXCommon/GXFileSystem.h>


GXVoid GXCALL GXExportCFG ( const GXEngineConfiguration &config )
{
	GXWriteToFile ( L"../Config/GXEngine.cfg", &config, sizeof ( GXEngineConfiguration) );
}
