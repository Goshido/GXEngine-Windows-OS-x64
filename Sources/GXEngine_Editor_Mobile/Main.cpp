// version 1.2

#include <GXEngine/GXCore.h>
#include <GXEngine_Editor_Mobile/EMGame.h>


int wmain ( int /*argc*/, LPWSTR /*commandline*/ )
{
    constexpr GXVec2 pointOf77Degrees ( 0.22495f, 0.97437f );

    GXCore& core = GXCore::GetInstance ();
    EMGame* game = new EMGame ();
    core.Start ( *game );

    delete &core;
    return 0;
}

//----------------------------------------------------------------------------------

int WINAPI wWinMain ( HINSTANCE /*hinst*/, HINSTANCE /*hprev*/, LPWSTR /*cmdLine*/, int /*mode*/ )
{
    GXCore& core = GXCore::GetInstance ();
    EMGame* game = new EMGame ();
    core.Start ( *game );

    delete &core;
    return 0;
}
