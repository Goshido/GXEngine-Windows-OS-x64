//version 1.0

#include <GXCommon/GXCFGLoader.h>
#include <GXCommon/GXFileSystem.h>


GXVoid GXCALL GXLoadCFG ( GXEngineConfiguration &out_config_info )
{
	GXVoid* buffer = nullptr;
	GXUInt size = 0;

	if ( !GXLoadFile ( L"../../Config/GXEngine.cfg", &buffer, size, GX_TRUE ) )
	{
		GXDebugBox ( L"GXLoadCFG::Error - �� ���� ��������� ����" );
		GXLogW ( L"GXLoadCFG::Error - �� ���� ��������� ���� %s", L"../../Config/GXEngine.cfg\n" );
	}

	memcpy ( &out_config_info, buffer, size );
	GXSafeFree ( buffer );
}