#include <GXBuddhabrot/BBScheduler.h>
#include <GXCommon/GXMemory.h>


#define DEFAULT_POINT_SIZE              1.0e-2
#define DEFAULT_SAMPLES_PER_POINT       777u

#define IDLE_TIMEOUT                    7u

//---------------------------------------------------------------------------------------------------------------------

BBProgressInfo::BBProgressInfo ():
    totalProgress ( 0.0f ),
    jobProgress ( nullptr ),
    jobs ( 0u )
{
    // NOTHING
}

BBProgressInfo::~BBProgressInfo ()
{
    if ( jobs == 0u ) return;

    delete jobProgress;
}

GXVoid BBProgressInfo::Init ( GXUPointer asyncJobs )
{
    GXUPointer size = asyncJobs * sizeof ( GXFloat );

    if ( asyncJobs != jobs )
    {
        jobs = asyncJobs;
        free ( jobProgress );
        jobProgress = static_cast<GXFloat*> ( malloc ( size ) );
    }

    memset ( jobProgress, 0, size );
    totalProgress = 0.0f;
}

//---------------------------------------------------------------------------------------------------------------------

BBScheduler::BBScheduler ():
    isAbort ( GX_FALSE ),
    isLoop ( GX_TRUE ),
    state ( eBBThreadState::Idle ),
    distributionPattern ( nullptr ),
    task ( nullptr ),
    pointSize ( DEFAULT_POINT_SIZE ),
    samplesPerPoint ( DEFAULT_SAMPLES_PER_POINT ),
    jobContexts ( nullptr )
{
    scheduerThread = new GXThread ( &BBScheduler::SchedulerThread, this );
}

BBScheduler::~BBScheduler ()
{
    Join ();
    delete scheduerThread;
}

GXVoid BBScheduler::Start ( BBTask &newTask, GXUPointer asyncJobs, GXUShort newSamplesPerPoint, GXDouble newPointSize )
{
    if ( task )
        Abort ();

    task = &newTask;
    pointSize = newPointSize;
    samplesPerPoint = newSamplesPerPoint;
    targetJobCount = asyncJobs;

    isAbort = GX_FALSE;
    state = eBBThreadState::JobProgress;
}

GXVoid BBScheduler::Abort ()
{
    if ( isAbort ) return;

    isAbort = GX_TRUE;

    while ( state != eBBThreadState::Idle )
    {
        // TODO sleep
    }
}

GXVoid BBScheduler::Join ()
{
    if ( !isLoop ) return;

    isLoop = GX_FALSE;
    scheduerThread->Join ();

    for ( GXUPointer i = 0u; i < progressInfo.jobs; ++i )
    {
        BBJobContext& context = jobContexts[ i ];

        context.isLoop = GX_FALSE;
        context.thread->Join ();

        delete context.thread;
        free ( context.points );
    }
}

GXVoid GXCALL BBScheduler::DoAbort ()
{
    for ( GXUPointer i = 0u; i < progressInfo.jobs; ++i )
    {
        BBJobContext& context = jobContexts[ i ];
        context.isAbort = GX_TRUE;

        while ( context.state != eBBThreadState::Idle )
        {
            scheduerThread->Sleep ( IDLE_TIMEOUT );
        }
    }

    state = eBBThreadState::Idle;
}

GXVoid GXCALL BBScheduler::InitJobs ()
{
    UpdateDistributionPattern ();

    if ( progressInfo.jobs == targetJobCount )
    {
        progressInfo.Init ( targetJobCount );
        return;
    }

    BBJobContext* newJobContexts = static_cast<BBJobContext*> ( malloc ( targetJobCount * sizeof ( BBJobContext ) ) );

    if ( progressInfo.jobs > targetJobCount )
    {
        memcpy ( newJobContexts, jobContexts, targetJobCount * sizeof ( BBJobContext ) );

        for ( GXUPointer i = progressInfo.jobs; i < targetJobCount; ++i )
        {
            BBJobContext& context = jobContexts[ i ];
            context.isLoop = GX_FALSE;
            context.thread->Join ();
            delete context.thread;
        }

        jobContexts = newJobContexts;
        progressInfo.Init ( targetJobCount );

        return;
    }

    memcpy ( newJobContexts, jobContexts, progressInfo.jobs * sizeof ( BBJobContext ) );
    GXUPointer pointStorageSize = samplesPerPoint * sizeof ( GXPreciseComplex );

    for ( GXUPointer i = progressInfo.jobs; i < targetJobCount; ++i )
    {
        BBJobContext& context = jobContexts[ i ];
        context.isLoop = GX_TRUE;
        context.isAbort = GX_FALSE;
        context.state = eBBThreadState::Idle;
        context.pointCount = 0u;
        context.points = static_cast<GXPreciseComplex*> ( malloc ( pointStorageSize ) );
        context.job = nullptr;
        context.thread = new GXThread ( &BBScheduler::JobThread, this );
        context.thread->Start ();
    }

    jobContexts = newJobContexts;
    progressInfo.Init ( targetJobCount );
}

GXVoid GXCALL BBScheduler::ScheduleJobs ()
{
    // TODO
}

GXVoid BBScheduler::UpdateDistributionPattern ()
{
    GXSafeDelete ( distributionPattern );

    GXRandomize ();

    // TODO
}

GXVoid GXCALL BBScheduler::OnJobProgress ( GXVoid* /*context*/ )
{
    // TODO
}

GXUPointer GXTHREADCALL BBScheduler::SchedulerThread ( GXVoid* argument, GXThread &thread )
{
    BBScheduler* scheduler = static_cast<BBScheduler*> ( argument );
    BBTask* currentTask = scheduler->task;

    while ( scheduler->isLoop )
    {
        switch ( scheduler->state )
        {
            case eBBThreadState::Idle:
                thread.Sleep ( IDLE_TIMEOUT );
            break;

            case eBBThreadState::JobProgress:
            {
                if ( currentTask != scheduler->task )
                {
                    scheduler->InitJobs ();
                    currentTask = scheduler->task;
                }

                if ( scheduler->isAbort )
                {
                    scheduler->DoAbort ();
                    continue;
                }

                scheduler->ScheduleJobs ();
            }
            break;
        }
    }

    return 0u;
}

GXUPointer GXTHREADCALL BBScheduler::JobThread ( GXVoid* /*argument*/, GXThread& /*thread*/ )
{
    return 0u;
}
