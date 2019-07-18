#include <GXCommon/Posix/GXMutex.h>
#include <GXCommon/GXLogger.h>


GXMutex::GXMutex ()
{
    pthread_mutexattr_t param;
    pthread_mutexattr_init ( &param );
    pthread_mutexattr_settype ( &param, PTHREAD_MUTEX_NORMAL );

    if ( pthread_mutex_init ( &mutex, &param ) != 0 )
        GXLogA ( "GXMutex::GXMutex::Error - pthread_mutex_init failed\n" );

    pthread_mutexattr_destroy ( &param );
}

GXMutex::~GXMutex ()
{
    if ( pthread_mutex_destroy ( &mutex ) != 0 )
        GXLogA ( "GXMutex::~GXMutex::Error - pthread_mutex_destroy failed\n" );
}

GXVoid GXMutex::Lock ()
{
    if ( pthread_mutex_lock ( &mutex ) != 0 )
        GXLogA ( "GXMutex::Lock::Error - pthread_mutex_lock failed\n" );
}

GXVoid GXMutex::Release ()
{
    if ( pthread_mutex_unlock ( &mutex ) != 0 )
        GXLogA ( "GXMutex::Release::Error - pthread_mutex_unlock failed\n" );
}
