#include <GXEngine_Editor_Mobile/EMLight.h>
#include <GXEngine/GXShaderProgram.h>


#define DEFAULT_COLOR_RED                           255u
#define DEFAULT_COLOR_GREEN                         255u
#define DEFAULT_COLOR_BLUE                          255u
#define DEFAULT_COLOR_ALPHA                         255u

#define DEFAULT_AMBIENT_COLOR_RED                   0u
#define DEFAULT_AMBIENT_COLOR_GREEN                 0u
#define DEFAULT_AMBIENT_COLOR_BLUE                  0u
#define DEFAULT_AMBIENT_COLOR_ALPHA                 0u

#define DEFAULT_INTENSITY                           1.0f

#define DEFAULT_BULP_INFLUENSE_DISTANCE             1.0f

#define DEFAULT_SPOT_LIGHT_CONE_ANGLE               90.0f
#define DEFAULT_SPOT_LIGHT_COMPRESSION_XY           1.0f
#define DEFAULT_SPOT_LIGHT_INFLUENSE_DISTANCE       10.0f

#define DEFAULT_SPOT_LIGHT_LOCATION_X               0.0f
#define DEFAULT_SPOT_LIGHT_LOCATION_Y               0.0f
#define DEFAULT_SPOT_LIGHT_LOCATION_Z               0.0f

#define DEFAULT_SPOT_LIGHT_ROTATION_PITCH           0.0f
#define DEFAULT_SPOT_LIGHT_ROTATION_YAW             0.0f
#define DEFAULT_SPOT_LIGHT_ROTATION_ROLL            0.0f

#define DEFAULT_DIRECTED_LIGHT_AMBIENT_RED          0u
#define DEFAULT_DIRECTED_LIGHT_AMBIENT_GREEN        0u
#define DEFAULT_DIRECTED_LIGHT_AMBIENT_BLUE         0u
#define DEFAULT_DIRECTED_LIGHT_AMBIENT_ALPHA        255u

#define DEFAULT_DIRECTED_LIGHT_AMBIENT_INTENSITY    0.0f

#define DEFAULT_DIRECTED_LIGHT_ROTATION_PITCH       GX_MATH_HALF_PI
#define DEFAULT_DIRECTED_LIGHT_ROTATION_YAW         0.0f
#define DEFAULT_DIRECTED_LIGHT_ROTATION_ROLL        0.0f

#define COLOR_FACTOR                                0.00392f

#define BULP_LIGHT_VOLUME_MESH                      L"Meshes/Editor Mobile/Bulp light volume.stm"
#define SPOT_LIGHT_VOLUME_MESH                      L"Meshes/Editor Mobile/Spot light volume.stm"

//---------------------------------------------------------------

EMLightEmitter* EMLightEmitter::_emitters = nullptr;

EMLightEmitter::EMLightEmitter ():
    _baseColor ( static_cast<GXUByte> ( DEFAULT_COLOR_RED ), static_cast<GXUByte> ( DEFAULT_COLOR_GREEN ), static_cast<GXUByte> ( DEFAULT_COLOR_BLUE ), static_cast<GXUByte> ( DEFAULT_COLOR_ALPHA ) ),
    _previous ( nullptr )
{
    _next = _emitters;

    if ( _emitters )
        _emitters->_previous = this;

    _emitters = this;

    SetIntensity ( DEFAULT_INTENSITY );
    _modelMatrix.Identity ();
}

EMLightEmitter::~EMLightEmitter ()
{
    if ( _next ) _next->_previous = _previous;

    if ( _previous )
    {
        _previous->_next = _next;
        return;
    }

    _emitters = _next;
}

eEMLightEmitterType EMLightEmitter::GetType () const
{
    return eEMLightEmitterType::Unknown;
}

GXVoid EMLightEmitter::GetBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue ) const
{
    red = static_cast<GXUByte> ( _baseColor._data[ 0u ] * 255.0f );
    green = static_cast<GXUByte> ( _baseColor._data[ 1u ] * 255.0f );
    blue = static_cast<GXUByte> ( _baseColor._data[ 2u ] * 255.0f );
}

GXVoid EMLightEmitter::SetBaseColor ( GXUByte red, GXUByte green, GXUByte blue )
{
    _baseColor.From ( red, green, blue, static_cast<GXUByte> ( DEFAULT_COLOR_ALPHA ) );

    _color._data[ 0u ] = ( _baseColor._data[ 0u ] * _intensity );
    _color._data[ 1u ] = ( _baseColor._data[ 1u ] * _intensity );
    _color._data[ 2u ] = ( _baseColor._data[ 2u ] * _intensity );
}

GXFloat EMLightEmitter::GetIntensity () const
{
    return _intensity;
}

GXVoid EMLightEmitter::SetIntensity ( GXFloat newIntensity )
{
    _intensity = newIntensity;

    _color._data[ 0u ] = ( _baseColor._data[ 0u ] * _intensity );
    _color._data[ 1u ] = ( _baseColor._data[ 1u ] * _intensity );
    _color._data[ 2u ] = ( _baseColor._data[ 2u ] * _intensity );
}

const GXColorRGB& EMLightEmitter::GetColor () const
{
    return _color;
}

EMLightEmitter* EMLightEmitter::GetEmitters ()
{
    return _emitters;
}

//---------------------------------------------------------------

EMBulp::EMBulp ()
{
    SetInfluenceDistance ( DEFAULT_BULP_INFLUENSE_DISTANCE );
    _lightVolume.LoadMesh ( BULP_LIGHT_VOLUME_MESH );
}

EMBulp::~EMBulp ()
{
    // NOTHING
}

eEMLightEmitterType EMBulp::GetType () const
{
    return eEMLightEmitterType::Bulp;
}

GXVoid EMBulp::SetInfluenceDistance ( GXFloat newDistance )
{
    _modelMatrix._m[ 0u ][ 0u ] = _modelMatrix._m[ 1u ][ 1u ] = _modelMatrix._m[ 2u ][ 2u ] = newDistance;
}

GXFloat EMBulp::GetInfluenceDistance ()
{
    return _modelMatrix._m[ 1u ][ 1u ];
}

GXVoid EMBulp::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
    _modelMatrix._m[ 3u ][ 0u ] = x;
    _modelMatrix._m[ 3u ][ 1u ] = y;
    _modelMatrix._m[ 3u ][ 2u ] = z;
}

GXVoid EMBulp::SetLocation ( const GXVec3 &location )
{
    _modelMatrix.SetW ( location );
}

GXVoid EMBulp::GetLocation ( GXVec3& location )
{
    _modelMatrix.GetW ( location );
}

GXVoid EMBulp::DrawLightVolume ()
{
    _lightVolume.Render ();
}

//---------------------------------------------------------------

EMSpotlight::EMSpotlight ():
    _coneAngle ( GXDegToRad ( DEFAULT_SPOT_LIGHT_CONE_ANGLE ) ),
    _compressionXY ( DEFAULT_SPOT_LIGHT_COMPRESSION_XY )
{
    _modelMatrix._m[ 3u ][ 0u ] = DEFAULT_SPOT_LIGHT_LOCATION_X;
    _modelMatrix._m[ 3u ][ 1u ] = DEFAULT_SPOT_LIGHT_LOCATION_Y;
    _modelMatrix._m[ 3u ][ 2u ] = DEFAULT_SPOT_LIGHT_LOCATION_Z;

    _rotationMatrix.RotationXYZ ( DEFAULT_SPOT_LIGHT_ROTATION_PITCH, DEFAULT_SPOT_LIGHT_ROTATION_YAW, DEFAULT_SPOT_LIGHT_ROTATION_ROLL );

    SetInfluenceDistance ( DEFAULT_SPOT_LIGHT_INFLUENSE_DISTANCE );
    _lightVolume.LoadMesh ( SPOT_LIGHT_VOLUME_MESH );
}

EMSpotlight::~EMSpotlight ()
{
    // NOTHING
}

eEMLightEmitterType EMSpotlight::GetType () const
{
    return eEMLightEmitterType::Spot;
}

GXFloat EMSpotlight::GetConeAngle () const
{
    return _coneAngle;
}

GXVoid EMSpotlight::SetConeAngle ( GXFloat radians )
{
    _coneAngle = radians;
    _compressionXY = tanf ( _coneAngle * 0.5f );
    SetInfluenceDistance ( _distance );
}

GXVoid EMSpotlight::SetInfluenceDistance ( GXFloat newDistance )
{
    _distance = newDistance;

    GXMat4 scaleMatrix;
    scaleMatrix.Scale ( _distance * _compressionXY, _distance * _compressionXY, _distance );

    GXVec3 location;
    _modelMatrix.GetW ( location );

    _modelMatrix.Multiply ( scaleMatrix, _rotationMatrix );
    _modelMatrix.SetW ( location );
}

GXFloat EMSpotlight::GetInfluenceDistance ()
{
    return _distance;
}

GXVoid EMSpotlight::GetLocation ( GXVec3 &out ) const
{
    _modelMatrix.GetW ( out );
}

GXVoid EMSpotlight::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
    _modelMatrix._m[ 3u ][ 0u ] = x;
    _modelMatrix._m[ 3u ][ 1u ] = y;
    _modelMatrix._m[ 3u ][ 2u ] = z;
}

GXVoid EMSpotlight::SetLocation ( const GXVec3 &location )
{
    _modelMatrix.SetW ( location );
}

const GXMat4& EMSpotlight::GetRotation () const
{
    return _rotationMatrix;
}

GXVoid EMSpotlight::SetRotation ( const GXMat4 &rotation )
{
    _rotationMatrix = rotation;
    SetInfluenceDistance ( _distance );
}

GXVoid EMSpotlight::SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
    _rotationMatrix.RotationXYZ ( pitchRadians, yawRadians, rollRadians );
    SetInfluenceDistance ( _distance );
}

GXVoid EMSpotlight::DrawLightVolume ()
{
    _lightVolume.Render ();
}

//---------------------------------------------------------------

EMDirectedLight::EMDirectedLight ():
    _ambientBase ( static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_RED ), static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_GREEN ), static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_BLUE ), static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_ALPHA ) )
{ 
    SetRotation ( DEFAULT_DIRECTED_LIGHT_ROTATION_PITCH, DEFAULT_DIRECTED_LIGHT_ROTATION_YAW, DEFAULT_DIRECTED_LIGHT_ROTATION_ROLL );
    SetAmbientIntensity ( DEFAULT_DIRECTED_LIGHT_AMBIENT_INTENSITY );
}

EMDirectedLight::~EMDirectedLight ()
{
    // NOTHING
}

eEMLightEmitterType EMDirectedLight::GetType () const
{
    return eEMLightEmitterType::Directed;
}

GXVoid EMDirectedLight::SetAmbientBaseColor ( GXUByte red, GXUByte green, GXUByte blue )
{
    _ambientBase.From ( red, green, blue, static_cast<GXUByte> ( DEFAULT_AMBIENT_COLOR_ALPHA ) );

    _ambientColor._data[ 0u ] = _ambientBase._data[ 0u ] * _ambientIntensity;
    _ambientColor._data[ 1u ] = _ambientBase._data[ 1u ] * _ambientIntensity;
    _ambientColor._data[ 2u ] = _ambientBase._data[ 2u ] * _ambientIntensity;
}

GXVoid EMDirectedLight::SetAmbientIntensity ( GXFloat newIntensity )
{
    _ambientIntensity = newIntensity;

    _ambientColor._data[ 0u ] = _ambientBase._data[ 0u ] * _ambientIntensity;
    _ambientColor._data[ 1u ] = _ambientBase._data[ 1u ] * _ambientIntensity;
    _ambientColor._data[ 2u ] = _ambientBase._data[ 2u ] * _ambientIntensity;
}

GXVoid EMDirectedLight::GetAmbientBaseColor ( GXUByte &red, GXUByte &green, GXUByte &blue )
{
    red = static_cast<GXUByte> ( _ambientBase._data[ 0u ] * 255.0f );
    green = static_cast<GXUByte> ( _ambientBase._data[ 1u ] * 255.0f );
    blue = static_cast<GXUByte> ( _ambientBase._data[ 2u ] * 255.0f );
}

const GXColorRGB& EMDirectedLight::GetAmbientColor ()
{
    return _ambientColor;
}

GXFloat EMDirectedLight::GetAmbientIntensity ()
{
    return _ambientIntensity;
}

GXVoid EMDirectedLight::SetRotation ( const GXMat4 &rotation )
{
    _modelMatrix = rotation;
}

GXVoid EMDirectedLight::SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
    _modelMatrix.RotationXYZ ( pitchRadians, yawRadians, rollRadians );
}

const GXMat4& EMDirectedLight::GetRotation ()
{
    return _modelMatrix;
}
