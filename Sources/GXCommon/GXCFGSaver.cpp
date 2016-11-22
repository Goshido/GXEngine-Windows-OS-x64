//version 1.1

#include <GXCommon/GXCFGSaver.h>
#include <GXCommon/GXFileSystem.h>


GXVoid GXCALL GXExportCFG ( const GXEngineConfiguration &config_info )
{
	GXWriteToFile ( L"../Config/GXEngine.cfg", (GXChar*)&config_info, sizeof ( GXEngineConfiguration) );
}
