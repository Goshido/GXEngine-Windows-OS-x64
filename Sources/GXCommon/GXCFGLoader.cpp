// version 1.2

#include <GXCommon/GXCFGLoader.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


GXVoid GXCALL GXLoadCFG ( GXEngineConfiguration &out )
{
	GXVoid* buffer = nullptr;
	GXUBigInt size = 0;

	if ( !GXLoadFile ( L"../../Config/GXEngine.cfg", &buffer, size, GX_TRUE ) )
	{
		GXDebugBox ( L"GXLoadCFG::Error - Cant't load file" );
		GXLogW ( L"GXLoadCFG::Error - Can't load file %s\n", L"../../Config/GXEngine.cfg\n" );
	}

	memcpy ( &out, buffer, size );
	GXSafeFree ( buffer );
}
