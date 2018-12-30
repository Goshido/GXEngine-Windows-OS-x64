#ifndef BB_JOB
#define BB_JOB


#include "BBScheduler.h"


// Note progress could be [0.0f, 1.0f].
typedef GXVoid ( GXCALL* PFNBBJOBPROGRESSPROC ) ( GXVoid* context, GXFloat progress, GXUPointer jobIndex );

class BBJob final
{
    private:
        GXBool                      isAbort;
        GXBool                      isLoop;
        GXBool                      isRelax;

        GXPreciseComplex*           escapePoints;
        GXUInt                      escapePointCount;

        GXFloat                     progress;
        PFNBBJOBPROGRESSPROC        progressCallback;
        GXVoid*                     progressCallbackContext;

        eBBThreadState              state;
        BBTask&                     task;

        BBJobContext*               jobContext;
        GXUPointer                  jobIndex;
        GXThread*                   thread;

    public:
        explicit BBJob ( BBTask &taskObject, PFNBBJOBPROGRESSPROC callback, GXVoid* callbackContext );
        ~BBJob ();

        eBBThreadState GetState () const;
        GXVoid Init ( BBJobContext &newJobContext, GXUPointer newJobIndex );

        // Note job restarting is not supported. Create new one instead.
        GXVoid Start ();
        GXVoid Abort ();
        GXVoid Join ();

        GXVoid Relax ();

    private:
        // Method returns GX_TRUE if point chain was appended to hit map.
        GXBool ProcessPoint ( GXUShort pointIndex );

        static GXUPointer GXTHREADCALL Thread ( GXVoid* argument, GXThread &thread );

        BBJob () = delete;
        BBJob ( const BBJob &other ) = delete;
        BBJob& operator = ( const BBJob &other ) = delete;
};


#endif // BB_JOB
