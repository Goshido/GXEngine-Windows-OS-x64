// version 1.0

#ifndef GX_UPOINTER_ATOMIC_WINDOWS
#define GX_UPOINTER_ATOMIC_WINDOWS


#include <GXCommon/GXUPointerAtomic.h>


class GXUPointerAtomic final : public GXAbstractUPointerAtomic
{
    private:
        volatile LONG64     v;

    public:
        GXUPointerAtomic ();
        explicit GXUPointerAtomic ( GXUPointer value );
        ~GXUPointerAtomic () override;

        GXUPointer Read () const override;
        GXVoid Write ( GXUPointer newValue ) override;

        GXVoid operator = ( GXUPointer newValue ) override;

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
