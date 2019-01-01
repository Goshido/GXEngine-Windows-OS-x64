// version 1.6

#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>

GX_DISABLE_COMMON_WARNINGS

#include <inttypes.h>

GX_RESTORE_WARNING_STATE


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
        GXLogA ( "\tnew allocated object total size: %" PRIuPTR "\n", static_cast<GXUPointer> ( newObjectSize ) );
    }

    if ( mallocObjects == static_cast<GXUPointer> ( 0u ) )
        return GX_FALSE;

    GXLogA ( "\tmalloc allocated object(s): %" PRIuPTR "\n", static_cast<GXUPointer> ( mallocObjects ) );
    GXLogA ( "\tmalloc allocated object total size: %" PRIuPTR "\n", static_cast<GXUPointer> ( mallocObjectSize ) );

    return GX_FALSE;
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

//---------------------------------------------------------------------------------------------------------------------

class GXHeapManager final
{
    private:
        const GXChar*       bindedName;

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
        GXHeapManager ( const GXHeapManager &other ) = delete;
        GXHeapManager& operator = ( const GXHeapManager &other ) = delete;
};

static GXHeapManager gx_HeapManager;

GXHeapManager::GXHeapManager ()
{
    // NOTHING
}

GXHeapManager::~GXHeapManager ()
{
    // NOTHING
}

GXBool GXHeapManager::CheckMemoryLeaks ()
{
    // TODO
    return GX_FALSE;
}

GXVoid GXHeapManager::BindName ( const GXChar* name )
{
    bindedName = name;
}

GXVoid* GXHeapManager::MakeNew ( GXUPointer /*size*/ )
{
    // TODO
    return nullptr;
}

GXVoid GXHeapManager::MakeDelete ( GXVoid* /*heapMemory*/ )
{
    // TODO
}

GXVoid* GXHeapManager::MakeMalloc ( GXUPointer /*size*/, const GXChar* /*initiatorClass*/ )
{
    // TODO
    return nullptr;
}

GXVoid GXHeapManager::MakeFree ( GXVoid* /*heapMemory*/ )
{
    // TODO
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

GXVoid* GXMemoryInspectorFull::Malloc ( GXUPointer size )
{
    return gx_HeapManager.MakeMalloc ( size, className );
}

GXVoid GXMemoryInspectorFull::Free ( GXVoid* heapMemory )
{
    gx_HeapManager.MakeFree ( heapMemory );
}
