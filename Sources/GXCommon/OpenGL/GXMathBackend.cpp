// version 1.56

#include <GXCommon/GXMath.h>


GXVoid GXMat4::Perspective ( GXFloat fieldOfViewYRadiands, GXFloat aspectRatio, GXFloat zNear, GXFloat zFar )
{
    const GXFloat halfFovy = fieldOfViewYRadiands * 0.5f;
    const GXFloat ctan = cosf ( halfFovy ) / sinf ( halfFovy );
    const GXFloat invRange = 1.0f / ( zFar - zNear );

    _m[ 0u ][ 0u ] = ctan / aspectRatio;
    _m[ 1u ][ 1u ] = ctan;
    _m[ 2u ][ 2u ] = ( zFar + zNear ) * invRange;
    _m[ 2u ][ 3u ] = 1.0f;
    _m[ 3u ][ 2u ] = -2.0f * zFar * zNear * invRange;

    _m[ 0u ][ 1u ] = _m[ 0u ][ 2u ] = _m[ 0u ][ 3u ] = 0.0f;
    _m[ 1u ][ 0u ] = _m[ 1u ][ 2u ] = _m[ 1u ][ 3u ] = 0.0f;
    _m[ 2u ][ 0u ] = _m[ 2u ][ 1u ] = 0.0f;
    _m[ 3u ][ 0u ] = _m[ 3u ][ 1u ] = _m[ 3u ][ 3u ] = 0.0f;
}

GXVoid GXMat4::Ortho ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar )
{
    const GXFloat invRange = 1.0f / ( zFar - zNear );

    _m[ 0u ][ 0u ] = 2.0f / width;
    _m[ 1u ][ 1u ] = 2.0f / height;
    _m[ 2u ][ 2u ] = 2.0f * invRange;
    _m[ 3u ][ 2u ] = -invRange * ( zFar + zNear );

    _m[ 0u ][ 1u ] = _m[ 0u ][ 2u ] = _m[ 0u ][ 3u ] = 0.0f;
    _m[ 1u ][ 0u ] = _m[ 1u ][ 2u ] = _m[ 1u ][ 3u ] = 0.0f;
    _m[ 2u ][ 0u ] = _m[ 2u ][ 1u ] = _m[ 2u ][ 3u ] = 0.0f;
    _m[ 3u ][ 0u ] = _m[ 3u ][ 1u ] = 0.0f;
    _m[ 3u ][ 3u ] = 1.0f;
}

GXVoid GXMat4::GetPerspectiveParams ( GXFloat &fieldOfViewYRadiands, GXFloat &aspectRatio, GXFloat &zNear, GXFloat &zFar )
{
    fieldOfViewYRadiands = 2.0f * atanf ( 1.0f / _m[ 1u ][ 1u ] );
    aspectRatio = _m[ 1u ][ 1u ] / _m[ 0u ][ 0u ];

    zNear = -_m[ 3u ][ 2u ] / ( _m[ 2u ][ 2u ] + 1.0f );
    zFar = ( zNear * _m[ 3u ][ 2u ] ) / ( _m[ 3u ][ 2u ] + 2.0f * zNear );
}

GXVoid GXMat4::GetOrthoParams ( GXFloat &width, GXFloat &height, GXFloat &zNear, GXFloat &zFar )
{
    width = 2.0f / _m[ 0u ][ 0u ];
    height = 2.0f / _m[ 1u ][ 1u ];

    const GXFloat factor = 1.0f / _m[ 2u ][ 2u ];

    zNear = -( 1.0f + _m[ 3u ][ 2u ] ) * factor;
    zFar = ( 2.0f + zNear * _m[ 2u ][ 2u ] ) * factor;
}

GXVoid GXMat4::GetRayPerspective ( GXVec3 &rayView, const GXVec2 &mouseCVV ) const
{
    rayView._data[ 0u ] = mouseCVV.GetX () / _m[ 0u ][ 0u ];
    rayView._data[ 1u ] = mouseCVV.GetY () / _m[ 1u ][ 1u ];
    rayView._data[ 2u ] = 1.0f;
}
