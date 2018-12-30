#include <GXBuddhabrot/BBJob.h>
#include <GXCommon/GXMemory.h>


#define IDLE_TIMEOUT                    7u
#define SQUARE_ESCAPE_RADIUS            4.0

//---------------------------------------------------------------------------------------------------------------------

BBJob::BBJob ( BBTask &taskObject, PFNBBJOBPROGRESSPROC callback, GXVoid* callbackContext ):
    isAbort ( GX_FALSE ),
    isLoop ( GX_TRUE ),
    isRelax ( GX_TRUE ),
    escapePoints ( static_cast<GXPreciseComplex*> ( malloc ( ( taskObject.GetIterationCap () + 1u ) * sizeof ( GXPreciseComplex ) ) ) ),
    escapePointCount ( 0u ),
    progress ( 0.0f ),
    progressCallback ( callback ),
    progressCallbackContext ( callbackContext ),
    state ( eBBThreadState::Idle ),
    task ( taskObject )
{
    thread = new GXThread ( &BBJob::Thread, this );
}

BBJob::~BBJob ()
{
    Join ();
    free ( escapePoints );
}

eBBThreadState BBJob::GetState () const
{
    return state;
}

GXVoid BBJob::Init ( BBJobContext &newJobContext, GXUPointer newJobIndex )
{
    jobContext = &newJobContext;
    jobIndex = newJobIndex;
    isRelax = GX_FALSE;
    state = eBBThreadState::JobProgress;
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
}

GXVoid BBJob::Join ()
{
    if ( !thread ) return;

    isLoop = GX_FALSE;
    thread->Join ();
    GXSafeDelete ( thread );
}

GXVoid BBJob::Relax ()
{
    isRelax = GX_TRUE;
}

GXBool BBJob::ProcessPoint ( GXUShort pointIndex )
{
    GXPreciseComplex z ( 0.0f, 0.0f );
    const GXPreciseComplex c = jobContext->points[ pointIndex ];
    const GXUPointer iterationCap = task.GetIterationCap ();

    for ( escapePointCount = 0u; escapePointCount < iterationCap; ++escapePointCount )
    {
        if ( z.SquaredLength () <= SQUARE_ESCAPE_RADIUS )
        {
            z = z * z + c;
            escapePoints[ escapePointCount ] = z;
            continue;
        }

        for ( GXUInt i = 0u; i < escapePointCount; ++i )
            task.MakeHit ( escapePoints[ i ] );

        task.MarkSampleMap ( c, escapePointCount );
        return GX_TRUE;
    }

    return GX_FALSE;
}

GXUPointer GXTHREADCALL BBJob::Thread ( GXVoid* argument, GXThread &thread )
{
    BBJob* job = static_cast<BBJob*> ( argument );

    if ( job->isAbort )
        return 0u;

    while ( job->isLoop )
    {
        switch ( job->state )
        {
            case eBBThreadState::Idle:
            {
                if ( !job->isRelax ) continue;

                thread.Sleep ( IDLE_TIMEOUT );
            }
            break;

            case eBBThreadState::JobProgress:
            {
                job->progressCallback ( job->progressCallbackContext, 0.0f, job->jobIndex );

                const GXUShort pointCount = job->jobContext->pointCount;
                const GXFloat progressFactor = 1.0f / static_cast<GXFloat> ( pointCount );

                for ( GXUShort i = 0u; i < pointCount; ++i )
                {
                    if ( job->isAbort ) break;

                    job->ProcessPoint ( i );
                    job->progressCallback ( job->progressCallbackContext, i * progressFactor, job->jobIndex );
                }

                if ( job->isAbort )
                {
                    job->state = eBBThreadState::Idle;
                    break;
                }

                job->progressCallback ( job->progressCallbackContext, 1.0f, job->jobIndex );
                job->state = eBBThreadState::Idle;
            }
            break;
        }
    }

    return 0u;
}
