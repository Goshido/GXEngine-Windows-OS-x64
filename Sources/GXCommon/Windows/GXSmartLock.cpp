// version 1.1

#include <GXCommon/Windows/GXSmartLock.h>


GXSmartLock::GXSmartLock ():
    srwLock ( SRWLOCK_INIT )
{
    // NOTHING
}

GXSmartLock::~GXSmartLock ()
{
    // NOTHING
}

GXVoid GXSmartLock::AcquireShared ()
{
    AcquireSRWLockShared ( &srwLock );
}

GXVoid GXSmartLock::ReleaseShared ()
{
    ReleaseSRWLockShared ( &srwLock );
}

GXVoid GXSmartLock::AcquireExclusive ()
{
    AcquireSRWLockExclusive ( &srwLock );
}

GXVoid GXSmartLock::ReleaseExclusive ()
{
    ReleaseSRWLockExclusive ( &srwLock );
}
