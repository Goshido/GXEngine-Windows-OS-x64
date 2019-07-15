// version 1.7

#ifndef EM_LIGHT
#define EM_LIGHT


#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXMath.h>


enum class eEMLightEmitterType
{
    Bulp,
    Directed,
    Spot,
    Unknown
};

class EMLightEmitter
{
    friend class EMRenderer;

    private:
        GXColorRGB                  _baseColor;
        EMLightEmitter*             _previous;

        EMLightEmitter*             _next;
        GXFloat                     _intensity;     // [ 0.0f, +inf )
        GXColorRGB                  _color;
        static EMLightEmitter*      _emitters;

    protected:
        GXMat4                      _modelMatrix;

    public:
        EMLightEmitter ();
        virtual ~EMLightEmitter ();

        // Must be implemented in derived class.
        virtual eEMLightEmitterType GetType () const;

        GXVoid GetBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue ) const;
        GXVoid SetBaseColor ( GXUByte red, GXUByte green, GXUByte blue );

        GXFloat GetIntensity () const;
        GXVoid SetIntensity ( GXFloat newIntensity );

        const GXColorRGB& GetColor () const;

    private:
        static EMLightEmitter* GetEmitters ();

        EMLightEmitter ( const EMLightEmitter &other ) = delete;
        EMLightEmitter& operator = ( const EMLightEmitter &other ) = delete;
};

class EMBulp final : public EMLightEmitter
{
    private:
        GXMeshGeometry      _lightVolume;

    public:
        EMBulp ();
        ~EMBulp () override;

        eEMLightEmitterType GetType () const override;

        GXVoid SetInfluenceDistance ( GXFloat newDistance );
        GXFloat GetInfluenceDistance ();

        GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
        GXVoid SetLocation ( const GXVec3 &location );
        GXVoid GetLocation ( GXVec3& location );

        GXVoid DrawLightVolume ();

    private:
        EMBulp ( const EMBulp &other ) = delete;
        EMBulp& operator = ( const EMBulp &other ) = delete;
};

class EMSpotlight final : public EMLightEmitter
{
    private:
        GXFloat             _coneAngle;
        GXFloat             _compressionXY;

        GXFloat             _distance;
        GXMat4              _rotationMatrix;
        GXMeshGeometry      _lightVolume;

    public:
        EMSpotlight ();
        ~EMSpotlight () override;

        eEMLightEmitterType GetType () const override;

        GXFloat GetConeAngle () const;
        GXVoid SetConeAngle ( GXFloat radians );

        GXFloat GetInfluenceDistance ();
        GXVoid SetInfluenceDistance ( GXFloat newDistance );

        GXVoid GetLocation ( GXVec3& out ) const;
        GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
        GXVoid SetLocation ( const GXVec3 &location );

        const GXMat4& GetRotation () const;
        GXVoid SetRotation ( const GXMat4 &rotation );
        GXVoid SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );

        GXVoid DrawLightVolume ();

    private:
        EMSpotlight ( const EMSpotlight &other ) = delete;
        EMSpotlight& operator = ( const EMSpotlight &other ) = delete;
};

class EMDirectedLight final : public EMLightEmitter
{
    private:
        GXColorRGB      _ambientBase;

        GXFloat         _ambientIntensity;
        GXColorRGB      _ambientColor;

    public:
        EMDirectedLight ();
        ~EMDirectedLight () override;

        eEMLightEmitterType GetType () const override;

        GXVoid SetAmbientBaseColor ( GXUByte red, GXUByte green, GXUByte blue );
        GXVoid SetAmbientIntensity ( GXFloat newIntensity );

        GXVoid GetAmbientBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue );
        const GXColorRGB& GetAmbientColor ();
        GXFloat GetAmbientIntensity ();

        GXVoid SetRotation ( const GXMat4 &rotation );
        GXVoid SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );

        const GXMat4& GetRotation ();

    private:
        EMDirectedLight ( const EMDirectedLight &other ) = delete;
        EMDirectedLight& operator = ( const EMDirectedLight &other ) = delete;
};


#endif // EM_LIGHT
