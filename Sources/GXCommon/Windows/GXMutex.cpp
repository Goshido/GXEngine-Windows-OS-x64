// version 1.3

#include <GXCommon/Windows/GXMutex.h>


GXMutex::GXMutex ()
{
    mutex = CreateMutexW ( nullptr, FALSE, nullptr );
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
