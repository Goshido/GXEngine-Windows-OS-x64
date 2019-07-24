// version 1.10

#ifndef GX_MEMORY
#define GX_MEMORY


#include "GXUPointerAtomic.h"

GX_DISABLE_COMMON_WARNINGS

#include <cstdlib>
#include <cstring>

GX_RESTORE_WARNING_STATE


#define GXSafeDelete(ptr)               \
        if ( ptr )                      \
        {                               \
            delete ptr;                 \
            ptr = nullptr;              \
        }

// Note memory inspector successor class MUST use GXMemoryInspector::SafeFree
// for each GXMemoryInspector::Malloc allocated obect instead!
#define GXSafeFree(ptr)                 \
        if ( ptr )                      \
        {                               \
            free ( ptr );               \
            ptr = nullptr;              \
        }

//---------------------------------------------------------------------------------------------------------------------

class GXMemoryInspectorLite
{
    public:
        // Method returns GX_TRUE if no any memory leaks are detected.
        static GXBool GXCALL CheckMemoryLeaks ();

        GXVoid* Malloc ( GXUPointer size ) const;
        GXVoid* Realloc ( GXVoid* heapMemory, GXUPointer newSize ) const;
        GXVoid Free ( GXVoid* heapMemory ) const;
        GXVoid SafeFree ( GXVoid** heapMemory ) const;

        static GXVoid* operator new ( GXUPointer size );
        static GXVoid operator delete ( GXVoid* heapMemory );

    protected:
        GXMemoryInspectorLite ();
        virtual ~GXMemoryInspectorLite ();
};

class GXMemoryInspectorFull
{
    private:
        GXChar*     _className;

    public:
        // Mandatory method for "new" ecosystem.
        // Note this method is invoked implicitly via GX_BIND_MEMORY_INSPECTOR_CLASS_NAME macro!
        static GXVoid GXCALL BindClassName ( const GXChar* className );

        // Method returns GX_TRUE if no any memory leaks are detected.
        static GXBool GXCALL CheckMemoryLeaks ();

        GXVoid* Malloc ( GXUPointer size ) const;
        GXVoid* Realloc ( GXVoid* heapMemory, GXUPointer newSize ) const;
        GXVoid Free ( GXVoid* heapMemory ) const;
        GXVoid SafeFree ( GXVoid** heapMemory ) const;

        static GXVoid* operator new ( GXUPointer size );
        static GXVoid operator delete ( GXVoid* heapMemory );

    protected:
        explicit GXMemoryInspectorFull ( const GXChar* name );
        virtual ~GXMemoryInspectorFull ();

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
        GXUPointer      _size;
        GXUPointer      _offset;
        GXUByte*        _buffer;

    public:
        explicit GXCircleBuffer ( GXUPointer size );
        ~GXCircleBuffer ();

        GXVoid* Allocate ( GXUPointer bytes );

    private:
        GXCircleBuffer ( const GXCircleBuffer &other ) = delete;
        GXCircleBuffer& operator = ( const GXCircleBuffer &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

class GXDynamicArray final : public GXMemoryInspector
{
    protected:
        GXUPointer      _elementSize;
        GXUPointer      _elements;
        GXUPointer      _reservedElements;

        GXUByte*        _data;

    public:
        explicit GXDynamicArray ( GXUPointer elementSize );

        // reservedElements is internal buffer capacity in elements at start.
        explicit GXDynamicArray ( GXUPointer elementSize, GXUPointer reservedElements );

        ~GXDynamicArray () override;

        GXVoid Reserve ( GXUPointer elements );

        // Internal buffer doubled if new element cannot be added.
        GXVoid PushBack ( const GXVoid* element );

        GXVoid SetValue ( GXUPointer i, const GXVoid* element );
        GXVoid* GetValue ( GXUPointer i ) const;

        GXVoid* GetData () const;
        GXUPointer GetLength () const;

        // Internal buffer is resized to specific element count.
        GXVoid Resize ( GXUPointer elements );

    private:
        GXVoid PreserveElements ( GXUByte* destination, const GXUByte* source, GXUPointer targetElements );

        GXDynamicArray ( const GXDynamicArray &other ) = delete;
        GXDynamicArray& operator = ( const GXDynamicArray &other ) = delete;
};


#endif // GX_MEMORY
