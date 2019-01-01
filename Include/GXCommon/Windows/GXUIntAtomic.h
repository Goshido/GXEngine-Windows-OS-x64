// version 1.2

#ifndef GX_UINT_ATOMIC_WINDOWS
#define GX_UINT_ATOMIC_WINDOWS


#include <GXCommon/GXUIntAtomic.h>


class GXUIntAtomic final : public GXAbstractUIntAtomic
{
    private:
        volatile LONG       v;

    public:
        GXUIntAtomic ();
        explicit GXUIntAtomic ( GXUInt value );
        ~GXUIntAtomic () override;

        GXUInt Read () const override;
        GXVoid Write ( GXUInt newValue ) override;

        GXVoid operator = ( GXUInt newValue ) override;

        // Note it is cast operator to GXUInt.
        operator GXUInt () const override;

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

    private:
        GXUIntAtomic ( const GXUIntAtomic &other ) = delete;
        GXUIntAtomic& operator = ( const GXUIntAtomic &other ) = delete;
};


#endif // GX_UINT_ATOMIC_WINDOWS
