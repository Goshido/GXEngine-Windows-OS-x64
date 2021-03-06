﻿// version 1.6

#include <GXEngine/GXPhysXFrontend.h>


#define GX_INVALID_MODULE nullptr

static GXPhysXCreateFunc        GXCreatePhysXInstance = nullptr;
static GXPhysXDestroyFunc       GXDestroyPhysXInstance = nullptr;

HMODULE gx_GXEngineDLLModuleHandle = GX_INVALID_MODULE;

static GXPhysXAdapter* gx_PhysX = nullptr;

GXPhysXAdapter* GXCALL GXGetPhysXInstance ()
{
    if ( gx_PhysX ) return gx_PhysX;

    gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );

    if ( !gx_GXEngineDLLModuleHandle )
    {
        GXLogA ( "GXPhysXInit::Error - Не удалось загрузить GXEngine.dll\n" );
        return nullptr;
    }

    GXCreatePhysXInstance = reinterpret_cast<GXPhysXCreateFunc> ( reinterpret_cast<GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXPhysXCreate" ) ) );

    if ( !GXCreatePhysXInstance )
    {
        GXLogA ( "GXPhysXInit::Error - Не удалось найти функцию GXPhysXCreate\n" );
        return nullptr;
    }

    GXDestroyPhysXInstance = reinterpret_cast<GXPhysXDestroyFunc> ( reinterpret_cast<GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXPhysXDestroy" ) ) );

    if ( !GXDestroyPhysXInstance )
    {
        GXLogA ( "GXPhysXInit::Error - Не удалось найти функцию GXPhysXDestroy\n" );
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
        GXLogA ( "GXPhysXDestroy::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll\n" );
        return GX_FALSE;
    }

    if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
    {
        GXLogA ( "GXPhysXDestroy::Error - Не удалось выгрузить библиотеку GXEngine.dll\n" );
        return GX_FALSE;
    }

    return GX_TRUE;
}
