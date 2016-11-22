//version 1.2

#include <GXEngine/GXGlobals.h>
#include <GXEngine_Editor_Mobile/EMMain.h>


int wmain ( int argc, LPWSTR commandline )
{
	gx_Core = new GXCore ( &EMOnInit, &EMOnClose, L"GXEngine Editor Mobile" );
	gx_Core->Start ();
	delete gx_Core;

	return 0;
}

//----------------------------------------------------------------------------------

int WINAPI wWinMain ( HINSTANCE hinst, HINSTANCE hprev, LPWSTR cmdLine, int mode )
{
	gx_Core = new GXCore ( &EMOnInit, &EMOnClose, L"GXEngine Editor Mobile" );
	gx_Core->Start ();
	delete gx_Core;

	return 0;
}
