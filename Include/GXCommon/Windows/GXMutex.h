﻿// version 1.4

#ifndef GX_MUTEX_WINDOWS
#define GX_MUTEX_WINDOWS


#include <GXCommon/GXMutex.h>


class GXMutex final : public GXAbstractMutex
{
    private:
        HANDLE      _mutex;

    public:
        GXMutex ();
        ~GXMutex () override;

        GXVoid Lock () override;
        GXVoid Release () override;

    private:
        GXMutex ( const GXMutex &other ) = delete;
        GXMutex& operator = ( const GXMutex &other ) = delete;
};


#endif // GX_MUTEX_WINDOWS
