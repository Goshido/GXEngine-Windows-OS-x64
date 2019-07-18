// version 1.4

#ifndef GX_UINT_ATOMIC_WINDOWS
#define GX_UINT_ATOMIC_WINDOWS


#include <GXCommon/GXUIntAtomic.h>


class GXUIntAtomic final : public GXAbstractUIntAtomic
{
    private:
        volatile LONG       _value;

    public:
        GXUIntAtomic ();
        explicit GXUIntAtomic ( GXUInt value );
        ~GXUIntAtomic () override;

        // Current value will be replaced by exchangeValue if current value is equal compareValue.
        // Compare exchange operation is atomic. Method returns original value.
        GXUInt CompareExchange ( GXUInt compareValue, GXUInt exchangeValue ) override;

        GXUInt Read () const override;
        GXVoid Write ( GXUInt value ) override;

        GXVoid operator = ( GXUInt value ) override;

        GXUInt operator + ( GXUInt value ) override;
        GXUInt operator += ( GXUInt value ) override;
        GXUInt operator ++ () override;

        GXUInt operator - ( GXUInt value ) override;
        GXUInt operator -= ( GXUInt value ) override;
        GXUInt operator -- () override;

        GXBool operator == ( GXUInt testValue ) const override;
        GXBool operator != ( GXUInt testValue ) const override;
        GXBool operator > ( GXUInt testValue ) const override;
        GXBool operator >= ( GXUInt testValue ) const override;
        GXBool operator < ( GXUInt testValue ) const override;
        GXBool operator <= ( GXUInt testValue ) const override;

        // Note it is cast operator to GXUInt.
        operator GXUInt () const override;

    private:
        GXUIntAtomic ( const GXUIntAtomic &other ) = delete;
        GXUIntAtomic& operator = ( const GXUIntAtomic &other ) = delete;
};


#endif // GX_UINT_ATOMIC_WINDOWS
