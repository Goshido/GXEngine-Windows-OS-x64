// version 1.2

#ifndef GX_UPOINTER_ATOMIC
#define GX_UPOINTER_ATOMIC


#include "GXTypes.h"


class GXAbstractUPointerAtomic
{
    public:
        // Current value will be replaced by exchangeValue if current value is equal compareValue.
        // Compare exchange operation is atomic. Method returns original value.
        virtual GXUPointer CompareExchange ( GXUPointer compareValue, GXUPointer exchangeValue ) = 0;

        virtual GXUPointer Read () const = 0;
        virtual GXVoid Write ( GXUPointer newValue ) = 0;

        virtual GXVoid operator = ( GXUPointer newValue ) = 0;

        virtual GXUPointer operator + ( GXUPointer value ) = 0;
        virtual GXUPointer operator += ( GXUPointer value ) = 0;
        virtual GXUPointer operator ++ () = 0;

        virtual GXUPointer operator - ( GXUPointer value ) = 0;
        virtual GXUPointer operator -= ( GXUPointer value ) = 0;
        virtual GXUPointer operator -- () = 0;

        virtual GXBool operator == ( GXUPointer testValue ) const = 0;
        virtual GXBool operator != ( GXUPointer testValue ) const = 0;
        virtual GXBool operator > ( GXUPointer testValue ) const = 0;
        virtual GXBool operator >= ( GXUPointer testValue ) const = 0;
        virtual GXBool operator < ( GXUPointer testValue ) const = 0;
        virtual GXBool operator <= ( GXUPointer testValue ) const = 0;

        // Note it is cast cast to GXUInt.
        virtual operator GXUPointer () const = 0;

    protected:
        GXAbstractUPointerAtomic ();
        virtual ~GXAbstractUPointerAtomic ();

    private:
        GXAbstractUPointerAtomic ( const GXAbstractUPointerAtomic &other ) = delete;
        GXAbstractUPointerAtomic& operator = ( const GXAbstractUPointerAtomic &other ) = delete;
};


#ifdef __GNUC__

// TODO implement this
#include "Posix/GXUPointerAtomic.h"

#else

#include "Windows/GXUPointerAtomic.h"

#endif // __GNU__


#endif // GX_UINT_ATOMIC
