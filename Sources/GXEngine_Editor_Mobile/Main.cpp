// version 1.2

#include <GXEngine/GXCrashReport.h>
#include <GXEngine/GXCore.h>
#include <GXEngine_Editor_Mobile/EMGame.h>


int wmain ( int /*argc*/, LPWSTR /*commandline*/ )
{
    GXCrashReport crashReport;

    GXCore& core = GXCore::GetInstance ();
    EMGame* game = new EMGame ();
    core.Start ( *game );

    delete &core;
    return 0;
}

//----------------------------------------------------------------------------------

int WINAPI wWinMain ( HINSTANCE /*hinst*/, HINSTANCE /*hprev*/, LPWSTR /*cmdLine*/, int /*mode*/ )
{
    GXCrashReport crashReport;

    GXCore& core = GXCore::GetInstance ();
    EMGame* game = new EMGame ();
    core.Start ( *game );

    delete &core;
    return 0;
}
