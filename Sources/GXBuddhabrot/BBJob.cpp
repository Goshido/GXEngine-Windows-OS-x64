#include <GXBuddhabrot/BBJob.h>
#include <GXCommon/GXMemory.h>


BBJob::BBJob ( BBTask &taskObject, const GXPreciseComplex* pointsViewport, GXUInt pointCount, GXVoid* onProgressContext, PFNBBJOBPROGRESSPROC onProgressCallback ):
    callback ( onProgressCallback ),
    context ( onProgressContext ),
    isAbort ( GX_FALSE ),
    points ( pointsViewport ),
    totalPoints ( pointCount ),
    task ( taskObject )
{
    thread = new GXThread ( &BBJob::Thread, this );
}

BBJob::~BBJob ()
{
    Abort ();
}

GXVoid BBJob::Start ()
{
    if ( !thread ) return;

    thread->Start ();
}

GXVoid BBJob::Abort ()
{
    if ( isAbort || !thread ) return;

    isAbort = GX_TRUE;
    thread->Join ();
    GXSafeDelete ( thread );
}

GXVoid BBJob::Join ()
{
    if ( !thread ) return;

    thread->Join ();
    GXSafeDelete ( thread );
}

GXUPointer GXTHREADCALL BBJob::Thread ( GXVoid* argument, GXThread& /*thread*/ )
{
    BBJob* job = static_cast<BBJob*> ( argument );

    if ( job->isAbort )
        return 0u;

    job->callback ( job->context, 0.0f );

    // TODO make Buddhabrot magic here!

    job->callback ( job->context, 1.0f );
    return 0u;
}
