// version 1.2

#ifndef GX_SMART_LOCK_WINDOWS
#define GX_SMART_LOCK_WINDOWS


#include <GXCommon/GXSmartLock.h>


class GXSmartLock final: public GXAbstractSmartLock
{
    private:
        SRWLOCK     _srwLock;

    public:
        GXSmartLock ();
        ~GXSmartLock () override;

        GXVoid AcquireShared () override;
        GXVoid ReleaseShared () override;

        GXVoid AcquireExclusive () override;
        GXVoid ReleaseExclusive () override;

    private:
        GXSmartLock ( const GXSmartLock &other ) = delete;
        GXSmartLock& operator = ( const GXSmartLock &other ) = delete;
};


#endif // GX_SMART_LOCK_WINDOWS
