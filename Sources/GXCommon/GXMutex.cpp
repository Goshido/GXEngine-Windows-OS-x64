//version 1.0

#include <GXCommon/GXMutex.h>


GXMutex::GXMutex ()
{
	mutex = CreateMutexW ( 0, FALSE, 0 );
}

GXMutex::~GXMutex ()
{
	CloseHandle ( mutex );
}

GXVoid GXMutex::Lock ()
{
	WaitForSingleObject ( mutex, INFINITE );
}

GXVoid GXMutex::Release ()
{
	ReleaseMutex ( mutex );
}