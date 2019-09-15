// version 1.11

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

static GXUPointerAtomic     gx_common_MemoryInspectorLiteNewObjects ( 0u );
static GXUPointerAtomic     gx_common_MemoryInspectorLiteNewObjectSize ( 0u );
static GXUPointerAtomic     gx_common_MemoryInspectorLiteMallocObjects ( 0u );
static GXUPointerAtomic     gx_common_MemoryInspectorLiteMallocObjectSize ( 0u );

GXBool GXCALL GXMemoryInspectorLite::CheckMemoryLeaks ()
{
    if ( gx_common_MemoryInspectorLiteNewObjects == static_cast<GXUPointer> ( 0u ) && gx_common_MemoryInspectorLiteMallocObjects == static_cast<GXUPointer> ( 0u ) )
        return GX_TRUE;

    GXLogA ( "GXMemoryInspectorLite::CheckMemoryLeaks::Error - Memory leaks were detected:\n" );

    if ( gx_common_MemoryInspectorLiteNewObjects > static_cast<GXUPointer> ( 0u ) )
    {
        GXLogA ( "\tnew allocated object(s): %" PRIuPTR "\n", static_cast<GXUPointer> ( gx_common_MemoryInspectorLiteNewObjects ) );
        GXLogA ( "\tnew allocated object total size: %" PRIuPTR " byte(s)\n", static_cast<GXUPointer> ( gx_common_MemoryInspectorLiteNewObjectSize ) );
    }

    if ( gx_common_MemoryInspectorLiteMallocObjects == static_cast<GXUPointer> ( 0u ) )
    {
        GXLogA ( "\n" );
        return GX_FALSE;
    }

    GXLogA ( "\tmalloc allocated object(s): %" PRIuPTR "\n", static_cast<GXUPointer> ( gx_common_MemoryInspectorLiteMallocObjects ) );
    GXLogA ( "\tmalloc allocated object total size: %" PRIuPTR " byte(s)\n\n", static_cast<GXUPointer> ( gx_common_MemoryInspectorLiteMallocObjectSize ) );

    return GX_FALSE;
}

GXVoid* GXMemoryInspectorLite::Malloc ( GXUPointer size ) const
{
    ++gx_common_MemoryInspectorLiteMallocObjects;
    gx_common_MemoryInspectorLiteMallocObjectSize += size;
    return malloc ( size );
}

GXVoid* GXMemoryInspectorLite::Realloc ( GXVoid* heapMemory, GXUPointer newSize ) const
{
    if ( !heapMemory )
        return Malloc ( newSize );

    GXUPointer currentSize = _msize ( heapMemory );
    GXVoid* newHeapMemory = realloc ( heapMemory, newSize );
    gx_common_MemoryInspectorLiteMallocObjectSize = gx_common_MemoryInspectorLiteMallocObjectSize - currentSize + newSize;

    return newHeapMemory;
}

GXVoid GXMemoryInspectorLite::Free ( GXVoid* heapMemory ) const
{
    if ( !heapMemory ) return;

    --gx_common_MemoryInspectorLiteMallocObjects;
    gx_common_MemoryInspectorLiteMallocObjectSize -= _msize ( heapMemory );
    free ( heapMemory );
}

GXVoid GXMemoryInspectorLite::SafeFree ( GXVoid** heapMemory ) const
{
    GXVoid* m = *heapMemory;

    if ( m == nullptr ) return;

    --gx_common_MemoryInspectorLiteMallocObjects;
    gx_common_MemoryInspectorLiteMallocObjectSize -= _msize ( m );
    free ( m );

    *heapMemory = nullptr;
}

GXVoid* GXMemoryInspectorLite::operator new ( GXUPointer size )
{
    ++gx_common_MemoryInspectorLiteNewObjects;
    gx_common_MemoryInspectorLiteNewObjectSize += size;
    return malloc ( size );
}

GXVoid GXMemoryInspectorLite::operator delete ( GXVoid* heapMemory )
{
    if ( !heapMemory ) return;

    --gx_common_MemoryInspectorLiteNewObjects;
    gx_common_MemoryInspectorLiteNewObjectSize -= _msize ( heapMemory );
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
    GXUPointer                  _nextIndex;
    GXUPointer                  _previousIndex;

    GXUPointer                  _allocationSize;
    eGXHeapAllocationType       _allocationType;

    GXVoid*                     _heapMemory;
    GXUPointer                  _heapMemoryMaxSize;

    GXChar*                     _initiatorClass;
    GXUPointer                  _initiatorClassMaxLength;

    GXVoid InitData ( eGXHeapAllocationType newAllocationType, GXUPointer newSize, const GXChar* newInitiatorClass );
    GXVoid Release ();
};

GXVoid GXHeapChunk::InitData ( eGXHeapAllocationType newAllocationType, GXUPointer newSize, const GXChar* newInitiatorClass )
{
    GXUPointer symbols = strlen ( newInitiatorClass ) + 1u;

    if ( symbols <= _initiatorClassMaxLength )
    {
        memcpy ( _initiatorClass, newInitiatorClass, symbols );
    }
    else
    {
        GXSafeFree ( _initiatorClass );
        GXCstrClone ( &_initiatorClass, newInitiatorClass );
        _initiatorClassMaxLength = symbols;
    }

    if ( newSize > _heapMemoryMaxSize )
    {
        GXSafeFree ( _heapMemory );
        _heapMemory = malloc ( newSize );
        _heapMemoryMaxSize = newSize;
    }

    _allocationSize = newSize;
    _allocationType = newAllocationType;
}

GXVoid GXHeapChunk::Release ()
{
    free ( _initiatorClass );
    free ( _heapMemory );
}

//---------------------------------------------------------------------------------------------------------------------

class GXHeapManager final
{
    private:
        const GXChar*       _bindedName;

        GXHeapChunk*        _chunks;
        GXUPointer          _chunkCount;

        GXUPointer          _freeChunkIndex;
        GXUPointer          _uninitiatedChunkIndex;
        GXUPointer          _usedChunkIndex;

        GXSmartLock         _smartLock;

    public:
        GXHeapManager ();
        ~GXHeapManager ();

        GXBool CheckMemoryLeaks ();

        GXVoid BindName ( const GXChar* name );
        GXVoid* MakeNew ( GXUPointer size );
        GXVoid MakeDelete ( GXVoid* heapMemory );

        GXVoid* MakeMalloc ( GXUPointer size, const GXChar* initiatorClass );
        GXVoid* MakeRealloc ( GXVoid* heapMemory, GXUPointer newSize, const GXChar* initiatorClass );
        GXVoid MakeFree ( GXVoid* heapMemory );

    private:
        GXVoid* AddChunk ( eGXHeapAllocationType allocationType, GXUPointer size, const GXChar* initiatorClass );
        GXVoid RemoveChunk ( GXVoid* heapMemory );

        GXUPointer FindChunkIndex ( GXVoid* heapMemory );

        GXVoid GrowChunks ();

        GXHeapManager ( const GXHeapManager &other ) = delete;
        GXHeapManager& operator = ( const GXHeapManager &other ) = delete;
};

static GXHeapManager gx_HeapManager;

GXHeapManager::GXHeapManager ():
    _bindedName ( nullptr ),
    _chunks ( nullptr ),
    _chunkCount ( 0u ),
    _freeChunkIndex ( END_INDEX ),
    _uninitiatedChunkIndex ( END_INDEX ),
    _usedChunkIndex ( END_INDEX )
{
    // NOTHING
}

GXHeapManager::~GXHeapManager ()
{
    GXUPointer chunkIndex = _usedChunkIndex;

    while ( chunkIndex != END_INDEX )
    {
        GXHeapChunk& chunk = _chunks[ chunkIndex ];
        chunk.Release ();
        chunkIndex = chunk._nextIndex;
    }

    chunkIndex = _freeChunkIndex;

    while ( chunkIndex != END_INDEX )
    {
        GXHeapChunk& chunk = _chunks[ chunkIndex ];
        chunk.Release ();
        chunkIndex = chunk._nextIndex;
    }

    GXSafeFree ( _chunks );
}

GXBool GXHeapManager::CheckMemoryLeaks ()
{
    if ( _usedChunkIndex == END_INDEX )
        return GX_TRUE;

    GXLogA ( "GXHeapManager::CheckMemoryLeaks::Error - Memory leaks were detected:\n" );

    GXUPointer leakedMallocObjects = 0u;
    GXUPointer leakedMallocSize = 0u;
    GXUPointer leakedNewObjects = 0u;
    GXUPointer leakedNewSize = 0u;

    for ( GXUPointer i = _usedChunkIndex; i != END_INDEX; )
    {
        GXHeapChunk& chunk = _chunks[ i ];

        switch ( chunk._allocationType )
        {
            case eGXHeapAllocationType::Malloc:
                ++leakedMallocObjects;
                leakedMallocSize += chunk._allocationSize;
                GXLogA ( "\tInitiator class: %s\n\tAllocation type: malloc\n\tAllocation size: %" PRIuPTR " byte(s)\n\n", chunk._initiatorClass, chunk._allocationSize );
            break;

            case eGXHeapAllocationType::New:
                ++leakedNewObjects;
                leakedNewSize += chunk._allocationSize;
                GXLogA ( "\tInitiator class: %s\n\tAllocation type: new\n\tAllocation size: %" PRIuPTR " byte(s)\n\n", chunk._initiatorClass, chunk._allocationSize );
            break;
        }

        i = chunk._nextIndex;
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
    _smartLock.AcquireExclusive ();
    _bindedName = name;
}

GXVoid* GXHeapManager::MakeNew ( GXUPointer size )
{
    GXVoid* memory = AddChunk ( eGXHeapAllocationType::New, size, _bindedName );

    // Note acquire will be done in BindName method.
    _smartLock.ReleaseExclusive ();

    return memory;
}

GXVoid GXHeapManager::MakeDelete ( GXVoid* heapMemory )
{
    RemoveChunk ( heapMemory );
}

GXVoid* GXHeapManager::MakeMalloc ( GXUPointer size, const GXChar* initiatorClass )
{
    _smartLock.AcquireExclusive ();
    GXVoid* memory = AddChunk ( eGXHeapAllocationType::Malloc, size, initiatorClass );
    _smartLock.ReleaseExclusive ();

    return memory;
}

GXVoid* GXHeapManager::MakeRealloc ( GXVoid* heapMemory, GXUPointer newSize, const GXChar* initiatorClass )
{
    if ( !heapMemory )
        return MakeMalloc ( newSize, initiatorClass );

    _smartLock.AcquireExclusive ();

    GXUPointer targetIndex = FindChunkIndex ( heapMemory );

    if ( targetIndex == END_INDEX )
    {
        _smartLock.ReleaseExclusive ();
        GXLogA ( "GXHeapManager::MakeReallocMalloc::Error - Can't find chunk with heap address 0x%p!\n", heapMemory );

        return nullptr;
    }

    GXHeapChunk& target = _chunks[ targetIndex ];
    target._heapMemoryMaxSize = newSize;
    target._heapMemory = realloc ( heapMemory, newSize );

    _smartLock.ReleaseExclusive ();
    return target._heapMemory;
}

GXVoid GXHeapManager::MakeFree ( GXVoid* heapMemory )
{
    RemoveChunk ( heapMemory );
}

GXVoid* GXHeapManager::AddChunk ( eGXHeapAllocationType allocationType, GXUPointer size, const GXChar* initiatorClass )
{
    if ( _freeChunkIndex == END_INDEX && _uninitiatedChunkIndex == END_INDEX )
        GrowChunks ();

    GXHeapChunk* workingChunk = nullptr;

    if ( _freeChunkIndex != END_INDEX )
    {
        workingChunk = _chunks + _freeChunkIndex;
        workingChunk->_previousIndex = END_INDEX;

        GXUPointer nextFreeChunkIndex = workingChunk->_nextIndex;
        workingChunk->_nextIndex = _usedChunkIndex;

        if ( _usedChunkIndex != END_INDEX )
        {
            GXHeapChunk& firstUsedChunk = _chunks[ _usedChunkIndex ];
            firstUsedChunk._previousIndex = _freeChunkIndex;
        }

        _usedChunkIndex = _freeChunkIndex;
        _freeChunkIndex = nextFreeChunkIndex;
    }
    else
    {
        workingChunk = _chunks + _uninitiatedChunkIndex;
        workingChunk->_previousIndex = END_INDEX;
        workingChunk->_nextIndex = _usedChunkIndex;

        if ( _usedChunkIndex != END_INDEX )
        {
            GXHeapChunk& firstUsedChunk = _chunks[ _usedChunkIndex ];
            firstUsedChunk._previousIndex = _uninitiatedChunkIndex;
        }

        _usedChunkIndex = _uninitiatedChunkIndex;
        ++_uninitiatedChunkIndex;

        if ( _uninitiatedChunkIndex >= _chunkCount )
        {
            _uninitiatedChunkIndex = END_INDEX;
        }
    }

    workingChunk->InitData ( allocationType, size, initiatorClass );
    return workingChunk->_heapMemory;
}

GXVoid GXHeapManager::RemoveChunk ( GXVoid* heapMemory )
{
    _smartLock.AcquireExclusive ();

    GXUPointer targetIndex = FindChunkIndex ( heapMemory );

    if ( targetIndex == END_INDEX )
    {
        _smartLock.ReleaseExclusive ();
        GXLogA ( "GXHeapManager::RemoveChunk::Error - Can't find chunk with heap address 0x%p!\n", heapMemory );

        return;
    }

    GXHeapChunk& target = _chunks[ targetIndex ];

    if ( target._nextIndex != END_INDEX )
    {
        GXHeapChunk& chunk = _chunks[ target._nextIndex ];
        chunk._previousIndex = target._previousIndex;
    }

    if ( target._previousIndex != END_INDEX )
    {
        GXHeapChunk& chunk = _chunks[ target._previousIndex ];
        chunk._nextIndex = target._nextIndex;
    }
    else
    {
        _usedChunkIndex = target._nextIndex;
    }

    target._previousIndex = END_INDEX;
    target._nextIndex = _freeChunkIndex;

    if ( _freeChunkIndex != END_INDEX )
    {
        GXHeapChunk& chunk = _chunks[ _freeChunkIndex ];
        chunk._previousIndex = targetIndex;
    }

    _freeChunkIndex = targetIndex;

    _smartLock.ReleaseExclusive ();
}

GXUPointer GXHeapManager::FindChunkIndex ( GXVoid* heapMemory )
{
    for ( GXUPointer i = _usedChunkIndex; i != END_INDEX; )
    {
        GXHeapChunk& chunk = _chunks[ i ];

        if ( chunk._heapMemory == heapMemory )
            return i;

        i = chunk._nextIndex;
    }

    return END_INDEX;
}

GXVoid GXHeapManager::GrowChunks ()
{
    if ( _chunkCount == 0u )
    {
        _chunkCount = 1u;
        _chunks = static_cast<GXHeapChunk*> ( malloc ( sizeof ( GXHeapChunk ) ) );
        memset ( _chunks, 0, sizeof ( GXHeapChunk ) );
        _uninitiatedChunkIndex = 0u;
        return;
    }

    GXHeapChunk* oldChunks = _chunks;
    GXUPointer newChunkCount = _chunkCount * HEAP_CHUNK_GROW_FACTOR;

    _chunks = static_cast<GXHeapChunk*> ( malloc ( newChunkCount * sizeof ( GXHeapChunk ) ) );
    memcpy ( _chunks, oldChunks, _chunkCount * sizeof ( GXHeapChunk ) );
    memset ( _chunks + _chunkCount, 0, ( newChunkCount - _chunkCount ) * sizeof ( GXHeapChunk ) );
    _uninitiatedChunkIndex = _chunkCount;
    _chunkCount = newChunkCount;

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

GXVoid* GXMemoryInspectorFull::Malloc ( GXUPointer size ) const
{
    return gx_HeapManager.MakeMalloc ( size, _className );
}

GXVoid* GXMemoryInspectorFull::Realloc ( GXVoid* heapMemory, GXUPointer newSize ) const
{
    return gx_HeapManager.MakeRealloc ( heapMemory, newSize, _className );
}

GXVoid GXMemoryInspectorFull::Free ( GXVoid* heapMemory ) const
{
    if ( !heapMemory ) return;

    gx_HeapManager.MakeFree ( heapMemory );
}

GXVoid GXMemoryInspectorFull::SafeFree ( GXVoid** heapMemory ) const
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
    if ( !heapMemory ) return;

    gx_HeapManager.MakeDelete ( heapMemory );
}

GXMemoryInspectorFull::GXMemoryInspectorFull ( const GXChar* name )
{
    GXCstrClone ( &_className, name );
}

GXMemoryInspectorFull::~GXMemoryInspectorFull ()
{
    free ( _className );
}
