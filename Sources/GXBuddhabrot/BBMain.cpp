#include <GXBuddhabrot/BBGame.h>
#include <GXEngine/GXCore.h>


int wmain ( int /*argc*/, LPWSTR /*commandline*/ )
{
    GXCore& core = GXCore::GetInstance ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "BBGame" );
    BBGame* game = new BBGame ();
    core.Start ( *game );

    delete &core;
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------

int WINAPI wWinMain ( HINSTANCE /*hinst*/, HINSTANCE /*hprev*/, LPWSTR /*cmdLine*/, int /*mode*/ )
{
    GXCore& core = GXCore::GetInstance ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "BBGame" );
    BBGame* game = new BBGame ();
    core.Start ( *game );

    delete &core;
    return 0;
}
