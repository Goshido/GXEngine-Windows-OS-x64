//version 1.1

#include <GXCommon/GXCFGLoader.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


GXVoid GXCALL GXLoadCFG ( GXEngineConfiguration &out_config_info )
{
	GXVoid* buffer = nullptr;
	GXUInt size = 0;

	if ( !GXLoadFile ( L"../../Config/GXEngine.cfg", &buffer, size, GX_TRUE ) )
	{
		GXDebugBox ( L"GXLoadCFG::Error - Cant't load file" );
		GXLogW ( L"GXLoadCFG::Error - Can't load file %s\n", L"../../Config/GXEngine.cfg\n" );
	}

	memcpy ( &out_config_info, buffer, size );
	GXSafeFree ( buffer );
}