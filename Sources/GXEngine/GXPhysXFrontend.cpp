//version 1.4

#include <GXEngine/GXPhysXFrontend.h>


#define GX_INVALID_MODULE (HMODULE)0

static PFNGXPHYSXCREATE GXCreatePhysXInstance		= nullptr;
static PFNGXPHYSXDESTROY GXDestroyPhysXInstance		= nullptr;

HMODULE gx_GXEngineDLLModuleHandle = GX_INVALID_MODULE;

static GXPhysXAdapter* gx_PhysX = nullptr;


GXPhysXAdapter* GXCALL GXGetPhysXInstance ()
{
	if ( gx_PhysX ) return gx_PhysX;

	gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXPhysXInit::Error - Не удалось загрузить GXEngine.dll\n" );
		return nullptr;
	}

	GXCreatePhysXInstance = (PFNGXPHYSXCREATE)GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXPhysXCreate" );
	if ( !GXCreatePhysXInstance )
	{
		GXLogW ( L"GXPhysXInit::Error - Не удалось найти функцию GXPhysXCreate\n" );
		return nullptr;
	}

	GXDestroyPhysXInstance = (PFNGXPHYSXDESTROY)GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXPhysXDestroy" );
	if ( !GXDestroyPhysXInstance )
	{
		GXLogW ( L"GXPhysXInit::Error - Не удалось найти функцию GXPhysXDestroy\n" );
		return nullptr;
	}

	gx_PhysX = GXCreatePhysXInstance ();

	return gx_PhysX;
}

GXBool GXCALL GXDestroyPhysX ()
{
	GXDestroyPhysXInstance ( gx_PhysX );
	gx_PhysX = nullptr;

	if ( gx_GXEngineDLLModuleHandle == GX_INVALID_MODULE )
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
