// version 1.7

#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXSmartLock.h>
#include <GXCommon/GXStrings.h>

GX_DISABLE_COMMON_WARNINGS

#include <inttypes.h>

GX_RESTORE_WARNING_STATE


#define END_INDEX                   SIZE_MAX

// Note value MUST be unsigned integer number!
#define HEAP_CHUNK_GROW_FACTOR      2u

//---------------------------------------------------------------------------------------------------------------------

GXUPointerAtomic     GXMemoryInspectorLite::newObjects;
GXUPointerAtomic     GXMemoryInspectorLite::newObjectSize;
GXUPointerAtomic     GXMemoryInspectorLite::mallocObjects;
GXUPointerAtomic     GXMemoryInspectorLite::mallocObjectSize;

GXBool GXCALL GXMemoryInspectorLite::CheckMemoryLeaks ()
{
    if ( newObjects == static_cast<GXUPointer> ( 0u ) && mallocObjects == static_cast<GXUPointer> ( 0u ) )
        return GX_TRUE;

    GXLogA ( "GXMemoryInspectorLite::CheckMemoryLeaks::Error - Memory leaks were detected:\n" );

    if ( newObjects > static_cast<GXUPointer> ( 0u ) )
    {
        GXLogA ( "\tnew allocated object(s): %" PRIuPTR "\n", static_cast<GXUPointer> ( newObjects ) );
        GXLogA ( "\tnew allocated object total size: %" PRIuPTR " byte(s)\n", static_cast<GXUPointer> ( newObjectSize ) );
    }

    if ( mallocObjects == static_cast<GXUPointer> ( 0u ) )
    {
        GXLogA ( "\n" );
        return GX_FALSE;
    }

    GXLogA ( "\tmalloc allocated object(s): %" PRIuPTR "\n", static_cast<GXUPointer> ( mallocObjects ) );
    GXLogA ( "\tmalloc allocated object total size: %" PRIuPTR " byte(s)\n\n", static_cast<GXUPointer> ( mallocObjectSize ) );

    return GX_FALSE;
}

GXVoid* GXMemoryInspectorLite::Malloc ( GXUPointer size )
{
    ++mallocObjects;
    mallocObjectSize += size;
    return malloc ( size );
}

GXVoid GXMemoryInspectorLite::Free ( GXVoid* heapMemory )
{
    --mallocObjects;
    mallocObjectSize -= _msize ( heapMemory );
    free ( heapMemory );
}

GXVoid GXMemoryInspectorLite::SafeFree ( GXVoid** heapMemory )
{
    GXVoid* m = *heapMemory;

    if ( m == nullptr ) return;

    --mallocObjects;
    mallocObjectSize -= _msize ( m );
    free ( m );

    *heapMemory = nullptr;
}

GXVoid* GXMemoryInspectorLite::operator new ( GXUPointer size )
{
    ++newObjects;
    newObjectSize += size;
    return malloc ( size );
}

GXVoid GXMemoryInspectorLite::operator delete ( GXVoid* heapMemory )
{
    --newObjects;
    newObjectSize -= _msize ( heapMemory );
    free ( heapMemory );
}

GXMemoryInspectorLite::GXMemoryInspectorLite ()
{
    // NOTHING
}

GXMemoryInspectorLite::~GXMemoryInspectorLite ()
{
    // NOTHING
}

//---------------------------------------------------------------------------------------------------------------------

enum class eGXHeapAllocationType : GXUByte
{
    New,
    Malloc
};

struct GXHeapChunk
{
    GXUPointer                  nextIndex;
    GXUPointer                  previousIndex;

    GXUPointer                  allocationSize;
    eGXHeapAllocationType       allocationType;

    GXVoid*                     heapMemory;
    GXUPointer                  heapMemoryMaxSize;

    GXChar*                     initiatorClass;
    GXUPointer                  initiatorClassMaxLength;

    GXVoid InitData ( eGXHeapAllocationType newAllocationType, GXUPointer newSize, const GXChar* newInitiatorClass );
    GXVoid Release ();
};

GXVoid GXHeapChunk::InitData ( eGXHeapAllocationType newAllocationType, GXUPointer newSize, const GXChar* newInitiatorClass )
{
    GXUPointer symbols = strlen ( newInitiatorClass ) + 1u;

    if ( symbols <= initiatorClassMaxLength )
    {
        memcpy ( initiatorClass, newInitiatorClass, symbols );
    }
    else
    {
        GXSafeFree ( initiatorClass );
        GXCstrClone ( &initiatorClass, newInitiatorClass );
        initiatorClassMaxLength = symbols;
    }

    if ( newSize > heapMemoryMaxSize )
    {
        GXSafeFree ( heapMemory );
        heapMemory = malloc ( newSize );
        heapMemoryMaxSize = newSize;
    }

    allocationSize = newSize;
    allocationType = newAllocationType;
}

GXVoid GXHeapChunk::Release ()
{
    free ( initiatorClass );
    free ( heapMemory );
}

//---------------------------------------------------------------------------------------------------------------------

class GXHeapManager final
{
    private:
        const GXChar*       bindedName;

        GXHeapChunk*        chunks;
        GXUPointer          chunkCount;

        GXUPointer          freeChunkIndex;
        GXUPointer          uninitiatedChunkIndex;
        GXUPointer          usedChunkIndex;

        GXSmartLock         smartLock;

    public:
        GXHeapManager ();
        ~GXHeapManager ();

        GXBool CheckMemoryLeaks ();

        GXVoid BindName ( const GXChar* name );
        GXVoid* MakeNew ( GXUPointer size );
        GXVoid MakeDelete ( GXVoid* heapMemory );

        GXVoid* MakeMalloc ( GXUPointer size, const GXChar* initiatorClass );
        GXVoid MakeFree ( GXVoid* heapMemory );

    private:
        GXVoid* AddChunk ( eGXHeapAllocationType allocationType, GXUPointer size, const GXChar* initiatorClass );
        GXVoid RemoveChunk ( GXVoid* heapMemory );

        GXVoid GrowChunks ();

        GXHeapManager ( const GXHeapManager &other ) = delete;
        GXHeapManager& operator = ( const GXHeapManager &other ) = delete;
};

static GXHeapManager gx_HeapManager;

GXHeapManager::GXHeapManager ():
    bindedName ( nullptr ),
    chunks ( nullptr ),
    chunkCount ( 0u ),
    freeChunkIndex ( END_INDEX ),
    uninitiatedChunkIndex ( END_INDEX ),
    usedChunkIndex ( END_INDEX )
{
    // NOTHING
}

GXHeapManager::~GXHeapManager ()
{
    GXUPointer chunkIndex = usedChunkIndex;

    while ( chunkIndex != END_INDEX )
    {
        GXHeapChunk& chunk = chunks[ chunkIndex ];
        chunk.Release ();
        chunkIndex = chunk.nextIndex;
    }

    chunkIndex = freeChunkIndex;

    while ( chunkIndex != END_INDEX )
    {
        GXHeapChunk& chunk = chunks[ chunkIndex ];
        chunk.Release ();
        chunkIndex = chunk.nextIndex;
    }

    GXSafeFree ( chunks );
}

GXBool GXHeapManager::CheckMemoryLeaks ()
{
    if ( usedChunkIndex == END_INDEX )
        return GX_TRUE;

    GXLogA ( "GXHeapManager::CheckMemoryLeaks::Error - Memory leaks were detected:\n" );

    GXUPointer leakedMallocObjects = 0u;
    GXUPointer leakedMallocSize = 0u;
    GXUPointer leakedNewObjects = 0u;
    GXUPointer leakedNewSize = 0u;

    for ( GXUPointer i = usedChunkIndex; i != END_INDEX; )
    {
        GXHeapChunk& chunk = chunks[ i ];

        switch ( chunk.allocationType )
        {
            case eGXHeapAllocationType::Malloc:
                ++leakedMallocObjects;
                leakedMallocSize += chunk.allocationSize;
                GXLogA ( "\tInitiator class: %s\n\tAllocation type: malloc\n\tAllocation size: %" PRIuPTR " byte(s)\n\n", chunk.initiatorClass, chunk.allocationSize );
            break;

            case eGXHeapAllocationType::New:
                ++leakedNewObjects;
                leakedNewSize += chunk.allocationSize;
                GXLogA ( "\tInitiator class: %s\n\tAllocation type: new\n\tAllocation size: %" PRIuPTR " byte(s)\n\n", chunk.initiatorClass, chunk.allocationSize );
            break;
        }

        i = chunk.nextIndex;
    }

    if ( leakedMallocObjects != 0u )
        GXLogA ( "%" PRIuPTR " malloc allocated object(s) with total size %" PRIuPTR " byte(s) was(were) leaked.\n", leakedMallocObjects, leakedMallocSize );

    if ( leakedNewObjects != 0u )
        GXLogA ( "%" PRIuPTR " new allocated object(s) with total size %" PRIuPTR " byte(s) was(were) leaked.\n", leakedNewObjects, leakedNewSize );

    GXLogA ( "\n" );

    return GX_FALSE;
}

GXVoid GXHeapManager::BindName ( const GXChar* name )
{
    // Note release will be done in MakeNew method.
    smartLock.AcquireExlusive ();
    bindedName = name;
}

GXVoid* GXHeapManager::MakeNew ( GXUPointer size )
{
    GXVoid* memory = AddChunk ( eGXHeapAllocationType::New, size, bindedName );

    // Note acquire will be done in BindName method.
    smartLock.ReleaseExlusive ();

    return memory;
}

GXVoid GXHeapManager::MakeDelete ( GXVoid* heapMemory )
{
    RemoveChunk ( heapMemory );
}

GXVoid* GXHeapManager::MakeMalloc ( GXUPointer size, const GXChar* initiatorClass )
{
    smartLock.AcquireExlusive ();
    GXVoid* memory = AddChunk ( eGXHeapAllocationType::Malloc, size, initiatorClass );
    smartLock.ReleaseExlusive ();

    return memory;
}

GXVoid GXHeapManager::MakeFree ( GXVoid* heapMemory )
{
    RemoveChunk ( heapMemory );
}

GXVoid* GXHeapManager::AddChunk ( eGXHeapAllocationType allocationType, GXUPointer size, const GXChar* initiatorClass )
{
    if ( freeChunkIndex == END_INDEX && uninitiatedChunkIndex == END_INDEX )
        GrowChunks ();

    GXHeapChunk* workingChunk = nullptr;

    if ( freeChunkIndex != END_INDEX )
    {
        workingChunk = chunks + freeChunkIndex;
        workingChunk->previousIndex = END_INDEX;

        GXUPointer nextFreeChunkIndex = workingChunk->nextIndex;
        workingChunk->nextIndex = usedChunkIndex;

        if ( usedChunkIndex != END_INDEX )
        {
            GXHeapChunk& firstUsedChunk = chunks[ usedChunkIndex ];
            firstUsedChunk.previousIndex = freeChunkIndex;
        }

        usedChunkIndex = freeChunkIndex;
        freeChunkIndex = nextFreeChunkIndex;
    }
    else
    {
        workingChunk = chunks + uninitiatedChunkIndex;
        workingChunk->previousIndex = END_INDEX;
        workingChunk->nextIndex = usedChunkIndex;

        if ( usedChunkIndex != END_INDEX )
        {
            GXHeapChunk& firstUsedChunk = chunks[ usedChunkIndex ];
            firstUsedChunk.previousIndex = uninitiatedChunkIndex;
        }

        usedChunkIndex = uninitiatedChunkIndex;
        ++uninitiatedChunkIndex;

        if ( uninitiatedChunkIndex >= chunkCount )
        {
            uninitiatedChunkIndex = END_INDEX;
        }
    }

    workingChunk->InitData ( allocationType, size, initiatorClass );
    return workingChunk->heapMemory;
}

GXVoid GXHeapManager::RemoveChunk ( GXVoid* heapMemory )
{
    smartLock.AcquireExlusive ();

    GXUPointer targetIndex = END_INDEX;

    for ( GXUPointer i = usedChunkIndex; i != END_INDEX; )
    {
        GXHeapChunk& chunk = chunks[ i ];

        if ( chunk.heapMemory == heapMemory )
        {
            targetIndex = i;
            break;
        }

        i = chunk.nextIndex;
    }

    if ( targetIndex == END_INDEX )
    {
        smartLock.ReleaseExlusive ();
        GXLogA ( "GXHeapManager::RemoveChunk::Error - Can't find chunk with heap address 0x%p!", heapMemory );
        return;
    }

    GXHeapChunk& target = chunks[ targetIndex ];

    if ( target.nextIndex != END_INDEX )
    {
        GXHeapChunk& chunk = chunks[ target.nextIndex ];
        chunk.previousIndex = target.previousIndex;
    }

    if ( target.previousIndex != END_INDEX )
    {
        GXHeapChunk& chunk = chunks[ target.previousIndex ];
        chunk.nextIndex = target.nextIndex;
    }
    else
    {
        usedChunkIndex = target.nextIndex;
    }

    target.previousIndex = END_INDEX;
    target.nextIndex = freeChunkIndex;

    if ( freeChunkIndex != END_INDEX )
    {
        GXHeapChunk& chunk = chunks[ freeChunkIndex ];
        chunk.previousIndex = targetIndex;
    }

    freeChunkIndex = targetIndex;

    smartLock.ReleaseExlusive ();
}

GXVoid GXHeapManager::GrowChunks ()
{
    if ( chunkCount == 0u )
    {
        chunkCount = 1u;
        chunks = static_cast<GXHeapChunk*> ( malloc ( sizeof ( GXHeapChunk ) ) );
        memset ( chunks, 0, sizeof ( GXHeapChunk ) );
        uninitiatedChunkIndex = 0u;
        return;
    }

    GXHeapChunk* oldChunks = chunks;
    GXUPointer newChunkCount = chunkCount * HEAP_CHUNK_GROW_FACTOR;

    chunks = static_cast<GXHeapChunk*> ( malloc ( newChunkCount * sizeof ( GXHeapChunk ) ) );
    memcpy ( chunks, oldChunks, chunkCount * sizeof ( GXHeapChunk ) );
    memset ( chunks + chunkCount, 0, ( newChunkCount - chunkCount ) * sizeof ( GXHeapChunk ) );
    uninitiatedChunkIndex = chunkCount;
    chunkCount = newChunkCount;

    free ( oldChunks );
}

//---------------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXMemoryInspectorFull::BindClassName ( const GXChar* className )
{
    gx_HeapManager.BindName ( className );
}

GXBool GXCALL GXMemoryInspectorFull::CheckMemoryLeaks ()
{
    return gx_HeapManager.CheckMemoryLeaks ();
}

GXVoid* GXMemoryInspectorFull::Malloc ( GXUPointer size )
{
    return gx_HeapManager.MakeMalloc ( size, className );
}

GXVoid GXMemoryInspectorFull::Free ( GXVoid* heapMemory )
{
    gx_HeapManager.MakeFree ( heapMemory );
}

GXVoid GXMemoryInspectorFull::SafeFree ( GXVoid** heapMemory )
{
    GXVoid* m = *heapMemory;

    if ( m == nullptr ) return;

    gx_HeapManager.MakeFree ( m );
    *heapMemory = nullptr;
}

GXVoid* GXMemoryInspectorFull::operator new ( GXUPointer size )
{
    return gx_HeapManager.MakeNew ( size );
}

GXVoid GXMemoryInspectorFull::operator delete ( GXVoid* heapMemory )
{
    gx_HeapManager.MakeDelete ( heapMemory );
}

GXMemoryInspectorFull::GXMemoryInspectorFull ( const GXChar* name )
{
    GXCstrClone ( &className, name );
}

GXMemoryInspectorFull::~GXMemoryInspectorFull ()
{
    free ( className );
}
