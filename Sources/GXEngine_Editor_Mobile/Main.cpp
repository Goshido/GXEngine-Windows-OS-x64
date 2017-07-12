//version 1.2

#include <GXEngine/GXCore.h>
#include <GXCommon/GXFileSystem.h>
#include <GXEngine_Editor_Mobile/EMGame.h>


int wmain ( int argc, LPWSTR commandline )
{
	GXCore& core = GXCore::GetInstance ();
	EMGame* game = new EMGame ();
	core.Start ( *game );
	delete game;
	delete &core;

	return 0;
}

//----------------------------------------------------------------------------------

int WINAPI wWinMain ( HINSTANCE hinst, HINSTANCE hprev, LPWSTR cmdLine, int mode )
{
	GXCore& core = GXCore::GetInstance ();
	EMGame* game = new EMGame ();
	core.Start ( *game );
	delete game;
	delete &core;

	return 0;
}
