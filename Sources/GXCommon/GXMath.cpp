﻿// version 1.53

#include <GXCommon/GXMath.h>
#include <GXCommon/GXLogger.h>

GX_DISABLE_COMMON_WARNINGS

#include <stdlib.h>
#include <cwchar>
#include <cstring>
#include <time.h>

GX_RESTORE_WARNING_STATE


#define COLOR_TO_FLOAT_FACTOR               0.00392157f

#define HSVA_FACTOR                         0.016666f
#define HSVA_TO_RGBA_FACTOR                 2.55f
#define HSVA_ALPHA_TO_RGBA_ALPHA_BYTE       2.55f
#define HSVA_TO_RGBA_FLOAT                  0.01f
#define RGBA_TO_UBYTE_FACTOR                255.0f

#define DEGREES_TO_RADIANS_FACTOR           0.0174533f
#define RADIANS_TO_DEGREES_FACTOR           57.295779f

#define FLOAT_EPSILON                       1.0e-4f
#define INVERSE_RAND_MAX                    3.05185e-5f

#define SOLUTION_ALPHA                      0u
#define SOLUTION_BETTA                      1u
#define SOLUTION_GAMMA                      2u
#define SOLUTION_YOTTA                      3u
#define UNKNOWN_SOLUTION                    0xFFu


GXVec2::GXVec2 ()
{
    memset ( this, 0, sizeof ( GXVec2 ) );
}

GXVec2::GXVec2 ( const GXVec2 &other )
{
    memcpy ( this, &other, sizeof ( GXVec2 ) );
}

GXVec2::GXVec2 ( GXFloat x, GXFloat y )
{
    data[ 0u ] = x;
    data[ 1u ] = y;
}

GXVoid GXVec2::SetX ( GXFloat x )
{
    data[ 0u ] = x;
}

GXFloat GXVec2::GetX () const
{
    return data[ 0u ];
}

GXVoid GXVec2::SetY ( GXFloat y )
{
    data[ 1u ] = y;
}

GXFloat GXVec2::GetY () const
{
    return data[ 1u ];
}

GXVoid GXVec2::Init ( GXFloat x, GXFloat y )
{
    data[ 0u ] = x;
    data[ 1u ] = y;
}

GXVoid GXVec2::Normalize ()
{
    Multiply ( *this, 1.0f / Length () );
}

GXVoid GXVec2::CalculateNormalFast ( const GXVec2 &a, const GXVec2 &b )
{
    data[ 0u ] = a.data[ 1u ] - b.data[ 1u ];
    data[ 1u ] = b.data[ 0u ] - a.data[ 0u ];
}

GXVoid GXVec2::CalculateNormal ( const GXVec2 &a, const GXVec2 &b )
{
    CalculateNormalFast ( a, b );
    Normalize ();
}

GXVoid GXVec2::Sum ( const GXVec2 &a, const GXVec2 &b )
{
    data[ 0u ] = a.data[ 0u ] + b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] + b.data[ 1u ];
}

GXVoid GXVec2::Sum ( const GXVec2 &a, GXFloat bScale, const GXVec2 &b )
{
    data[ 0u ] = a.data[ 0u ] + bScale * b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] + bScale * b.data[ 1u ];
}

GXVoid GXVec2::Substract ( const GXVec2 &a, const GXVec2 &b )
{
    data[ 0u ] = a.data[ 0u ] - b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] - b.data[ 1u ];
}

GXVoid GXVec2::Multiply ( const GXVec2 &a, const GXVec2 &b )
{
    data[ 0u ] = a.data[ 0u ] * b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] * b.data[ 1u ];
}

GXVoid GXVec2::Multiply ( const GXVec2 &v, GXFloat scale )
{
    data[ 0u ] = v.data[ 0u ] * scale;
    data[ 1u ] = v.data[ 1u ] * scale;
}

GXFloat GXVec2::DotProduct ( const GXVec2 &other ) const
{
    return data[ 0u ] * other.data[ 0u ] + data[ 1u ] * other.data[ 1u ];
}

GXFloat GXVec2::Length () const
{
    return sqrtf ( SquaredLength () );
}

GXFloat GXVec2::SquaredLength () const
{
    return DotProduct ( *this );
}

GXBool GXVec2::IsEqual ( const GXVec2 &other ) const
{
    if ( data[ 0u ] != other.data[ 0u ] ) return GX_FALSE;
    if ( data[ 1u ] != other.data[ 1u ] ) return GX_FALSE;

    return GX_TRUE;
}

GXVec2& GXVec2::operator = ( const GXVec2 &vector )
{
    memcpy ( this, &vector, sizeof ( GXVec2 ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

eGXLineRelationship GXCALL GXLineIntersection2D ( GXVec2 &intersectionPoint, const GXVec2 &a0, const GXVec2 &a1, const GXVec2 &b0, const GXVec2 &b1 )
{
    GXVec2 alpha;
    alpha.Substract ( a1, a0 );

    GXVec2 betta;
    betta.Substract ( b1, b0 );

    GXVec2 yotta ( -alpha.GetY (), alpha.GetX () );
    GXFloat omega = yotta.DotProduct ( betta );

    if ( omega == 0.0f )
    {
        GXVec2 gamma ( a0.IsEqual ( b0 ) ? b1 : b0 );

        GXVec2 zetta;
        zetta.Substract ( gamma, a0 );
        zetta.Normalize ();

        alpha.Normalize ();

        GXFloat eta = alpha.DotProduct ( zetta );

        if ( eta == 1.0f || eta == -1.0f )
        {
            intersectionPoint = a0;
            return eGXLineRelationship::Overlap;
        }

        intersectionPoint.Init ( FLT_MAX, FLT_MAX );
        return eGXLineRelationship::NoIntersection;
    }

    GXVec2 phi;
    phi.Substract ( b0, a0 );

    intersectionPoint.Sum ( b0, -phi.DotProduct ( yotta ) / omega, betta );
    return eGXLineRelationship::Intersection;
}

//------------------------------------------------------------------------------------------------

static GXVec3 absoluteX ( 1.0f, 0.0f, 0.0 );
static GXVec3 absoluteY ( 0.0f, 1.0f, 0.0 );
static GXVec3 absoluteZ ( 0.0f, 0.0f, 1.0 );

GXVec3::GXVec3 ()
{
    memset ( data, 0, sizeof ( GXVec3 ) );
}

GXVec3::GXVec3 ( const GXVec3 &other )
{
    memcpy ( this, &other, sizeof ( GXVec3 ) );
}

GXVec3::GXVec3 ( GXFloat x, GXFloat y, GXFloat z )
{
    SetX ( x );
    SetY ( y );
    SetZ ( z );
}

GXVoid GXVec3::SetX ( GXFloat x )
{
    data[ 0u ] = x;
}

GXFloat GXVec3::GetX () const
{
    return data[ 0u ];
}

GXVoid GXVec3::SetY ( GXFloat y )
{
    data[ 1 ] = y;
}

GXFloat GXVec3::GetY () const
{
    return data[ 1u ];
}

GXVoid GXVec3::SetZ ( GXFloat z )
{
    data[ 2u ] = z;
}

GXFloat GXVec3::GetZ () const
{
    return data[ 2u ];
}

GXVoid GXVec3::Init ( GXFloat x, GXFloat y, GXFloat z )
{
    data[ 0u ] = x;
    data[ 1u ] = y;
    data[ 2u ] = z;
}

GXVoid GXVec3::Normalize ()
{
    Multiply ( *this, 1.0f / Length () );
}

GXVoid GXVec3::Reverse ()
{
    data[ 0u ] = -data[ 0u ];
    data[ 1u ] = -data[ 1u ];
    data[ 2u ] = -data[ 2u ];
}

GXVoid GXVec3::Sum ( const GXVec3 &a, const GXVec3 &b )
{
    data[ 0u ] = a.data[ 0u ] + b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] + b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] + b.data[ 2u ];
}

GXVoid GXVec3::Sum ( const GXVec3 &a, GXFloat bScale, const GXVec3 &b )
{
    data[ 0u ] = a.data[ 0u ] + bScale * b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] + bScale * b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] + bScale * b.data[ 2u ];
}

GXVoid GXVec3::Substract ( const GXVec3 &a, const GXVec3 &b )
{
    data[ 0u ] = a.data[ 0u ] - b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] - b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] - b.data[ 2u ];
}

GXVoid GXVec3::Multiply ( const GXVec3 &a, GXFloat scale )
{
    data[ 0u ] = a.data[ 0u ] * scale;
    data[ 1u ] = a.data[ 1u ] * scale;
    data[ 2u ] = a.data[ 2u ] * scale;
}

GXVoid GXVec3::Multiply ( const GXVec3 &a, const GXVec3 &b )
{
    data[ 0u ] = a.data[ 0u ] * b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] * b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] * b.data[ 2u ];
}

GXFloat GXVec3::DotProduct ( const GXVec3 &other ) const
{
    return data[ 0u ] * other.data[ 0u ] + data[ 1u ] * other.data[ 1u ] + data[ 2u ] * other.data[ 2u ];
}

GXVoid GXVec3::CrossProduct ( const GXVec3 &a, const GXVec3 &b )
{
    data[ 0u ] = a.data[ 1u ] * b.data[ 2u ] - a.data[ 2u ] * b.data[ 1u ];
    data[ 1u ] = a.data[ 2u ] * b.data[ 0u ] - a.data[ 0u ] * b.data[ 2u ];
    data[ 2u ] = a.data[ 0u ] * b.data[ 1u ] - a.data[ 1u ] * b.data[ 0u ];
}

GXFloat GXVec3::Length () const
{
    return sqrtf ( DotProduct ( *this ) );
}

GXFloat GXVec3::SquaredLength () const
{
    return DotProduct ( *this );
}

GXFloat GXVec3::Distance ( const GXVec3 &other ) const
{
    GXVec3 difference;
    difference.Substract ( *this, other );
    return difference.Length ();
}

GXFloat GXVec3::SquaredDistance ( const GXVec3 &other ) const
{
    GXVec3 difference;
    difference.Substract ( *this, other );
    return difference.SquaredLength ();
}

GXVoid GXVec3::LinearInterpolation ( const GXVec3 &start, const GXVec3 &finish, GXFloat interpolationFactor )
{
    GXVec3 difference;
    difference.Substract ( finish, start );
    Sum ( start, interpolationFactor, difference );
}

GXVoid GXVec3::Project ( const GXVec3 &vector, const GXVec3 &axis )
{
    GXVec3 normalVector ( vector );
    normalVector.Normalize ();
    GXFloat factor = vector.Length () * axis.DotProduct ( normalVector );
    Multiply ( axis, factor );
}

GXBool GXVec3::IsEqual ( const GXVec3 &other )
{
    if ( data[ 0u ] != other.data[ 0u ] ) return GX_FALSE;
    if ( data[ 1u ] != other.data[ 1u ] ) return GX_FALSE;
    if ( data[ 2u ] != other.data[ 2u ] ) return GX_FALSE;

    return GX_TRUE;
}

const GXVec3& GXVec3::GetAbsoluteX ()
{
    return absoluteX;
}

const GXVec3& GXVec3::GetAbsoluteY ()
{
    return absoluteY;
}

const GXVec3& GXVec3::GetAbsoluteZ ()
{
    return absoluteZ;
}

GXVoid GXCALL GXVec3::MakeOrthonormalBasis ( GXVec3 &baseX, GXVec3 &adjustedY, GXVec3 &adjustedZ )
{
    adjustedZ.CrossProduct ( baseX, adjustedY );

    if ( adjustedZ.SquaredLength () == 0.0f )
    {
        GXLogW ( L"GXMakeOrthonormalBasis::Error - Can't make this!\n" );
        return;
    }

    adjustedY.CrossProduct ( adjustedZ, baseX );

    baseX.Normalize ();
    adjustedY.Normalize ();
    adjustedZ.Normalize ();
}

GXVec3& GXVec3::operator = ( const GXVec3 &vector )
{
    memcpy ( this, &vector, sizeof ( GXVec3 ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXBool GXCALL GXRayTriangleIntersection3D ( GXFloat &outT, const GXVec3 &origin, const GXVec3 &direction, GXFloat length, const GXVec3 &a, const GXVec3 &b, const GXVec3 &c )
{
    // Implementation https://graphics.stanford.edu/courses/cs348b-98/gg/intersect.html

    GXPlane plane;
    plane.From ( a, b, c );
    GXVec3 normal ( plane.a, plane.b, plane.c );

    GXFloat t = ( plane.d + normal.DotProduct ( origin ) ) / ( -normal.DotProduct ( direction ) );

    if ( t < 0.0f ) return GX_FALSE;

    if ( t > length ) return GX_FALSE;

    GXVec3 triangle[ 3u ];
    triangle[ 0u ] = a;
    triangle[ 1u ] = b;
    triangle[ 2u ] = c;

    GXVec3 point;
    point.Sum ( origin, t, direction );

    GXUByte selector;

    GXFloat gamma = fabsf ( plane.a );
    GXFloat omega = fabsf ( plane.b );
    GXFloat yotta = fabsf ( plane.c );

    if ( gamma > omega )
        selector = gamma > yotta ? 0u : 2u;
    else
        selector = omega > yotta ? 1u : 2u;

    GXUByte i1;
    GXUByte i2;

    switch ( selector )
    {
        case 0u:
            i1 = 1u;
            i2 = 2u;
        break;

        case 1u:
            i1 = 2u;
            i2 = 0u;
        break;

        case 2u:
            i1 = 0u;
            i2 = 1u;
        break;

        default:
            // NOTHING
        break;
    }

    GXFloat u0 = point.data[ i1 ] - triangle[ 0u ].data[ i1 ];
    GXFloat v0 = point.data[ i2 ] - triangle[ 0u ].data[ i2 ];

    GXFloat u1 = triangle[ 1u ].data[ i1 ] - triangle[ 0u ].data[ i1 ];
    GXFloat v1 = triangle[ 1u ].data[ i2 ] - triangle[ 0u ].data[ i2 ];

    GXFloat u2 = triangle[ 2u ].data[ i1 ] - triangle[ 0u ].data[ i1 ];
    GXFloat v2 = triangle[ 2u ].data[ i2 ] - triangle[ 0u ].data[ i2 ];

    gamma = 1.0f / ( u1 * v2 - v1 * u2 );
    GXFloat alpha = ( u0 * v2 - v0 * u2 ) * gamma;

    if ( alpha < 0.0f || alpha > 1.0f ) return GX_FALSE;

    GXFloat betta = ( u1 * v0 - v1 * u0 ) * gamma;

    if ( betta < 0.0f || betta > 1.0f || ( alpha + betta ) > 1.0f ) return GX_FALSE;

    outT = t;
    return GX_TRUE;
}

//----------------------------------------------------------------------------

GXEuler::GXEuler ():
    pitchRadians ( 0.0f ),
    yawRadians ( 0.0f ),
    rollRadians ( 0.0f )
{
    // NOTHING
}

GXEuler::GXEuler ( const GXEuler &other )
{
    memcpy ( this, &other, sizeof ( GXEuler ) );
}

GXEuler::GXEuler ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
    this->pitchRadians = pitchRadians;
    this->yawRadians = yawRadians;
    this->rollRadians = rollRadians;
}

GXEuler& GXEuler::operator = ( const GXEuler &other )
{
    memcpy ( this, &other, sizeof ( GXEuler ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXVec4::GXVec4 ()
{
    // NOTHING
}

GXVec4::GXVec4 ( const GXVec4 &other )
{
    memcpy ( this, &other, sizeof ( GXVec4 ) );
}

GXVec4::GXVec4 ( const GXVec3 &vector, GXFloat w )
{
    memcpy ( data, vector.data, sizeof ( GXVec3 ) );
    data[ 3u ] = w;
}

GXVec4::GXVec4 ( GXFloat x, GXFloat y, GXFloat z, GXFloat w )
{
    Init ( x, y, z, w );
}

GXVoid GXVec4::Init ( GXFloat x, GXFloat y, GXFloat z, GXFloat w )
{
    data[ 0u ] = x;
    data[ 1u ] = y;
    data[ 2u ] = z;
    data[ 3u ] = w;
}

GXVoid GXVec4::SetX ( GXFloat x )
{
    data[ 0u ] = x;
}

GXFloat GXVec4::GetX () const
{
    return data[ 0u ];
}

GXVoid GXVec4::SetY ( GXFloat y )
{
    data[ 1u ] = y;
}

GXFloat GXVec4::GetY () const
{
    return data[ 1u ];
}

GXVoid GXVec4::SetZ ( GXFloat z )
{
    data[ 2u ] = z;
}

GXFloat GXVec4::GetZ () const
{
    return data[ 2u ];
}

GXVoid GXVec4::SetW ( GXFloat w )
{
    data[ 3u ] = w;
}

GXFloat GXVec4::GetW () const
{
    return data[ 3u ];
}

GXVoid GXVec4::Sum ( const GXVec4 &a, const GXVec4 &b )
{
    data[ 0u ] = a.data[ 0u ] + b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] + b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] + b.data[ 2u ];
    data[ 3u ] = a.data[ 3u ] + b.data[ 3u ];
}

GXVoid GXVec4::Sum ( const GXVec4 &a, GXFloat bScale, const GXVec4 &b )
{
    data[ 0u ] = a.data[ 0u ] + bScale * b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] + bScale * b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] + bScale * b.data[ 2u ];
    data[ 3u ] = a.data[ 3u ] + bScale * b.data[ 3u ];
}

GXVoid GXVec4::Substract ( const GXVec4 &a, const GXVec4 &b )
{
    data[ 0u ] = a.data[ 0u ] - b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] - b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] - b.data[ 2u ];
    data[ 3u ] = a.data[ 3u ] - b.data[ 3u ];
}

GXFloat GXVec4::DotProduct ( const GXVec4 &other ) const
{
    return data[ 0u ] * other.data[ 0u ] + data[ 1u ] * other.data[ 1u ] + data[ 2u ] * other.data[ 2u ] + data[ 3u ] * other.data[ 3u ];
}

GXFloat GXVec4::Length () const
{
    return sqrtf ( DotProduct ( *this ) );
}

GXFloat GXVec4::SquaredLength () const
{
    return DotProduct ( *this );
}

GXVec4& GXVec4::operator = ( const GXVec4 &vector )
{
    memcpy ( this, &vector, sizeof ( GXVec4 ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXVec6::GXVec6 ()
{
    // NOTHING
}

GXVec6::GXVec6 ( const GXVec6 &other )
{
    memcpy ( this, &other, sizeof ( GXVec6 ) );
}

GXVec6::GXVec6 ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 )
{
    data[ 0u ] = a1;
    data[ 1u ] = a2;
    data[ 2u ] = a3;
    data[ 3u ] = a4;
    data[ 4u ] = a5;
    data[ 5u ] = a6;
}

GXVoid GXVec6::Init ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 )
{
    data[ 0u ] = a1;
    data[ 1u ] = a2;
    data[ 2u ] = a3;
    data[ 3u ] = a4;
    data[ 4u ] = a5;
    data[ 5u ] = a6;
}

GXVoid GXVec6::From ( const GXVec3 &v1, const GXVec3 &v2 )
{
    memcpy ( data, &v1, sizeof ( GXVec3 ) );
    memcpy ( data + 3u, &v2, sizeof ( GXVec3 ) );
}

GXFloat GXVec6::DotProduct ( const GXVec6 &other ) const
{
    return data[ 0u ] * other.data[ 0u ] + data[ 1u ] * other.data[ 1u ] + data[ 2u ] * other.data[ 2u ] + data[ 3u ] * other.data[ 3u ] + data[ 4u ] * other.data[ 4u ] + data[ 5u ] * other.data[ 5u ];
}

GXVoid GXVec6::Sum ( const GXVec6 &a, const GXVec6 &b )
{
    data[ 0u ] = a.data[ 0u ] + b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] + b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] + b.data[ 2u ];
    data[ 3u ] = a.data[ 3u ] + b.data[ 3u ];
    data[ 4u ] = a.data[ 4u ] + b.data[ 4u ];
    data[ 5u ] = a.data[ 5u ] + b.data[ 5u ];
}

GXVoid GXVec6::Sum ( const GXVec6 &a, GXFloat bScale, const GXVec6 &b )
{
    data[ 0u ] = a.data[ 0u ] + bScale * b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] + bScale * b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] + bScale * b.data[ 2u ];
    data[ 3u ] = a.data[ 3u ] + bScale * b.data[ 3u ];
    data[ 4u ] = a.data[ 4u ] + bScale * b.data[ 4u ];
    data[ 5u ] = a.data[ 5u ] + bScale * b.data[ 5u ];
}

GXVoid GXVec6::Multiply ( const GXVec6 &a, GXFloat factor )
{
    data[ 0u ] = a.data[ 0u ] * factor;
    data[ 1u ] = a.data[ 1u ] * factor;
    data[ 2u ] = a.data[ 2u ] * factor;
    data[ 3u ] = a.data[ 3u ] * factor;
    data[ 4u ] = a.data[ 4u ] * factor;
    data[ 5u ] = a.data[ 5u ] * factor;
}

GXVec6& GXVec6::operator = ( const GXVec6 &other )
{
    memcpy ( this, &other, sizeof ( GXVec6 ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXColorRGB::GXColorRGB ()
{
    memset ( data, 0, 4u * sizeof ( GXFloat ) );
}

GXColorRGB::GXColorRGB ( const GXColorRGB &other )
{
    memcpy ( this, &other, sizeof ( GXColorRGB ) );
}

GXColorRGB::GXColorRGB ( GXFloat red, GXFloat green, GXFloat blue, GXFloat alpha )
{
    Init ( red, green, blue, alpha );
}

GXColorRGB::GXColorRGB ( GXUByte red, GXUByte green, GXUByte blue, GXFloat alpha )
{
    From ( red, green, blue, alpha );
}

GXColorRGB::GXColorRGB ( const GXColorHSV &color )
{
    From ( color );
}

GXVoid GXColorRGB::Init ( GXFloat red, GXFloat green, GXFloat blue, GXFloat alpha )
{
    data[ 0u ] = red;
    data[ 1u ] = green;
    data[ 2u ] = blue;
    data[ 3u ] = alpha;
}

GXVoid GXColorRGB::SetRed ( GXFloat red )
{
    data[ 0u ] = red;
}

GXFloat GXColorRGB::GetRed () const
{
    return data[ 0u ];
}

GXVoid GXColorRGB::SetGreen ( GXFloat green )
{
    data[ 1u ] = green;
}

GXFloat GXColorRGB::GetGreen () const
{
    return data[ 1u ];
}

GXVoid GXColorRGB::SetBlue ( GXFloat blue )
{
    data[ 2u ] = blue;
}

GXFloat GXColorRGB::GetBlue () const
{
    return data[ 2u ];
}

GXVoid GXColorRGB::SetAlpha ( GXFloat alpha )
{
    data[ 3u ] = alpha;
}

GXFloat GXColorRGB::GetAlpha () const
{
    return data[ 3u ];
}

GXVoid GXColorRGB::From ( GXUByte red, GXUByte green, GXUByte blue, GXFloat alpha )
{
    data[ 0u ] = red * COLOR_TO_FLOAT_FACTOR;
    data[ 1u ] = green * COLOR_TO_FLOAT_FACTOR;
    data[ 2u ] = blue * COLOR_TO_FLOAT_FACTOR;
    data[ 3u ] = alpha * COLOR_TO_FLOAT_FACTOR;
}

GXVoid GXColorRGB::From ( const GXColorHSV &color )
{
    GXFloat correctedHue = color.GetHue ();

    while ( correctedHue >= 360.0f )
        correctedHue -= 360.0f;

    while ( correctedHue < 0.0f )
        correctedHue += 360.0f;

    GXFloat value = color.GetValue ();

    GXUByte selector = static_cast<GXUByte> ( ( static_cast<GXInt> ( correctedHue ) / 60 ) % 6 );
    GXFloat minValue = ( ( 100.0f - color.GetSaturation () ) * value ) * 0.01f;
    GXFloat delta = ( value - minValue ) * ( ( static_cast<GXInt> ( correctedHue ) % 60 ) * HSVA_FACTOR );
    GXFloat increment = minValue + delta;
    GXFloat decrement = value - delta;

    data[ 3 ] = color.GetAlpha () * HSVA_TO_RGBA_FLOAT;

    switch ( selector )
    {
        case 0u:
            data[ 0u ] = value * HSVA_TO_RGBA_FLOAT;
            data[ 1u ] = increment * HSVA_TO_RGBA_FLOAT;
            data[ 2u ] = minValue * HSVA_TO_RGBA_FLOAT;
        break;

        case 1u:
            data[ 0u ] = decrement * HSVA_TO_RGBA_FLOAT;
            data[ 1u ] = value * HSVA_TO_RGBA_FLOAT;
            data[ 2u ] = minValue * HSVA_TO_RGBA_FLOAT;
        break;

        case 2u:
            data[ 0u ] = minValue * HSVA_TO_RGBA_FLOAT;
            data[ 1u ] = value * HSVA_TO_RGBA_FLOAT;
            data[ 2u ] = increment * HSVA_TO_RGBA_FLOAT;
        break;

        case 3u:
            data[ 0u ] = minValue * HSVA_TO_RGBA_FLOAT;
            data[ 1u ] = decrement * HSVA_TO_RGBA_FLOAT;
            data[ 2u ] = value * HSVA_TO_RGBA_FLOAT;
        break;

        case 4u:
            data[ 0u ] = increment * HSVA_TO_RGBA_FLOAT;
            data[ 1u ] = minValue * HSVA_TO_RGBA_FLOAT;
            data[ 2u ] = value * HSVA_TO_RGBA_FLOAT;
        break;

        case 5u:
            data[ 0u ] = value * HSVA_TO_RGBA_FLOAT;
            data[ 1u ] = minValue * HSVA_TO_RGBA_FLOAT;
            data[ 2u ] = decrement * HSVA_TO_RGBA_FLOAT;
        break;

        default:
            GXLogW ( L"GXConvertHSVAToRGBA::Error (branch two) - Что-то пошло не так!\n" );
        break;
    }
}

GXVoid GXColorRGB::ConvertToUByte ( GXUByte &red, GXUByte &green, GXUByte &blue, GXUByte &alpha ) const
{
    red = static_cast<GXUByte> ( data[ 0u ] * RGBA_TO_UBYTE_FACTOR + 0.5f );
    green = static_cast<GXUByte> ( data[ 1u ] * RGBA_TO_UBYTE_FACTOR + 0.5f );
    blue = static_cast<GXUByte> ( data[ 2u ] * RGBA_TO_UBYTE_FACTOR + 0.5f );
    alpha = static_cast<GXUByte> ( data[ 3u ] * RGBA_TO_UBYTE_FACTOR + 0.5f );
}

GXColorRGB& GXColorRGB::operator = ( const GXColorRGB &other )
{
    memcpy ( this, &other, sizeof ( GXColorRGB ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXColorHSV::GXColorHSV ()
{
    memset ( data, 0, 4u * sizeof ( GXFloat ) );
}

GXColorHSV::GXColorHSV ( const GXColorHSV &other )
{
    memcpy ( this, &other, sizeof ( GXColorHSV ) );
}

GXColorHSV::GXColorHSV ( GXFloat hue, GXFloat saturation, GXFloat value, GXFloat alpha )
{
    data[ 0u ] = hue;
    data[ 1u ] = saturation;
    data[ 2u ] = value;
    data[ 3u ] = alpha;
}

GXColorHSV::GXColorHSV ( const GXColorRGB &color )
{
    From ( color );
}

GXVoid GXColorHSV::SetHue ( GXFloat hue )
{
    data[ 0u ] = hue;
}

GXFloat GXColorHSV::GetHue () const
{
    return data[ 0u ];
}

GXVoid GXColorHSV::SetSaturation ( GXFloat saturation )
{
    data[ 1u ] = saturation;
}

GXFloat GXColorHSV::GetSaturation () const
{
    return data[ 1u ];
}

GXVoid GXColorHSV::SetValue ( GXFloat value )
{
    data[ 2u ] = value;
}

GXFloat GXColorHSV::GetValue () const
{
    return data[ 2u ];
}

GXVoid GXColorHSV::SetAlpha ( GXFloat alpha )
{
    data[ 3u ] = alpha;
}

GXFloat GXColorHSV::GetAlpha () const
{
    return data[ 3u ];
}

GXVoid GXColorHSV::From ( const GXColorRGB &color )
{
    GXFloat maxValue = GXMaxf ( GXMaxf ( color.GetRed (), color.GetGreen () ), color.GetBlue () );
    GXFloat minValue = GXMinf ( GXMinf ( color.GetRed (), color.GetGreen () ), color.GetBlue () );

    if ( maxValue == minValue )
    {
        data[ 0u ] = 0.0f;
    }
    else if ( maxValue == color.GetRed () && color.GetGreen () >= color.GetBlue () )
    {
        data[ 0u ] = 60.0f * ( ( color.GetGreen () - color.GetBlue () ) / ( maxValue - minValue ) );
    }
    else if ( maxValue == color.GetRed () && color.GetGreen () < color.GetBlue () )
    {
        data[ 0u ] = 60.0f * ( ( color.GetGreen () - color.GetBlue () ) / ( maxValue - minValue ) ) + 360.0f;
    }
    else if ( maxValue == color.GetGreen () )
    {
        data[ 0u ] = 60.0f * ( ( color.GetBlue () - color.GetRed () ) / ( maxValue - minValue ) ) + 120.0f;
    }
    else if ( maxValue == color.GetBlue () )
    {
        data[ 0u ] = 60.0f * ( ( color.GetRed () - color.GetGreen () ) / ( maxValue - minValue ) ) + 240.0f;
    }
    else
    {
        GXUInt wtf = 0u;
        GXLogA ( "GXColorHSV::From - Unhanled hue! %u\n", wtf );
    }

    if ( maxValue == 0.0f )
        data[ 1u ] = 0.0f;
    else
        data[ 1u ] = 100.0f * ( 1.0f - minValue / maxValue );

    data[ 2u ] = 100.0f * maxValue;
    data[ 3u ] = 100.0f * color.GetAlpha ();
}

GXVoid GXColorHSV::operator = ( const GXColorHSV &other )
{
    memcpy ( this, &other, sizeof ( GXColorHSV ) );
}

//---------------------------------------------------------------------------------------------------------------------

GXPreciseComplex::GXPreciseComplex ()
{
    // NOTHING
}

GXPreciseComplex::GXPreciseComplex ( const GXPreciseComplex &other ):
    r ( other.r ),
    i ( other.i )
{
    // NOTHING
}

GXPreciseComplex::GXPreciseComplex ( GXDouble real, GXDouble imaginary ):
    r ( real ),
    i ( imaginary )
{
    // NOTHING
}

GXPreciseComplex::~GXPreciseComplex ()
{
    // NOTHING
}

GXVoid GXPreciseComplex::Init ( GXDouble real, GXDouble imaginary )
{
    r = real;
    i = imaginary;
}

GXDouble GXPreciseComplex::Length ()
{
    return sqrt ( SquaredLength () );
}

GXDouble GXPreciseComplex::SquaredLength ()
{
    return r * r + i * i;
}

GXBool GXPreciseComplex::Power ( GXUInt power )
{
    if ( power == 1u )
        return GX_TRUE;

    if ( power > 1u )
    {
        GXPreciseComplex alpha ( *this );

        while ( power > 0u )
        {
            *this = *this * alpha;
            --power;
        }

        return GX_TRUE;
    }

    if ( r == 0.0 && i == 0.0 )
    {
        GXLogA ( "GXPreciseComplex::Power - ( 0.0 + 0.0i ) ^ 0 is undefined!" );
        return GX_FALSE;
    }

    r = 1.0;
    i = 0.0;

    return GX_FALSE;
}

GXPreciseComplex& GXPreciseComplex::operator = ( const GXPreciseComplex &other )
{
    memcpy ( this, &other, sizeof ( GXPreciseComplex ) );
    return *this;
}

GXPreciseComplex GXPreciseComplex::operator + ( const GXPreciseComplex &other )
{
    return GXPreciseComplex ( r + other.r, i + other.i );
}

GXPreciseComplex GXPreciseComplex::operator - ( const GXPreciseComplex &other )
{
    return GXPreciseComplex ( r - other.r, i - other.i );
}

GXPreciseComplex GXPreciseComplex::operator * ( const GXPreciseComplex &other )
{
    return GXPreciseComplex ( r * other.r - i * other.i, r * other.i + i * other.r );
}

GXPreciseComplex GXPreciseComplex::operator * ( GXDouble a )
{
    return GXPreciseComplex ( r * a, i * a );
}

GXPreciseComplex GXPreciseComplex::operator / ( GXDouble a )
{
    const GXDouble invA = 1.0 / a;
    return GXPreciseComplex ( r * invA, i * invA );
}

//---------------------------------------------------------------------------------------------------------------------

GXQuat::GXQuat ()
{
    memset ( data, 0, 4u * sizeof ( GXFloat ) );
}

GXQuat::GXQuat ( const GXQuat &other )
{
    memcpy ( this, &other, sizeof ( GXQuat ) );
}

GXQuat::GXQuat ( GXFloat r, GXFloat a, GXFloat b, GXFloat c )
{
    Init ( r, a, b, c );
}

GXQuat::GXQuat ( const GXMat3 &rotationMatrix )
{
    From ( rotationMatrix );
}

GXQuat::GXQuat ( const GXMat4 &rotationMatrix )
{
    From ( rotationMatrix );
}

GXVoid GXQuat::Init ( GXFloat r, GXFloat a, GXFloat b, GXFloat c )
{
    data[ 0u ] = r;
    data[ 1u ] = a;
    data[ 2u ] = b;
    data[ 3u ] = c;
}

GXVoid GXQuat::SetR ( GXFloat r )
{
    data[ 0u ] = r;
}

GXFloat GXQuat::GetR () const
{
    return data[ 0u ];
}

GXVoid GXQuat::SetA ( GXFloat a )
{
    data[ 1u ] = a;
}

GXFloat GXQuat::GetA () const
{
    return data[ 1u ];
}

GXVoid GXQuat::SetB ( GXFloat b )
{
    data[ 2u ] = b;
}

GXFloat GXQuat::GetB () const
{
    return data[ 2u ];
}

GXVoid GXQuat::SetC ( GXFloat c )
{
    data[ 3u ] = c;
}

GXFloat GXQuat::GetC () const
{
    return data[ 3u ];
}

GXVoid GXQuat::Identity ()
{
    data[ 0u ] = 1.0f;
    data[ 1u ] = data[ 2u ] = data[ 3u ] = 0.0f;
}

GXVoid GXQuat::Normalize ()
{
    GXFloat squaredLength = data[ 0u ] * data[ 0u ] + data[ 1u ] * data[ 1u ] + data[ 2u ] * data[ 2u ] + data[ 3u ] * data[ 3u ];

    if ( fabsf ( squaredLength ) < FLOAT_EPSILON )
    {
        GXLogW ( L"GXQuat::Normalize - Error\n" );
        return;
    }

    Multiply ( *this, 1.0f / sqrtf ( squaredLength ) );
}

GXVoid GXQuat::Inverse ( const GXQuat &q )
{
    GXFloat squaredLength = q.data[ 0u ] * q.data[ 0u ] + q.data[ 1u ] * q.data[ 1u ] + q.data[ 2u ] * q.data[ 2u ] + q.data[ 3u ] * q.data[ 3u ];

    if ( fabsf ( squaredLength ) > FLOAT_EPSILON )
    {
        GXFloat inverseSquaredLength = 1.0f / squaredLength;

        data[ 0u ] = q.data[ 0u ] * inverseSquaredLength;
        data[ 1u ] = -q.data[ 1u ] * inverseSquaredLength;
        data[ 2u ] = -q.data[ 2u ] * inverseSquaredLength;
        data[ 3u ] = -q.data[ 3u ] * inverseSquaredLength;

        return;
    }
    else
    {
        GXLogW ( L"GXQuat::Inverse - Error\n" );
        Identity ();
    }
}

GXVoid GXQuat::FromAxisAngle ( GXFloat x, GXFloat y, GXFloat z, GXFloat angle )
{
    GXFloat halfAngle = 0.5f * angle;
    GXFloat sinom = sinf ( halfAngle );

    data[ 0u ] = cosf ( halfAngle );
    data[ 1u ] = x * sinom;
    data[ 2u ] = y * sinom;
    data[ 3u ] = z * sinom;
}

GXVoid GXQuat::FromAxisAngle ( const GXVec3 &axis, GXFloat angle )
{
    FromAxisAngle ( axis.data[ 0u ], axis.data[ 1u ], axis.data[ 2u ], angle );
}

GXVoid GXQuat::From ( const GXMat3 &rotationMatrix )
{
    GXMat3 pureRotationMatrix;
    pureRotationMatrix.ClearRotation ( rotationMatrix );
    FromFast ( pureRotationMatrix );
}

GXVoid GXQuat::From ( const GXMat4 &rotationMatrix )
{
    GXMat4 pureRotationMatrix;
    pureRotationMatrix.ClearRotation ( rotationMatrix );
    FromFast ( pureRotationMatrix );
}

GXVoid GXQuat::FromFast ( const GXMat3 &pureRotationMatrix )
{
    // In ideal mathematics world all solutions are right.
    // But in practice more precise solution is the biggest "solutionFactorXXX" because of square root operation.

    GXFloat solutionFactorAlpha = pureRotationMatrix.m[ 0u ][ 0u ] + pureRotationMatrix.m[ 1u ][ 1u ] + pureRotationMatrix.m[ 2u ][ 2u ] + 1.0f;
    GXFloat solutionFactorBetta = pureRotationMatrix.m[ 0u ][ 0u ] - pureRotationMatrix.m[ 1u ][ 1u ] - pureRotationMatrix.m[ 2u ][ 2u ] + 1.0f;
    GXFloat solutionFactorGamma = -pureRotationMatrix.m[ 0u ][ 0u ] + pureRotationMatrix.m[ 1u ][ 1u ] - pureRotationMatrix.m[ 2u ][ 2u ] + 1.0f;
    GXFloat solutionFactorYotta = -pureRotationMatrix.m[ 0u ][ 0u ] - pureRotationMatrix.m[ 1u ][ 1u ] + pureRotationMatrix.m[ 2u ][ 2u ] + 1.0f;

    GXUByte solution = static_cast<GXUByte> ( UNKNOWN_SOLUTION );

    if ( solutionFactorAlpha > solutionFactorBetta )
    {
        if ( solutionFactorAlpha > solutionFactorGamma )
        {
            solution = solutionFactorAlpha > solutionFactorYotta ? static_cast<GXUByte> ( SOLUTION_ALPHA ) : solution = static_cast<GXUByte> ( SOLUTION_YOTTA );
        }
        else if ( solutionFactorGamma > solutionFactorYotta )
        {
            solution = static_cast<GXUByte> ( SOLUTION_GAMMA );
        }
        else
        {
            solution = static_cast<GXUByte> ( SOLUTION_YOTTA );
        }
    }
    else if ( solutionFactorBetta > solutionFactorGamma )
    {
        solution = solutionFactorBetta > solutionFactorYotta ? static_cast<GXUByte> ( SOLUTION_BETTA ) : static_cast<GXUByte> ( SOLUTION_YOTTA );
    }
    else if ( solutionFactorGamma > solutionFactorYotta )
    {
        solution = static_cast<GXUByte> ( SOLUTION_GAMMA );
    }
    else
    {
        solution = static_cast<GXUByte> ( SOLUTION_YOTTA );
    }

    switch ( solution )
    {
        case SOLUTION_ALPHA:
        {
            GXFloat phi = 0.5f * sqrtf ( solutionFactorAlpha );
            GXFloat omega = 1.0f / ( 4.0f * phi );

            data[ 0u ] = phi;
            data[ 1u ] = omega * ( pureRotationMatrix.m[ 1u ][ 2u ] - pureRotationMatrix.m[ 2u ][ 1u ] );
            data[ 2u ] = omega * ( pureRotationMatrix.m[ 2u ][ 0u ] - pureRotationMatrix.m[ 0u ][ 2u ] );
            data[ 3u ] = omega * ( pureRotationMatrix.m[ 0u ][ 1u ] - pureRotationMatrix.m[ 1u ][ 0u ] );
        }
        break;

        case SOLUTION_BETTA:
        {
            GXFloat phi = 0.5f * sqrtf ( solutionFactorBetta );
            GXFloat omega = 1.0f / ( 4.0f * phi );

            data[ 0u ] = omega * ( pureRotationMatrix.m[ 1u ][ 2u ] - pureRotationMatrix.m[ 2u ][ 1u ] );
            data[ 1u ] = phi;
            data[ 2u ] = omega * ( pureRotationMatrix.m[ 0u ][ 1u ] + pureRotationMatrix.m[ 1u ][ 0u ] );
            data[ 3u ] = omega * ( pureRotationMatrix.m[ 0u ][ 2u ] + pureRotationMatrix.m[ 2u ][ 0u ] );
        }
        break;

        case SOLUTION_GAMMA:
        {
            GXFloat phi = 0.5f * sqrtf ( solutionFactorGamma );
            GXFloat omega = 1.0f / ( 4.0f * phi );

            data[ 0u ] = omega * ( pureRotationMatrix.m[ 2u ][ 0u ] - pureRotationMatrix.m[ 0u ][ 2u ] );
            data[ 1u ] = omega * ( pureRotationMatrix.m[ 0u ][ 1u ] + pureRotationMatrix.m[ 1u ][ 0u ] );
            data[ 2u ] = phi;
            data[ 3u ] = omega * ( pureRotationMatrix.m[ 1u ][ 2u ] + pureRotationMatrix.m[ 2u ][ 1u ] );
        }
        break;

        case SOLUTION_YOTTA:
        {
            GXFloat phi = 0.5f * sqrtf ( solutionFactorYotta );
            GXFloat omega = 1.0f / ( 4.0f * phi );

            data[ 0u ] = omega * ( pureRotationMatrix.m[ 0u ][ 1u ] - pureRotationMatrix.m[ 1u ][ 0u ] );
            data[ 1u ] = omega * ( pureRotationMatrix.m[ 0u ][ 2u ] + pureRotationMatrix.m[ 2u ][ 0u ] );
            data[ 2u ] = omega * ( pureRotationMatrix.m[ 1u ][ 2u ] + pureRotationMatrix.m[ 2u ][ 1u ] );
            data[ 3u ] = phi;
        }
        break;

        default:
            // NOTHING
        break;
    }
}

GXVoid GXQuat::FromFast ( const GXMat4 &pureRotationMatrix )
{
    // In ideal mathematics world all solutions are right.
    // But in practice more precise solution is the biggest "solutionFactorXXX" because of square root operation.

    GXFloat solutionFactorAlpha = pureRotationMatrix.m[ 0u ][ 0u ] + pureRotationMatrix.m[ 1u ][ 1u ] + pureRotationMatrix.m[ 2u ][ 2u ] + 1.0f;
    GXFloat solutionFactorBetta = pureRotationMatrix.m[ 0u ][ 0u ] - pureRotationMatrix.m[ 1u ][ 1u ] - pureRotationMatrix.m[ 2u ][ 2u ] + 1.0f;
    GXFloat solutionFactorGamma = -pureRotationMatrix.m[ 0u ][ 0u ] + pureRotationMatrix.m[ 1u ][ 1u ] - pureRotationMatrix.m[ 2u ][ 2u ] + 1.0f;
    GXFloat solutionFactorYotta = -pureRotationMatrix.m[ 0u ][ 0u ] - pureRotationMatrix.m[ 1u ][ 1u ] + pureRotationMatrix.m[ 2u ][ 2u ] + 1.0f;

    GXUByte solution = static_cast<GXUByte> ( UNKNOWN_SOLUTION );

    if ( solutionFactorAlpha > solutionFactorBetta )
    {
        if ( solutionFactorAlpha > solutionFactorGamma )
        {
            solution = solutionFactorAlpha > solutionFactorYotta ? static_cast<GXUByte> ( SOLUTION_ALPHA ) : solution = static_cast<GXUByte> ( SOLUTION_YOTTA );
        }
        else if ( solutionFactorGamma > solutionFactorYotta )
        {
            solution = static_cast<GXUByte> ( SOLUTION_GAMMA );
        }
        else
        {
            solution = static_cast<GXUByte> ( SOLUTION_YOTTA );
        }
    }
    else if ( solutionFactorBetta > solutionFactorGamma )
    {
        solution = solutionFactorBetta > solutionFactorYotta ? static_cast<GXUByte> ( SOLUTION_BETTA ) : static_cast<GXUByte> ( SOLUTION_YOTTA );
    }
    else if ( solutionFactorGamma > solutionFactorYotta )
    {
        solution = static_cast<GXUByte> ( SOLUTION_GAMMA );
    }
    else
    {
        solution = static_cast<GXUByte> ( SOLUTION_YOTTA );
    }

    switch ( solution )
    {
        case SOLUTION_ALPHA:
        {
            GXFloat phi = 0.5f * sqrtf ( solutionFactorAlpha );
            GXFloat omega = 1.0f / ( 4.0f * phi );

            data[ 0u ] = phi;
            data[ 1u ] = omega * ( pureRotationMatrix.m[ 1u ][ 2u ] - pureRotationMatrix.m[ 2u ][ 1u ] );
            data[ 2u ] = omega * ( pureRotationMatrix.m[ 2u ][ 0u ] - pureRotationMatrix.m[ 0u ][ 2u ] );
            data[ 3u ] = omega * ( pureRotationMatrix.m[ 0u ][ 1u ] - pureRotationMatrix.m[ 1u ][ 0u ] );
        }
        break;

        case SOLUTION_BETTA:
        {
            GXFloat phi = 0.5f * sqrtf ( solutionFactorBetta );
            GXFloat omega = 1.0f / ( 4.0f * phi );

            data[ 0u ] = omega * ( pureRotationMatrix.m[ 1u ][ 2u ] - pureRotationMatrix.m[ 2u ][ 1u ] );
            data[ 1u ] = phi;
            data[ 2u ] = omega * ( pureRotationMatrix.m[ 0u ][ 1u ] + pureRotationMatrix.m[ 1u ][ 0u ] );
            data[ 3u ] = omega * ( pureRotationMatrix.m[ 0u ][ 2u ] + pureRotationMatrix.m[ 2u ][ 0u ] );
        }
        break;

        case SOLUTION_GAMMA:
        {
            GXFloat phi = 0.5f * sqrtf ( solutionFactorGamma );
            GXFloat omega = 1.0f / ( 4.0f * phi );

            data[ 0u ] = omega * ( pureRotationMatrix.m[ 2u ][ 0u ] - pureRotationMatrix.m[ 0u ][ 2u ] );
            data[ 1u ] = omega * ( pureRotationMatrix.m[ 0u ][ 1u ] + pureRotationMatrix.m[ 1u ][ 0u ] );
            data[ 2u ] = phi;
            data[ 3u ] = omega * ( pureRotationMatrix.m[ 1u ][ 2u ] + pureRotationMatrix.m[ 2u ][ 1u ] );
        }
        break;

        case SOLUTION_YOTTA:
        {
            GXFloat phi = 0.5f * sqrtf ( solutionFactorYotta );
            GXFloat omega = 1.0f / ( 4.0f * phi );

            data[ 0u ] = omega * ( pureRotationMatrix.m[ 0u ][ 1u ] - pureRotationMatrix.m[ 1u ][ 0u ] );
            data[ 1u ] = omega * ( pureRotationMatrix.m[ 0u ][ 2u ] + pureRotationMatrix.m[ 2u ][ 0u ] );
            data[ 2u ] = omega * ( pureRotationMatrix.m[ 1u ][ 2u ] + pureRotationMatrix.m[ 2u ][ 1u ] );
            data[ 3u ] = phi;
        }
        break;

        default:
            // NOTHING
        break;
    }
}

GXVoid GXQuat::Multiply ( const GXQuat &a, const GXQuat &b )
{
    data[ 0u ] = a.data[ 0u ] * b.data[ 0u ] - a.data[ 1u ] * b.data[ 1u ] - a.data[ 2u ] * b.data[ 2u ] - a.data[ 3u ] * b.data[ 3u ];
    data[ 1u ] = a.data[ 0u ] * b.data[ 1u ] + a.data[ 1u ] * b.data[ 0u ] + a.data[ 2u ] * b.data[ 3u ] - a.data[ 3u ] * b.data[ 2u ];
    data[ 2u ] = a.data[ 0u ] * b.data[ 2u ] - a.data[ 1u ] * b.data[ 3u ] + a.data[ 2u ] * b.data[ 0u ] + a.data[ 3u ] * b.data[ 1u ];
    data[ 3u ] = a.data[ 0u ] * b.data[ 3u ] + a.data[ 1u ] * b.data[ 2u ] - a.data[ 2u ] * b.data[ 1u ] + a.data[ 3u ] * b.data[ 0u ];
}

GXVoid GXQuat::Multiply ( const GXQuat &q, GXFloat scale )
{
    data[ 0u ] = q.data[ 0u ] * scale;
    data[ 1u ] = q.data[ 1u ] * scale;
    data[ 2u ] = q.data[ 2u ] * scale;
    data[ 3u ] = q.data[ 3u ] * scale;
}

GXVoid GXQuat::Sum ( const GXQuat &a, const GXQuat &b )
{
    data[ 0u ] = a.data[ 0u ] + b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] + b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] + b.data[ 2u ];
    data[ 3u ] = a.data[ 3u ] + b.data[ 3u ];
}

GXVoid GXQuat::Substract ( const GXQuat &a, const GXQuat &b )
{
    data[ 0u ] = a.data[ 0u ] - b.data[ 0u ];
    data[ 1u ] = a.data[ 1u ] - b.data[ 1u ];
    data[ 2u ] = a.data[ 2u ] - b.data[ 2u ];
    data[ 3u ] = a.data[ 3u ] - b.data[ 3u ];
}

GXVoid GXQuat::SphericalLinearInterpolation ( const GXQuat &start, const GXQuat &finish, GXFloat interpolationFactor )
{
    GXQuat temp;
    GXFloat omega;
    GXFloat cosom;
    GXFloat sinom;
    GXFloat scale0;
    GXFloat scale1;

    if ( interpolationFactor <= 0.0f )
    {
        *this = start;
        return;
    }

    if ( interpolationFactor >= 1.0f )
    {
        *this = finish;
        return;
    }

    cosom = start.data[ 0u ] * finish.data[ 0u ] + start.data[ 1u ] * finish.data[ 1u ] + start.data[ 2u ] * finish.data[ 2u ] + start.data[ 3u ] * finish.data[ 3u ];

    if ( cosom < 0.0f )
    {
        temp.data[ 0u ] = -finish.data[ 0u ];
        temp.data[ 1u ] = -finish.data[ 1u ];
        temp.data[ 2u ] = -finish.data[ 2u ];
        temp.data[ 3u ] = -finish.data[ 3u ];
        cosom = -cosom;
    }
    else
    {
        temp = finish;
    }

    if ( ( 1.0f - cosom ) > FLOAT_EPSILON )
    {
        omega = acosf ( cosom );
        sinom = 1.0f / sinf ( omega );
        scale0 = sinf ( ( 1.0f - interpolationFactor ) * omega ) * sinom;
        scale1 = sinf ( interpolationFactor * omega ) * sinom;
    }
    else
    {
        scale0 = 1.0f - interpolationFactor;
        scale1 = interpolationFactor;
    }

    data[ 0u ] = start.data[ 0u ] * scale0 + temp.data[ 0u ] * scale1;
    data[ 1u ] = start.data[ 1u ] * scale0 + temp.data[ 1u ] * scale1;
    data[ 2u ] = start.data[ 2u ] * scale0 + temp.data[ 2u ] * scale1;
    data[ 3u ] = start.data[ 3u ] * scale0 + temp.data[ 3u ] * scale1;
}

GXVoid GXQuat::GetAxisAngle ( GXVec3 &axis, GXFloat &angle ) const
{
    GXQuat q ( *this );

    if ( fabsf ( q.data[ 0u ] ) > 1.0f )
        q.Normalize ();

    angle = 2.0f * acosf ( q.data[ 0u ] );

    axis.data[ 0u ] = q.data[ 1u ];
    axis.data[ 1u ] = q.data[ 2u ];
    axis.data[ 2u ] = q.data[ 3u ];

    GXFloat s = sqrtf ( 1.0f - q.data[ 0u ] * q.data[ 0u ] );

    if ( s < FLOAT_EPSILON ) return;

    axis.Multiply ( axis, 1.0f / s );
}

GXVoid GXQuat::Transform ( GXVec3 &out, const GXVec3 &v ) const
{
    GXFloat rr = data[ 0u ] * data[ 0u ];
    GXFloat ra2 = data[ 0u ] * data[ 1u ] * 2.0f;
    GXFloat rb2 = data[ 0u ] * data[ 2u ] * 2.0f;
    GXFloat rc2 = data[ 0u ] * data[ 3u ] * 2.0f;

    GXFloat aa = data[ 1u ] * data[ 1u ];
    GXFloat ab2 = data[ 1u ] * data[ 2u ] * 2.0f;
    GXFloat ac2 = data[ 1u ] * data[ 3u ] * 2.0f;

    GXFloat bb = data[ 2u ] * data[ 2u ];
    GXFloat bc2 = data[ 2u ] * data[ 3u ] * 2.0f;

    GXFloat cc = data[ 3u ] * data[ 3u ];

    GXFloat inverseSquaredLength = 1.0f / ( rr + aa + bb + cc );

    out.data[ 0u ] = inverseSquaredLength * ( v.data[ 0u ] * ( rr + aa - bb - cc ) + v.data[ 1u ] * ( ab2 - rc2 ) + v.data[ 2u ] * ( rb2 + ac2 ) );
    out.data[ 1u ] = inverseSquaredLength * ( v.data[ 0u ] * ( rc2 + ab2 ) + v.data[ 1u ] * ( rr - aa + bb - cc ) + v.data[ 2u ] * ( bc2 - ra2 ) );
    out.data[ 2u ] = inverseSquaredLength * ( v.data[ 0u ] * ( ac2 - rb2 ) + v.data[ 1u ] * ( ra2 + bc2 ) + v.data[ 2u ] * ( rr - aa - bb + cc ) );
}

GXVoid GXQuat::TransformFast ( GXVec3 &out, const GXVec3 &v ) const
{
    GXFloat rr = data[ 0u ] * data[ 0u ];
    GXFloat ra2 = data[ 0u ] * data[ 1u ] * 2.0f;
    GXFloat rb2 = data[ 0u ] * data[ 2u ] * 2.0f;
    GXFloat rc2 = data[ 0u ] * data[ 3u ] * 2.0f;

    GXFloat aa = data[ 1u ] * data[ 1u ];
    GXFloat ab2 = data[ 1u ] * data[ 2u ] * 2.0f;
    GXFloat ac2 = data[ 1u ] * data[ 3u ] * 2.0f;

    GXFloat bb = data[ 2u ] * data[ 2u ];
    GXFloat bc2 = data[ 2u ] * data[ 3u ] * 2.0f;

    GXFloat cc = data[ 3u ] * data[ 3u ];

    out.data[ 0u ] = v.data[ 0u ] * ( rr + aa - bb - cc ) + v.data[ 1u ] * ( ab2 - rc2 ) + v.data[ 2u ] * ( rb2 + ac2 );
    out.data[ 1u ] = v.data[ 0u ] * ( rc2 + ab2 ) + v.data[ 1u ] * ( rr - aa + bb - cc ) + v.data[ 2u ] * ( bc2 - ra2 );
    out.data[ 2u ] = v.data[ 0u ] * ( ac2 - rb2 ) + v.data[ 1u ] * ( ra2 + bc2 ) + v.data[ 2u ] * ( rr - aa - bb + cc );
}

GXQuat& GXQuat::operator = ( const GXVec4 &other )
{
    memcpy ( this, &other, sizeof ( GXQuat ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXMat3::GXMat3 ()
{
    Zeros ();
}

GXMat3::GXMat3 ( const GXMat4 &matrix )
{
    From ( matrix );
}

GXMat3::GXMat3 ( const GXMat3 &other )
{
    memcpy ( this, &other, sizeof ( GXMat3 ) );
}

GXVoid GXMat3::From ( const GXQuat &quaternion )
{
    GXFloat rr = quaternion.data[ 0u ] * quaternion.data[ 0u ];
    GXFloat ra2 = quaternion.data[ 0u ] * quaternion.data[ 1u ] * 2.0f;
    GXFloat rb2 = quaternion.data[ 0u ] * quaternion.data[ 2u ] * 2.0f;
    GXFloat rc2 = quaternion.data[ 0u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat aa = quaternion.data[ 1u ] * quaternion.data[ 1u ];
    GXFloat ab2 = quaternion.data[ 1u ] * quaternion.data[ 2u ] * 2.0f;
    GXFloat ac2 = quaternion.data[ 1u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat bb = quaternion.data[ 2u ] * quaternion.data[ 2u ];
    GXFloat bc2 = quaternion.data[ 2u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat cc = quaternion.data[ 3u ] * quaternion.data[ 3u ];

    GXFloat inverseSquaredLength = 1.0f / ( rr + aa + bb + cc );

    m[ 0u ][ 0u ] = inverseSquaredLength * ( rr + aa - bb - cc );
    m[ 0u ][ 1u ] = inverseSquaredLength * ( rc2 + ab2 );
    m[ 0u ][ 2u ] = inverseSquaredLength * ( ac2 - rb2 );

    m[ 1u ][ 0u ] = inverseSquaredLength * ( ab2 - rc2 );
    m[ 1u ][ 1u ] = inverseSquaredLength * ( rr - aa + bb - cc );
    m[ 1u ][ 2u ] = inverseSquaredLength * ( ra2 + bc2 );

    m[ 2u ][ 0u ] = inverseSquaredLength * ( rb2 + ac2 );
    m[ 2u ][ 1u ] = inverseSquaredLength * ( bc2 - ra2 );
    m[ 2u ][ 2u ] = inverseSquaredLength * ( rr - aa - bb + cc );
}

GXVoid GXMat3::From ( const GXMat4 &matrix )
{
    GXUPointer lineSize = 3u * sizeof ( GXFloat );

    memcpy ( data, matrix.data, lineSize );
    memcpy ( data + 3u, matrix.data + 4u, lineSize );
    memcpy ( data + 6u, matrix.data + 8u, lineSize );
}

GXVoid GXMat3::From ( const GXVec3 &zDirection )
{
    GXVec3 xAxis;
    GXVec3 yAxis;

    if ( zDirection.DotProduct ( GXVec3::GetAbsoluteX () ) < 0.5f )
    {
        GXVec3 tmp;
        tmp.CrossProduct ( zDirection, GXVec3::GetAbsoluteX () );
        xAxis.CrossProduct ( tmp, zDirection );
        xAxis.Normalize ();
        yAxis.CrossProduct ( zDirection, xAxis );
    }
    else
    {
        GXVec3 tmp;
        tmp.CrossProduct ( zDirection, GXVec3::GetAbsoluteY () );
        yAxis.CrossProduct ( zDirection, tmp );
        yAxis.Normalize ();
        xAxis.CrossProduct ( yAxis, zDirection );
    }

    SetX ( xAxis );
    SetY ( yAxis );
    SetZ ( zDirection );
}

GXVoid GXMat3::FromFast ( const GXQuat &quaternion )
{
    GXFloat rr = quaternion.data[ 0u ] * quaternion.data[ 0u ];
    GXFloat ra2 = quaternion.data[ 0u ] * quaternion.data[ 1u ] * 2.0f;
    GXFloat rb2 = quaternion.data[ 0u ] * quaternion.data[ 2u ] * 2.0f;
    GXFloat rc2 = quaternion.data[ 0u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat aa = quaternion.data[ 1u ] * quaternion.data[ 1u ];
    GXFloat ab2 = quaternion.data[ 1u ] * quaternion.data[ 2u ] * 2.0f;
    GXFloat ac2 = quaternion.data[ 1u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat bb = quaternion.data[ 2u ] * quaternion.data[ 2u ];
    GXFloat bc2 = quaternion.data[ 2u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat cc = quaternion.data[ 3u ] * quaternion.data[ 3u ];

    m[ 0u ][ 0u ] = rr + aa - bb - cc;
    m[ 0u ][ 1u ] = rc2 + ab2;
    m[ 0u ][ 2u ] = ac2 - rb2;

    m[ 1u ][ 0u ] = ab2 - rc2;
    m[ 1u ][ 1u ] = rr - aa + bb - cc;
    m[ 1u ][ 2u ] = ra2 + bc2;

    m[ 2u ][ 0u ] = rb2 + ac2;
    m[ 2u ][ 1u ] = bc2 - ra2;
    m[ 2u ][ 2u ] = rr - aa - bb + cc;
}

GXVoid GXMat3::SetX ( const GXVec3 &x )
{
    memcpy ( data, &x, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::GetX ( GXVec3 &x ) const
{
    memcpy ( &x, data, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::SetY ( const GXVec3& y )
{
    memcpy ( data + 3u, &y, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::GetY ( GXVec3 &y ) const
{
    memcpy ( &y, data + 3u, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::SetZ ( const GXVec3 &z )
{
    memcpy ( data + 6u, &z, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::GetZ ( GXVec3 &z ) const
{
    memcpy ( &z, data + 6u, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::Identity ()
{
    m[ 0u ][ 0u ] = m[ 1u ][ 1u ] = m[ 2u ][ 2u ] = 1.0f;
    m[ 0u ][ 1u ] = m[ 0u ][ 2u ] = 0.0f;
    m[ 1u ][ 0u ] = m[ 1u ][ 2u ] = 0.0f;
    m[ 2u ][ 0u ] = m[ 2u ][ 1u ] = 0.0f;
}

GXVoid GXMat3::Zeros ()
{
    memset ( data, 0, 9u * sizeof ( GXFloat ) );
}

GXVoid GXMat3::Inverse ( const GXMat3 &sourceMatrix )
{
    GXFloat determinant = sourceMatrix.m[ 0u ][ 0u ] * ( sourceMatrix.m[ 1u ][ 1u ] * sourceMatrix.m[ 2u ][ 2u ] - sourceMatrix.m[ 2u ][ 1u ] * sourceMatrix.m[ 1u ][ 2u ] );
    determinant -= sourceMatrix.m[ 0u ][ 1u ] * ( sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 2u ][ 2u ] - sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 1u ][ 2u ] );
    determinant += sourceMatrix.m[ 0u ][ 2u ] * ( sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 2u ][ 1u ] - sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 1u ][ 1u ] );

    GXFloat invDeterminant = 1.0f / determinant;

    m[ 0u ][ 0u ] = invDeterminant * ( sourceMatrix.m[ 1u ][ 1u ] * sourceMatrix.m[ 2u ][ 2u ] - sourceMatrix.m[ 2u ][ 1u ] * sourceMatrix.m[ 1u ][ 2u ] );
    m[ 0u ][ 1u ] = invDeterminant * ( sourceMatrix.m[ 0u ][ 2u ] * sourceMatrix.m[ 2u ][ 1u ] - sourceMatrix.m[ 2u ][ 2u ] * sourceMatrix.m[ 0u ][ 1u ] );
    m[ 0u ][ 2u ] = invDeterminant * ( sourceMatrix.m[ 0u ][ 1u ] * sourceMatrix.m[ 1u ][ 2u ] - sourceMatrix.m[ 1u ][ 1u ] * sourceMatrix.m[ 0u ][ 2u ] );

    m[ 1u ][ 0u ] = invDeterminant * ( sourceMatrix.m[ 1u ][ 2u ] * sourceMatrix.m[ 2u ][ 0u ] - sourceMatrix.m[ 2u ][ 2u ] * sourceMatrix.m[ 1u ][ 0u ] );
    m[ 1u ][ 1u ] = invDeterminant * ( sourceMatrix.m[ 0u ][ 0u ] * sourceMatrix.m[ 2u ][ 2u ] - sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 0u ][ 2u ] );
    m[ 1u ][ 2u ] = invDeterminant * ( sourceMatrix.m[ 0u ][ 2u ] * sourceMatrix.m[ 1u ][ 0u ] - sourceMatrix.m[ 1u ][ 2u ] * sourceMatrix.m[ 0u ][ 0u ] );

    m[ 2u ][ 0u ] = invDeterminant * ( sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 2u ][ 1u ] - sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 1u ][ 1u ] );
    m[ 2u ][ 1u ] = invDeterminant * ( sourceMatrix.m[ 0u ][ 1u ] * sourceMatrix.m[ 2u ][ 0u ] - sourceMatrix.m[ 2u ][ 1u ] * sourceMatrix.m[ 0u ][ 0u ] );
    m[ 2u ][ 2u ] = invDeterminant * ( sourceMatrix.m[ 0u ][ 0u ] * sourceMatrix.m[ 1u ][ 1u ] - sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 0u ][ 1u ] );
}

GXVoid GXMat3::Transponse ( const GXMat3 &sourceMatrix )
{
    m[ 0u ][ 0u ] = sourceMatrix.m[ 0u ][ 0u ];
    m[ 0u ][ 1u ] = sourceMatrix.m[ 1u ][ 0u ];
    m[ 0u ][ 2u ] = sourceMatrix.m[ 2u ][ 0u ];

    m[ 1u ][ 0u ] = sourceMatrix.m[ 0u ][ 1u ];
    m[ 1u ][ 1u ] = sourceMatrix.m[ 1u ][ 1u ];
    m[ 1u ][ 2u ] = sourceMatrix.m[ 2u ][ 1u ];

    m[ 2u ][ 0u ] = sourceMatrix.m[ 0u ][ 2u ];
    m[ 2u ][ 1u ] = sourceMatrix.m[ 1u ][ 2u ];
    m[ 2u ][ 2u ] = sourceMatrix.m[ 2u ][ 2u ];
}

GXVoid GXMat3::ClearRotation ( const GXMat3 &sourceMatrix )
{
    GXVec3 outX;
    GXVec3 outY;
    GXVec3 outZ;

    GXVec3 modelX;
    GXVec3 modelY;
    GXVec3 modelZ;

    sourceMatrix.GetX ( modelX );
    sourceMatrix.GetY ( modelY );
    sourceMatrix.GetZ ( modelZ );

    GXVec3 tmp;
    tmp.Multiply ( modelX, 1.0f / modelX.Length () );
    SetX ( tmp );

    tmp.Multiply ( modelY, 1.0f / modelY.Length () );
    SetY ( tmp );

    tmp.Multiply ( modelZ, 1.0f / modelZ.Length () );
    SetZ ( tmp );
}

GXVoid GXMat3::ClearRotation ( const GXMat4 &sourceMatrix )
{
    GXVec3 outX;
    GXVec3 outY;
    GXVec3 outZ;

    GXVec3 modelX;
    GXVec3 modelY;
    GXVec3 modelZ;

    sourceMatrix.GetX ( modelX );
    sourceMatrix.GetY ( modelY );
    sourceMatrix.GetZ ( modelZ );

    GXVec3 tmp;
    tmp.Multiply ( modelX, 1.0f / modelX.Length () );
    SetX ( tmp );

    tmp.Multiply ( modelY, 1.0f / modelY.Length () );
    SetY ( tmp );

    tmp.Multiply ( modelZ, 1.0f / modelZ.Length () );
    SetZ ( tmp );
}

GXVoid GXMat3::SkewSymmetric ( const GXVec3 &base )
{
    m[ 0u ][ 0u ] = m[ 1u ][ 1u ] = m[ 2u ][ 2u ] = 0.0f;

    m[ 0u ][ 1u ] = -base.GetZ ();
    m[ 0u ][ 2u ] = base.GetY ();

    m[ 1u ][ 0u ] = base.GetZ ();
    m[ 1u ][ 2u ] = -base.GetX ();

    m[ 2u ][ 0u ] = -base.GetY ();
    m[ 2u ][ 1u ] = base.GetX ();
}

GXVoid GXMat3::Sum ( const GXMat3 &a, const GXMat3 &b )
{
    m[ 0u ][ 0u ] = a.m[ 0u ][ 0u ] + b.m[ 0u ][ 0u ];
    m[ 0u ][ 1u ] = a.m[ 0u ][ 1u ] + b.m[ 0u ][ 1u ];
    m[ 0u ][ 2u ] = a.m[ 0u ][ 2u ] + b.m[ 0u ][ 2u ];

    m[ 1u ][ 0u ] = a.m[ 1u ][ 0u ] + b.m[ 1u ][ 0u ];
    m[ 1u ][ 1u ] = a.m[ 1u ][ 1u ] + b.m[ 1u ][ 1u ];
    m[ 1u ][ 2u ] = a.m[ 1u ][ 2u ] + b.m[ 1u ][ 2u ];

    m[ 2u ][ 0u ] = a.m[ 2u ][ 0u ] + b.m[ 2u ][ 0u ];
    m[ 2u ][ 1u ] = a.m[ 2u ][ 1u ] + b.m[ 2u ][ 1u ];
    m[ 2u ][ 2u ] = a.m[ 2u ][ 2u ] + b.m[ 2u ][ 2u ];
}

GXVoid GXMat3::Substract ( const GXMat3 &a, const GXMat3 &b )
{
    m[ 0u ][ 0u ] = a.m[ 0u ][ 0u ] - b.m[ 0u ][ 0u ];
    m[ 0u ][ 1u ] = a.m[ 0u ][ 1u ] - b.m[ 0u ][ 1u ];
    m[ 0u ][ 2u ] = a.m[ 0u ][ 2u ] - b.m[ 0u ][ 2u ];

    m[ 1u ][ 0u ] = a.m[ 1u ][ 0u ] - b.m[ 1u ][ 0u ];
    m[ 1u ][ 1u ] = a.m[ 1u ][ 1u ] - b.m[ 1u ][ 1u ];
    m[ 1u ][ 2u ] = a.m[ 1u ][ 2u ] - b.m[ 1u ][ 2u ];

    m[ 2u ][ 0u ] = a.m[ 2u ][ 0u ] - b.m[ 2u ][ 0u ];
    m[ 2u ][ 1u ] = a.m[ 2u ][ 1u ] - b.m[ 2u ][ 1u ];
    m[ 2u ][ 2u ] = a.m[ 2u ][ 2u ] - b.m[ 2u ][ 2u ];
}

GXVoid GXMat3::Multiply ( const GXMat3 &a, const GXMat3 &b )
{
    m[ 0u ][ 0u ] = a.m[ 0u ][ 0u ] * b.m[ 0u ][ 0u ] + a.m[ 0u ][ 1u ] * b.m[ 1u ][ 0u ] + a.m[ 0u ][ 2u ] * b.m[ 2u ][ 0u ];
    m[ 0u ][ 1u ] = a.m[ 0u ][ 0u ] * b.m[ 0u ][ 1u ] + a.m[ 0u ][ 1u ] * b.m[ 1u ][ 1u ] + a.m[ 0u ][ 2u ] * b.m[ 2u ][ 1u ];
    m[ 0u ][ 2u ] = a.m[ 0u ][ 0u ] * b.m[ 0u ][ 2u ] + a.m[ 0u ][ 1u ] * b.m[ 1u ][ 2u ] + a.m[ 0u ][ 2u ] * b.m[ 2u ][ 2u ];

    m[ 1u ][ 0u ] = a.m[ 1u ][ 0u ] * b.m[ 0u ][ 0u ] + a.m[ 1u ][ 1u ] * b.m[ 1u ][ 0u ] + a.m[ 1u ][ 2u ] * b.m[ 2u ][ 0u ];
    m[ 1u ][ 1u ] = a.m[ 1u ][ 0u ] * b.m[ 0u ][ 1u ] + a.m[ 1u ][ 1u ] * b.m[ 1u ][ 1u ] + a.m[ 1u ][ 2u ] * b.m[ 2u ][ 1u ];
    m[ 1u ][ 2u ] = a.m[ 1u ][ 0u ] * b.m[ 0u ][ 2u ] + a.m[ 1u ][ 1u ] * b.m[ 1u ][ 2u ] + a.m[ 1u ][ 2u ] * b.m[ 2u ][ 2u ];

    m[ 2u ][ 0u ] = a.m[ 2u ][ 0u ] * b.m[ 0u ][ 0u ] + a.m[ 2u ][ 1u ] * b.m[ 1u ][ 0u ] + a.m[ 2u ][ 2u ] * b.m[ 2u ][ 0u ];
    m[ 2u ][ 1u ] = a.m[ 2u ][ 0u ] * b.m[ 0u ][ 1u ] + a.m[ 2u ][ 1u ] * b.m[ 1u ][ 1u ] + a.m[ 2u ][ 2u ] * b.m[ 2u ][ 1u ];
    m[ 2u ][ 2u ] = a.m[ 2u ][ 0u ] * b.m[ 0u ][ 2u ] + a.m[ 2u ][ 1u ] * b.m[ 1u ][ 2u ] + a.m[ 2u ][ 2u ] * b.m[ 2u ][ 2u ];
}

GXVoid GXMat3::MultiplyVectorMatrix ( GXVec3 &out, const GXVec3 &v ) const
{
    out.data[ 0u ] = v.data[ 0u ] * m[ 0u ][ 0u ] + v.data[ 1u ] * m[ 1u ][ 0u ] + v.data[ 2u ] * m[ 2u ][ 0u ];
    out.data[ 1u ] = v.data[ 0u ] * m[ 0u ][ 1u ] + v.data[ 1u ] * m[ 1u ][ 1u ] + v.data[ 2u ] * m[ 2u ][ 1u ];
    out.data[ 2u ] = v.data[ 0u ] * m[ 0u ][ 2u ] + v.data[ 1u ] * m[ 1u ][ 2u ] + v.data[ 2u ] * m[ 2u ][ 2u ];
}

GXVoid GXMat3::MultiplyMatrixVector ( GXVec3 &out, const GXVec3 &v ) const
{
    out.data[ 0u ] = m[ 0u ][ 0u ] * v.data[ 0u ] + m[ 0u ][ 1u ] * v.data[ 1u ] + m[ 0u ][ 2u ] * v.data[ 2u ];
    out.data[ 1u ] = m[ 1u ][ 0u ] * v.data[ 0u ] + m[ 1u ][ 1u ] * v.data[ 1u ] + m[ 1u ][ 2u ] * v.data[ 2u ];
    out.data[ 2u ] = m[ 2u ][ 0u ] * v.data[ 0u ] + m[ 2u ][ 1u ] * v.data[ 1u ] + m[ 2u ][ 2u ] * v.data[ 2u ];
}

GXVoid GXMat3::Multiply ( const GXMat3 &a, GXFloat factor )
{
    m[ 0u ][ 0u ] = a.m[ 0u ][ 0u ] * factor;
    m[ 0u ][ 1u ] = a.m[ 0u ][ 1u ] * factor;
    m[ 0u ][ 2u ] = a.m[ 0u ][ 2u ] * factor;

    m[ 1u ][ 0u ] = a.m[ 1u ][ 0u ] * factor;
    m[ 1u ][ 1u ] = a.m[ 1u ][ 1u ] * factor;
    m[ 1u ][ 2u ] = a.m[ 1u ][ 2u ] * factor;

    m[ 2u ][ 0u ] = a.m[ 2u ][ 0u ] * factor;
    m[ 2u ][ 1u ] = a.m[ 2u ][ 1u ] * factor;
    m[ 2u ][ 2u ] = a.m[ 2u ][ 2u ] * factor;
}

GXMat3& GXMat3::operator = ( const GXMat3 &matrix )
{
    memcpy ( this, &matrix, sizeof ( GXMat3 ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXMat4::GXMat4 ()
{
    memset ( data, 0, 16 * sizeof ( GXFloat ) );
}

GXMat4::GXMat4 ( const GXMat4 &other )
{
    memcpy ( this, &other, sizeof ( GXMat4 ) );
} 

GXVoid GXMat4::SetRotation ( const GXQuat &quaternion )
{
    GXFloat rr = quaternion.data[ 0u ] * quaternion.data[ 0u ];
    GXFloat ra2 = quaternion.data[ 0u ] * quaternion.data[ 1u ] * 2.0f;
    GXFloat rb2 = quaternion.data[ 0u ] * quaternion.data[ 2u ] * 2.0f;
    GXFloat rc2 = quaternion.data[ 0u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat aa = quaternion.data[ 1u ] * quaternion.data[ 1u ];
    GXFloat ab2 = quaternion.data[ 1u ] * quaternion.data[ 2u ] * 2.0f;
    GXFloat ac2 = quaternion.data[ 1u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat bb = quaternion.data[ 2u ] * quaternion.data[ 2u ];
    GXFloat bc2 = quaternion.data[ 2u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat cc = quaternion.data[ 3u ] * quaternion.data[ 3u ];

    GXFloat inverseSquaredLength = 1.0f / ( rr + aa + bb + cc );

    m[ 0u ][ 0u ] = inverseSquaredLength * ( rr + aa - bb - cc );
    m[ 0u ][ 1u ] = inverseSquaredLength * ( rc2 + ab2 );
    m[ 0u ][ 2u ] = inverseSquaredLength * ( ac2 - rb2 );

    m[ 1u ][ 0u ] = inverseSquaredLength * ( ab2 - rc2 );
    m[ 1u ][ 1u ] = inverseSquaredLength * ( rr - aa + bb - cc );
    m[ 1u ][ 2u ] = inverseSquaredLength * ( ra2 + bc2 );

    m[ 2u ][ 0u ] = inverseSquaredLength * ( rb2 + ac2 );
    m[ 2u ][ 1u ] = inverseSquaredLength * ( bc2 - ra2 );
    m[ 2u ][ 2u ] = inverseSquaredLength * ( rr - aa - bb + cc );
}

GXVoid GXMat4::SetRotationFast ( const GXQuat &quaternion )
{
    GXFloat rr = quaternion.data[ 0u ] * quaternion.data[ 0u ];
    GXFloat ra2 = quaternion.data[ 0u ] * quaternion.data[ 1u ] * 2.0f;
    GXFloat rb2 = quaternion.data[ 0u ] * quaternion.data[ 2u ] * 2.0f;
    GXFloat rc2 = quaternion.data[ 0u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat aa = quaternion.data[ 1u ] * quaternion.data[ 1u ];
    GXFloat ab2 = quaternion.data[ 1u ] * quaternion.data[ 2u ] * 2.0f;
    GXFloat ac2 = quaternion.data[ 1u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat bb = quaternion.data[ 2u ] * quaternion.data[ 2u ];
    GXFloat bc2 = quaternion.data[ 2u ] * quaternion.data[ 3u ] * 2.0f;

    GXFloat cc = quaternion.data[ 3u ] * quaternion.data[ 3u ];

    m[ 0u ][ 0u ] = rr + aa - bb - cc;
    m[ 0u ][ 1u ] = rc2 + ab2;
    m[ 0u ][ 2u ] = ac2 - rb2;

    m[ 1u ][ 0u ] = ab2 - rc2;
    m[ 1u ][ 1u ] = rr - aa + bb - cc;
    m[ 1u ][ 2u ] = ra2 + bc2;

    m[ 2u ][ 0u ] = rb2 + ac2;
    m[ 2u ][ 1u ] = bc2 - ra2;
    m[ 2u ][ 2u ] = rr - aa - bb + cc;
}

GXVoid GXMat4::SetOrigin ( const GXVec3 &origin )
{ 
    SetW ( origin );
}

GXVoid GXMat4::From ( const GXQuat &quaternion, const GXVec3 &origin )
{
    SetRotation ( quaternion );
    SetOrigin ( origin );

    m[ 0u ][ 3u ] = m[ 1u ][ 3u ] = m[ 2u ][ 3u ] = 0.0f;
    m[ 3u ][ 3u ] = 1.0f;
}

GXVoid GXMat4::From ( const GXMat3 &rotation, const GXVec3 &origin )
{
    GXVec3 tmp;
    rotation.GetX ( tmp );
    SetX ( tmp );

    rotation.GetY ( tmp );
    SetY ( tmp );

    rotation.GetZ ( tmp );
    SetZ ( tmp );

    SetW ( origin );

    m[ 0u ][ 3u ] = m[ 1u ][ 3u ] = m[ 2u ][ 3u ] = 0.0f;
    m[ 3u ][ 3u ] = 1.0f;
}

GXVoid GXMat4::From ( const GXVec3 &zDirection, const GXVec3 &origin )
{
    GXVec3 xAxis;
    GXVec3 yAxis;

    if ( zDirection.DotProduct ( GXVec3::GetAbsoluteX () ) < 0.5f )
    {
        GXVec3 tmp;
        tmp.CrossProduct ( zDirection, GXVec3::GetAbsoluteX () );
        xAxis.CrossProduct ( tmp, zDirection );
        xAxis.Normalize ();
        yAxis.CrossProduct ( zDirection, xAxis );
    }
    else
    {
        GXVec3 tmp;
        tmp.CrossProduct ( zDirection, GXVec3::GetAbsoluteY () );
        yAxis.CrossProduct ( zDirection, tmp );
        yAxis.Normalize ();
        xAxis.CrossProduct ( yAxis, zDirection );
    }

    SetX ( xAxis );
    SetY ( yAxis );
    SetZ ( zDirection );
    SetW ( origin );

    m[ 3u ][ 3u ] = 1.0f;
}

GXVoid GXMat4::FromFast ( const GXQuat &quaternion, const GXVec3 &origin )
{
    SetRotationFast ( quaternion );
    SetOrigin ( origin );

    m[ 0u ][ 3u ] = m[ 1u ][ 3u ] = m[ 2u ][ 3u ] = 0.0f;
    m[ 3u ][ 3u ] = 1.0f;
}

GXVoid GXMat4::SetX ( const GXVec3 &x )
{
    memcpy ( data, &x, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetX ( GXVec3 &x ) const
{
    memcpy ( &x, data, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::SetY ( const GXVec3 &y )
{
    memcpy ( data + 4u, &y, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetY ( GXVec3 &y ) const
{
    memcpy ( &y, data + 4u, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::SetZ ( const GXVec3 &z )
{
    memcpy ( data + 8u, &z, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetZ ( GXVec3 &z ) const
{
    memcpy ( &z, data + 8u, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::SetW ( const GXVec3 &w )
{
    memcpy ( data + 12u, &w, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetW ( GXVec3 &w ) const
{
    memcpy ( &w, data + 12u, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::Identity ()
{
    m[ 0u ][ 1u ] = m[ 0u ][ 2u ] = m[ 0u ][ 3u ] = 0.0f;
    m[ 1u ][ 0u ] = m[ 1u ][ 2u ] = m[ 1u ][ 3u ] = 0.0f;
    m[ 2u ][ 0u ] = m[ 2u ][ 1u ] = m[ 2u ][ 3u ] = 0.0f;
    m[ 3u ][ 0u ] = m[ 3u ][ 1u ] = m[ 3u ][ 2u ] = 0.0f;

    m[ 0u ][ 0u ] = m[ 1u ][ 1u ] = m[ 2u ][ 2u ] = m[ 3u ][ 3u ] = 1.0f;
}

GXVoid GXMat4::Perspective ( GXFloat fieldOfViewYRadiands, GXFloat aspectRatio, GXFloat zNear, GXFloat zFar )
{
    GXFloat halfFovy = fieldOfViewYRadiands * 0.5f;
    GXFloat ctan = cosf ( halfFovy ) / sinf ( halfFovy );
    GXFloat invRange = 1.0f / ( zFar - zNear );

    m[ 0u ][ 0u ] = ctan / aspectRatio;
    m[ 1u ][ 1u ] = ctan;
    m[ 2u ][ 2u ] = ( zFar + zNear ) * invRange;
    m[ 2u ][ 3u ] = 1.0f;
    m[ 3u ][ 2u ] = -2.0f * zFar * zNear * invRange;

    m[ 0u ][ 1u ] = m[ 0u ][ 2u ] = m[ 0u ][ 3u ] = 0.0f;
    m[ 1u ][ 0u ] = m[ 1u ][ 2u ] = m[ 1u ][ 3u ] = 0.0f;
    m[ 2u ][ 0u ] = m[ 2u ][ 1u ] = 0.0f;
    m[ 3u ][ 0u ] = m[ 3u ][ 1u ] = m[ 3u ][ 3u ] = 0.0f;
}

GXVoid GXMat4::Ortho ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar )
{
    GXFloat invRange = 1.0f / ( zFar - zNear );

    m[ 0u ][ 0u ] = 2.0f / width;
    m[ 1u ][ 1u ] = 2.0f / height;
    m[ 2u ][ 2u ] = 2.0f * invRange;
    m[ 3u ][ 2u ] = -invRange * ( zFar + zNear );

    m[ 0u ][ 1u ] = m[ 0u ][ 2u ] = m[ 0u ][ 3u ] = 0.0f;
    m[ 1u ][ 0u ] = m[ 1u ][ 2u ] = m[ 1u ][ 3u ] = 0.0f;
    m[ 2u ][ 0u ] = m[ 2u ][ 1u ] = m[ 2u ][ 3u ] = 0.0f;
    m[ 3u ][ 0u ] = m[ 3u ][ 1u ] = 0.0f;
    m[ 3u ][ 3u ] = 1.0f;
}

GXVoid GXMat4::Translation ( GXFloat x, GXFloat y, GXFloat z )
{
    m[ 0u ][ 1u ] = m[ 0u ][ 2u ] = m[ 0u ][ 3u ] = 0.0f;
    m[ 1u ][ 0u ] = m[ 1u ][ 2u ] = m[ 1u ][ 3u ] = 0.0f;
    m[ 2u ][ 0u ] = m[ 2u ][ 1u ] = m[ 2u ][ 3u ] = 0.0f;

    m[ 0u ][ 0u ] = m[ 1u ][ 1u ] = m[ 2u ][ 2u ] = m[ 3u ][ 3u ] = 1.0f;

    m[ 3u ][ 0u ] = x;
    m[ 3u ][ 1u ] = y;
    m[ 3u ][ 2u ] = z;
}

GXVoid GXMat4::TranslateTo ( GXFloat x, GXFloat y, GXFloat z )
{
    GXVec3 origin ( x, y, z );
    SetW ( origin );
}

GXVoid GXMat4::TranslateTo ( const GXVec3 &location )
{
    SetW ( location );
}

GXVoid GXMat4::RotationX ( GXFloat angle )
{
    m[ 0u ][ 1u ] = m[ 0u ][ 2u ] = 0.0f;
    m[ 1u ][ 0u ] = 0.0f;
    m[ 2u ][ 0u ] = 0.0f;

    GXFloat c;
    GXFloat s;

    c = cosf ( angle );
    s = sinf ( angle );

    m[ 1u ][ 1u ] = c;      m[ 1u ][ 2u ] = s;
    m[ 2u ][ 1u ] = -s;     m[ 2u ][ 2u ] = c;

    m[ 0u ][ 0u ] = 1.0f;

    m[ 3u ][ 3u ] = 1.0f;
    m[ 3u ][ 0u ] = m[ 3u ][ 1u ] = m[ 3u ][ 2u ] = m[ 0u ][ 3u ] = m[ 1u ][ 3u ] = m[ 2u ][ 3u ] = 0.0f;
}

GXVoid GXMat4::RotationY ( GXFloat angle )
{
    m[ 0u ][ 1u ] = 0.0f;
    m[ 1u ][ 0u ] = m[ 1u ][ 2u ] = 0.0f;
    m[ 2u ][ 1u ] = 0.0f;

    GXFloat c;
    GXFloat s;

    c = cosf ( angle );
    s = sinf ( angle );

    m[ 0u ][ 0u ] = c;      m[ 0u ][ 2u ] = -s;
    m[ 2u ][ 0u ] = s;      m[ 2u ][ 2u ] = c;

    m[ 1u ][ 1u ] = 1.0f;

    m[ 3u ][ 3u ] = 1.0f;
    m[ 3u ][ 0u ] = m[ 3u ][ 1u ] = m[ 3u ][ 2u ] = m[ 0u ][ 3u ] = m[ 1u ][ 3u ] = m[ 2u ][ 3u ] = 0.0f;
}

GXVoid GXMat4::RotationZ ( GXFloat angle )
{
    m[ 0u ][ 2u ] = 0.0f;
    m[ 1u ][ 2u ] = 0.0f;
    m[ 2u ][ 0u ] = m[ 2u ][ 1u ] = 0.0f;

    GXFloat c;
    GXFloat s;

    c = cosf ( angle );
    s = sinf ( angle );

    m[ 0u ][ 0u ] = c;      m[ 0u ][ 1u ] = s;
    m[ 1u ][ 0u ] = -s;     m[ 1u ][ 1u ] = c;

    m[ 2u ][ 2u ] = 1.0f;

    m[ 3u ][ 3u ] = 1.0f;
    m[ 3u ][ 0u ] = m[ 3u ][ 1u ] = m[ 3u ][ 2u ] = m[ 0u ][ 3u ] = m[ 1u ][ 3u ] = m[ 2u ][ 3u ] = 0.0f;
}

GXVoid GXMat4::RotationXY ( GXFloat pitchRadians, GXFloat yawRadians )
{
    GXMat4 x;
    x.RotationX ( pitchRadians );

    GXMat4 y;
    y.RotationY ( yawRadians );

    Multiply ( x, y );
}

GXVoid GXMat4::RotationXYZ ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
    GXMat4 x;
    x.RotationX ( pitchRadians );

    GXMat4 y;
    y.RotationY ( yawRadians );

    GXMat4 z;
    z.RotationZ ( rollRadians );

    GXMat4 temp;
    temp.Multiply ( x, y );

    Multiply ( temp, z );
}

GXVoid GXMat4::ClearRotation ( const GXMat3 &sourceMatrix )
{
    GXVec3 outX;
    GXVec3 outY;
    GXVec3 outZ;

    GXVec3 modelX;
    GXVec3 modelY;
    GXVec3 modelZ;

    sourceMatrix.GetX ( modelX );
    sourceMatrix.GetY ( modelY );
    sourceMatrix.GetZ ( modelZ );

    GXVec3 tmp;
    tmp.Multiply ( modelX, 1.0f / modelX.Length () );
    SetX ( tmp );

    tmp.Multiply ( modelY, 1.0f / modelY.Length () );
    SetY ( tmp );

    tmp.Multiply ( modelZ, 1.0f / modelZ.Length () );
    SetZ ( tmp );

    m[ 0u ][ 3u ] = m[ 1u ][ 3u ] = m[ 2u ][ 3u ] = 0.0f;
    m[ 3u ][ 0u ] = m[ 3u ][ 1u ] = m[ 3u ][ 2u ] = 0.0f;

    m[ 3u ][ 3u ] = 1.0f;
}

GXVoid GXMat4::ClearRotation ( const GXMat4 &sourceMatrix )
{
    GXVec3 outX;
    GXVec3 outY;
    GXVec3 outZ;

    GXVec3 modelX;
    GXVec3 modelY;
    GXVec3 modelZ;

    sourceMatrix.GetX ( modelX );
    sourceMatrix.GetY ( modelY );
    sourceMatrix.GetZ ( modelZ );

    GXVec3 tmp;
    tmp.Multiply ( modelX, 1.0f / modelX.Length () );
    SetX ( tmp );

    tmp.Multiply ( modelY, 1.0f / modelY.Length () );
    SetY ( tmp );

    tmp.Multiply ( modelZ, 1.0f / modelZ.Length () );
    SetZ ( tmp );

    m[ 0u ][ 3u ] = m[ 1u ][ 3u ] = m[ 2u ][ 3u ] = 0.0f;
    m[ 3u ][ 0u ] = m[ 3u ][ 1u ] = m[ 3u ][ 2u ] = 0.0f;

    m[ 3u ][ 3u ] = 1.0f;
}

GXVoid GXMat4::Scale ( GXFloat x, GXFloat y, GXFloat z )
{
    m[ 0u ][ 1u ] = m[ 0u ][ 2u ] = 0.0f;
    m[ 1u ][ 0u ] = m[ 1u ][ 2u ] = 0.0f;
    m[ 2u ][ 0u ] = m[ 2u ][ 1u ] = 0.0f;

    m[ 0u ][ 0u ] = x;
    m[ 1u ][ 1u ] = y;
    m[ 2u ][ 2u ] = z;

    m[ 3u ][ 3u ] = 1.0f;
    m[ 3u ][ 0u ] = m[ 3u ][ 1u ] = m[ 3u ][ 2u ] = m[ 0u ][ 3u ] = m[ 1u ][ 3u ] = m[ 2u ][ 3u ] = 0.0f;
}

GXVoid GXMat4::ClearScale ( GXVec3 &scale ) const
{
    GXVec3 alpha;

    GetX ( alpha );
    scale.data[ 0u ] = alpha.Length ();

    GetY ( alpha );
    scale.data[ 1u ] = alpha.Length ();

    GetZ ( alpha );
    scale.data[ 2u ] = alpha.Length ();
}

GXVoid GXMat4::Inverse ( const GXMat4 &sourceMatrix )
{
    // 2x2 sub-determinants required to calculate 4x4 determinant
    GXFloat det2_01_01 = sourceMatrix.m[ 0u ][ 0u ] * sourceMatrix.m[ 1u ][ 1u ] - sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 0u ][ 1u ];
    GXFloat det2_01_02 = sourceMatrix.m[ 0u ][ 0u ] * sourceMatrix.m[ 2u ][ 1u ] - sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 0u ][ 1u ];
    GXFloat det2_01_03 = sourceMatrix.m[ 0u ][ 0u ] * sourceMatrix.m[ 3u ][ 1u ] - sourceMatrix.m[ 3u ][ 0u ] * sourceMatrix.m[ 0u ][ 1u ];
    GXFloat det2_01_12 = sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 2u ][ 1u ] - sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 1u ][ 1u ];
    GXFloat det2_01_13 = sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 3u ][ 1u ] - sourceMatrix.m[ 3u ][ 0u ] * sourceMatrix.m[ 1u ][ 1u ];
    GXFloat det2_01_23 = sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 3u ][ 1u ] - sourceMatrix.m[ 3u ][ 0u ] * sourceMatrix.m[ 2u ][ 1u ];

    // 3x3 sub-determinants required to calculate 4x4 determinant
    GXFloat det3_201_012 = sourceMatrix.m[ 0u ][ 2u ] * det2_01_12 - sourceMatrix.m[ 1u ][ 2u ] * det2_01_02 + sourceMatrix.m[ 2u ][ 2u ] * det2_01_01;
    GXFloat det3_201_013 = sourceMatrix.m[ 0u ][ 2u ] * det2_01_13 - sourceMatrix.m[ 1u ][ 2u ] * det2_01_03 + sourceMatrix.m[ 3u ][ 2u ] * det2_01_01;
    GXFloat det3_201_023 = sourceMatrix.m[ 0u ][ 2u ] * det2_01_23 - sourceMatrix.m[ 2u ][ 2u ] * det2_01_03 + sourceMatrix.m[ 3u ][ 2u ] * det2_01_02;
    GXFloat det3_201_123 = sourceMatrix.m[ 1u ][ 2u ] * det2_01_23 - sourceMatrix.m[ 2u ][ 2u ] * det2_01_13 + sourceMatrix.m[ 3u ][ 2u ] * det2_01_12;

    GXFloat inverseDeterminant = 1.0f / ( -det3_201_123 * sourceMatrix.m[ 0u ][ 3u ] + det3_201_023 * sourceMatrix.m[ 1u ][ 3u ] - det3_201_013 * sourceMatrix.m[ 2u ][ 3u ] + det3_201_012 * sourceMatrix.m[ 3u ][ 3u ] );

    // remaining 2x2 sub-determinants
    GXFloat det2_03_01 = sourceMatrix.m[ 0u ][ 0u ] * sourceMatrix.m[ 1u ][ 3u ] - sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 0u ][ 3u ];
    GXFloat det2_03_02 = sourceMatrix.m[ 0u ][ 0u ] * sourceMatrix.m[ 2u ][ 3u ] - sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 0u ][ 3u ];
    GXFloat det2_03_03 = sourceMatrix.m[ 0u ][ 0u ] * sourceMatrix.m[ 3u ][ 3u ] - sourceMatrix.m[ 3u ][ 0u ] * sourceMatrix.m[ 0u ][ 3u ];
    GXFloat det2_03_12 = sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 2u ][ 3u ] - sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 1u ][ 3u ];
    GXFloat det2_03_13 = sourceMatrix.m[ 1u ][ 0u ] * sourceMatrix.m[ 3u ][ 3u ] - sourceMatrix.m[ 3u ][ 0u ] * sourceMatrix.m[ 1u ][ 3u ];
    GXFloat det2_03_23 = sourceMatrix.m[ 2u ][ 0u ] * sourceMatrix.m[ 3u ][ 3u ] - sourceMatrix.m[ 3u ][ 0u ] * sourceMatrix.m[ 2u ][ 3u ];

    GXFloat det2_13_01 = sourceMatrix.m[ 0u ][ 1u ] * sourceMatrix.m[ 1u ][ 3u ] - sourceMatrix.m[ 1u ][ 1u ] * sourceMatrix.m[ 0u ][ 3u ];
    GXFloat det2_13_02 = sourceMatrix.m[ 0u ][ 1u ] * sourceMatrix.m[ 2u ][ 3u ] - sourceMatrix.m[ 2u ][ 1u ] * sourceMatrix.m[ 0u ][ 3u ];
    GXFloat det2_13_03 = sourceMatrix.m[ 0u ][ 1u ] * sourceMatrix.m[ 3u ][ 3u ] - sourceMatrix.m[ 3u ][ 1u ] * sourceMatrix.m[ 0u ][ 3u ];
    GXFloat det2_13_12 = sourceMatrix.m[ 1u ][ 1u ] * sourceMatrix.m[ 2u ][ 3u ] - sourceMatrix.m[ 2u ][ 1u ] * sourceMatrix.m[ 1u ][ 3u ];
    GXFloat det2_13_13 = sourceMatrix.m[ 1u ][ 1u ] * sourceMatrix.m[ 3u ][ 3u ] - sourceMatrix.m[ 3u ][ 1u ] * sourceMatrix.m[ 1u ][ 3u ];
    GXFloat det2_13_23 = sourceMatrix.m[ 2u ][ 1u ] * sourceMatrix.m[ 3u ][ 3u ] - sourceMatrix.m[ 3u ][ 1u ] * sourceMatrix.m[ 2u ][ 3u ];

    // remaining 3x3 sub-determinants
    GXFloat det3_203_012 = sourceMatrix.m[ 0u ][ 2u ] * det2_03_12 - sourceMatrix.m[ 1u ][ 2u ] * det2_03_02 + sourceMatrix.m[ 2u ][ 2u ] * det2_03_01;
    GXFloat det3_203_013 = sourceMatrix.m[ 0u ][ 2u ] * det2_03_13 - sourceMatrix.m[ 1u ][ 2u ] * det2_03_03 + sourceMatrix.m[ 3u ][ 2u ] * det2_03_01;
    GXFloat det3_203_023 = sourceMatrix.m[ 0u ][ 2u ] * det2_03_23 - sourceMatrix.m[ 2u ][ 2u ] * det2_03_03 + sourceMatrix.m[ 3u ][ 2u ] * det2_03_02;
    GXFloat det3_203_123 = sourceMatrix.m[ 1u ][ 2u ] * det2_03_23 - sourceMatrix.m[ 2u ][ 2u ] * det2_03_13 + sourceMatrix.m[ 3u ][ 2u ] * det2_03_12;

    GXFloat det3_213_012 = sourceMatrix.m[ 0u ][ 2u ] * det2_13_12 - sourceMatrix.m[ 1u ][ 2u ] * det2_13_02 + sourceMatrix.m[ 2u ][ 2u ] * det2_13_01;
    GXFloat det3_213_013 = sourceMatrix.m[ 0u ][ 2u ] * det2_13_13 - sourceMatrix.m[ 1u ][ 2u ] * det2_13_03 + sourceMatrix.m[ 3u ][ 2u ] * det2_13_01;
    GXFloat det3_213_023 = sourceMatrix.m[ 0u ][ 2u ] * det2_13_23 - sourceMatrix.m[ 2u ][ 2u ] * det2_13_03 + sourceMatrix.m[ 3u ][ 2u ] * det2_13_02;
    GXFloat det3_213_123 = sourceMatrix.m[ 1u ][ 2u ] * det2_13_23 - sourceMatrix.m[ 2u ][ 2u ] * det2_13_13 + sourceMatrix.m[ 3u ][ 2u ] * det2_13_12;

    GXFloat det3_301_012 = sourceMatrix.m[ 0u ][ 3u ] * det2_01_12 - sourceMatrix.m[ 1u ][ 3u ] * det2_01_02 + sourceMatrix.m[ 2u ][ 3u ] * det2_01_01;
    GXFloat det3_301_013 = sourceMatrix.m[ 0u ][ 3u ] * det2_01_13 - sourceMatrix.m[ 1u ][ 3u ] * det2_01_03 + sourceMatrix.m[ 3u ][ 3u ] * det2_01_01;
    GXFloat det3_301_023 = sourceMatrix.m[ 0u ][ 3u ] * det2_01_23 - sourceMatrix.m[ 2u ][ 3u ] * det2_01_03 + sourceMatrix.m[ 3u ][ 3u ] * det2_01_02;
    GXFloat det3_301_123 = sourceMatrix.m[ 1u ][ 3u ] * det2_01_23 - sourceMatrix.m[ 2u ][ 3u ] * det2_01_13 + sourceMatrix.m[ 3u ][ 3u ] * det2_01_12;

    m[ 0u ][ 0u ] = -det3_213_123 * inverseDeterminant;
    m[ 0u ][ 1u ] = +det3_213_023 * inverseDeterminant;
    m[ 0u ][ 2u ] = -det3_213_013 * inverseDeterminant;
    m[ 0u ][ 3u ] = +det3_213_012 * inverseDeterminant;

    m[ 1u ][ 0u ] = +det3_203_123 * inverseDeterminant;
    m[ 1u ][ 1u ] = -det3_203_023 * inverseDeterminant;
    m[ 1u ][ 2u ] = +det3_203_013 * inverseDeterminant;
    m[ 1u ][ 3u ] = -det3_203_012 * inverseDeterminant;

    m[ 2u ][ 0u ] = +det3_301_123 * inverseDeterminant;
    m[ 2u ][ 1u ] = -det3_301_023 * inverseDeterminant;
    m[ 2u ][ 2u ] = +det3_301_013 * inverseDeterminant;
    m[ 2u ][ 3u ] = -det3_301_012 * inverseDeterminant;

    m[ 3u ][ 0u ] = -det3_201_123 * inverseDeterminant;
    m[ 3u ][ 1u ] = +det3_201_023 * inverseDeterminant;
    m[ 3u ][ 2u ] = -det3_201_013 * inverseDeterminant;
    m[ 3u ][ 3u ] = +det3_201_012 * inverseDeterminant;
}

GXVoid GXMat4::Multiply ( const GXMat4 &a, const GXMat4 &b )
{
    m[ 0u ][ 0u ] = a.m[ 0u ][ 0u ] * b.m[ 0u ][ 0u ] + a.m[ 0u ][ 1u ] * b.m[ 1u ][ 0u ] + a.m[ 0u ][ 2u ] * b.m[ 2u ][ 0u ] + a.m[ 0u ][ 3u ] * b.m[ 3u ][ 0u ];
    m[ 0u ][ 1u ] = a.m[ 0u ][ 0u ] * b.m[ 0u ][ 1u ] + a.m[ 0u ][ 1u ] * b.m[ 1u ][ 1u ] + a.m[ 0u ][ 2u ] * b.m[ 2u ][ 1u ] + a.m[ 0u ][ 3u ] * b.m[ 3u ][ 1u ];
    m[ 0u ][ 2u ] = a.m[ 0u ][ 0u ] * b.m[ 0u ][ 2u ] + a.m[ 0u ][ 1u ] * b.m[ 1u ][ 2u ] + a.m[ 0u ][ 2u ] * b.m[ 2u ][ 2u ] + a.m[ 0u ][ 3u ] * b.m[ 3u ][ 2u ];
    m[ 0u ][ 3u ] = a.m[ 0u ][ 0u ] * b.m[ 0u ][ 3u ] + a.m[ 0u ][ 1u ] * b.m[ 1u ][ 3u ] + a.m[ 0u ][ 2u ] * b.m[ 2u ][ 3u ] + a.m[ 0u ][ 3u ] * b.m[ 3u ][ 3u ];

    m[ 1u ][ 0u ] = a.m[ 1u ][ 0u ] * b.m[ 0u ][ 0u ] + a.m[ 1u ][ 1u ] * b.m[ 1u ][ 0u ] + a.m[ 1u ][ 2u ] * b.m[ 2u ][ 0u ] + a.m[ 1u ][ 3u ] * b.m[ 3u ][ 0u ];
    m[ 1u ][ 1u ] = a.m[ 1u ][ 0u ] * b.m[ 0u ][ 1u ] + a.m[ 1u ][ 1u ] * b.m[ 1u ][ 1u ] + a.m[ 1u ][ 2u ] * b.m[ 2u ][ 1u ] + a.m[ 1u ][ 3u ] * b.m[ 3u ][ 1u ];
    m[ 1u ][ 2u ] = a.m[ 1u ][ 0u ] * b.m[ 0u ][ 2u ] + a.m[ 1u ][ 1u ] * b.m[ 1u ][ 2u ] + a.m[ 1u ][ 2u ] * b.m[ 2u ][ 2u ] + a.m[ 1u ][ 3u ] * b.m[ 3u ][ 2u ];
    m[ 1u ][ 3u ] = a.m[ 1u ][ 0u ] * b.m[ 0u ][ 3u ] + a.m[ 1u ][ 1u ] * b.m[ 1u ][ 3u ] + a.m[ 1u ][ 2u ] * b.m[ 2u ][ 3u ] + a.m[ 1u ][ 3u ] * b.m[ 3u ][ 3u ];

    m[ 2u ][ 0u ] = a.m[ 2u ][ 0u ] * b.m[ 0u ][ 0u ] + a.m[ 2u ][ 1u ] * b.m[ 1u ][ 0u ] + a.m[ 2u ][ 2u ] * b.m[ 2u ][ 0u ] + a.m[ 2u ][ 3u ] * b.m[ 3u ][ 0u ];
    m[ 2u ][ 1u ] = a.m[ 2u ][ 0u ] * b.m[ 0u ][ 1u ] + a.m[ 2u ][ 1u ] * b.m[ 1u ][ 1u ] + a.m[ 2u ][ 2u ] * b.m[ 2u ][ 1u ] + a.m[ 2u ][ 3u ] * b.m[ 3u ][ 1u ];
    m[ 2u ][ 2u ] = a.m[ 2u ][ 0u ] * b.m[ 0u ][ 2u ] + a.m[ 2u ][ 1u ] * b.m[ 1u ][ 2u ] + a.m[ 2u ][ 2u ] * b.m[ 2u ][ 2u ] + a.m[ 2u ][ 3u ] * b.m[ 3u ][ 2u ];
    m[ 2u ][ 3u ] = a.m[ 2u ][ 0u ] * b.m[ 0u ][ 3u ] + a.m[ 2u ][ 1u ] * b.m[ 1u ][ 3u ] + a.m[ 2u ][ 2u ] * b.m[ 2u ][ 3u ] + a.m[ 2u ][ 3u ] * b.m[ 3u ][ 3u ];

    m[ 3u ][ 0u ] = a.m[ 3u ][ 0u ] * b.m[ 0u ][ 0u ] + a.m[ 3u ][ 1u ] * b.m[ 1u ][ 0u ] + a.m[ 3u ][ 2u ] * b.m[ 2u ][ 0u ] + a.m[ 3u ][ 3u ] * b.m[ 3u ][ 0u ];
    m[ 3u ][ 1u ] = a.m[ 3u ][ 0u ] * b.m[ 0u ][ 1u ] + a.m[ 3u ][ 1u ] * b.m[ 1u ][ 1u ] + a.m[ 3u ][ 2u ] * b.m[ 2u ][ 1u ] + a.m[ 3u ][ 3u ] * b.m[ 3u ][ 1u ];
    m[ 3u ][ 2u ] = a.m[ 3u ][ 0u ] * b.m[ 0u ][ 2u ] + a.m[ 3u ][ 1u ] * b.m[ 1u ][ 2u ] + a.m[ 3u ][ 2u ] * b.m[ 2u ][ 2u ] + a.m[ 3u ][ 3u ] * b.m[ 3u ][ 2u ];
    m[ 3u ][ 3u ] = a.m[ 3u ][ 0u ] * b.m[ 0u ][ 3u ] + a.m[ 3u ][ 1u ] * b.m[ 1u ][ 3u ] + a.m[ 3u ][ 2u ] * b.m[ 2u ][ 3u ] + a.m[ 3u ][ 3u ] * b.m[ 3u ][ 3u ];
}

GXVoid GXMat4::MultiplyVectorMatrix ( GXVec4 &out, const GXVec4 &v ) const
{
    out.data[ 0u ] = v.data[ 0u ] * m[ 0u ][ 0u ] + v.data[ 1u ] * m[ 1u ][ 0u ] + v.data[ 2u ] * m[ 2u ][ 0u ] + v.data[ 3u ] * m[ 3u ][ 0u ];
    out.data[ 1u ] = v.data[ 0u ] * m[ 0u ][ 1u ] + v.data[ 1u ] * m[ 1u ][ 1u ] + v.data[ 2u ] * m[ 2u ][ 1u ] + v.data[ 3u ] * m[ 3u ][ 1u ];
    out.data[ 2u ] = v.data[ 0u ] * m[ 0u ][ 2u ] + v.data[ 1u ] * m[ 1u ][ 2u ] + v.data[ 2u ] * m[ 2u ][ 2u ] + v.data[ 3u ] * m[ 3u ][ 2u ];
    out.data[ 3u ] = v.data[ 0u ] * m[ 0u ][ 3u ] + v.data[ 1u ] * m[ 1u ][ 3u ] + v.data[ 2u ] * m[ 2u ][ 3u ] + v.data[ 3u ] * m[ 3u ][ 3u ];
}

GXVoid GXMat4::MultiplyMatrixVector ( GXVec4 &out, const GXVec4 &v ) const
{
    out.data[ 0u ] = m[ 0u ][ 0u ] * v.data[ 0u ] + m[ 0u ][ 1u ] * v.data[ 1u ] + m[ 0u ][ 2u ] * v.data[ 2u ] + m[ 0u ][ 3u ] * v.data[ 3u ];
    out.data[ 1u ] = m[ 1u ][ 0u ] * v.data[ 0u ] + m[ 1u ][ 1u ] * v.data[ 1u ] + m[ 1u ][ 2u ] * v.data[ 2u ] + m[ 1u ][ 3u ] * v.data[ 3u ];
    out.data[ 2u ] = m[ 2u ][ 0u ] * v.data[ 0u ] + m[ 2u ][ 1u ] * v.data[ 1u ] + m[ 2u ][ 2u ] * v.data[ 2u ] + m[ 2u ][ 3u ] * v.data[ 3u ];
    out.data[ 3u ] = m[ 3u ][ 0u ] * v.data[ 0u ] + m[ 3u ][ 1u ] * v.data[ 1u ] + m[ 3u ][ 2u ] * v.data[ 2u ] + m[ 3u ][ 3u ] * v.data[ 3u ];
}

GXVoid GXMat4::MultiplyAsNormal ( GXVec3 &out, const GXVec3 &v ) const
{
    out.data[ 0u ] = v.data[ 0u ] * m[ 0u ][ 0u ] + v.data[ 1u ] * m[ 1u ][ 0u ] + v.data[ 2u ] * m[ 2u ][ 0u ];
    out.data[ 1u ] = v.data[ 0u ] * m[ 0u ][ 1u ] + v.data[ 1u ] * m[ 1u ][ 1u ] + v.data[ 2u ] * m[ 2u ][ 1u ];
    out.data[ 2u ] = v.data[ 0u ] * m[ 0u ][ 2u ] + v.data[ 1u ] * m[ 1u ][ 2u ] + v.data[ 2u ] * m[ 2u ][ 2u ];
}

GXVoid GXMat4::MultiplyAsPoint ( GXVec3 &out, const GXVec3 &v ) const
{
    out.data[ 0u ] = v.data[ 0u ] * m[ 0u ][ 0u ] + v.data[ 1u ] * m[ 1u ][ 0u ] + v.data[ 2u ] * m[ 2u ][ 0u ] + m[ 3u ][ 0u ];
    out.data[ 1u ] = v.data[ 0u ] * m[ 0u ][ 1u ] + v.data[ 1u ] * m[ 1u ][ 1u ] + v.data[ 2u ] * m[ 2u ][ 1u ] + m[ 3u ][ 1u ];
    out.data[ 2u ] = v.data[ 0u ] * m[ 0u ][ 2u ] + v.data[ 1u ] * m[ 1u ][ 2u ] + v.data[ 2u ] * m[ 2u ][ 2u ] + m[ 3u ][ 2u ];
}

GXVoid GXMat4::GetPerspectiveParams ( GXFloat &fieldOfViewYRadiands, GXFloat &aspectRatio, GXFloat &zNear, GXFloat &zFar )
{
    fieldOfViewYRadiands = 2.0f * atanf ( 1.0f / m[ 1u ][ 1u ] );
    aspectRatio = m[ 1u ][ 1u ] / m[ 0u ][ 0u ];

    zNear = -m[ 3u ][ 2u ] / ( m[ 2u ][ 2u ] + 1.0f );
    zFar = ( zNear * m[ 3u ][ 2u ] ) / ( m[ 3u ][ 2u ] + 2.0f * zNear );
}

GXVoid GXMat4::GetOrthoParams ( GXFloat &width, GXFloat &height, GXFloat &zNear, GXFloat &zFar )
{
    width = 2.0f / m[ 0u ][ 0u ];
    height = 2.0f / m[ 1u ][ 1u ];

    GXFloat factor = 1.0f / m[ 2u ][ 2u ];

    zNear = -( 1.0f + m[ 3u ][ 2u ] ) * factor;
    zFar = ( 2.0f + zNear * m[ 2u ][ 2u ] ) * factor;
}

GXVoid GXMat4::GetRayPerspective ( GXVec3 &rayView, const GXVec2 &mouseCVV ) const
{
    rayView.SetX ( mouseCVV.GetX () / m[ 0u ][ 0u ] );
    rayView.SetY ( mouseCVV.GetY () / m[ 1u ][ 1u ] );
    rayView.SetZ ( 1.0f );
}

GXMat4& GXMat4::operator = ( const GXMat4 &matrix )
{
    memcpy ( this, &matrix, sizeof ( GXMat4 ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXAABB::GXAABB ()
{
    Empty ();
}

GXAABB::GXAABB ( const GXAABB &other )
{
    memcpy ( this, &other, sizeof ( GXAABB ) );
}

GXVoid GXAABB::Empty ()
{
    vertices = 0u;

    min = GXVec3 ( FLT_MAX, FLT_MAX, FLT_MAX );
    max = GXVec3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
}

GXVoid GXAABB::Transform ( GXAABB &bounds, const GXMat4 &transform ) const
{
    GXVec3 verticesLocal[ 8u ];
    verticesLocal[ 0u ].Init ( min.data[ 0u ], min.data[ 1u ], min.data[ 2u ] );
    verticesLocal[ 1u ].Init ( max.data[ 0u ], min.data[ 1u ], min.data[ 2u ] );
    verticesLocal[ 2u ].Init ( max.data[ 0u ], min.data[ 1u ], max.data[ 2u ] );
    verticesLocal[ 3u ].Init ( min.data[ 0u ], min.data[ 1u ], max.data[ 2u ] );

    verticesLocal[ 4u ].Init ( min.data[ 0u ], max.data[ 1u ], min.data[ 2u ] );
    verticesLocal[ 5u ].Init ( max.data[ 0u ], max.data[ 1u ], min.data[ 2u ] );
    verticesLocal[ 6u ].Init ( max.data[ 0u ], max.data[ 1u ], max.data[ 2u ] );
    verticesLocal[ 7u ].Init ( min.data[ 0u ], max.data[ 1u ], max.data[ 2u ] );

    bounds.Empty ();

    for ( GXUByte i = 0u; i < 8u; ++i )
    {
        GXVec3 vertex;
        transform.MultiplyAsPoint ( vertex, verticesLocal[ i ] );
        bounds.AddVertex ( vertex );
    }
}

GXVoid GXAABB::AddVertex ( const GXVec3 &vertex )
{
    AddVertex ( vertex.data[ 0u ], vertex.data[ 1u ], vertex.data[ 2u ] );
}

GXVoid GXAABB::AddVertex ( GXFloat x, GXFloat y, GXFloat z )
{
    if ( vertices > 1u )
    {
        if ( min.data[ 0u ] > x )
            min.data[ 0u ] = x;
        else if ( max.data[ 0u ] < x )
            max.data[ 0u ] = x;

        if ( min.data[ 1u ] > y )
            min.data[ 1u ] = y;
        else if ( max.data[ 1u ] < y )
            max.data[ 1u ] = y;

        if ( min.data[ 2u ] > z )
            min.data[ 2u ] = z;
        else if ( max.data[ 2u ] < z )
            max.data[ 2u ] = z;

        return;
    }
    else if ( vertices == 0u )
    {
        ++vertices;

        min.Init ( x, y, z );
        return;
    }

    ++vertices;

    if ( min.data[ 0u ] > x )
    {
        max.data[ 0u ] = min.data[ 0u ];
        min.data[ 0u ] = x;
    }
    else
    {
        max.data[ 0u ] = x;
    }

    if ( min.data[ 1u ] > y )
    {
        max.data[ 1u ] = min.data[ 1u ];
        min.data[ 1u ] = y;
    }
    else
    {
        max.data[ 1u ] = y;
    }

    if ( min.data[ 2u ]  > z )
    {
        max.data[ 2u ] = min.data[ 2u ];
        min.data[ 2u ] = z;
    }
    else
    {
        max.data[ 2u ] = z;
    }
}

GXBool GXAABB::IsOverlaped ( const GXAABB &other ) const
{
    if ( min.data[ 0u ] > other.max.data[ 0u ] ) return GX_FALSE;
    if ( min.data[ 1u ] > other.max.data[ 1u ] ) return GX_FALSE;
    if ( min.data[ 2u ] > other.max.data[ 2u ] ) return GX_FALSE;

    if ( other.min.data[ 0u ] > max.data[ 0u ] ) return GX_FALSE;
    if ( other.min.data[ 1u ] > max.data[ 1u ] ) return GX_FALSE;
    if ( other.min.data[ 2u ] > max.data[ 2u ] ) return GX_FALSE;

    return GX_TRUE;
}

GXBool GXAABB::IsOverlaped ( const GXVec3 &point ) const
{
    return IsOverlaped ( point.data[ 0u ], point.data[ 1u ], point.data[ 2u ] );
}

GXBool GXAABB::IsOverlaped ( GXFloat x, GXFloat y, GXFloat z ) const
{
    if ( min.data[ 0u ] > x ) return GX_FALSE;
    if ( min.data[ 1u ] > y ) return GX_FALSE;
    if ( min.data[ 2u ] > z ) return GX_FALSE;

    if ( max.data[ 0u ] < x ) return GX_FALSE;
    if ( max.data[ 1u ] < y ) return GX_FALSE;
    if ( max.data[ 2u ] < z ) return GX_FALSE;

    return GX_TRUE;
}

GXVoid GXAABB::GetCenter ( GXVec3 &center ) const
{
    center.SetX ( ( min.data[ 0u ] + max.data[ 0u ] ) * 0.5f );
    center.SetY ( ( min.data[ 1u ] + max.data[ 1u ] ) * 0.5f );
    center.SetZ ( ( min.data[ 2u ] + max.data[ 2u ] ) * 0.5f );
}

GXFloat GXAABB::GetWidth () const
{
    if ( vertices < 2u )
        return -1.0f;

    return max.data[ 0u ] - min.data[ 0u ];
}

GXFloat GXAABB::GetHeight () const
{
    if ( vertices < 2u )
        return -1.0f;

    return max.data[ 1u ] - min.data[ 1u ];
}

GXFloat GXAABB::GetDepth () const
{
    if ( vertices < 2u )
        return -1.0f;

    return max.data[ 2u ] - min.data[ 2u ];
}

GXFloat GXAABB::GetSphereRadius () const
{
    GXVec3 center;
    GetCenter ( center );
    return center.Distance ( min );
}

GXAABB& GXAABB::operator = ( const GXAABB &other )
{
    memcpy ( this, &other, sizeof ( GXAABB ) );
    return *this;
}

//------------------------------------------------------------------

GXPlane::GXPlane ()
{
    a = 0.0f;
    b = 1.0f;
    c = 0.0f;
    d = 0.0f;
}

GXPlane::GXPlane ( const GXPlane &other )
{
    memcpy ( this, &other, sizeof ( GXPlane ) );
}

GXVoid GXPlane::From ( const GXVec3 &pointA, const GXVec3 &pointB, const GXVec3 &pointC )
{
    GXVec3 ab;
    ab.Substract ( pointB, pointA );

    GXVec3 ac;
    ac.Substract ( pointC, pointA );

    GXVec3 normal;
    normal.CrossProduct ( ab, ac );
    normal.Normalize ();

    this->a = normal.data[ 0u ];
    this->b = normal.data[ 1u ];
    this->c = normal.data[ 2u ];
    this->d = -normal.DotProduct ( pointA );
}

GXVoid GXPlane::FromLineToPoint ( const GXVec3 &lineStart, const GXVec3 &lineEnd, const GXVec3 &point )
{
    GXVec3 startToPoint;
    startToPoint.Substract ( point, lineStart );

    GXVec3 startToEnd;
    startToEnd.Substract ( lineEnd, lineStart );

    GXVec3 tempCross;
    tempCross.CrossProduct ( startToEnd, startToPoint );

    GXVec3 normal;
    normal.CrossProduct ( tempCross, startToEnd );

    a = normal.data[ 0u ];
    b = normal.data[ 1u ];
    c = normal.data[ 3u ];
    d = -a * lineStart.data[ 0u ] - b * lineStart.data[ 1u ] - c * lineStart.data[ 2u ];

    if ( ClassifyVertex ( point ) != eGXPlaneClassifyVertex::Behind ) return;

    Flip ();
}

GXVoid GXPlane::Normalize ()
{
    GXFloat inverseLength = 1.0f / sqrtf ( a * a + b * b + c * c );

    a *= inverseLength;
    b *= inverseLength;
    c *= inverseLength;
    d *= inverseLength;
}

GXVoid GXPlane::Flip ()
{
    a = -a;
    b = -b;
    c = -c;
    d = -d;
}

eGXPlaneClassifyVertex GXPlane::ClassifyVertex ( const GXVec3 &vertex ) const
{
    return ClassifyVertex ( vertex.data[ 0u ], vertex.data[ 1u ], vertex.data[ 2u ] );
}

eGXPlaneClassifyVertex GXPlane::ClassifyVertex ( GXFloat x, GXFloat y, GXFloat z ) const
{
    GXFloat test = a * x + b * y + c * z + d;

    if ( test < 0.0f )
        return eGXPlaneClassifyVertex::Behind;

    if ( test > 0.0f )
        return eGXPlaneClassifyVertex::InFront;

    return eGXPlaneClassifyVertex::On;
}

GXPlane& GXPlane::operator = ( const GXPlane &other )
{
    memcpy ( this, &other, sizeof ( GXPlane ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

GXProjectionClipPlanes::GXProjectionClipPlanes ()
{
    memset ( planes, 0, 6 * sizeof ( GXPlane ) );
}

GXProjectionClipPlanes::GXProjectionClipPlanes ( const GXMat4 &src )
{
    From ( src );
}

GXVoid GXProjectionClipPlanes::From ( const GXMat4 &src )
{
    // Left clipping plane
    planes[ 0u ].a = src.m[ 0u ][ 3u ] + src.m[ 0u ][ 0u ];
    planes[ 0u ].b = src.m[ 1u ][ 3u ] + src.m[ 1u ][ 0u ];
    planes[ 0u ].c = src.m[ 2u ][ 3u ] + src.m[ 2u ][ 0u ];
    planes[ 0u ].d = src.m[ 3u ][ 3u ] + src.m[ 3u ][ 0u ];
    
    // Right clipping plane
    planes[ 1u ].a = src.m[ 0u ][ 3u ] - src.m[ 0u ][ 0u ];
    planes[ 1u ].b = src.m[ 1u ][ 3u ] - src.m[ 1u ][ 0u ];
    planes[ 1u ].c = src.m[ 2u ][ 3u ] - src.m[ 2u ][ 0u ];
    planes[ 1u ].d = src.m[ 3u ][ 3u ] - src.m[ 3u ][ 0u ];
    
    // Top clipping plane
    planes[ 2u ].a = src.m[ 0u ][ 3u ] - src.m[ 0u ][ 1u ];
    planes[ 2u ].b = src.m[ 1u ][ 3u ] - src.m[ 1u ][ 1u ];
    planes[ 2u ].c = src.m[ 2u ][ 3u ] - src.m[ 2u ][ 1u ];
    planes[ 2u ].d = src.m[ 3u ][ 3u ] - src.m[ 3u ][ 1u ];

    // Bottom clipping plane
    planes[ 3u ].a = src.m[ 0u ][ 3u ] + src.m[ 0u ][ 1u ];
    planes[ 3u ].b = src.m[ 1u ][ 3u ] + src.m[ 1u ][ 1u ];
    planes[ 3u ].c = src.m[ 2u ][ 3u ] + src.m[ 2u ][ 1u ];
    planes[ 3u ].d = src.m[ 3u ][ 3u ] + src.m[ 3u ][ 1u ];

    // Near clipping plane
    planes[ 4u ].a = src.m[ 0u ][ 3u ] + src.m[ 0u ][ 2u ];
    planes[ 4u ].b = src.m[ 1u ][ 3u ] + src.m[ 1u ][ 2u ];
    planes[ 4u ].c = src.m[ 2u ][ 3u ] + src.m[ 2u ][ 2u ];
    planes[ 4u ].d = src.m[ 3u ][ 3u ] + src.m[ 3u ][ 2u ];

    // Far clipping plane
    planes[ 5u ].a = src.m[ 0u ][ 3u ] - src.m[ 0u ][ 2u ];
    planes[ 5u ].b = src.m[ 1u ][ 3u ] - src.m[ 1u ][ 2u ];
    planes[ 5u ].c = src.m[ 2u ][ 3u ] - src.m[ 2u ][ 2u ];
    planes[ 5u ].d = src.m[ 3u ][ 3u ] - src.m[ 3u ][ 2u ];
}

GXBool GXProjectionClipPlanes::IsVisible ( const GXAABB &bounds )
{
    GXInt flags = static_cast<GXInt> ( PlaneTest ( bounds.min.data[ 0u ], bounds.min.data[ 1u ], bounds.min.data[ 2u ] ) );
    flags &= static_cast<GXInt> ( PlaneTest ( bounds.min.data[ 0u ], bounds.max.data[ 1u ], bounds.min.data[ 2u ] ) );
    flags &= static_cast<GXInt> ( PlaneTest ( bounds.max.data[ 0u ], bounds.max.data[ 1u ], bounds.min.data[ 2u ] ) );
    flags &= static_cast<GXInt> ( PlaneTest ( bounds.max.data[ 0u ], bounds.min.data[ 1u ], bounds.min.data[ 2u ] ) );

    flags &= static_cast<GXInt> ( PlaneTest ( bounds.min.data[ 0u ], bounds.min.data[ 1u ], bounds.max.data[ 2u ] ) );
    flags &= static_cast<GXInt> ( PlaneTest ( bounds.min.data[ 0u ], bounds.max.data[ 1u ], bounds.max.data[ 2u ] ) );
    flags &= static_cast<GXInt> ( PlaneTest ( bounds.max.data[ 0u ], bounds.max.data[ 1u ], bounds.max.data[ 2u ] ) );
    flags &= static_cast<GXInt> ( PlaneTest ( bounds.max.data[ 0u ], bounds.min.data[ 1u ], bounds.max.data[ 2u ] ) );

    return ( flags > 0 ) ? GX_FALSE : GX_TRUE;
}

GXProjectionClipPlanes& GXProjectionClipPlanes::operator = ( const GXProjectionClipPlanes &clipPlanes )
{
    memcpy ( this, &clipPlanes, sizeof ( GXProjectionClipPlanes ) );
    return *this;
}

GXUByte GXProjectionClipPlanes::PlaneTest ( GXFloat x, GXFloat y, GXFloat z )
{
    GXUByte flags = 0u;

    for ( GXUByte i = 0u; i < 6u; ++i )
    {
        if ( planes[ i ].ClassifyVertex ( x, y, z ) != eGXPlaneClassifyVertex::Behind ) continue;

        flags |= static_cast<GXUByte> ( 1u << i );
    }

    return flags;
}

//------------------------------------------------------------------

GXFloat GXCALL GXDegToRad ( GXFloat degrees )
{
    return degrees * DEGREES_TO_RADIANS_FACTOR;
}

GXFloat GXCALL GXRadToDeg ( GXFloat radians )
{
    return radians * RADIANS_TO_DEGREES_FACTOR;
}

GXVoid GXCALL GXConvert3DSMaxToGXEngine ( GXVec3 &gx_out, GXFloat max_x, GXFloat max_y, GXFloat max_z )
{
    gx_out.Init ( -max_x, max_z, -max_y );
}

GXVoid GXCALL GXRandomize ()
{
    srand ( static_cast<GXUInt> ( time ( nullptr ) ) );
}

GXFloat GXCALL GXRandomNormalize ()
{
    return static_cast<GXFloat> ( rand () ) * INVERSE_RAND_MAX;
}

GXFloat GXCALL GXRandomBetween ( GXFloat from, GXFloat to )
{
    GXFloat delta = to - from;
    return from + delta * GXRandomNormalize ();
}

GXVoid GXCALL GXRandomBetween ( GXVec3 &out, const GXVec3 &from, const GXVec3 &to )
{
    out.data[ 0u ] = GXRandomBetween ( from.data[ 0u ], to.data[ 0u ] );
    out.data[ 1u ] = GXRandomBetween ( from.data[ 1u ], to.data[ 1u ] );
    out.data[ 2u ] = GXRandomBetween ( from.data[ 2u ], to.data[ 2u ] );
}

GXVoid GXCALL GXGetTangentBitangent ( GXVec3 &outTangent, GXVec3 &outBitangent, GXUByte vertexID, const GXUByte* vertices, GXUPointer vertexStride, const GXUByte* uvs, GXUPointer uvStride )
{
    const GXVec3* v0;
    const GXVec3* v1;
    const GXVec3* v2;

    const GXVec2* uv0;
    const GXVec2* uv1;
    const GXVec2* uv2;

    switch ( vertexID )
    {
        case 0u:
        {
            v0 = reinterpret_cast<const GXVec3*> ( vertices );
            v1 = reinterpret_cast<const GXVec3*> ( vertices + vertexStride );
            v2 = reinterpret_cast<const GXVec3*> ( vertices + 2u * vertexStride );

            uv0 = reinterpret_cast<const GXVec2*> ( uvs );
            uv1 = reinterpret_cast<const GXVec2*> ( uvs + uvStride );
            uv2 = reinterpret_cast<const GXVec2*> ( uvs + 2u * uvStride );
        }
        break;

        case 1u:
        {
            v0 = reinterpret_cast<const GXVec3*> ( vertices + vertexStride );
            v1 = reinterpret_cast<const GXVec3*> ( vertices + 2u * vertexStride );
            v2 = reinterpret_cast<const GXVec3*> ( vertices );

            uv0 = reinterpret_cast<const GXVec2*> ( uvs + uvStride );
            uv1 = reinterpret_cast<const GXVec2*> ( uvs + 2u * uvStride );
            uv2 = reinterpret_cast<const GXVec2*> ( uvs );
        }
        break;

        case 2u:
        default:
        {
            v0 = reinterpret_cast<const GXVec3*> ( vertices + 2u * vertexStride );
            v1 = reinterpret_cast<const GXVec3*> ( vertices );
            v2 = reinterpret_cast<const GXVec3*> ( vertices + vertexStride );

            uv0 = reinterpret_cast<const GXVec2*> ( uvs + 2u * uvStride );
            uv1 = reinterpret_cast<const GXVec2*> ( uvs );
            uv2 = reinterpret_cast<const GXVec2*> ( uvs + uvStride );
        }
        break;
    }

    GXVec3 a;
    GXVec3 b;
    a.Substract ( *v1, *v0 );
    b.Substract ( *v2, *v0 );

    GXVec2 dUVa;
    GXVec2 dUVb;
    dUVa.Substract ( *uv1, *uv0 );
    dUVb.Substract ( *uv2, *uv0 );

    GXFloat factor = 1.0f / ( dUVa.data[ 0u ] * dUVb.data[ 1u ] - dUVb.data[ 0u ] * dUVa.data[ 1u ] );

    outTangent.data[ 0u ] = factor * ( dUVb.data[ 1u ] * a.data[ 0u ] - dUVa.data[ 1u ] * b.data[ 0u ] );
    outTangent.data[ 1u ] = factor * ( dUVb.data[ 1u ] * a.data[ 1u ] - dUVa.data[ 1u ] * b.data[ 1u ] );
    outTangent.data[ 2u ] = factor * ( dUVb.data[ 1u ] * a.data[ 2u ] - dUVa.data[ 1u ] * b.data[ 2u ] );
    outTangent.Normalize ();

    outBitangent.data[ 0u ] = factor * ( -dUVb.data[ 0u ] * a.data[ 0u ] + dUVa.data[ 0u ] * b.data[ 0u ] );
    outBitangent.data[ 1u ] = factor * ( -dUVb.data[ 0u ] * a.data[ 1u ] + dUVa.data[ 0u ] * b.data[ 1u ] );
    outBitangent.data[ 2u ] = factor * ( -dUVb.data[ 0u ] * a.data[ 2u ] + dUVa.data[ 0u ] * b.data[ 2u ] );
    outBitangent.Normalize ();
}

GXFloat GXCALL GXClampf ( GXFloat value, GXFloat minValue, GXFloat maxValue )
{
    return ( value < minValue ) ? minValue : ( value > maxValue ) ? maxValue : value;
}

GXInt GXCALL GXClampi ( GXInt value, GXInt minValue, GXInt maxValue )
{
    return ( value < minValue ) ? minValue : ( value > maxValue ) ? maxValue : value;
}

GXFloat GXCALL GXMinf ( GXFloat a, GXFloat b )
{
    return a < b ? a : b;
}

GXFloat GXCALL GXMaxf ( GXFloat a, GXFloat b )
{
    return a > b ? a : b;
}

GXVoid GXCALL GXGetBarycentricCoords ( GXVec3 &out, const GXVec3 &point, const GXVec3 &a, const GXVec3 &b, const GXVec3 &c )
{
    GXVec3 v0;
    GXVec3 v1;
    GXVec3 v2;

    v0.Substract ( b, a );
    v1.Substract ( c, a );
    v2.Substract ( point, a );

    GXFloat d00 = v0.DotProduct ( v0 );
    GXFloat d01 = v0.DotProduct ( v1 );
    GXFloat d11 = v1.DotProduct ( v1 );
    GXFloat d20 = v2.DotProduct ( v0 );
    GXFloat d21 = v2.DotProduct ( v1 );

    GXFloat denom = 1.0f / ( d00 * d11 - d01 * d01 );

    out.data[ 1 ] = ( d11 * d20 - d01 * d21 ) * denom;
    out.data[ 2 ] = ( d00 * d21 - d01 * d20 ) * denom;
    out.data[ 0 ] = 1.0f - out.data[ 1u ] - out.data[ 2u ];
}

GXVoid GXCALL GXGetRayFromViewer ( GXVec3 &origin, GXVec3 &direction, GXUShort x, GXUShort y, GXUShort viewportWidth, GXUShort viewportHeight, const GXVec3& viewerLocation, const GXMat4 &viewProjectionMatrix )
{
    GXFloat halfWidth = viewportWidth * 0.5f;
    GXFloat halfHeight = viewportHeight * 0.5f;

    GXVec4 pointCVV ( ( static_cast<GXFloat> ( x ) - halfWidth ) / halfWidth, ( static_cast<GXFloat> ( y ) - halfHeight ) / halfHeight, 1.0f, 1.0f );

    GXMat4 inverseViewProjectionMatrix;
    inverseViewProjectionMatrix.Inverse ( viewProjectionMatrix );

    GXVec4 pointWorld;
    inverseViewProjectionMatrix.MultiplyVectorMatrix ( pointWorld, pointCVV );
    GXFloat alpha = 1.0f / pointWorld.data[ 3u ];

    pointWorld.data[ 0u ] *= alpha;
    pointWorld.data[ 1u ] *= alpha;
    pointWorld.data[ 2u ] *= alpha;

    direction.Substract ( GXVec3 ( pointWorld.data[ 0u ], pointWorld.data[ 1u ], pointWorld.data[ 2u ] ), viewerLocation );
    direction.Normalize ();

    origin = viewerLocation;
}
