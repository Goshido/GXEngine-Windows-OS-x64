#include <GXBuddhabrot/BBTask.h>
#include <GXCommon/GXMemory.h>


BBTask::BBTask ( const GXPreciseComplex &minimumViewport, const GXPreciseComplex &maximumViewport, GXUInt imageWidth, GXUInt imageHeight, GXUInt maximumIterations ):
    iterationCap ( maximumIterations ),
    minimum ( minimumViewport ),
    maximum ( maximumViewport ),
    width ( static_cast<GXUPointer> ( imageWidth ) ),
    height ( static_cast<GXUPointer> ( imageHeight ) ),
    mapSize ( imageWidth * imageHeight * sizeof ( GXUInt ) ),
    resampleX ( 1.0 / ( maximumViewport.r - minimumViewport.r ) ),
    resampleY ( 1.0 / ( maximumViewport.i - minimumViewport.i ) )
{
    bestSampleMap = static_cast<GXUInt*> ( malloc ( mapSize ) );
    memset ( bestSampleMap, 0, mapSize );

    hitMap = static_cast<GXUIntAtomic*> ( malloc ( mapSize ) );
    memset ( hitMap, 0, mapSize );
}

BBTask::~BBTask ()
{
    free ( bestSampleMap );
    free ( hitMap );
}

const GXUInt* BBTask::GetBestSampleMap ()
{
    smartLock.AcquireExlusive ();

    GXUInt* result = static_cast<GXUInt*> ( malloc ( mapSize ) );
    memcpy ( result, bestSampleMap, mapSize );

    smartLock.ReleaseExlusive ();

    return result;
}

const GXUInt* BBTask::GetHitMap ()
{
    smartLock.AcquireExlusive ();

    GXUInt* result = static_cast<GXUInt*> ( malloc ( mapSize ) );
    memcpy ( result, hitMap, mapSize );

    smartLock.ReleaseExlusive ();

    return result;
}

GXUInt BBTask::GetIterationCap () const
{
    return iterationCap;
}

GXVoid BBTask::GetViewport ( GXPreciseComplex &minimumViewport, GXPreciseComplex &maximumViewport ) const
{
    minimumViewport = minimum;
    maximumViewport = maximum;
}

GXVoid BBTask::MakeHit ( const GXPreciseComplex &locationViewport )
{
    smartLock.AcquireShared ();
    ++hitMap[ GetSampleIndex ( locationViewport ) ];
    smartLock.ReleaseShared ();
}

GXVoid BBTask::MarkSampleMap ( const GXPreciseComplex &locationViewport, GXUInt iterations )
{
    smartLock.AcquireShared ();
    bestSampleMap[ GetSampleIndex ( locationViewport ) ] = iterations;
    smartLock.ReleaseShared ();
}

GXUPointer BBTask::GetSampleIndex ( const GXPreciseComplex &locationViewport )
{
    GXPreciseComplex delta = GXPreciseComplex ( locationViewport ) - minimum;
    return static_cast<GXUPointer> ( delta.i * resampleY ) * width + static_cast<GXUPointer> ( delta.r * resampleX );
}
