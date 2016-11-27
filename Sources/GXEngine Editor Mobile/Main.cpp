//version 1.2

#include <GXEngine/GXCore.h>
#include <GXEngine_Editor_Mobile/EMMain.h>


int wmain ( int argc, LPWSTR commandline )
{
	GXCore* core = new GXCore ( &EMOnInit, &EMOnClose, L"GXEngine Editor Mobile" );
	core->Start ();
	delete core;

	return 0;
}

//----------------------------------------------------------------------------------

int WINAPI wWinMain ( HINSTANCE hinst, HINSTANCE hprev, LPWSTR cmdLine, int mode )
{
	GXCore* core = new GXCore ( &EMOnInit, &EMOnClose, L"GXEngine Editor Mobile" );
	core->Start ();
	delete core;

	return 0;
}
