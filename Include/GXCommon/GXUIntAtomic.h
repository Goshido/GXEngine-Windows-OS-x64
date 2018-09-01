// version 1.0

#ifndef GX_UINT_ATOMIC
#define GX_UINT_ATOMIC


#include "GXTypes.h"


class GXAbstractUIntAtomic
{
	public:
		virtual GXUInt Read () const = 0;
		virtual GXVoid Write ( GXUInt newValue ) = 0;

		virtual GXVoid operator = ( GXUInt newValue ) = 0;

		// Note it is cast cast to GXUInt.
		virtual operator GXUInt () const = 0;

		virtual GXUInt operator ++ () = 0;
		virtual GXUInt operator -- () = 0;

		virtual GXBool operator == ( GXUInt testValue ) const = 0;
		virtual GXBool operator != ( GXUInt testValue ) const = 0;
		virtual GXBool operator > ( GXUInt testValue ) const = 0;
		virtual GXBool operator >= ( GXUInt testValue ) const = 0;
		virtual GXBool operator < ( GXUInt testValue ) const = 0;
		virtual GXBool operator <= ( GXUInt testValue ) const = 0;

	protected:
		GXAbstractUIntAtomic ();
		virtual ~GXAbstractUIntAtomic ();

	private:
		GXAbstractUIntAtomic ( const GXAbstractUIntAtomic &other ) = delete;
		GXAbstractUIntAtomic& operator = ( const GXAbstractUIntAtomic &other ) = delete;
};


#ifdef __GNUC__
// TODO implement this
#include "Posix/GXUIntAtomic.h"
#else
#include "Windows/GXUIntAtomic.h"
#endif // __GNU__


#endif // GX_UINT_ATOMIC
