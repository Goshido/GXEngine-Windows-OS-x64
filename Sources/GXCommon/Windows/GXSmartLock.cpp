// version 1.2

#include <GXCommon/Windows/GXSmartLock.h>


GXSmartLock::GXSmartLock ():
    _srwLock ( SRWLOCK_INIT )
{
    // NOTHING
}

GXSmartLock::~GXSmartLock ()
{
    // NOTHING
}

GXVoid GXSmartLock::AcquireShared ()
{
    AcquireSRWLockShared ( &_srwLock );
}

GXVoid GXSmartLock::ReleaseShared ()
{
    ReleaseSRWLockShared ( &_srwLock );
}

GXVoid GXSmartLock::AcquireExclusive ()
{
    AcquireSRWLockExclusive ( &_srwLock );
}

GXVoid GXSmartLock::ReleaseExclusive ()
{
    ReleaseSRWLockExclusive ( &_srwLock );
}
