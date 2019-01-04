// version 1.3

#include <GXCommon/Windows/GXUIntAtomic.h>


#define DEFAULT_VALUE       0

//---------------------------------------------------------------------------------------------------------------------

GXUIntAtomic::GXUIntAtomic ():
    v ( DEFAULT_VALUE )
{
    // NOTHING
}

GXUIntAtomic::GXUIntAtomic ( GXUInt value ):
    v ( static_cast<LONG> ( value ) )
{
    // NOTHING
}

GXUIntAtomic::~GXUIntAtomic ()
{
    // NOTHING
}

GXUInt GXUIntAtomic::CompareExchange ( GXUInt compareValue, GXUInt exchangeValue )
{
    return static_cast<GXUInt> ( InterlockedCompareExchange ( reinterpret_cast<volatile GXULong*> ( &v ), exchangeValue, compareValue ) );
}

GXUInt GXUIntAtomic::Read () const
{
    return static_cast<GXUInt> ( *this );
}

GXVoid GXUIntAtomic::Write ( GXUInt newValue )
{
    InterlockedExchange ( reinterpret_cast<volatile GXULong*> ( &v ), static_cast<GXULong> ( newValue ) );
}

GXVoid GXUIntAtomic::operator = ( GXUInt newValue )
{
    Write ( newValue );
}

GXUInt GXUIntAtomic::operator + ( GXUInt value )
{
    return static_cast<GXUInt> ( *this ) + value;
}

GXUInt GXUIntAtomic::operator += ( GXUInt value )
{
    return static_cast<GXUInt> ( InterlockedAdd ( &v, static_cast<LONG> ( value ) ) );
}

GXUInt GXUIntAtomic::operator ++ ()
{
    return static_cast<GXUInt> ( InterlockedIncrement ( &v ) );
}

GXUInt GXUIntAtomic::operator - ( GXUInt value )
{
    return static_cast<GXUInt> ( *this ) - value;
}

GXUInt GXUIntAtomic::operator -= ( GXUInt value )
{
    return static_cast<GXUInt> ( InterlockedAdd ( &v, -static_cast<LONG> ( value ) ) );
}

GXUInt GXUIntAtomic::operator -- ()
{
    return static_cast<GXUInt> ( InterlockedDecrement ( &v ) );
}

GXBool GXUIntAtomic::operator == ( GXUInt testValue ) const
{
    return static_cast<GXUInt> ( *this ) == testValue;
}

GXBool GXUIntAtomic::operator != ( GXUInt testValue ) const
{
    return static_cast<GXUInt> ( *this ) != testValue;
}

GXBool GXUIntAtomic::operator > ( GXUInt testValue ) const
{
    return static_cast<GXUInt> ( *this ) > testValue;
}

GXBool GXUIntAtomic::operator >= ( GXUInt testValue ) const
{
    return static_cast<GXUInt> ( *this ) >= testValue;
}

GXBool GXUIntAtomic::operator < ( GXUInt testValue ) const
{
    return static_cast<GXUInt> ( *this ) < testValue;
}

GXBool GXUIntAtomic::operator <= ( GXUInt testValue ) const
{
    return static_cast<GXUInt> ( *this ) <= testValue;
}

GXUIntAtomic::operator GXUInt () const
{
    GXUInt result = static_cast<GXUInt> ( v );

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
