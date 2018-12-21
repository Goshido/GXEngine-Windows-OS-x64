#include <GXBuddhabrot/BBJob.h>
#include <GXCommon/GXMemory.h>


BBJob::BBJob ( BBTask &taskObject, const GXPreciseComplex* pointsViewport, GXUShort pointCount, GXVoid* onProgressContext, PFNBBJOBPROGRESSPROC onProgressCallback ):
    callback ( onProgressCallback ),
    context ( onProgressContext ),
    isAbort ( GX_FALSE ),
    points ( pointsViewport ),
    totalPoints ( pointCount ),
    progress ( 0.0f ),
    task ( taskObject )
{
    thread = new GXThread ( &BBJob::Thread, this );
}

BBJob::~BBJob ()
{
    Abort ();
}

GXFloat BBJob::GetProgress () const
{
    return progress;
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

    job->callback ( job->context );

    // TODO make Buddhabrot magic here!

    job->progress = 1.0f;
    job->callback ( job->context );

    return 0u;
}
