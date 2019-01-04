// version 1.1

#include <GXCommon/Windows/GXUPointerAtomic.h>


#define DEFAULT_VALUE                                                   0

#ifdef _M_X64

#define GXInterlockedAdd(addend,value)                                  InterlockedAdd64 ( addend, value )
#define GXInterlockedCompareExchange(destination,exchange,comperand)    InterlockedCompareExchange64 ( destination, exchange, comperand )
#define GXInterlockedExchange(value,newValue)                           InterlockedExchange64 ( value, newValue )
#define GXInterlockedDecrement(value)                                   InterlockedDecrement64 ( value )
#define GXInterlockedIncrement(value)                                   InterlockedIncrement64 ( value )

#else

#define GXInterlockedAdd(addend,value)                                  InterlockedAdd ( addend, value )
#define GXInterlockedCompareExchange(destination,exchange,comperand)    InterlockedCompareExchange ( destination, exchange, comperand )
#define GXInterlockedExchange(value,newValue)                           InterlockedExchange ( value, newValue )
#define GXInterlockedDecrement(value)                                   InterlockedDecrement ( value )
#define GXInterlockedIncrement(value)                                   InterlockedIncrement ( value )

#endif

//---------------------------------------------------------------------------------------------------------------------

GXUPointerAtomic::GXUPointerAtomic ():
    v ( DEFAULT_VALUE )
{
    // NOTHING
}

GXUPointerAtomic::GXUPointerAtomic ( GXUPointer value ):
    v ( static_cast<GXAtomicType> ( value ) )
{
    // NOTHING
}

GXUPointerAtomic::~GXUPointerAtomic ()
{
    // NOTHING
}

GXUPointer GXUPointerAtomic::CompareExchange ( GXUPointer compareValue, GXUPointer exchangeValue )
{
    return static_cast<GXUPointer> ( GXInterlockedCompareExchange ( &v, static_cast<GXAtomicType> ( exchangeValue ), static_cast<GXAtomicType> ( compareValue ) ) );
}

GXUPointer GXUPointerAtomic::Read () const
{
    return static_cast<GXUPointer> ( *this );
}

GXVoid GXUPointerAtomic::Write ( GXUPointer newValue )
{
    GXInterlockedExchange ( &v, static_cast<GXAtomicType> ( newValue ) );
}

GXVoid GXUPointerAtomic::operator = ( GXUPointer newValue )
{
    Write ( newValue );
}

GXUPointer GXUPointerAtomic::operator + ( GXUPointer value )
{
    return static_cast<GXUPointer> ( *this ) + value;
}

GXUPointer GXUPointerAtomic::operator += ( GXUPointer value )
{
    return static_cast<GXUPointer> ( GXInterlockedAdd ( &v, static_cast<GXAtomicType> ( value ) ) );
}

GXUPointer GXUPointerAtomic::operator ++ ()
{
    return static_cast<GXUPointer> ( GXInterlockedIncrement ( &v ) );
}

GXUPointer GXUPointerAtomic::operator - ( GXUPointer value )
{
    return static_cast<GXUPointer> ( *this ) - value;
}

GXUPointer GXUPointerAtomic::operator -= ( GXUPointer value )
{
    return static_cast<GXUPointer> ( GXInterlockedAdd ( &v, -static_cast<GXAtomicType> ( value ) ) );
}

GXUPointer GXUPointerAtomic::operator -- ()
{
    return static_cast<GXUPointer> ( GXInterlockedDecrement ( &v ) );
}

GXBool GXUPointerAtomic::operator == ( GXUPointer testValue ) const
{
    return static_cast<GXUPointer> ( *this ) == testValue;
}

GXBool GXUPointerAtomic::operator != ( GXUPointer testValue ) const
{
    return static_cast<GXUPointer> ( *this ) != testValue;
}

GXBool GXUPointerAtomic::operator > ( GXUPointer testValue ) const
{
    return static_cast<GXUPointer> ( *this ) > testValue;
}

GXBool GXUPointerAtomic::operator >= ( GXUPointer testValue ) const
{
    return static_cast<GXUPointer> ( *this ) >= testValue;
}

GXBool GXUPointerAtomic::operator < ( GXUPointer testValue ) const
{
    return static_cast<GXUPointer> ( *this ) < testValue;
}

GXBool GXUPointerAtomic::operator <= ( GXUPointer testValue ) const
{
    return static_cast<GXUPointer> ( *this ) <= testValue;
}

GXUPointerAtomic::operator GXUPointer () const
{
    GXUPointer result = static_cast<GXUPointer> ( v );

#ifdef _M_X64

    // Note _ReadWriteBarrier used by analogy from MSVC std::atomic implementation.
    // According to MSDN it is deprecated method but by design GXEngine doesn't use stardart library.
    // see https://docs.microsoft.com/en-us/cpp/intrinsics/readwritebarrier?view=vs-2015
    _ReadWriteBarrier ();

#else

#error Unsupported platform detected!

#endif

    return result;
}
