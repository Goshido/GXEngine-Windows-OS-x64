#ifndef BB_TASK
#define BB_TASK


#include <GXCommon/GXMath.h>
#include <GXCommon/GXUIntAtomic.h>
#include <GXCommon/GXSmartLock.h>


class BBTask final
{
    private:
        const GXPreciseComplex      minimum;
        const GXPreciseComplex      maximum;

        const GXUPointer            width;
        const GXUPointer            height;
        const GXUPointer            mapSize;

        const GXDouble              resampleX;
        const GXDouble              resampleY;

        GXSmartLock                 smartLock;

        GXUInt*                     bestSampleMap;
        GXUIntAtomic*               hitMap;

    public:
        explicit BBTask ( const GXPreciseComplex &minimumViewport, const GXPreciseComplex &maximumViewport, GXUInt imageWidth, GXUInt imageHeight );
        ~BBTask ();

        // Allocation will be done via malloc. User code MUST call free when best sample map is no needed anymore.
        const GXUInt* GetBestSampleMap ();

        // Allocation will be done via malloc. User code MUST call free when hit map is no needed anymore.
        const GXUInt* GetHitMap ();

        GXVoid GetViewport ( GXPreciseComplex &minimumVieport, GXPreciseComplex &maximumVieport ) const;

        GXVoid MakeHit ( const GXPreciseComplex &locationViewport );
        GXVoid MarkSampleMap ( const GXPreciseComplex &locationViewport, GXUInt iterations );

    private:
        GXUPointer GetSampleIndex ( const GXPreciseComplex &locationViewport );

        BBTask () = delete;
        BBTask ( const BBTask &other ) = delete;
        BBTask& operator = ( const BBTask &other ) = delete;
};


#endif // BB_TASK
