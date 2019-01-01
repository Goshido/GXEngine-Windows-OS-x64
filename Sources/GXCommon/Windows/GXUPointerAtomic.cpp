// version 1.0

#include <GXCommon/Windows/GXUPointerAtomic.h>


#define DEFAULT_VALUE                           0

#ifdef _M_X64

#define GXInterlockedExchange(value,newValue)   InterlockedExchange64 ( value, newValue )
#define GXInterlockedIncrement(value)           InterlockedIncrement64 ( value )
#define GXInterlockedDecrement(value)           InterlockedDecrement64 ( value )
#define GXInterlockedAdd(addend,value)          InterlockedAdd64 ( addend, value )

#else

#define GXInterlockedExchange(value,newValue)   InterlockedExchange ( value, newValue )
#define GXInterlockedIncrement(value)           InterlockedIncrement ( value )
#define GXInterlockedDecrement(value)           InterlockedDecrement ( value )
#define GXInterlockedAdd(addend,value)          InterlockedAdd ( addend, value )

#endif

//---------------------------------------------------------------------------------------------------------------------

GXUPointerAtomic::GXUPointerAtomic ():
    v ( DEFAULT_VALUE )
{
    // NOTHING
}

GXUPointerAtomic::GXUPointerAtomic ( GXUPointer value ):
    v ( static_cast<LONG64> ( value ) )
{
    // NOTHING
}

GXUPointerAtomic::~GXUPointerAtomic ()
{
    // NOTHING
}

GXUPointer GXUPointerAtomic::Read () const
{
    return static_cast<GXUPointer> ( *this );
}

GXVoid GXUPointerAtomic::Write ( GXUPointer newValue )
{
    GXInterlockedExchange ( &v, static_cast<LONG64> ( newValue ) );
}

GXVoid GXUPointerAtomic::operator = ( GXUPointer newValue )
{
    Write ( newValue );
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

GXUPointer GXUPointerAtomic::operator + ( GXUPointer value )
{
    return static_cast<GXUPointer> ( *this ) + value;
}

GXUPointer GXUPointerAtomic::operator += ( GXUPointer value )
{
    return static_cast<GXUPointer> ( GXInterlockedAdd ( &v, static_cast<LONG64> ( value ) ) );
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
    return static_cast<GXUPointer> ( GXInterlockedAdd ( &v, -static_cast<LONG64> ( value ) ) );
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
