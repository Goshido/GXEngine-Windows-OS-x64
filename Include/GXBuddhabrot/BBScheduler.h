#ifndef BB_SCHEDULER
#define BB_SCHEDULER


#include "BBJob.h"
#include <GXCommon/GXThread.h>
#include <GXCommon/GXSmartLock.h>


// Note progress can be [0.0f, 1.0f].
class BBProgressInfo final
{
    public:
        GXFloat         totalProgress;

        GXFloat*        jobProgress;
        GXUPointer      jobs;

    public:
        BBProgressInfo ();
        ~BBProgressInfo ();

        GXVoid Init ( GXUPointer asyncJobs );

    private:
        BBProgressInfo ( const BBProgressInfo &other ) = delete;
        BBProgressInfo& operator = ( const BBProgressInfo &other ) = delete;
};

typedef GXVoid ( GXCALL* PFNBBSCHEDULERPROGRESSPROC ) ( const BBProgressInfo &info );

enum class eBBThreadState : GXUByte
{
    Idle,
    JobProgress
};

struct BBJobContext
{
    GXBool                  isLoop;
    GXBool                  isAbort;
    GXThread*               thread;
    eBBThreadState          state;
    GXUInt                  pointCount;
    GXPreciseComplex*       points;
    BBJob*                  job;
};

class BBScheduler final
{
    private:
        GXBool                  isAbort;
        GXBool                  isLoop;
        eBBThreadState          state;

        GXPreciseComplex*       distributionPattern;

        BBTask*                 task;
        GXDouble                pointSize;          // perfect square point
        GXUShort                samplesPerPoint;

        BBJobContext*           jobContexts;
        GXUPointer              targetJobCount;

        GXThread*               scheduerThread;
        BBProgressInfo          progressInfo;

        GXPreciseComplex        jobVieportMininum;
        GXPreciseComplex        jobViewportMaximum;

    public:
        BBScheduler ();
        ~BBScheduler ();

        // Note newSamplesPerPoint MUST be in range [1, 0x7FFF].
        GXVoid Start ( BBTask &newTask, GXUPointer asyncJobs, GXUShort newSamplesPerPoint, GXDouble newPointSize );

        GXVoid Abort ();
        GXVoid Join ();

    private:
        GXVoid DoAbort ();
        GXVoid InitJobs ();
        GXVoid ScheduleJobs ();

        GXVoid UpdateDistributionPattern ();

        static GXVoid GXCALL OnJobProgress ( GXVoid* context );

        static GXUPointer GXTHREADCALL SchedulerThread ( GXVoid* argument, GXThread &thread );
        static GXUPointer GXTHREADCALL JobThread ( GXVoid* argument, GXThread &thread );

        BBScheduler ( const BBScheduler &other ) = delete;
        BBScheduler& operator = ( const BBScheduler &other ) = delete;
};


#endif // BB_SCHEDULER
