// version 1.4

#include <GXCommon/Windows/GXMutex.h>


GXMutex::GXMutex ()
{
    _mutex = CreateMutexW ( nullptr, FALSE, nullptr );
}

GXMutex::~GXMutex ()
{
    CloseHandle ( _mutex );
}

GXVoid GXMutex::Lock ()
{
    WaitForSingleObject ( _mutex, INFINITE );
}

GXVoid GXMutex::Release ()
{
    ReleaseMutex ( _mutex );
}
