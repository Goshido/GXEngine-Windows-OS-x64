// version 1.0

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

GXVoid GXSmartLock::AcquireExlusive ()
{
	AcquireSRWLockExclusive ( &srwLock );
}

GXVoid GXSmartLock::ReleaseExlusive ()
{
	ReleaseSRWLockExclusive ( &srwLock );
}
