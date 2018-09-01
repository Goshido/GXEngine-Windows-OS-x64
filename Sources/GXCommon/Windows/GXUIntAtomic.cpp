// version 1.0

#include <GXCommon/Windows/GXUIntAtomic.h>


#define DEFAULT_VALUE	0u

//---------------------------------------------------------------------------------------------------------------------

GXUIntAtomic::GXUIntAtomic ():
	value ( DEFAULT_VALUE )
{
	// NOTHING
}

GXUIntAtomic::GXUIntAtomic ( GXUInt value ):
	value ( value )
{
	// NOTHING
}

GXUIntAtomic::~GXUIntAtomic ()
{
	// NOTHING
}

GXUInt GXUIntAtomic::Read () const
{
	return static_cast<GXUInt> ( *this );
}

GXVoid GXUIntAtomic::Write ( GXUInt newValue )
{
	InterlockedExchange ( &value, newValue );
}

GXVoid GXUIntAtomic::operator = ( GXUInt newValue )
{
	Write ( newValue );
}

GXUIntAtomic::operator GXUInt () const
{
	GXUInt result = value;

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

GXUInt GXUIntAtomic::operator ++ ()
{
	return InterlockedIncrement ( &value );
}

GXUInt GXUIntAtomic::operator -- ()
{
	return InterlockedDecrement ( &value );
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
