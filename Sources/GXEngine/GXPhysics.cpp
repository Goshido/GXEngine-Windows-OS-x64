//version 1.3

#include <GXEngine/GXPhysics.h>


PFNGXPHYSICSCREATE	GXPhysicsCreate				= nullptr;
PFNGXPHYSICSDESTROY	GXPhysicsDestroy			= nullptr;

HMODULE				gx_GXEngineDLLModuleHandle	= 0;


GXBool GXCALL GXPhysXInit ()
{
	gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXPhysXUnit::Error - Не удалось загрузить GXEngine.dll\n" );
		return GX_FALSE;
	}

	GXPhysicsCreate = (PFNGXPHYSICSCREATE)GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXPhysicsCreate" );
	if ( !GXPhysicsCreate )
	{
		GXLogW ( L"GXPhysXUnit::Error - Не удалось найти функцию GXPhysicsCreate\n" );
		return GX_FALSE;
	}

	GXPhysicsDestroy = (PFNGXPHYSICSDESTROY)GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXPhysicsDestroy" );
	if ( !GXPhysicsDestroy )
	{
		GXLogW ( L"GXPhysXUnit::Error - Не удалось найти функцию GXPhysicsDestroy\n" );
		return GX_FALSE;
	}

	return GX_TRUE;
}

GXBool GXCALL GXPhysXDestroy ()
{
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXPhysXDestroy::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll\n" );
		return GX_FALSE;
	}

	if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
	{
		GXLogW ( L"GXPhysXDestroy::Error - Не удалось выгрузить библиотеку GXEngine.dll\n" );
		return GX_FALSE;
	}

	return GX_TRUE;
}
