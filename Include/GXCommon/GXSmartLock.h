// version 1.1

#ifndef GX_SMART_LOCK
#define GX_SMART_LOCK


#include "GXTypes.h"


// Note recursive locking will cause undefined behaviour.
// Note this lock type is valid for single process environment only.

class GXAbstractSmartLock
{
public:
    GXAbstractSmartLock ();
    virtual ~GXAbstractSmartLock ();

    virtual GXVoid AcquireShared () = 0;
    virtual GXVoid ReleaseShared () = 0;

    virtual GXVoid AcquireExclusive () = 0;
    virtual GXVoid ReleaseExclusive () = 0;

private:
    GXAbstractSmartLock ( const GXAbstractSmartLock &other ) = delete;
    GXAbstractSmartLock& operator = ( const GXAbstractSmartLock &other ) = delete;
};


#ifdef __GNUC__

    // TODO implement this
    #include "Posix/GXSmartLock.h"

#else

    #include "Windows/GXSmartLock.h"

#endif // __GNU__


#endif // GX_SMART_LOCK
