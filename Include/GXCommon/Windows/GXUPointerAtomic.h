// version 1.2

#ifndef GX_UPOINTER_ATOMIC_WINDOWS
#define GX_UPOINTER_ATOMIC_WINDOWS


#include <GXCommon/GXUPointerAtomic.h>


class GXUPointerAtomic final : public GXAbstractUPointerAtomic
{
    private:

#ifdef _M_X64
        typedef LONG64              GXAtomicType;
#else
        typedef LONG                GXAtomicType;
#endif

        volatile GXAtomicType       _value;

    public:
        GXUPointerAtomic ();
        explicit GXUPointerAtomic ( GXUPointer value );
        ~GXUPointerAtomic () override;

        // Current value will be replaced by exchangeValue if current value is equal compareValue.
        // Compare exchange operation is atomic. Method returns original value.
        GXUPointer CompareExchange ( GXUPointer compareValue, GXUPointer exchangeValue ) override;

        GXUPointer Read () const override;
        GXVoid Write ( GXUPointer value ) override;

        GXVoid operator = ( GXUPointer value ) override;

        // Note it is cast operator to GXUPointer.
        operator GXUPointer () const override;

        GXUPointer operator + ( GXUPointer value ) override;
        GXUPointer operator += ( GXUPointer value ) override;
        GXUPointer operator ++ () override;

        GXUPointer operator - ( GXUPointer value ) override;
        GXUPointer operator -= ( GXUPointer value ) override;
        GXUPointer operator -- () override;

        GXBool operator == ( GXUPointer testValue ) const override;
        GXBool operator != ( GXUPointer testValue ) const override;
        GXBool operator > ( GXUPointer testValue ) const override;
        GXBool operator >= ( GXUPointer testValue ) const override;
        GXBool operator < ( GXUPointer testValue ) const override;
        GXBool operator <= ( GXUPointer testValue ) const override;

    private:
        GXUPointerAtomic ( const GXUPointerAtomic &other ) = delete;
        GXUPointerAtomic& operator = ( const GXUPointerAtomic &other ) = delete;
};


#endif // GX_UPOINTER_ATOMIC_WINDOWS
