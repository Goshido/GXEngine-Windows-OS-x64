// version 1.19

#ifndef GX_CORE
#define GX_CORE


#include "GXGame.h"


class GXCore final
{
    private:
        static GXBool       _loopFlag;
        static GXCore*      _instance;

    public:
        static GXCore& GXCALL GetInstance ();
        ~GXCore ();

        // Note game object will be deleted implicitly by GXCore.
        GXVoid Start ( GXGame &game );
        GXVoid Exit ();

    private:
        GXCore ();
        GXVoid CheckMemoryLeak ();

        GXCore ( const GXCore &other ) = delete;
        GXCore& operator = ( const GXCore &other ) = delete;
};


#endif // GX_CORE
