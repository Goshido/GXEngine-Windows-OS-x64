// version 1.6

#ifndef GX_MEMORY
#define GX_MEMORY


#include "GXUPointerAtomic.h"

GX_DISABLE_COMMON_WARNINGS

#include <cstdlib>
#include <cstring>

GX_RESTORE_WARNING_STATE


#define GXSafeDelete(ptr)       \
        if ( ptr )              \
        {                       \
            delete ptr;         \
            ptr = nullptr;      \
        }

#define GXSafeFree(ptr)         \
        if ( ptr )              \
        {                       \
            free ( ptr );       \
            ptr = nullptr;      \
        }

//---------------------------------------------------------------------------------------------------------------------

class GXMemoryInspectorLite
{
    private:
        static GXUPointerAtomic     newObjects;
        static GXUPointerAtomic     newObjectSize;

        static GXUPointerAtomic     mallocObjects;
        static GXUPointerAtomic     mallocObjectSize;

    public:
        // Method returns GX_TRUE if no any memory leaks are detected.
        static GXBool GXCALL CheckMemoryLeaks ();

        static GXVoid* operator new ( GXUPointer size );
        static GXVoid operator delete ( GXVoid* heapMemory );

    protected:
        GXMemoryInspectorLite ();
        virtual ~GXMemoryInspectorLite ();

        GXVoid* Malloc ( GXUPointer size );
        GXVoid Free ( GXVoid* heapMemory );
};

class GXMemoryInspectorFull
{
    private:
        GXChar*     className;

    public:
        // Mandatory method for "new" ecosystem.
        // Note this method is invoked implicitly via GX_BIND_MEMORY_INSPECTOR_CLASS_NAME macro!
        static GXVoid GXCALL BindClassName ( const GXChar* className );

        // Method returns GX_TRUE if no any memory leaks are detected.
        static GXBool GXCALL CheckMemoryLeaks ();

        static GXVoid* operator new ( GXUPointer size );
        static GXVoid operator delete ( GXVoid* heapMemory );

    protected:
        explicit GXMemoryInspectorFull ( const GXChar* name );
        virtual ~GXMemoryInspectorFull ();

        GXVoid* Malloc ( GXUPointer size );
        GXVoid Free ( GXVoid* heapMemory );

    private:
        GXMemoryInspectorFull () = delete;
};

//GX_INSPECT_MEMORY
//GX_FULL_MEMORY_LEAK_INFO

#ifdef GX_FULL_MEMORY_LEAK_INFO

#define GXMemoryInspector GXMemoryInspectorFull

#define GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE(name)        \
            : GXMemoryInspectorFull ( name )

#define GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST(name)      \
            : GXMemoryInspectorFull ( name ),

#define GX_BIND_MEMORY_INSPECTOR_CLASS_NAME(name)           \
            GXMemoryInspectorFull::BindClassName ( name )

#else

#define GXMemoryInspector GXMemoryInspectorLite
#define GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE(name)
#define GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST(name) :
#define GX_BIND_MEMORY_INSPECTOR_CLASS_NAME(name)

#endif

//---------------------------------------------------------------------------------------------------------------------

class GXCircleBuffer final : public GXMemoryInspector
{
    private:
        GXUPointer      size;
        GXUPointer      offset;
        GXUByte*        buffer;

    public:
        explicit GXCircleBuffer ( GXUPointer size );
        ~GXCircleBuffer ();

        GXVoid* Allocate ( GXUPointer bytes );

    private:
        GXCircleBuffer ( const GXCircleBuffer &other ) = delete;
        GXCircleBuffer& operator = ( const GXCircleBuffer &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

class GXDynamicArray final
{
    protected:
        GXUByte*        data;
        GXUPointer      elementSize;
        GXUPointer      numElements;

    public:
        explicit GXDynamicArray ( GXUPointer elementSize );
        ~GXDynamicArray ();

        GXVoid SetValue ( GXUPointer i, const GXVoid* element );
        GXVoid* GetValue ( GXUPointer i ) const;
        GXVoid* GetData () const;
        GXUPointer GetLength () const;

        GXVoid Resize ( GXUPointer totalElements );

    private:
        GXDynamicArray ( const GXDynamicArray &other ) = delete;
        GXDynamicArray& operator = ( const GXDynamicArray &other ) = delete;
};


#endif // GX_MEMORY
