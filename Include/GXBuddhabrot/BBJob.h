#ifndef BB_JOB
#define BB_JOB


#include "BBTask.h"
#include <GXCommon/GXThread.h>


// Note progress could be [0.0f, 1.0f].
typedef GXVoid ( GXCALL* PFNBBJOBPROGRESSPROC ) ( GXVoid* context );

class BBJob final
{
    private:
        PFNBBJOBPROGRESSPROC        callback;
        GXVoid*                     context;

        GXBool                      isAbort;

        const GXPreciseComplex*     points;
        GXUShort                    totalPoints;

        GXFloat                     progress;
        BBTask&                     task;

        GXThread*                   thread;

    public:
        // Note pointsViewport MUST be valid memory until this object live.
        explicit BBJob ( BBTask &task, const GXPreciseComplex* pointsViewport, GXUShort pointCount, GXVoid* onProgressContext, PFNBBJOBPROGRESSPROC onProgressCallback );

        ~BBJob ();

        GXFloat GetProgress () const;

        // Note job restarting is not supported. Create new one instead.
        GXVoid Start ();
        GXVoid Abort ();
        GXVoid Join ();

    private:
        static GXUPointer GXTHREADCALL Thread ( GXVoid* argument, GXThread &thread );

        BBJob () = delete;
        BBJob ( const BBJob &other ) = delete;
        BBJob& operator = ( const BBJob &other ) = delete;
};


#endif // BB_JOB
