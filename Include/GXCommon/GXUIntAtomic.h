// version 1.4

#ifndef GX_UINT_ATOMIC
#define GX_UINT_ATOMIC


#include "GXTypes.h"


class GXAbstractUIntAtomic
{
    public:
        // Current value will be replaced by exchangeValue if current value is equal compareValue.
        // Compare exchange operation is atomic. Method returns original value.
        virtual GXUInt CompareExchange ( GXUInt compareValue, GXUInt exchangeValue ) = 0;

        virtual GXUInt Read () const = 0;
        virtual GXVoid Write ( GXUInt newValue ) = 0;

        virtual GXVoid operator = ( GXUInt newValue ) = 0;

        virtual GXUInt operator + ( GXUInt value ) = 0;
        virtual GXUInt operator += ( GXUInt value ) = 0;
        virtual GXUInt operator ++ () = 0;

        virtual GXUInt operator - ( GXUInt value ) = 0;
        virtual GXUInt operator -= ( GXUInt value ) = 0;
        virtual GXUInt operator -- () = 0;

        virtual GXBool operator == ( GXUInt testValue ) const = 0;
        virtual GXBool operator != ( GXUInt testValue ) const = 0;
        virtual GXBool operator > ( GXUInt testValue ) const = 0;
        virtual GXBool operator >= ( GXUInt testValue ) const = 0;
        virtual GXBool operator < ( GXUInt testValue ) const = 0;
        virtual GXBool operator <= ( GXUInt testValue ) const = 0;

        // Note it is cast to GXUInt.
        virtual operator GXUInt () const = 0;

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
