#include <GXBuddhabrot/BBGame.h>
#include <GXEngine/GXCore.h>


int wmain ( int /*argc*/, LPWSTR /*commandline*/ )
{
    GXCore& core = GXCore::GetInstance ();
    BBGame* game = new BBGame ();
    core.Start ( *game );
    delete game;
    delete &core;

    return 0;
}

//---------------------------------------------------------------------------------------------------------------------

int WINAPI wWinMain ( HINSTANCE /*hinst*/, HINSTANCE /*hprev*/, LPWSTR /*cmdLine*/, int /*mode*/ )
{
    GXCore& core = GXCore::GetInstance ();
    BBGame* game = new BBGame ();
    core.Start ( *game );
    delete game;
    delete &core;

    return 0;
}
