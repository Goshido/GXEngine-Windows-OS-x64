#include <GXBuddhabrot/BBScheduler.h>
#include <GXBuddhabrot/BBJob.h>
#include <GXCommon/GXMemory.h>


#define DEFAULT_POINT_SIZE              1.0e-2
#define DEFAULT_SAMPLES_PER_POINT       777u

#define IDLE_TIMEOUT                    7u

#define MINIMUM_REAL                    -2.0
#define MAXIMUM_REAL                    1.0
#define MINIMUM_IMAGINARY               -1.0
#define MAXIMUM_IMAGINARY               1.0

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
    InitCollectors ();
    scheduerThread = new GXThread ( &BBScheduler::SchedulerThread, this );
}

BBScheduler::~BBScheduler ()
{
    Join ();
    delete scheduerThread;
    GXSafeDelete ( distributionPattern );
}

GXVoid BBScheduler::Start ( BBTask &newTask, GXUPointer asyncJobs, GXUShort newSamplesPerPoint, GXDouble newPointSize )
{
    if ( task )
        Abort ();

    task = &newTask;
    pointSize = newPointSize;
    currentPointIndex = 0u;

    GXPreciseComplex minimumViewport;
    GXPreciseComplex maximumViewport;
    newTask.GetViewport ( minimumViewport, maximumViewport );

    const GXPreciseComplex delta = ( maximumViewport - minimumViewport ) / newPointSize;
    progressPerPoint = static_cast<GXFloat> ( 1.0 / ( ceil ( delta.r ) * ceil ( delta.i ) ) );

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
        delete context.job;
        free ( context.points );
    }
}

GXVoid BBScheduler::DoAbort ()
{
    for ( GXUPointer i = 0u; i < progressInfo.jobs; ++i )
        jobContexts[ i ].job->Abort ();

    for ( GXUPointer i = 0u; i < progressInfo.jobs; ++i )
    {
        BBJob& job = *( jobContexts[ i ].job );

        while ( job.GetState () != eBBThreadState::Idle )
        {
            scheduerThread->Sleep ( IDLE_TIMEOUT );
        }
    }

    state = eBBThreadState::Idle;
}

GXVoid BBScheduler::CollectJobPointNoCheck ( BBJobContext& jobContext, GXPreciseComplex &activePoint )
{
    for ( GXUShort i = 0u; i < samplesPerPoint; ++i )
        jobContext.points[ i ] = activePoint + distributionPattern[ i ];

    jobContext.pointCount = samplesPerPoint;
    activePoint.r += pointSize;
}

GXVoid BBScheduler::CollectJobPointRealCheck ( BBJobContext& jobContext, GXPreciseComplex &activePoint )
{
    jobContext.pointCount = 0u;

    for ( GXUShort i = 0u; i < samplesPerPoint; ++i )
    {
        const GXPreciseComplex& offset = distributionPattern[ i ];
        const GXDouble real = activePoint.r + offset.r;

        if ( real > MAXIMUM_REAL ) continue;

        jobContext.points[ jobContext.pointCount ].Init ( real, activePoint.i + offset.i );
        ++jobContext.pointCount;
    }

    activePoint.r = MINIMUM_REAL;
    activePoint.i += pointSize;
}

GXVoid BBScheduler::CollectJobPointImaginaryCheck ( BBJobContext& jobContext, GXPreciseComplex &activePoint )
{
    jobContext.pointCount = 0u;

    for ( GXUShort i = 0u; i < samplesPerPoint; ++i )
    {
        const GXPreciseComplex& offset = distributionPattern[ i ];
        const GXDouble imaginary = activePoint.i + offset.i;

        if ( imaginary > MAXIMUM_IMAGINARY ) continue;

        jobContext.points[ jobContext.pointCount ].Init ( activePoint.r + offset.r, imaginary );
        ++jobContext.pointCount;
    }

    activePoint.r += pointSize;
}

GXVoid BBScheduler::CollectJobPointCheckBoth ( BBJobContext& jobContext, GXPreciseComplex &activePoint )
{
    for ( GXUShort i = 0u; i < samplesPerPoint; ++i )
    {
        const GXPreciseComplex& offset = distributionPattern[ i ];
        const GXDouble real = activePoint.r + offset.r;
        const GXDouble imaginary = activePoint.i + offset.i;

        if ( real > MAXIMUM_REAL || imaginary > MAXIMUM_IMAGINARY ) continue;

        jobContext.points[ jobContext.pointCount ].Init ( real, imaginary );
        ++jobContext.pointCount;
    }

    activePoint.r = MAXIMUM_REAL;
    activePoint.i = MAXIMUM_IMAGINARY;
}

GXVoid BBScheduler::InitCollectors ()
{
    // Mapping is two bit integer:
    // Most significant bit is real part is greater or equal than MAXIMUM_REAL.
    // Least significant bit is imaginary part is greater or equal than MAXIMUM_IMAGINARY.

    // Real part is less than MAXIMUM_REAL. Imaginary part is less than MAXIMUM_IMAGINARY.
    collectors[ 0u ] = &BBScheduler::CollectJobPointCheckBoth;

    // Real part is less than MAXIMUM_REAL. Imaginary part is greater or equal than MAXIMUM_IMAGINARY.
    collectors[ 1u ] = &BBScheduler::CollectJobPointImaginaryCheck;

    // Real part is greater or equal than MAXIMUM_REAL. Imaginary part is less than MAXIMUM_IMAGINARY.
    collectors[ 2u ] = &BBScheduler::CollectJobPointRealCheck;

    // Real part is greater or equal than MAXIMUM_REAL. Imaginary part is greater or equal than MAXIMUM_IMAGINARY.
    collectors[ 3u ] = &BBScheduler::CollectJobPointCheckBoth;
}

GXVoid BBScheduler::InitJobs ()
{
    UpdateDistributionPattern ();
    currentPoint.Init ( MINIMUM_REAL, MINIMUM_IMAGINARY );

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
            delete jobContexts[ i ].job;

        jobContexts = newJobContexts;
        progressInfo.Init ( targetJobCount );

        return;
    }

    memcpy ( newJobContexts, jobContexts, progressInfo.jobs * sizeof ( BBJobContext ) );
    GXUPointer pointStorageSize = samplesPerPoint * sizeof ( GXPreciseComplex );

    for ( GXUPointer i = progressInfo.jobs; i < targetJobCount; ++i )
    {
        BBJobContext& context = jobContexts[ i ];
        context.pointCount = 0u;
        context.points = static_cast<GXPreciseComplex*> ( malloc ( pointStorageSize ) );
        context.job = new BBJob ( *task, &BBScheduler::OnJobProgress, this );
        context.job->Start ();
    }

    jobContexts = newJobContexts;
    progressInfo.Init ( targetJobCount );
}

GXVoid BBScheduler::ScheduleJobs ()
{
    if ( currentPoint.r >= MAXIMUM_REAL && currentPoint.i >= MAXIMUM_IMAGINARY )
    {
        for ( GXUPointer i = 0u; i < targetJobCount; ++i )
        {
            if ( jobContexts[ i ].job->GetState () == eBBThreadState::Idle ) continue;

            return;
        }

        for ( GXUPointer i = 0u; i < targetJobCount; ++i )
            jobContexts[ i ].job->Relax ();

        progressInfo.totalProgress = 1.0f;
        state = eBBThreadState::Idle;
        return;
    }

    for ( GXUPointer jobIndex = 0u; jobIndex < targetJobCount; ++jobIndex )
    {
        BBJobContext& jobContext = jobContexts[ jobIndex ];

        if ( jobContext.job->GetState () == eBBThreadState::JobProgress ) continue;

        GXUByte collectorIndex = currentPoint.r + pointSize > MAXIMUM_REAL ? 2u : 0u;

        if ( currentPoint.i + pointSize > MAXIMUM_IMAGINARY )
            collectorIndex += 1u;

        PFNBBSCHEDULERCOLLECTORPROC collector = collectors[ collectorIndex ];

        // Note it is calling method by pointer C++ syntax.
        ( this->*collector ) ( jobContext, currentPoint );

        ++currentPointIndex;
        progressInfo.totalProgress = currentPointIndex * progressPerPoint;

        if ( jobContext.pointCount == 0u ) break;

        jobContext.job->Init ( jobContext, jobIndex );
    }
}

GXVoid BBScheduler::UpdateDistributionPattern ()
{
    GXSafeDelete ( distributionPattern );
    distributionPattern = static_cast<GXPreciseComplex*> ( malloc ( samplesPerPoint * sizeof ( GXPreciseComplex ) ) );

    GXRandomize ();

    for ( GXUShort i = 0u; i < samplesPerPoint; ++i )
    {
        GXPreciseComplex& sample = distributionPattern[ i ];
        sample.r = static_cast<GXDouble> ( GXRandomNormalize () ) * pointSize;
        sample.i = static_cast<GXDouble> ( GXRandomNormalize () ) * pointSize;
    }
}

GXVoid GXCALL BBScheduler::OnJobProgress ( GXVoid* context, GXFloat progress, GXUPointer jobIndex )
{
    BBScheduler* scheduler = static_cast<BBScheduler*> ( context );
    scheduler->progressInfo.jobProgress[ jobIndex ] = progress;
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
