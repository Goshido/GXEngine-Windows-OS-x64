// version 1.1

#ifndef GX_SMART_LOCK_WINDOWS
#define GX_SMART_LOCK_WINDOWS


#include <GXCommon/GXSmartLock.h>


class GXSmartLock final: public GXAbstractSmartLock
{
    private:
        SRWLOCK     srwLock;

    public:
        GXSmartLock ();
        ~GXSmartLock () override;

        GXVoid AcquireShared () override;
        GXVoid ReleaseShared () override;

        GXVoid AcquireExlusive () override;
        GXVoid ReleaseExlusive () override;

    private:
        GXSmartLock ( const GXSmartLock &other ) = delete;
        GXSmartLock& operator = ( const GXSmartLock &other ) = delete;
};


#endif // GX_SMART_LOCK_WINDOWS
