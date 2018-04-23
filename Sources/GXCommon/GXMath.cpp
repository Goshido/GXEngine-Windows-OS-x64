// version 1.46

#include <GXCommon/GXMath.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <stdlib.h>
#include <cwchar>
#include <cstring>
#include <time.h>
#include <GXCommon/GXRestoreWarnings.h>


#define COLOR_TO_FLOAT_FACTOR			0.00392157f

#define HSVA_FACTOR						0.016666f
#define HSVA_TO_RGBA_FACTOR				2.55f
#define HSVA_ALPHA_TO_RGBA_ALPHA_BYTE	2.55f
#define HSVA_TO_RGBA_FLOAT				0.01f
#define RGBA_TO_UBYTE_FACTOR			255.0f

#define DEGREES_TO_RADIANS_FACTOR		0.0174533f
#define RADIANS_TO_DEGREES_FACTOR		57.295779f

#define FLOAT_EPSILON					1.0e-4f
#define INVERSE_RAND_MAX				3.05185e-5f

#define SOLUTION_ALPHA					0
#define SOLUTION_BETTA					1
#define SOLUTION_GAMMA					2
#define SOLUTION_YOTTA					3
#define UNKNOWN_SOLUTION				0xFF


GXVec2::GXVec2 ()
{
	memset ( this, 0, sizeof ( GXVec2 ) );
}

GXVec2::GXVec2 ( GXFloat x, GXFloat y )
{
	data[ 0 ] = x;
	data[ 1 ] = y;
}

GXVec2::GXVec2 ( const GXVec2 &other )
{
	memcpy ( this, &other, sizeof ( GXVec2 ) );
}

GXVoid GXVec2::SetX ( GXFloat x )
{
	data[ 0 ] = x;
}

GXFloat GXVec2::GetX () const
{
	return data[ 0 ];
}

GXVoid GXVec2::SetY ( GXFloat y )
{
	data[ 1 ] = y;
}

GXFloat GXVec2::GetY () const
{
	return data[ 1 ];
}

GXVoid GXVec2::Init ( GXFloat x, GXFloat y )
{
	data[ 0 ] = x;
	data[ 1 ] = y;
}

GXVoid GXVec2::Normalize ()
{
	Multiply ( *this, 1.0f / Length () );
}

GXVoid GXVec2::CalculateNormalFast ( const GXVec2 &a, const GXVec2 &b )
{
	data[ 0 ] = a.data[ 1 ] - b.data[ 1 ];
	data[ 1 ] = b.data[ 0 ] - a.data[ 0 ];
}

GXVoid GXVec2::CalculateNormal ( const GXVec2 &a, const GXVec2 &b )
{
	CalculateNormalFast ( a, b );
	Normalize ();
}

GXVoid GXVec2::Sum ( const GXVec2 &a, const GXVec2 &b )
{
	data[ 0 ] = a.data[ 0 ] + b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] + b.data[ 1 ];
}

GXVoid GXVec2::Sum ( const GXVec2 &a, GXFloat scale, const GXVec2 &b )
{
	data[ 0 ] = a.data[ 0 ] + scale * b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] + scale * b.data[ 1 ];
}

GXVoid GXVec2::Substract ( const GXVec2 &a, const GXVec2 &b )
{
	data[ 0 ] = a.data[ 0 ] - b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] - b.data[ 1 ];
}

GXVoid GXVec2::Multiply ( const GXVec2 &a, const GXVec2 &b )
{
	data[ 0 ] = a.data[ 0 ] * b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] * b.data[ 1 ];
}

GXVoid GXVec2::Multiply ( const GXVec2 &v, GXFloat scale )
{
	data[ 0 ] = v.data[ 0 ] * scale;
	data[ 1 ] = v.data[ 1 ] * scale;
}

GXFloat GXVec2::DotProduct ( const GXVec2 &other ) const
{
	return data[ 0 ] * other.data[ 0 ] + data[ 1 ] * other.data[ 1 ];
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
	if ( data[ 0 ] != other.data[ 0 ] ) return GX_FALSE;
	if ( data[ 1 ] != other.data[ 1 ] ) return GX_FALSE;

	return GX_TRUE;
}

GXVec2& GXVec2::operator = ( const GXVec2 &vector )
{
	memcpy ( this, &vector, sizeof ( GXVec2 ) );
	return *this;
}

//------------------------------------------------------------------------------------------------

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
		GXVec2 gamma = a0.IsEqual ( b0 ) ? b1 : b0;

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
	memset ( data, 0, 3 * sizeof ( GXFloat ) );
}

GXVec3::GXVec3 ( GXFloat x, GXFloat y, GXFloat z )
{
	SetX ( x );
	SetY ( y );
	SetZ ( z );
}

GXVec3::GXVec3 ( const GXVec3 &other )
{
	memcpy ( this, &other, sizeof ( GXVec3 ) );
}

GXVoid GXVec3::SetX ( GXFloat x )
{
	data[ 0 ] = x;
}

GXFloat GXVec3::GetX () const
{
	return data[ 0 ];
}

GXVoid GXVec3::SetY ( GXFloat y )
{
	data[ 1 ] = y;
}

GXFloat GXVec3::GetY () const
{
	return data[ 1 ];
}

GXVoid GXVec3::SetZ ( GXFloat z )
{
	data[ 2 ] = z;
}

GXFloat GXVec3::GetZ () const
{
	return data[ 2 ];
}

GXVoid GXVec3::Init ( GXFloat x, GXFloat y, GXFloat z )
{
	data[ 0 ] = x;
	data[ 1 ] = y;
	data[ 2 ] = z;
}

GXVoid GXVec3::Normalize ()
{
	Multiply ( *this, 1.0f / Length () );
}

GXVoid GXVec3::Reverse ()
{
	data[ 0 ] = -data[ 0 ];
	data[ 1 ] = -data[ 1 ];
	data[ 2 ] = -data[ 2 ];
}

GXVoid GXVec3::Sum ( const GXVec3 &a, const GXVec3 &b )
{
	data[ 0 ] = a.data[ 0 ] + b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] + b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] + b.data[ 2 ];
}

GXVoid GXVec3::Sum ( const GXVec3 &a, GXFloat scale, const GXVec3 &b )
{
	data[ 0 ] = a.data[ 0 ] + scale * b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] + scale * b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] + scale * b.data[ 2 ];
}

GXVoid GXVec3::Substract ( const GXVec3 &a, const GXVec3 &b )
{
	data[ 0 ] = a.data[ 0 ] - b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] - b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] - b.data[ 2 ];
}

GXVoid GXVec3::Multiply ( const GXVec3 &a, GXFloat scale )
{
	data[ 0 ] = a.data[ 0 ] * scale;
	data[ 1 ] = a.data[ 1 ] * scale;
	data[ 2 ] = a.data[ 2 ] * scale;
}

GXVoid GXVec3::Multiply ( const GXVec3 &a, const GXVec3 &b )
{
	data[ 0 ] = a.data[ 0 ] * b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] * b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] * b.data[ 2 ];
}

GXFloat GXVec3::DotProduct ( const GXVec3 &other ) const
{
	return data[ 0 ] * other.data[ 0 ] + data[ 1 ] * other.data[ 1 ] + data[ 2 ] * other.data[ 2 ];
}

GXVoid GXVec3::CrossProduct ( const GXVec3 &a, const GXVec3 &b )
{
	data[ 0 ] = a.data[ 1 ] * b.data[ 2 ] - a.data[ 2 ] * b.data[ 1 ];
	data[ 1 ] = a.data[ 2 ] * b.data[ 0 ] - a.data[ 0 ] * b.data[ 2 ];
	data[ 2 ] = a.data[ 0 ] * b.data[ 1 ] - a.data[ 1 ] * b.data[ 0 ];
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
	GXVec3 normalVector = vector;
	normalVector.Normalize ();
	GXFloat factor = vector.Length () * axis.DotProduct ( normalVector );
	Multiply ( axis, factor );
}

GXBool GXVec3::IsEqual ( const GXVec3 &other )
{
	if ( data[ 0 ] != other.data[ 0 ] ) return GX_FALSE;
	if ( data[ 1 ] != other.data[ 1 ] ) return GX_FALSE;
	if ( data[ 2 ] != other.data[ 2 ] ) return GX_FALSE;

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

//----------------------------------------------------------------------------

GXBool GXCALL GXRayTriangleIntersection3D ( GXFloat &outT, const GXVec3 &origin, const GXVec3 &direction, GXFloat length, const GXVec3 &a, const GXVec3 &b, const GXVec3 &c )
{
	// Implementation https://graphics.stanford.edu/courses/cs348b-98/gg/intersect.html

	GXPlane plane;
	plane.From ( a, b, c );
	GXVec3 normal ( plane.a, plane.b, plane.c );

	GXFloat t = ( plane.d + normal.DotProduct ( origin ) ) / ( -normal.DotProduct ( direction ) );

	if ( t < 0.0f ) return GX_FALSE;

	if ( t > length ) return GX_FALSE;

	GXVec3 triangle[ 3 ];
	triangle[ 0 ] = a;
	triangle[ 1 ] = b;
	triangle[ 2 ] = c;

	GXVec3 point;
	point.Sum ( origin, t, direction );

	GXUByte selector;

	GXFloat gamma = fabsf ( plane.a );
	GXFloat omega = fabsf ( plane.b );
	GXFloat yotta = fabsf ( plane.c );

	if ( gamma > omega )
	{
		if ( gamma > yotta )
			selector = 0;
		else
			selector = 2;
	}
	else
	{
		if ( omega > yotta )
			selector = 1;
		else
			selector = 2;
	}

	GXUByte i1;
	GXUByte i2;

	switch ( selector )
	{
		case 0:
			i1 = 1;
			i2 = 2;
		break;

		case 1:
			i1 = 2;
			i2 = 0;
		break;

		case 2:
			i1 = 0;
			i2 = 1;
		break;

		default:
			// NOTHING
		break;
	}

	GXFloat u0 = point.data[ i1 ] - triangle[ 0 ].data[ i1 ];
	GXFloat v0 = point.data[ i2 ] - triangle[ 0 ].data[ i2 ];

	GXFloat u1 = triangle[ 1 ].data[ i1 ] - triangle[ 0 ].data[ i1 ];
	GXFloat v1 = triangle[ 1 ].data[ i2 ] - triangle[ 0 ].data[ i2 ];

	GXFloat u2 = triangle[ 2 ].data[ i1 ] - triangle[ 0 ].data[ i1 ];
	GXFloat v2 = triangle[ 2 ].data[ i2 ] - triangle[ 0 ].data[ i2 ];

	gamma = 1.0f / ( u1 * v2 - v1 * u2 );
	GXFloat alpha = ( u0 * v2 - v0 * u2 ) * gamma;

	if ( alpha < 0.0f || alpha > 1.0f ) return GX_FALSE;

	GXFloat betta = ( u1 * v0 - v1 * u0 ) * gamma;

	if ( betta < 0.0f || betta > 1.0f || ( alpha + betta ) > 1.0f ) return GX_FALSE;

	outT = t;
	return GX_TRUE;
}

//----------------------------------------------------------------------------

GXEuler::GXEuler ()
{
	pitchRadians = 0.0f;
	yawRadians = 0.0f;
	rollRadians = 0.0f;
}

GXEuler::GXEuler ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
	this->pitchRadians = pitchRadians;
	this->yawRadians = yawRadians;
	this->rollRadians = rollRadians;
}

GXEuler::GXEuler ( const GXEuler &other )
{
	memcpy ( this, &other, sizeof ( GXEuler ) );
}

GXEuler& GXEuler::operator = ( const GXEuler &other )
{
	memcpy ( this, &other, sizeof ( GXEuler ) );
	return *this;
}

//----------------------------------------------------------------------------------------

GXVec4::GXVec4 ()
{
	// NOTHING
}

GXVec4::GXVec4 ( const GXVec3 &vector, GXFloat w )
{
	memcpy ( data, vector.data, 3 * sizeof ( GXFloat ) );
	data[ 3 ] = w;
}

GXVec4::GXVec4 ( GXFloat x, GXFloat y, GXFloat z, GXFloat w )
{
	Init ( x, y, z, w );
}

GXVoid GXVec4::Init ( GXFloat x, GXFloat y, GXFloat z, GXFloat w )
{
	data[ 0 ] = x;
	data[ 1 ] = y;
	data[ 2 ] = z;
	data[ 3 ] = w;
}

GXVoid GXVec4::SetX ( GXFloat x )
{
	data[ 0 ] = x;
}

GXFloat GXVec4::GetX () const
{
	return data[ 0 ];
}

GXVoid GXVec4::SetY ( GXFloat y )
{
	data[ 1 ] = y;
}

GXFloat GXVec4::GetY () const
{
	return data[ 1 ];
}

GXVoid GXVec4::SetZ ( GXFloat z )
{
	data[ 2 ] = z;
}

GXFloat GXVec4::GetZ () const
{
	return data[ 2 ];
}

GXVoid GXVec4::SetW ( GXFloat w )
{
	data[ 3 ] = w;
}

GXFloat GXVec4::GetW () const
{
	return data[ 3 ];
}

GXVoid GXVec4::Sum ( const GXVec4 &a, const GXVec4 &b )
{
	data[ 0 ] = a.data[ 0 ] + b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] + b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] + b.data[ 2 ];
	data[ 3 ] = a.data[ 3 ] + b.data[ 3 ];
}

GXVoid GXVec4::Sum ( const GXVec4 &a, GXFloat scale, const GXVec4 &b )
{
	data[ 0 ] = a.data[ 0 ] + scale * b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] + scale * b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] + scale * b.data[ 2 ];
	data[ 3 ] = a.data[ 3 ] + scale * b.data[ 3 ];
}

GXVoid GXVec4::Substract ( const GXVec4 &a, const GXVec4 &b )
{
	data[ 0 ] = a.data[ 0 ] - b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] - b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] - b.data[ 2 ];
	data[ 3 ] = a.data[ 3 ] - b.data[ 3 ];
}

GXFloat GXVec4::DotProduct ( const GXVec4 &other ) const
{
	return data[ 0 ] * other.data[ 0 ] + data[ 1 ] * other.data[ 1 ] + data[ 2 ] * other.data[ 2 ] + data[ 3 ] * other.data[ 3 ];
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

//------------------------------------------------------------------

GXVec6::GXVec6 ()
{
	// NOTHING
}

GXVec6::GXVec6 ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 )
{
	data[ 0 ] = a1;
	data[ 1 ] = a2;
	data[ 2 ] = a3;
	data[ 3 ] = a4;
	data[ 4 ] = a5;
	data[ 5 ] = a6;
}

GXVoid GXVec6::Init ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 )
{
	data[ 0 ] = a1;
	data[ 1 ] = a2;
	data[ 2 ] = a3;
	data[ 3 ] = a4;
	data[ 4 ] = a5;
	data[ 5 ] = a6;
}

GXVoid GXVec6::From ( const GXVec3 &v1, const GXVec3 &v2 )
{
	memcpy ( data, &v1, sizeof ( GXVec3 ) );
	memcpy ( data + 3, &v2, sizeof ( GXVec3 ) );
}

GXFloat GXVec6::DotProduct ( const GXVec6 &other ) const
{
	return data[ 0 ] * other.data[ 0 ] + data[ 1 ] * other.data[ 1 ] + data[ 2 ] * other.data[ 2 ] + data[ 3 ] * other.data[ 3 ] + data[ 4 ] * other.data[ 4 ] + data[ 5 ] * other.data[ 5 ];
}

GXVoid GXVec6::Sum ( const GXVec6 &a, const GXVec6 &b )
{
	data[ 0 ] = a.data[ 0 ] + b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] + b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] + b.data[ 2 ];
	data[ 3 ] = a.data[ 3 ] + b.data[ 3 ];
	data[ 4 ] = a.data[ 4 ] + b.data[ 4 ];
	data[ 5 ] = a.data[ 5 ] + b.data[ 5 ];
}

GXVoid GXVec6::Multiply ( const GXVec6 &a, GXFloat factor )
{
	data[ 0 ] = a.data[ 0 ] * factor;
	data[ 1 ] = a.data[ 1 ] * factor;
	data[ 2 ] = a.data[ 2 ] * factor;
	data[ 3 ] = a.data[ 3 ] * factor;
	data[ 4 ] = a.data[ 4 ] * factor;
	data[ 5 ] = a.data[ 5 ] * factor;
}

GXVec6& GXVec6::operator = ( const GXVec6 &other )
{
	memcpy ( this, &other, sizeof ( GXVec6 ) );
	return *this;
}

//------------------------------------------------------------------

GXColorRGB::GXColorRGB ()
{
	memset ( data, 0, 4 * sizeof ( GXFloat ) );
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

GXColorRGB::GXColorRGB ( const GXColorRGB &other )
{
	memcpy ( this, &other, sizeof ( GXColorRGB ) );
}

GXVoid GXColorRGB::Init ( GXFloat red, GXFloat green, GXFloat blue, GXFloat alpha )
{
	data[ 0 ] = red;
	data[ 1 ] = green;
	data[ 2 ] = blue;
	data[ 3 ] = alpha;
}

GXVoid GXColorRGB::SetRed ( GXFloat red )
{
	data[ 0 ] = red;
}

GXFloat GXColorRGB::GetRed () const
{
	return data[ 0 ];
}

GXVoid GXColorRGB::SetGreen ( GXFloat green )
{
	data[ 1 ] = green;
}

GXFloat GXColorRGB::GetGreen () const
{
	return data[ 1 ];
}

GXVoid GXColorRGB::SetBlue ( GXFloat blue )
{
	data[ 2 ] = blue;
}

GXFloat GXColorRGB::GetBlue () const
{
	return data[ 2 ];
}

GXVoid GXColorRGB::SetAlpha ( GXFloat alpha )
{
	data[ 3 ] = alpha;
}

GXFloat GXColorRGB::GetAlpha () const
{
	return data[ 3 ];
}

GXVoid GXColorRGB::From ( GXUByte red, GXUByte green, GXUByte blue, GXFloat alpha )
{
	data[ 0 ] = red * COLOR_TO_FLOAT_FACTOR;
	data[ 1 ] = green * COLOR_TO_FLOAT_FACTOR;
	data[ 2 ] = blue * COLOR_TO_FLOAT_FACTOR;
	data[ 3 ] = alpha * COLOR_TO_FLOAT_FACTOR;
}

GXVoid GXColorRGB::From ( const GXColorHSV &color )
{
	GXFloat correctedHue = color.GetHue ();

	while ( correctedHue >= 360.0f )
		correctedHue -= 360.0f;

	while ( correctedHue < 0.0f )
		correctedHue += 360.0f;

	GXFloat value = color.GetValue ();

	GXUByte selector = (GXUByte)( ( (GXInt)correctedHue / 60 ) % 6 );
	GXFloat minValue = ( ( 100.0f - color.GetSaturation () ) * value ) * 0.01f;
	GXFloat delta = ( value - minValue ) * ( ( (GXInt)correctedHue % 60 ) * HSVA_FACTOR );
	GXFloat increment = minValue + delta;
	GXFloat decrement = value - delta;

	data[ 3 ] = color.GetAlpha () * HSVA_TO_RGBA_FLOAT;

	switch ( selector )
	{
		case 0:
			data[ 0 ] = value * HSVA_TO_RGBA_FLOAT;
			data[ 1 ] = increment * HSVA_TO_RGBA_FLOAT;
			data[ 2 ] = minValue * HSVA_TO_RGBA_FLOAT;
		break;

		case 1:
			data[ 0 ] = decrement * HSVA_TO_RGBA_FLOAT;
			data[ 1 ] = value * HSVA_TO_RGBA_FLOAT;
			data[ 2 ] = minValue * HSVA_TO_RGBA_FLOAT;
		break;

		case 2:
			data[ 0 ] = minValue * HSVA_TO_RGBA_FLOAT;
			data[ 1 ] = value * HSVA_TO_RGBA_FLOAT;
			data[ 2 ] = increment * HSVA_TO_RGBA_FLOAT;
		break;

		case 3:
			data[ 0 ] = minValue * HSVA_TO_RGBA_FLOAT;
			data[ 1 ] = decrement * HSVA_TO_RGBA_FLOAT;
			data[ 2 ] = value * HSVA_TO_RGBA_FLOAT;
		break;

		case 4:
			data[ 0 ] = increment * HSVA_TO_RGBA_FLOAT;
			data[ 1 ] = minValue * HSVA_TO_RGBA_FLOAT;
			data[ 2 ] = value * HSVA_TO_RGBA_FLOAT;
		break;

		case 5:
			data[ 0 ] = value * HSVA_TO_RGBA_FLOAT;
			data[ 1 ] = minValue * HSVA_TO_RGBA_FLOAT;
			data[ 2 ] = decrement * HSVA_TO_RGBA_FLOAT;
		break;

		default:
			GXLogW ( L"GXConvertHSVAToRGBA::Error (branch two) - Что-то пошло не так!\n" );
		break;
	}
}

GXVoid GXColorRGB::ConvertToUByte ( GXUByte &red, GXUByte &green, GXUByte &blue, GXUByte &alpha ) const
{
	red = (GXUByte)( data[ 0 ] * RGBA_TO_UBYTE_FACTOR + 0.5f );
	green = (GXUByte)( data[ 1 ] * RGBA_TO_UBYTE_FACTOR + 0.5f );
	blue = (GXUByte)( data[ 2 ] * RGBA_TO_UBYTE_FACTOR + 0.5f );
	alpha = (GXUByte)( data[ 3 ] * RGBA_TO_UBYTE_FACTOR + 0.5f );
}

GXColorRGB& GXColorRGB::operator = ( const GXColorRGB &other )
{
	memcpy ( this, &other, sizeof ( GXColorRGB ) );
	return *this;
}

//------------------------------------------------------------------

GXColorHSV::GXColorHSV ()
{
	memset ( data, 0, 4 * sizeof ( GXFloat ) );
}

GXColorHSV::GXColorHSV ( GXFloat hue, GXFloat saturation, GXFloat value, GXFloat alpha )
{
	data[ 0 ] = hue;
	data[ 1 ] = saturation;
	data[ 2 ] = value;
	data[ 3 ] = alpha;
}

GXColorHSV::GXColorHSV ( const GXColorRGB &color )
{
	From ( color );
}

GXColorHSV::GXColorHSV ( const GXColorHSV &other )
{
	memcpy ( this, &other, sizeof ( GXColorHSV ) );
}

GXVoid GXColorHSV::SetHue ( GXFloat hue )
{
	data[ 0 ] = hue;
}

GXFloat GXColorHSV::GetHue () const
{
	return data[ 0 ];
}

GXVoid GXColorHSV::SetSaturation ( GXFloat saturation )
{
	data[ 1 ] = saturation;
}

GXFloat GXColorHSV::GetSaturation () const
{
	return data[ 1 ];
}

GXVoid GXColorHSV::SetValue ( GXFloat value )
{
	data[ 2 ] = value;
}

GXFloat GXColorHSV::GetValue () const
{
	return data[ 2 ];
}

GXVoid GXColorHSV::SetAlpha ( GXFloat alpha )
{
	data[ 3 ] = alpha;
}

GXFloat GXColorHSV::GetAlpha () const
{
	return data[ 3 ];
}

GXVoid GXColorHSV::From ( const GXColorRGB &color )
{
	GXFloat maxValue = GXMaxf ( GXMaxf ( color.GetRed (), color.GetGreen () ), color.GetBlue () );
	GXFloat minValue = GXMinf ( GXMinf ( color.GetRed (), color.GetGreen () ), color.GetBlue () );

	if ( maxValue == minValue )
		data[ 0 ] = 0.0f;
	else if ( maxValue == color.GetRed () && color.GetGreen () >= color.GetBlue () )
		data[ 0 ] = 60.0f * ( ( color.GetGreen () - color.GetBlue () ) / ( maxValue - minValue ) );
	else if ( maxValue == color.GetRed () && color.GetGreen () < color.GetBlue () )
		data[ 0 ] = 60.0f * ( ( color.GetGreen () - color.GetBlue () ) / ( maxValue - minValue ) ) + 360.0f;
	else if ( maxValue == color.GetGreen () )
		data[ 0 ] = 60.0f * ( ( color.GetBlue () - color.GetRed () ) / ( maxValue - minValue ) ) + 120.0f;
	else if ( maxValue == color.GetBlue () )
		data[ 0 ] = 60.0f * ( ( color.GetRed () - color.GetGreen () ) / ( maxValue - minValue ) ) + 240.0f;
	else
	{
		GXUInt wtf = 0;
		GXLogA ( "GXColorHSV::From - Unhanled hue! %u\n", wtf );
	}

	if ( maxValue == 0.0f )
		data[ 1 ] = 0.0f;
	else
		data[ 1 ] = 100.0f * ( 1.0f - minValue / maxValue );

	data[ 2 ] = 100.0f * maxValue;

	data[ 3 ] = 100.0f * color.GetAlpha ();
}

GXVoid GXColorHSV::operator = ( const GXColorHSV &other )
{
	memcpy ( this, &other, sizeof ( GXColorHSV ) );
}

//------------------------------------------------------------------

GXQuat::GXQuat ()
{
	memset ( data, 0, 4 * sizeof ( GXFloat ) );
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

GXQuat::GXQuat ( const GXQuat &other )
{
	memcpy ( this, &other, sizeof ( GXQuat ) );
}

GXVoid GXQuat::Init ( GXFloat r, GXFloat a, GXFloat b, GXFloat c )
{
	data[ 0 ] = r;
	data[ 1 ] = a;
	data[ 2 ] = b;
	data[ 3 ] = c;
}

GXVoid GXQuat::SetR ( GXFloat r )
{
	data[ 0 ] = r;
}

GXFloat GXQuat::GetR () const
{
	return data[ 0 ];
}

GXVoid GXQuat::SetA ( GXFloat a )
{
	data[ 1 ] = a;
}

GXFloat GXQuat::GetA () const
{
	return data[ 1 ];
}

GXVoid GXQuat::SetB ( GXFloat b )
{
	data[ 2 ] = b;
}

GXFloat GXQuat::GetB () const
{
	return data[ 2 ];
}

GXVoid GXQuat::SetC ( GXFloat c )
{
	data[ 3 ] = c;
}

GXFloat GXQuat::GetC () const
{
	return data[ 3 ];
}

GXVoid GXQuat::Identity ()
{
	data[ 0 ] = 1.0f;
	data[ 1 ] = data[ 2 ] = data[ 3 ] = 0.0f;
}

GXVoid GXQuat::Normalize ()
{
	GXFloat squaredLength = data[ 0 ] * data[ 0 ] + data[ 1 ] * data[ 1 ] + data[ 2 ] * data[ 2 ] + data[ 3 ] * data[ 3 ];

	if ( fabsf ( squaredLength ) < FLOAT_EPSILON )
	{
		GXLogW ( L"GXQuat::Normalize - Error\n" );
		return;
	}

	Multiply ( *this, 1.0f / sqrtf ( squaredLength ) );
}

GXVoid GXQuat::Inverse ( const GXQuat &q )
{
	GXFloat squaredLength = q.data[ 0 ] * q.data[ 0 ] + q.data[ 1 ] * q.data[ 1 ] + q.data[ 2 ] * q.data[ 2 ] + q.data[ 3 ] * q.data[ 3 ];

	if ( fabsf ( squaredLength ) > FLOAT_EPSILON )
	{
		GXFloat inverseSquaredLength = 1.0f / squaredLength;

		data[ 0 ] = q.data[ 0 ] * inverseSquaredLength;
		data[ 1 ] = -q.data[ 1 ] * inverseSquaredLength;
		data[ 2 ] = -q.data[ 2 ] * inverseSquaredLength;
		data[ 3 ] = -q.data[ 3 ] * inverseSquaredLength;

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

	data[ 0 ] = cosf ( halfAngle );
	data[ 1 ] = x * sinom;
	data[ 2 ] = y * sinom;
	data[ 3 ] = z * sinom;
}

GXVoid GXQuat::FromAxisAngle ( const GXVec3 &axis, GXFloat angle )
{
	FromAxisAngle ( axis.GetX (), axis.GetY (), axis.GetZ (), angle );
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

	GXFloat solutionFactorAlpha = pureRotationMatrix.m[ 0 ][ 0 ] + pureRotationMatrix.m[ 1 ][ 1 ] + pureRotationMatrix.m[ 2 ][ 2 ] + 1.0f;
	GXFloat solutionFactorBetta = pureRotationMatrix.m[ 0 ][ 0 ] - pureRotationMatrix.m[ 1 ][ 1 ] - pureRotationMatrix.m[ 2 ][ 2 ] + 1.0f;
	GXFloat solutionFactorGamma = -pureRotationMatrix.m[ 0 ][ 0 ] + pureRotationMatrix.m[ 1 ][ 1 ] - pureRotationMatrix.m[ 2 ][ 2 ] + 1.0f;
	GXFloat solutionFactorYotta = -pureRotationMatrix.m[ 0 ][ 0 ] - pureRotationMatrix.m[ 1 ][ 1 ] + pureRotationMatrix.m[ 2 ][ 2 ] + 1.0f;

	GXUByte solution = UNKNOWN_SOLUTION;

	if ( solutionFactorAlpha > solutionFactorBetta )
	{
		if ( solutionFactorAlpha > solutionFactorGamma )
		{
			if ( solutionFactorAlpha > solutionFactorYotta )
			{
				solution = SOLUTION_ALPHA;
			}
			else
			{
				solution = SOLUTION_YOTTA;
			}
		}
		else if ( solutionFactorGamma > solutionFactorYotta )
		{
			solution = SOLUTION_GAMMA;
		}
		else
		{
			solution = SOLUTION_YOTTA;
		}
	}
	else if ( solutionFactorBetta > solutionFactorGamma )
	{
		if ( solutionFactorBetta > solutionFactorYotta )
		{
			solution = SOLUTION_BETTA;
		}
		else
		{
			solution = SOLUTION_YOTTA;
		}
	}
	else if ( solutionFactorGamma > solutionFactorYotta )
	{
		solution = SOLUTION_GAMMA;
	}
	else
	{
		solution = SOLUTION_YOTTA;
	}

	switch ( solution )
	{
		case SOLUTION_ALPHA:
		{
			GXFloat phi = 0.5f * sqrtf ( solutionFactorAlpha );
			GXFloat omega = 1.0f / ( 4.0f * phi );

			data[ 0 ] = phi;
			data[ 1 ] = omega * ( pureRotationMatrix.m[ 1 ][ 2 ] - pureRotationMatrix.m[ 2 ][ 1 ] );
			data[ 2 ] = omega * ( pureRotationMatrix.m[ 2 ][ 0 ] - pureRotationMatrix.m[ 0 ][ 2 ] );
			data[ 3 ] = omega * ( pureRotationMatrix.m[ 0 ][ 1 ] - pureRotationMatrix.m[ 1 ][ 0 ] );
		}
		break;

		case SOLUTION_BETTA:
		{
			GXFloat phi = 0.5f * sqrtf ( solutionFactorBetta );
			GXFloat omega = 1.0f / ( 4.0f * phi );

			data[ 0 ] = omega * ( pureRotationMatrix.m[ 1 ][ 2 ] - pureRotationMatrix.m[ 2 ][ 1 ] );
			data[ 1 ] = phi;
			data[ 2 ] = omega * ( pureRotationMatrix.m[ 0 ][ 1 ] + pureRotationMatrix.m[ 1 ][ 0 ] );
			data[ 3 ] = omega * ( pureRotationMatrix.m[ 0 ][ 2 ] + pureRotationMatrix.m[ 2 ][ 0 ] );
		}
		break;

		case SOLUTION_GAMMA:
		{
			GXFloat phi = 0.5f * sqrtf ( solutionFactorGamma );
			GXFloat omega = 1.0f / ( 4.0f * phi );

			data[ 0 ] = omega * ( pureRotationMatrix.m[ 2 ][ 0 ] - pureRotationMatrix.m[ 0 ][ 2 ] );
			data[ 1 ] = omega * ( pureRotationMatrix.m[ 0 ][ 1 ] + pureRotationMatrix.m[ 1 ][ 0 ] );
			data[ 2 ] = phi;
			data[ 3 ] = omega * ( pureRotationMatrix.m[ 1 ][ 2 ] + pureRotationMatrix.m[ 2 ][ 1 ] );
		}
		break;

		case SOLUTION_YOTTA:
		{
			GXFloat phi = 0.5f * sqrtf ( solutionFactorYotta );
			GXFloat omega = 1.0f / ( 4.0f * phi );

			data[ 0 ] = omega * ( pureRotationMatrix.m[ 0 ][ 1 ] - pureRotationMatrix.m[ 1 ][ 0 ] );
			data[ 1 ] = omega * ( pureRotationMatrix.m[ 0 ][ 2 ] + pureRotationMatrix.m[ 2 ][ 0 ] );
			data[ 2 ] = omega * ( pureRotationMatrix.m[ 1 ][ 2 ] + pureRotationMatrix.m[ 2 ][ 1 ] );
			data[ 3 ] = phi;
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

	GXFloat solutionFactorAlpha = pureRotationMatrix.m[ 0 ][ 0 ] + pureRotationMatrix.m[ 1 ][ 1 ] + pureRotationMatrix.m[ 2 ][ 2 ] + 1.0f;
	GXFloat solutionFactorBetta = pureRotationMatrix.m[ 0 ][ 0 ] - pureRotationMatrix.m[ 1 ][ 1 ] - pureRotationMatrix.m[ 2 ][ 2 ] + 1.0f;
	GXFloat solutionFactorGamma = -pureRotationMatrix.m[ 0 ][ 0 ] + pureRotationMatrix.m[ 1 ][ 1 ] - pureRotationMatrix.m[ 2 ][ 2 ] + 1.0f;
	GXFloat solutionFactorYotta = -pureRotationMatrix.m[ 0 ][ 0 ] - pureRotationMatrix.m[ 1 ][ 1 ] + pureRotationMatrix.m[ 2 ][ 2 ] + 1.0f;

	GXUByte solution = UNKNOWN_SOLUTION;

	if ( solutionFactorAlpha > solutionFactorBetta )
	{
		if ( solutionFactorAlpha > solutionFactorGamma )
		{
			if ( solutionFactorAlpha > solutionFactorYotta )
			{
				solution = SOLUTION_ALPHA;
			}
			else
			{
				solution = SOLUTION_YOTTA;
			}
		}
		else if ( solutionFactorGamma > solutionFactorYotta )
		{
			solution = SOLUTION_GAMMA;
		}
		else
		{
			solution = SOLUTION_YOTTA;
		}
	}
	else if ( solutionFactorBetta > solutionFactorGamma )
	{
		if ( solutionFactorBetta > solutionFactorYotta )
		{
			solution = SOLUTION_BETTA;
		}
		else
		{
			solution = SOLUTION_YOTTA;
		}
	}
	else if ( solutionFactorGamma > solutionFactorYotta )
	{
		solution = SOLUTION_GAMMA;
	}
	else
	{
		solution = SOLUTION_YOTTA;
	}

	switch ( solution )
	{
		case SOLUTION_ALPHA:
		{
			GXFloat phi = 0.5f * sqrtf ( solutionFactorAlpha );
			GXFloat omega = 1.0f / ( 4.0f * phi );

			data[ 0 ] = phi;
			data[ 1 ] = omega * ( pureRotationMatrix.m[ 1 ][ 2 ] - pureRotationMatrix.m[ 2 ][ 1 ] );
			data[ 2 ] = omega * ( pureRotationMatrix.m[ 2 ][ 0 ] - pureRotationMatrix.m[ 0 ][ 2 ] );
			data[ 3 ] = omega * ( pureRotationMatrix.m[ 0 ][ 1 ] - pureRotationMatrix.m[ 1 ][ 0 ] );
		}
		break;

		case SOLUTION_BETTA:
		{
			GXFloat phi = 0.5f * sqrtf ( solutionFactorBetta );
			GXFloat omega = 1.0f / ( 4.0f * phi );

			data[ 0 ] = omega * ( pureRotationMatrix.m[ 1 ][ 2 ] - pureRotationMatrix.m[ 2 ][ 1 ] );
			data[ 1 ] = phi;
			data[ 2 ] = omega * ( pureRotationMatrix.m[ 0 ][ 1 ] + pureRotationMatrix.m[ 1 ][ 0 ] );
			data[ 3 ] = omega * ( pureRotationMatrix.m[ 0 ][ 2 ] + pureRotationMatrix.m[ 2 ][ 0 ] );
		}
		break;

		case SOLUTION_GAMMA:
		{
			GXFloat phi = 0.5f * sqrtf ( solutionFactorGamma );
			GXFloat omega = 1.0f / ( 4.0f * phi );

			data[ 0 ] = omega * ( pureRotationMatrix.m[ 2 ][ 0 ] - pureRotationMatrix.m[ 0 ][ 2 ] );
			data[ 1 ] = omega * ( pureRotationMatrix.m[ 0 ][ 1 ] + pureRotationMatrix.m[ 1 ][ 0 ] );
			data[ 2 ] = phi;
			data[ 3 ] = omega * ( pureRotationMatrix.m[ 1 ][ 2 ] + pureRotationMatrix.m[ 2 ][ 1 ] );
		}
		break;

		case SOLUTION_YOTTA:
		{
			GXFloat phi = 0.5f * sqrtf ( solutionFactorYotta );
			GXFloat omega = 1.0f / ( 4.0f * phi );

			data[ 0 ] = omega * ( pureRotationMatrix.m[ 0 ][ 1 ] - pureRotationMatrix.m[ 1 ][ 0 ] );
			data[ 1 ] = omega * ( pureRotationMatrix.m[ 0 ][ 2 ] + pureRotationMatrix.m[ 2 ][ 0 ] );
			data[ 2 ] = omega * ( pureRotationMatrix.m[ 1 ][ 2 ] + pureRotationMatrix.m[ 2 ][ 1 ] );
			data[ 3 ] = phi;
		}
		break;

		default:
			// NOTHING
		break;
	}
}

GXVoid GXQuat::Multiply ( const GXQuat &a, const GXQuat &b )
{
	data[ 0 ] = a.data[ 0 ] * b.data[ 0 ] - a.data[ 1 ] * b.data[ 1 ] - a.data[ 2 ] * b.data[ 2 ] - a.data[ 3 ] * b.data[ 3 ];
	data[ 1 ] = a.data[ 0 ] * b.data[ 1 ] + a.data[ 1 ] * b.data[ 0 ] + a.data[ 2 ] * b.data[ 3 ] - a.data[ 3 ] * b.data[ 2 ];
	data[ 2 ] = a.data[ 0 ] * b.data[ 2 ] - a.data[ 1 ] * b.data[ 3 ] + a.data[ 2 ] * b.data[ 0 ] + a.data[ 3 ] * b.data[ 1 ];
	data[ 3 ] = a.data[ 0 ] * b.data[ 3 ] + a.data[ 1 ] * b.data[ 2 ] - a.data[ 2 ] * b.data[ 1 ] + a.data[ 3 ] * b.data[ 0 ];
}

GXVoid GXQuat::Multiply ( const GXQuat &q, GXFloat scale )
{
	data[ 0 ] = q.data[ 0 ] * scale;
	data[ 1 ] = q.data[ 1 ] * scale;
	data[ 2 ] = q.data[ 2 ] * scale;
	data[ 3 ] = q.data[ 3 ] * scale;
}

GXVoid GXQuat::Sum ( const GXQuat &a, const GXQuat &b )
{
	data[ 0 ] = a.data[ 0 ] + b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] + b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] + b.data[ 2 ];
	data[ 3 ] = a.data[ 3 ] + b.data[ 3 ];
}

GXVoid GXQuat::Substract ( const GXQuat &a, const GXQuat &b )
{
	data[ 0 ] = a.data[ 0 ] - b.data[ 0 ];
	data[ 1 ] = a.data[ 1 ] - b.data[ 1 ];
	data[ 2 ] = a.data[ 2 ] - b.data[ 2 ];
	data[ 3 ] = a.data[ 3 ] - b.data[ 3 ];
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

	cosom = start.data[ 0 ] * finish.data[ 0 ] + start.data[ 1 ] * finish.data[ 1 ] + start.data[ 2 ] * finish.data[ 2 ] + start.data[ 3 ] * finish.data[ 3 ];

	if ( cosom < 0.0f )
	{
		temp.data[ 0 ] = -finish.data[ 0 ];
		temp.data[ 1 ] = -finish.data[ 1 ];
		temp.data[ 2 ] = -finish.data[ 2 ];
		temp.data[ 3 ] = -finish.data[ 3 ];
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

	data[ 0 ] = start.data[ 0 ] * scale0 + temp.data[ 0 ] * scale1;
	data[ 1 ] = start.data[ 1 ] * scale0 + temp.data[ 1 ] * scale1;
	data[ 2 ] = start.data[ 2 ] * scale0 + temp.data[ 2 ] * scale1;
	data[ 3 ] = start.data[ 3 ] * scale0 + temp.data[ 3 ] * scale1;
}

GXVoid GXQuat::GetAxisAngle ( GXVec3 &axis, GXFloat &angle ) const
{
	GXQuat q ( *this );

	if ( fabsf ( q.data[ 0 ] ) > 1.0f )
		q.Normalize ();

	angle = 2.0f * acosf ( q.data[ 0 ] );

	axis.data[ 0 ] = q.data[ 1 ];
	axis.data[ 1 ] = q.data[ 2 ];
	axis.data[ 2 ] = q.data[ 3 ];

	GXFloat s = sqrtf ( 1.0f - q.data[ 0 ] * q.data[ 0 ] );

	if ( s < FLOAT_EPSILON ) return;

	axis.Multiply ( axis, 1.0f / s );
}

GXVoid GXQuat::Transform ( GXVec3 &out, const GXVec3 &v ) const
{
	GXFloat rr = data[ 0 ] * data[ 0 ];
	GXFloat ra2 = data[ 0 ] * data[ 1 ] * 2.0f;
	GXFloat rb2 = data[ 0 ] * data[ 2 ] * 2.0f;
	GXFloat rc2 = data[ 0 ] * data[ 3 ] * 2.0f;

	GXFloat aa = data[ 1 ] * data[ 1 ];
	GXFloat ab2 = data[ 1 ] * data[ 2 ] * 2.0f;
	GXFloat ac2 = data[ 1 ] * data[ 3 ] * 2.0f;

	GXFloat bb = data[ 2 ] * data[ 2 ];
	GXFloat bc2 = data[ 2 ] * data[ 3 ] * 2.0f;

	GXFloat cc = data[ 3 ] * data[ 3 ];

	GXFloat inverseSquaredLength = 1.0f / ( rr + aa + bb + cc );

	out.data[ 0 ] = inverseSquaredLength * ( v.data[ 0 ] * ( rr + aa - bb - cc ) + v.data[ 1 ] * ( ab2 - rc2 ) + v.data[ 2 ] * ( rb2 + ac2 ) );
	out.data[ 1 ] = inverseSquaredLength * ( v.data[ 0 ] * ( rc2 + ab2 ) + v.data[ 1 ] * ( rr - aa + bb - cc ) + v.data[ 2 ] * ( bc2 - ra2 ) );
	out.data[ 2 ] = inverseSquaredLength * ( v.data[ 0 ] * ( ac2 - rb2 ) + v.data[ 1 ] * ( ra2 + bc2 ) + v.data[ 2 ] * ( rr - aa - bb + cc ) );
}

GXVoid GXQuat::TransformFast ( GXVec3 &out, const GXVec3 &v ) const
{
	GXFloat rr = data[ 0 ] * data[ 0 ];
	GXFloat ra2 = data[ 0 ] * data[ 1 ] * 2.0f;
	GXFloat rb2 = data[ 0 ] * data[ 2 ] * 2.0f;
	GXFloat rc2 = data[ 0 ] * data[ 3 ] * 2.0f;

	GXFloat aa = data[ 1 ] * data[ 1 ];
	GXFloat ab2 = data[ 1 ] * data[ 2 ] * 2.0f;
	GXFloat ac2 = data[ 1 ] * data[ 3 ] * 2.0f;

	GXFloat bb = data[ 2 ] * data[ 2 ];
	GXFloat bc2 = data[ 2 ] * data[ 3 ] * 2.0f;

	GXFloat cc = data[ 3 ] * data[ 3 ];

	out.data[ 0 ] = v.data[ 0 ] * ( rr + aa - bb - cc ) + v.data[ 1 ] * ( ab2 - rc2 ) + v.data[ 2 ] * ( rb2 + ac2 );
	out.data[ 1 ] = v.data[ 0 ] * ( rc2 + ab2 ) + v.data[ 1 ] * ( rr - aa + bb - cc ) + v.data[ 2 ] * ( bc2 - ra2 );
	out.data[ 2 ] = v.data[ 0 ] * ( ac2 - rb2 ) + v.data[ 1 ] * ( ra2 + bc2 ) + v.data[ 2 ] * ( rr - aa - bb + cc );
}

GXQuat& GXQuat::operator = ( const GXVec4 &other )
{
	memcpy ( this, &other, sizeof ( GXQuat ) );
	return *this;
}

//-------------------------------------------------------------------

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
	GXFloat rr = quaternion.data[ 0 ] * quaternion.data[ 0 ];
	GXFloat ra2 = quaternion.data[ 0 ] * quaternion.data[ 1 ] * 2.0f;
	GXFloat rb2 = quaternion.data[ 0 ] * quaternion.data[ 2 ] * 2.0f;
	GXFloat rc2 = quaternion.data[ 0 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat aa = quaternion.data[ 1 ] * quaternion.data[ 1 ];
	GXFloat ab2 = quaternion.data[ 1 ] * quaternion.data[ 2 ] * 2.0f;
	GXFloat ac2 = quaternion.data[ 1 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat bb = quaternion.data[ 2 ] * quaternion.data[ 2 ];
	GXFloat bc2 = quaternion.data[ 2 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat cc = quaternion.data[ 3 ] * quaternion.data[ 3 ];

	GXFloat inverseSquaredLength = 1.0f / ( rr + aa + bb + cc );

	m[ 0 ][ 0 ] = inverseSquaredLength * ( rr + aa - bb - cc );
	m[ 0 ][ 1 ] = inverseSquaredLength * ( rc2 + ab2 );
	m[ 0 ][ 2 ] = inverseSquaredLength * ( ac2 - rb2 );

	m[ 1 ][ 0 ] = inverseSquaredLength * ( ab2 - rc2 );
	m[ 1 ][ 1 ] = inverseSquaredLength * ( rr - aa + bb - cc );
	m[ 1 ][ 2 ] = inverseSquaredLength * ( ra2 + bc2 );

	m[ 2 ][ 0 ] = inverseSquaredLength * ( rb2 + ac2 );
	m[ 2 ][ 1 ] = inverseSquaredLength * ( bc2 - ra2 );
	m[ 2 ][ 2 ] = inverseSquaredLength * ( rr - aa - bb + cc );
}

GXVoid GXMat3::From ( const GXMat4 &matrix )
{
	GXUPointer lineSize = 3 * sizeof ( GXFloat );

	memcpy ( data, matrix.data, lineSize );
	memcpy ( data + 3, matrix.data + 4, lineSize );
	memcpy ( data + 6, matrix.data + 8, lineSize );
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
	GXFloat rr = quaternion.data[ 0 ] * quaternion.data[ 0 ];
	GXFloat ra2 = quaternion.data[ 0 ] * quaternion.data[ 1 ] * 2.0f;
	GXFloat rb2 = quaternion.data[ 0 ] * quaternion.data[ 2 ] * 2.0f;
	GXFloat rc2 = quaternion.data[ 0 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat aa = quaternion.data[ 1 ] * quaternion.data[ 1 ];
	GXFloat ab2 = quaternion.data[ 1 ] * quaternion.data[ 2 ] * 2.0f;
	GXFloat ac2 = quaternion.data[ 1 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat bb = quaternion.data[ 2 ] * quaternion.data[ 2 ];
	GXFloat bc2 = quaternion.data[ 2 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat cc = quaternion.data[ 3 ] * quaternion.data[ 3 ];

	m[ 0 ][ 0 ] = rr + aa - bb - cc;
	m[ 0 ][ 1 ] = rc2 + ab2;
	m[ 0 ][ 2 ] = ac2 - rb2;

	m[ 1 ][ 0 ] = ab2 - rc2;
	m[ 1 ][ 1 ] = rr - aa + bb - cc;
	m[ 1 ][ 2 ] = ra2 + bc2;

	m[ 2 ][ 0 ] = rb2 + ac2;
	m[ 2 ][ 1 ] = bc2 - ra2;
	m[ 2 ][ 2 ] = rr - aa - bb + cc;
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
	memcpy ( data + 3, &y, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::GetY ( GXVec3 &y ) const
{
	memcpy ( &y, data + 3, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::SetZ ( const GXVec3 &z )
{
	memcpy ( data + 6, &z, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::GetZ ( GXVec3 &z ) const
{
	memcpy ( &z, data + 6, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::Identity ()
{
	m[ 0 ][ 0 ] = m[ 1 ][ 1 ] = m[ 2 ][ 2 ] = 1.0f;
	m[ 0 ][ 1 ] = m[ 0 ][ 2 ] = 0.0f;
	m[ 1 ][ 0 ] = m[ 1 ][ 2 ] = 0.0f;
	m[ 2 ][ 0 ] = m[ 2 ][ 1 ] = 0.0f;
}

GXVoid GXMat3::Zeros ()
{
	memset ( data, 0, 9 * sizeof ( GXFloat ) );
}

GXVoid GXMat3::Inverse ( const GXMat3 &sourceMatrix )
{
	GXFloat determinant = sourceMatrix.m[ 0 ][ 0 ] * ( sourceMatrix.m[ 1 ][ 1 ] * sourceMatrix.m[ 2 ][ 2 ] - sourceMatrix.m[ 2 ][ 1 ] * sourceMatrix.m[ 1 ][ 2 ] );
	determinant -= sourceMatrix.m[ 0 ][ 1 ] * ( sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 2 ][ 2 ] - sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 1 ][ 2 ] );
	determinant += sourceMatrix.m[ 0 ][ 2 ] * ( sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 2 ][ 1 ] - sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 1 ][ 1 ] );

	GXFloat invDeterminant = 1.0f / determinant;

	m[ 0 ][ 0 ] = invDeterminant * ( sourceMatrix.m[ 1 ][ 1 ] * sourceMatrix.m[ 2 ][ 2 ] - sourceMatrix.m[ 2 ][ 1 ] * sourceMatrix.m[ 1 ][ 2 ] );
	m[ 0 ][ 1 ] = invDeterminant * ( sourceMatrix.m[ 0 ][ 2 ] * sourceMatrix.m[ 2 ][ 1 ] - sourceMatrix.m[ 2 ][ 2 ] * sourceMatrix.m[ 0 ][ 1 ] );
	m[ 0 ][ 2 ] = invDeterminant * ( sourceMatrix.m[ 0 ][ 1 ] * sourceMatrix.m[ 1 ][ 2 ] - sourceMatrix.m[ 1 ][ 1 ] * sourceMatrix.m[ 0 ][ 2 ] );

	m[ 1 ][ 0 ] = invDeterminant * ( sourceMatrix.m[ 1 ][ 2 ] * sourceMatrix.m[ 2 ][ 0 ] - sourceMatrix.m[ 2 ][ 2 ] * sourceMatrix.m[ 1 ][ 0 ] );
	m[ 1 ][ 1 ] = invDeterminant * ( sourceMatrix.m[ 0 ][ 0 ] * sourceMatrix.m[ 2 ][ 2 ] - sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 0 ][ 2 ] );
	m[ 1 ][ 2 ] = invDeterminant * ( sourceMatrix.m[ 0 ][ 2 ] * sourceMatrix.m[ 1 ][ 0 ] - sourceMatrix.m[ 1 ][ 2 ] * sourceMatrix.m[ 0 ][ 0 ] );

	m[ 2 ][ 0 ] = invDeterminant * ( sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 2 ][ 1 ] - sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 1 ][ 1 ] );
	m[ 2 ][ 1 ] = invDeterminant * ( sourceMatrix.m[ 0 ][ 1 ] * sourceMatrix.m[ 2 ][ 0 ] - sourceMatrix.m[ 2 ][ 1 ] * sourceMatrix.m[ 0 ][ 0 ] );
	m[ 2 ][ 2 ] = invDeterminant * ( sourceMatrix.m[ 0 ][ 0 ] * sourceMatrix.m[ 1 ][ 1 ] - sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 0 ][ 1 ] );
}

GXVoid GXMat3::Transponse ( const GXMat3 &sourceMatrix )
{
	m[ 0 ][ 0 ] = sourceMatrix.m[ 0 ][ 0 ];
	m[ 0 ][ 1 ] = sourceMatrix.m[ 1 ][ 0 ];
	m[ 0 ][ 2 ] = sourceMatrix.m[ 2 ][ 0 ];

	m[ 1 ][ 0 ] = sourceMatrix.m[ 0 ][ 1 ];
	m[ 1 ][ 1 ] = sourceMatrix.m[ 1 ][ 1 ];
	m[ 1 ][ 2 ] = sourceMatrix.m[ 2 ][ 1 ];

	m[ 2 ][ 0 ] = sourceMatrix.m[ 0 ][ 2 ];
	m[ 2 ][ 1 ] = sourceMatrix.m[ 1 ][ 2 ];
	m[ 2 ][ 2 ] = sourceMatrix.m[ 2 ][ 2 ];
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
	m[ 0 ][ 0 ] = m[ 1 ][ 1 ] = m[ 2 ][ 2 ] = 0.0f;

	m[ 0 ][ 1 ] = -base.GetZ ();
	m[ 0 ][ 2 ] = base.GetY ();

	m[ 1 ][ 0 ] = base.GetZ ();
	m[ 1 ][ 2 ] = -base.GetX ();

	m[ 2 ][ 0 ] = -base.GetY ();
	m[ 2 ][ 1 ] = base.GetX ();
}

GXVoid GXMat3::Sum ( const GXMat3 &a, const GXMat3 &b )
{
	m[ 0 ][ 0 ] = a.m[ 0 ][ 0 ] + b.m[ 0 ][ 0 ];
	m[ 0 ][ 1 ] = a.m[ 0 ][ 1 ] + b.m[ 0 ][ 1 ];
	m[ 0 ][ 2 ] = a.m[ 0 ][ 2 ] + b.m[ 0 ][ 2 ];

	m[ 1 ][ 0 ] = a.m[ 1 ][ 0 ] + b.m[ 1 ][ 0 ];
	m[ 1 ][ 1 ] = a.m[ 1 ][ 1 ] + b.m[ 1 ][ 1 ];
	m[ 1 ][ 2 ] = a.m[ 1 ][ 2 ] + b.m[ 1 ][ 2 ];

	m[ 2 ][ 0 ] = a.m[ 2 ][ 0 ] + b.m[ 2 ][ 0 ];
	m[ 2 ][ 1 ] = a.m[ 2 ][ 1 ] + b.m[ 2 ][ 1 ];
	m[ 2 ][ 2 ] = a.m[ 2 ][ 2 ] + b.m[ 2 ][ 2 ];
}

GXVoid GXMat3::Substract ( const GXMat3 &a, const GXMat3 &b )
{
	m[ 0 ][ 0 ] = a.m[ 0 ][ 0 ] - b.m[ 0 ][ 0 ];
	m[ 0 ][ 1 ] = a.m[ 0 ][ 1 ] - b.m[ 0 ][ 1 ];
	m[ 0 ][ 2 ] = a.m[ 0 ][ 2 ] - b.m[ 0 ][ 2 ];

	m[ 1 ][ 0 ] = a.m[ 1 ][ 0 ] - b.m[ 1 ][ 0 ];
	m[ 1 ][ 1 ] = a.m[ 1 ][ 1 ] - b.m[ 1 ][ 1 ];
	m[ 1 ][ 2 ] = a.m[ 1 ][ 2 ] - b.m[ 1 ][ 2 ];

	m[ 2 ][ 0 ] = a.m[ 2 ][ 0 ] - b.m[ 2 ][ 0 ];
	m[ 2 ][ 1 ] = a.m[ 2 ][ 1 ] - b.m[ 2 ][ 1 ];
	m[ 2 ][ 2 ] = a.m[ 2 ][ 2 ] - b.m[ 2 ][ 2 ];
}

GXVoid GXMat3::Multiply ( const GXMat3 &a, const GXMat3 &b )
{
	m[ 0 ][ 0 ] = a.m[ 0 ][ 0 ] * b.m[ 0 ][ 0 ] + a.m[ 0 ][ 1 ] * b.m[ 1 ][ 0 ] + a.m[ 0 ][ 2 ] * b.m[ 2 ][ 0 ];
	m[ 0 ][ 1 ] = a.m[ 0 ][ 0 ] * b.m[ 0 ][ 1 ] + a.m[ 0 ][ 1 ] * b.m[ 1 ][ 1 ] + a.m[ 0 ][ 2 ] * b.m[ 2 ][ 1 ];
	m[ 0 ][ 2 ] = a.m[ 0 ][ 0 ] * b.m[ 0 ][ 2 ] + a.m[ 0 ][ 1 ] * b.m[ 1 ][ 2 ] + a.m[ 0 ][ 2 ] * b.m[ 2 ][ 2 ];

	m[ 1 ][ 0 ] = a.m[ 1 ][ 0 ] * b.m[ 0 ][ 0 ] + a.m[ 1 ][ 1 ] * b.m[ 1 ][ 0 ] + a.m[ 1 ][ 2 ] * b.m[ 2 ][ 0 ];
	m[ 1 ][ 1 ] = a.m[ 1 ][ 0 ] * b.m[ 0 ][ 1 ] + a.m[ 1 ][ 1 ] * b.m[ 1 ][ 1 ] + a.m[ 1 ][ 2 ] * b.m[ 2 ][ 1 ];
	m[ 1 ][ 2 ] = a.m[ 1 ][ 0 ] * b.m[ 0 ][ 2 ] + a.m[ 1 ][ 1 ] * b.m[ 1 ][ 2 ] + a.m[ 1 ][ 2 ] * b.m[ 2 ][ 2 ];

	m[ 2 ][ 0 ] = a.m[ 2 ][ 0 ] * b.m[ 0 ][ 0 ] + a.m[ 2 ][ 1 ] * b.m[ 1 ][ 0 ] + a.m[ 2 ][ 2 ] * b.m[ 2 ][ 0 ];
	m[ 2 ][ 1 ] = a.m[ 2 ][ 0 ] * b.m[ 0 ][ 1 ] + a.m[ 2 ][ 1 ] * b.m[ 1 ][ 1 ] + a.m[ 2 ][ 2 ] * b.m[ 2 ][ 1 ];
	m[ 2 ][ 2 ] = a.m[ 2 ][ 0 ] * b.m[ 0 ][ 2 ] + a.m[ 2 ][ 1 ] * b.m[ 1 ][ 2 ] + a.m[ 2 ][ 2 ] * b.m[ 2 ][ 2 ];
}

GXVoid GXMat3::MultiplyVectorMatrix ( GXVec3 &out, const GXVec3 &v ) const
{
	out.data[ 0 ] = v.data[ 0 ] * m[ 0 ][ 0 ] + v.data[ 1 ] * m[ 1 ][ 0 ] + v.data[ 2 ] * m[ 2 ][ 0 ];
	out.data[ 1 ] = v.data[ 0 ] * m[ 0 ][ 1 ] + v.data[ 1 ] * m[ 1 ][ 1 ] + v.data[ 2 ] * m[ 2 ][ 1 ];
	out.data[ 2 ] = v.data[ 0 ] * m[ 0 ][ 2 ] + v.data[ 1 ] * m[ 1 ][ 2 ] + v.data[ 2 ] * m[ 2 ][ 2 ];
}

GXVoid GXMat3::MultiplyMatrixVector ( GXVec3 &out, const GXVec3 &v ) const
{
	out.data[ 0 ] = m[ 0 ][ 0 ] * v.data[ 0 ] + m[ 0 ][ 1 ] * v.data[ 1 ] + m[ 0 ][ 2 ] * v.data[ 2 ];
	out.data[ 1 ] = m[ 1 ][ 0 ] * v.data[ 0 ] + m[ 1 ][ 1 ] * v.data[ 1 ] + m[ 1 ][ 2 ] * v.data[ 2 ];
	out.data[ 2 ] = m[ 2 ][ 0 ] * v.data[ 0 ] + m[ 2 ][ 1 ] * v.data[ 1 ] + m[ 2 ][ 2 ] * v.data[ 2 ];
}

GXVoid GXMat3::Multiply ( const GXMat3 &a, GXFloat factor )
{
	m[ 0 ][ 0 ] = a.m[ 0 ][ 0 ] * factor;
	m[ 0 ][ 1 ] = a.m[ 0 ][ 1 ] * factor;
	m[ 0 ][ 2 ] = a.m[ 0 ][ 2 ] * factor;

	m[ 1 ][ 0 ] = a.m[ 1 ][ 0 ] * factor;
	m[ 1 ][ 1 ] = a.m[ 1 ][ 1 ] * factor;
	m[ 1 ][ 2 ] = a.m[ 1 ][ 2 ] * factor;

	m[ 2 ][ 0 ] = a.m[ 2 ][ 0 ] * factor;
	m[ 2 ][ 1 ] = a.m[ 2 ][ 1 ] * factor;
	m[ 2 ][ 2 ] = a.m[ 2 ][ 2 ] * factor;
}

GXMat3& GXMat3::operator = ( const GXMat3 &matrix )
{
	memcpy ( this, &matrix, sizeof ( GXMat3 ) );
	return *this;
}

//------------------------------------------------------------------

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
	GXFloat rr = quaternion.data[ 0 ] * quaternion.data[ 0 ];
	GXFloat ra2 = quaternion.data[ 0 ] * quaternion.data[ 1 ] * 2.0f;
	GXFloat rb2 = quaternion.data[ 0 ] * quaternion.data[ 2 ] * 2.0f;
	GXFloat rc2 = quaternion.data[ 0 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat aa = quaternion.data[ 1 ] * quaternion.data[ 1 ];
	GXFloat ab2 = quaternion.data[ 1 ] * quaternion.data[ 2 ] * 2.0f;
	GXFloat ac2 = quaternion.data[ 1 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat bb = quaternion.data[ 2 ] * quaternion.data[ 2 ];
	GXFloat bc2 = quaternion.data[ 2 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat cc = quaternion.data[ 3 ] * quaternion.data[ 3 ];

	GXFloat inverseSquaredLength = 1.0f / ( rr + aa + bb + cc );

	m[ 0 ][ 0 ] = inverseSquaredLength * ( rr + aa - bb - cc );
	m[ 0 ][ 1 ] = inverseSquaredLength * ( rc2 + ab2 );
	m[ 0 ][ 2 ] = inverseSquaredLength * ( ac2 - rb2 );

	m[ 1 ][ 0 ] = inverseSquaredLength * ( ab2 - rc2 );
	m[ 1 ][ 1 ] = inverseSquaredLength * ( rr - aa + bb - cc );
	m[ 1 ][ 2 ] = inverseSquaredLength * ( ra2 + bc2 );

	m[ 2 ][ 0 ] = inverseSquaredLength * ( rb2 + ac2 );
	m[ 2 ][ 1 ] = inverseSquaredLength * ( bc2 - ra2 );
	m[ 2 ][ 2 ] = inverseSquaredLength * ( rr - aa - bb + cc );
}

GXVoid GXMat4::SetRotationFast ( const GXQuat &quaternion )
{
	GXFloat rr = quaternion.data[ 0 ] * quaternion.data[ 0 ];
	GXFloat ra2 = quaternion.data[ 0 ] * quaternion.data[ 1 ] * 2.0f;
	GXFloat rb2 = quaternion.data[ 0 ] * quaternion.data[ 2 ] * 2.0f;
	GXFloat rc2 = quaternion.data[ 0 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat aa = quaternion.data[ 1 ] * quaternion.data[ 1 ];
	GXFloat ab2 = quaternion.data[ 1 ] * quaternion.data[ 2 ] * 2.0f;
	GXFloat ac2 = quaternion.data[ 1 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat bb = quaternion.data[ 2 ] * quaternion.data[ 2 ];
	GXFloat bc2 = quaternion.data[ 2 ] * quaternion.data[ 3 ] * 2.0f;

	GXFloat cc = quaternion.data[ 3 ] * quaternion.data[ 3 ];

	m[ 0 ][ 0 ] = rr + aa - bb - cc;
	m[ 0 ][ 1 ] = rc2 + ab2;
	m[ 0 ][ 2 ] = ac2 - rb2;

	m[ 1 ][ 0 ] = ab2 - rc2;
	m[ 1 ][ 1 ] = rr - aa + bb - cc;
	m[ 1 ][ 2 ] = ra2 + bc2;

	m[ 2 ][ 0 ] = rb2 + ac2;
	m[ 2 ][ 1 ] = bc2 - ra2;
	m[ 2 ][ 2 ] = rr - aa - bb + cc;
}

GXVoid GXMat4::SetOrigin ( const GXVec3 &origin )
{ 
	SetW ( origin );
}

GXVoid GXMat4::From ( const GXQuat &quaternion, const GXVec3 &origin )
{
	SetRotation ( quaternion );
	SetOrigin ( origin );

	m[ 0 ][ 3 ] = m[ 1 ][ 3 ] = m[ 2 ][ 3 ] = 0.0f;
	m[ 3 ][ 3 ] = 1.0f;
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

	m[ 0 ][ 3 ] = m[ 1 ][ 3 ] = m[ 2 ][ 3 ] = 0.0f;
	m[ 3 ][ 3 ] = 1.0f;
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

	m[ 3 ][ 3 ] = 1.0f;
}

GXVoid GXMat4::FromFast ( const GXQuat &quaternion, const GXVec3 &origin )
{
	SetRotationFast ( quaternion );
	SetOrigin ( origin );

	m[ 0 ][ 3 ] = m[ 1 ][ 3 ] = m[ 2 ][ 3 ] = 0.0f;
	m[ 3 ][ 3 ] = 1.0f;
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
	memcpy ( data + 4, &y, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetY ( GXVec3 &y ) const
{
	memcpy ( &y, data + 4, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::SetZ ( const GXVec3 &z )
{
	memcpy ( data + 8, &z, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetZ ( GXVec3 &z ) const
{
	memcpy ( &z, data + 8, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::SetW ( const GXVec3 &w )
{
	memcpy ( data + 12, &w, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetW ( GXVec3 &w ) const
{
	memcpy ( &w, data + 12, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::Identity ()
{
	m[ 0 ][ 1 ] = m[ 0 ][ 2 ] = m[ 0 ][ 3 ] = 0.0f;
	m[ 1 ][ 0 ] = m[ 1 ][ 2 ] = m[ 1 ][ 3 ] = 0.0f;
	m[ 2 ][ 0 ] = m[ 2 ][ 1 ] = m[ 2 ][ 3 ] = 0.0f;
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = m[ 3 ][ 2 ] = 0.0f;

	m[ 0 ][ 0 ] = m[ 1 ][ 1 ] = m[ 2 ][ 2 ] = m[ 3 ][ 3 ] = 1.0f;
}

GXVoid GXMat4::Perspective ( GXFloat fieldOfViewYRadiands, GXFloat aspectRatio, GXFloat zNear, GXFloat zFar )
{
	GXFloat halfFovy = fieldOfViewYRadiands * 0.5f;
	GXFloat ctan = cosf ( halfFovy ) / sinf ( halfFovy );
	GXFloat invRange = 1.0f / ( zFar - zNear );

	m[ 0 ][ 0 ] = ctan / aspectRatio;
	m[ 1 ][ 1 ] = ctan;
	m[ 2 ][ 2 ] = ( zFar + zNear ) * invRange;
	m[ 2 ][ 3 ] = 1.0f;
	m[ 3 ][ 2 ] = -2.0f * zFar * zNear * invRange;

	m[ 0 ][ 1 ] = m[ 0 ][ 2 ] = m[ 0 ][ 3 ] = 0.0f;
	m[ 1 ][ 0 ] = m[ 1 ][ 2 ] = m[ 1 ][ 3 ] = 0.0f;
	m[ 2 ][ 0 ] = m[ 2 ][ 1 ] = 0.0f;
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = m[ 3 ][ 3 ] = 0.0f;
}

GXVoid GXMat4::Ortho ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar )
{
	GXFloat invRange = 1.0f / ( zFar - zNear );

	m[ 0 ][ 0 ] = 2.0f / width;
	m[ 1 ][ 1 ] = 2.0f / height;
	m[ 2 ][ 2 ] = 2.0f * invRange;
	m[ 3 ][ 2 ] = -invRange * ( zFar + zNear );

	m[ 0 ][ 1 ] = m[ 0 ][ 2 ] = m[ 0 ][ 3 ] = 0.0f;
	m[ 1 ][ 0 ] = m[ 1 ][ 2 ] = m[ 1 ][ 3 ] = 0.0f;
	m[ 2 ][ 0 ] = m[ 2 ][ 1 ] = m[ 2 ][ 3 ] = 0.0f;
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = 0.0f;
	m[ 3 ][ 3 ] = 1.0f;
}

GXVoid GXMat4::Translation ( GXFloat x, GXFloat y, GXFloat z )
{
	m[ 0 ][ 1 ] = m[ 0 ][ 2 ] = m[ 0 ][ 3 ] = 0.0f;
	m[ 1 ][ 0 ] = m[ 1 ][ 2 ] = m[ 1 ][ 3 ] = 0.0f;
	m[ 2 ][ 0 ] = m[ 2 ][ 1 ] = m[ 2 ][ 3 ] = 0.0f;

	m[ 0 ][ 0 ] = m[ 1 ][ 1 ] = m[ 2 ][ 2 ] = m[ 3 ][ 3 ] = 1.0f;

	m[ 3 ][ 0 ] = x;
	m[ 3 ][ 1 ] = y;
	m[ 3 ][ 2 ] = z;
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
	m[ 0 ][ 1 ] = m[ 0 ][ 2 ] = 0.0f;
	m[ 1 ][ 0 ] = 0.0f;
	m[ 2 ][ 0 ] = 0.0f;

	GXFloat c;
	GXFloat s;

	c = cosf ( angle );
	s = sinf ( angle );

	m[ 1 ][ 1 ] = c;	m[ 1 ][ 2 ] = s;
	m[ 2 ][ 1 ] = -s;	m[ 2 ][ 2 ] = c;

	m[ 0 ][ 0 ] = 1.0f;

	m[ 3 ][ 3 ] = 1.0f;
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = m[ 3 ][ 2 ] = m[ 0 ][ 3 ] = m[ 1 ][ 3 ] = m[ 2 ][ 3 ] = 0.0f;
}

GXVoid GXMat4::RotationY ( GXFloat angle )
{
	m[ 0 ][ 1 ] = 0.0f;
	m[ 1 ][ 0 ] = m[ 1 ][ 2 ] = 0.0f;
	m[ 2 ][ 1 ] = 0.0f;

	GXFloat c;
	GXFloat s;

	c = cosf ( angle );
	s = sinf ( angle );

	m[ 0 ][ 0 ] = c;	m[ 0 ][ 2 ] = -s;
	m[ 2 ][ 0 ] = s;	m[ 2 ][ 2 ] = c;

	m[ 1 ][ 1 ] = 1.0f;

	m[ 3 ][ 3 ] = 1.0f;
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = m[ 3 ][ 2 ] = m[ 0 ][ 3 ] = m[ 1 ][ 3 ] = m[ 2 ][ 3 ] = 0.0f;
}

GXVoid GXMat4::RotationZ ( GXFloat angle )
{
	m[ 0 ][ 2 ] = 0.0f;
	m[ 1 ][ 2 ] = 0.0f;
	m[ 2 ][ 0 ] = m[ 2 ][ 1 ] = 0.0f;

	GXFloat c;
	GXFloat s;

	c = cosf ( angle );
	s = sinf ( angle );

	m[ 0 ][ 0 ] = c;	m[ 0 ][ 1 ] = s;
	m[ 1 ][ 0 ] = -s;	m[ 1 ][ 1 ] = c;

	m[ 2 ][ 2 ] = 1.0f;

	m[ 3 ][ 3 ] = 1.0f;
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = m[ 3 ][ 2 ] = m[ 0 ][ 3 ] = m[ 1 ][ 3 ] = m[ 2 ][ 3 ] = 0.0f;
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

	m[ 0 ][ 3 ] = m[ 1 ][ 3 ] = m[ 2 ][ 3 ] = 0.0f;
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = m[ 3 ][ 2 ] = 0.0f;

	m[ 3 ][ 3 ] = 1.0f;
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

	m[ 0 ][ 3 ] = m[ 1 ][ 3 ] = m[ 2 ][ 3 ] = 0.0f;
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = m[ 3 ][ 2 ] = 0.0f;

	m[ 3 ][ 3 ] = 1.0f;
}

GXVoid GXMat4::Scale ( GXFloat x, GXFloat y, GXFloat z )
{
	m[ 0 ][ 1 ] = m[ 0 ][ 2 ] = 0.0f;
	m[ 1 ][ 0 ] = m[ 1 ][ 2 ] = 0.0f;
	m[ 2 ][ 0 ] = m[ 2 ][ 1 ] = 0.0f;

	m[ 0 ][ 0 ] = x;
	m[ 1 ][ 1 ] = y;
	m[ 2 ][ 2 ] = z;

	m[ 3 ][ 3 ] = 1.0f;
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = m[ 3 ][ 2 ] = m[ 0 ][ 3 ] = m[ 1 ][ 3 ] = m[ 2 ][ 3 ] = 0.0f;
}

GXVoid GXMat4::ClearScale ( GXVec3 &scale ) const
{
	GXVec3 alpha;

	GetX ( alpha );
	scale.data[ 0 ] = alpha.Length ();

	GetY ( alpha );
	scale.data[ 1 ] = alpha.Length ();

	GetZ ( alpha );
	scale.data[ 2 ] = alpha.Length ();
}

GXVoid GXMat4::Inverse ( const GXMat4 &sourceMatrix )
{
	// 2x2 sub-determinants required to calculate 4x4 determinant
	GXFloat det2_01_01 = sourceMatrix.m[ 0 ][ 0 ] * sourceMatrix.m[ 1 ][ 1 ] - sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 0 ][ 1 ];
	GXFloat det2_01_02 = sourceMatrix.m[ 0 ][ 0 ] * sourceMatrix.m[ 2 ][ 1 ] - sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 0 ][ 1 ];
	GXFloat det2_01_03 = sourceMatrix.m[ 0 ][ 0 ] * sourceMatrix.m[ 3 ][ 1 ] - sourceMatrix.m[ 3 ][ 0 ] * sourceMatrix.m[ 0 ][ 1 ];
	GXFloat det2_01_12 = sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 2 ][ 1 ] - sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 1 ][ 1 ];
	GXFloat det2_01_13 = sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 3 ][ 1 ] - sourceMatrix.m[ 3 ][ 0 ] * sourceMatrix.m[ 1 ][ 1 ];
	GXFloat det2_01_23 = sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 3 ][ 1 ] - sourceMatrix.m[ 3 ][ 0 ] * sourceMatrix.m[ 2 ][ 1 ];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	GXFloat det3_201_012 = sourceMatrix.m[ 0 ][ 2 ] * det2_01_12 - sourceMatrix.m[ 1 ][ 2 ] * det2_01_02 + sourceMatrix.m[ 2 ][ 2 ] * det2_01_01;
	GXFloat det3_201_013 = sourceMatrix.m[ 0 ][ 2 ] * det2_01_13 - sourceMatrix.m[ 1 ][ 2 ] * det2_01_03 + sourceMatrix.m[ 3 ][ 2 ] * det2_01_01;
	GXFloat det3_201_023 = sourceMatrix.m[ 0 ][ 2 ] * det2_01_23 - sourceMatrix.m[ 2 ][ 2 ] * det2_01_03 + sourceMatrix.m[ 3 ][ 2 ] * det2_01_02;
	GXFloat det3_201_123 = sourceMatrix.m[ 1 ][ 2 ] * det2_01_23 - sourceMatrix.m[ 2 ][ 2 ] * det2_01_13 + sourceMatrix.m[ 3 ][ 2 ] * det2_01_12;

	GXFloat inverseDeterminant = 1.0f / ( -det3_201_123 * sourceMatrix.m[ 0 ][ 3 ] + det3_201_023 * sourceMatrix.m[ 1 ][ 3 ] - det3_201_013 * sourceMatrix.m[ 2 ][ 3 ] + det3_201_012 * sourceMatrix.m[ 3 ][ 3 ] );

	// remaining 2x2 sub-determinants
	GXFloat det2_03_01 = sourceMatrix.m[ 0 ][ 0 ] * sourceMatrix.m[ 1 ][ 3 ] - sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 0 ][ 3 ];
	GXFloat det2_03_02 = sourceMatrix.m[ 0 ][ 0 ] * sourceMatrix.m[ 2 ][ 3 ] - sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 0 ][ 3 ];
	GXFloat det2_03_03 = sourceMatrix.m[ 0 ][ 0 ] * sourceMatrix.m[ 3 ][ 3 ] - sourceMatrix.m[ 3 ][ 0 ] * sourceMatrix.m[ 0 ][ 3 ];
	GXFloat det2_03_12 = sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 2 ][ 3 ] - sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 1 ][ 3 ];
	GXFloat det2_03_13 = sourceMatrix.m[ 1 ][ 0 ] * sourceMatrix.m[ 3 ][ 3 ] - sourceMatrix.m[ 3 ][ 0 ] * sourceMatrix.m[ 1 ][ 3 ];
	GXFloat det2_03_23 = sourceMatrix.m[ 2 ][ 0 ] * sourceMatrix.m[ 3 ][ 3 ] - sourceMatrix.m[ 3 ][ 0 ] * sourceMatrix.m[ 2 ][ 3 ];

	GXFloat det2_13_01 = sourceMatrix.m[ 0 ][ 1 ] * sourceMatrix.m[ 1 ][ 3 ] - sourceMatrix.m[ 1 ][ 1 ] * sourceMatrix.m[ 0 ][ 3 ];
	GXFloat det2_13_02 = sourceMatrix.m[ 0 ][ 1 ] * sourceMatrix.m[ 2 ][ 3 ] - sourceMatrix.m[ 2 ][ 1 ] * sourceMatrix.m[ 0 ][ 3 ];
	GXFloat det2_13_03 = sourceMatrix.m[ 0 ][ 1 ] * sourceMatrix.m[ 3 ][ 3 ] - sourceMatrix.m[ 3 ][ 1 ] * sourceMatrix.m[ 0 ][ 3 ];
	GXFloat det2_13_12 = sourceMatrix.m[ 1 ][ 1 ] * sourceMatrix.m[ 2 ][ 3 ] - sourceMatrix.m[ 2 ][ 1 ] * sourceMatrix.m[ 1 ][ 3 ];
	GXFloat det2_13_13 = sourceMatrix.m[ 1 ][ 1 ] * sourceMatrix.m[ 3 ][ 3 ] - sourceMatrix.m[ 3 ][ 1 ] * sourceMatrix.m[ 1 ][ 3 ];
	GXFloat det2_13_23 = sourceMatrix.m[ 2 ][ 1 ] * sourceMatrix.m[ 3 ][ 3 ] - sourceMatrix.m[ 3 ][ 1 ] * sourceMatrix.m[ 2 ][ 3 ];

	// remaining 3x3 sub-determinants
	GXFloat det3_203_012 = sourceMatrix.m[ 0 ][ 2 ] * det2_03_12 - sourceMatrix.m[ 1 ][ 2 ] * det2_03_02 + sourceMatrix.m[ 2 ][ 2 ] * det2_03_01;
	GXFloat det3_203_013 = sourceMatrix.m[ 0 ][ 2 ] * det2_03_13 - sourceMatrix.m[ 1 ][ 2 ] * det2_03_03 + sourceMatrix.m[ 3 ][ 2 ] * det2_03_01;
	GXFloat det3_203_023 = sourceMatrix.m[ 0 ][ 2 ] * det2_03_23 - sourceMatrix.m[ 2 ][ 2 ] * det2_03_03 + sourceMatrix.m[ 3 ][ 2 ] * det2_03_02;
	GXFloat det3_203_123 = sourceMatrix.m[ 1 ][ 2 ] * det2_03_23 - sourceMatrix.m[ 2 ][ 2 ] * det2_03_13 + sourceMatrix.m[ 3 ][ 2 ] * det2_03_12;

	GXFloat det3_213_012 = sourceMatrix.m[ 0 ][ 2 ] * det2_13_12 - sourceMatrix.m[ 1 ][ 2 ] * det2_13_02 + sourceMatrix.m[ 2 ][ 2 ] * det2_13_01;
	GXFloat det3_213_013 = sourceMatrix.m[ 0 ][ 2 ] * det2_13_13 - sourceMatrix.m[ 1 ][ 2 ] * det2_13_03 + sourceMatrix.m[ 3 ][ 2 ] * det2_13_01;
	GXFloat det3_213_023 = sourceMatrix.m[ 0 ][ 2 ] * det2_13_23 - sourceMatrix.m[ 2 ][ 2 ] * det2_13_03 + sourceMatrix.m[ 3 ][ 2 ] * det2_13_02;
	GXFloat det3_213_123 = sourceMatrix.m[ 1 ][ 2 ] * det2_13_23 - sourceMatrix.m[ 2 ][ 2 ] * det2_13_13 + sourceMatrix.m[ 3 ][ 2 ] * det2_13_12;

	GXFloat det3_301_012 = sourceMatrix.m[ 0 ][ 3 ] * det2_01_12 - sourceMatrix.m[ 1 ][ 3 ] * det2_01_02 + sourceMatrix.m[ 2 ][ 3 ] * det2_01_01;
	GXFloat det3_301_013 = sourceMatrix.m[ 0 ][ 3 ] * det2_01_13 - sourceMatrix.m[ 1 ][ 3 ] * det2_01_03 + sourceMatrix.m[ 3 ][ 3 ] * det2_01_01;
	GXFloat det3_301_023 = sourceMatrix.m[ 0 ][ 3 ] * det2_01_23 - sourceMatrix.m[ 2 ][ 3 ] * det2_01_03 + sourceMatrix.m[ 3 ][ 3 ] * det2_01_02;
	GXFloat det3_301_123 = sourceMatrix.m[ 1 ][ 3 ] * det2_01_23 - sourceMatrix.m[ 2 ][ 3 ] * det2_01_13 + sourceMatrix.m[ 3 ][ 3 ] * det2_01_12;

	m[ 0 ][ 0 ] = -det3_213_123 * inverseDeterminant;
	m[ 0 ][ 1 ] = +det3_213_023 * inverseDeterminant;
	m[ 0 ][ 2 ] = -det3_213_013 * inverseDeterminant;
	m[ 0 ][ 3 ] = +det3_213_012 * inverseDeterminant;

	m[ 1 ][ 0 ] = +det3_203_123 * inverseDeterminant;
	m[ 1 ][ 1 ] = -det3_203_023 * inverseDeterminant;
	m[ 1 ][ 2 ] = +det3_203_013 * inverseDeterminant;
	m[ 1 ][ 3 ] = -det3_203_012 * inverseDeterminant;

	m[ 2 ][ 0 ] = +det3_301_123 * inverseDeterminant;
	m[ 2 ][ 1 ] = -det3_301_023 * inverseDeterminant;
	m[ 2 ][ 2 ] = +det3_301_013 * inverseDeterminant;
	m[ 2 ][ 3 ] = -det3_301_012 * inverseDeterminant;

	m[ 3 ][ 0 ] = -det3_201_123 * inverseDeterminant;
	m[ 3 ][ 1 ] = +det3_201_023 * inverseDeterminant;
	m[ 3 ][ 2 ] = -det3_201_013 * inverseDeterminant;
	m[ 3 ][ 3 ] = +det3_201_012 * inverseDeterminant;
}

GXVoid GXMat4::Multiply ( const GXMat4 &a, const GXMat4 &b )
{
	m[ 0 ][ 0 ] = a.m[ 0 ][ 0 ] * b.m[ 0 ][ 0 ] + a.m[ 0 ][ 1 ] * b.m[ 1 ][ 0 ] + a.m[ 0 ][ 2 ] * b.m[ 2 ][ 0 ] + a.m[ 0 ][ 3 ] * b.m[ 3 ][ 0 ];
	m[ 0 ][ 1 ] = a.m[ 0 ][ 0 ] * b.m[ 0 ][ 1 ] + a.m[ 0 ][ 1 ] * b.m[ 1 ][ 1 ] + a.m[ 0 ][ 2 ] * b.m[ 2 ][ 1 ] + a.m[ 0 ][ 3 ] * b.m[ 3 ][ 1 ];
	m[ 0 ][ 2 ] = a.m[ 0 ][ 0 ] * b.m[ 0 ][ 2 ] + a.m[ 0 ][ 1 ] * b.m[ 1 ][ 2 ] + a.m[ 0 ][ 2 ] * b.m[ 2 ][ 2 ] + a.m[ 0 ][ 3 ] * b.m[ 3 ][ 2 ];
	m[ 0 ][ 3 ] = a.m[ 0 ][ 0 ] * b.m[ 0 ][ 3 ] + a.m[ 0 ][ 1 ] * b.m[ 1 ][ 3 ] + a.m[ 0 ][ 2 ] * b.m[ 2 ][ 3 ] + a.m[ 0 ][ 3 ] * b.m[ 3 ][ 3 ];

	m[ 1 ][ 0 ] = a.m[ 1 ][ 0 ] * b.m[ 0 ][ 0 ] + a.m[ 1 ][ 1 ] * b.m[ 1 ][ 0 ] + a.m[ 1 ][ 2 ] * b.m[ 2 ][ 0 ] + a.m[ 1 ][ 3 ] * b.m[ 3 ][ 0 ];
	m[ 1 ][ 1 ] = a.m[ 1 ][ 0 ] * b.m[ 0 ][ 1 ] + a.m[ 1 ][ 1 ] * b.m[ 1 ][ 1 ] + a.m[ 1 ][ 2 ] * b.m[ 2 ][ 1 ] + a.m[ 1 ][ 3 ] * b.m[ 3 ][ 1 ];
	m[ 1 ][ 2 ] = a.m[ 1 ][ 0 ] * b.m[ 0 ][ 2 ] + a.m[ 1 ][ 1 ] * b.m[ 1 ][ 2 ] + a.m[ 1 ][ 2 ] * b.m[ 2 ][ 2 ] + a.m[ 1 ][ 3 ] * b.m[ 3 ][ 2 ];
	m[ 1 ][ 3 ] = a.m[ 1 ][ 0 ] * b.m[ 0 ][ 3 ] + a.m[ 1 ][ 1 ] * b.m[ 1 ][ 3 ] + a.m[ 1 ][ 2 ] * b.m[ 2 ][ 3 ] + a.m[ 1 ][ 3 ] * b.m[ 3 ][ 3 ];

	m[ 2 ][ 0 ] = a.m[ 2 ][ 0 ] * b.m[ 0 ][ 0 ] + a.m[ 2 ][ 1 ] * b.m[ 1 ][ 0 ] + a.m[ 2 ][ 2 ] * b.m[ 2 ][ 0 ] + a.m[ 2 ][ 3 ] * b.m[ 3 ][ 0 ];
	m[ 2 ][ 1 ] = a.m[ 2 ][ 0 ] * b.m[ 0 ][ 1 ] + a.m[ 2 ][ 1 ] * b.m[ 1 ][ 1 ] + a.m[ 2 ][ 2 ] * b.m[ 2 ][ 1 ] + a.m[ 2 ][ 3 ] * b.m[ 3 ][ 1 ];
	m[ 2 ][ 2 ] = a.m[ 2 ][ 0 ] * b.m[ 0 ][ 2 ] + a.m[ 2 ][ 1 ] * b.m[ 1 ][ 2 ] + a.m[ 2 ][ 2 ] * b.m[ 2 ][ 2 ] + a.m[ 2 ][ 3 ] * b.m[ 3 ][ 2 ];
	m[ 2 ][ 3 ] = a.m[ 2 ][ 0 ] * b.m[ 0 ][ 3 ] + a.m[ 2 ][ 1 ] * b.m[ 1 ][ 3 ] + a.m[ 2 ][ 2 ] * b.m[ 2 ][ 3 ] + a.m[ 2 ][ 3 ] * b.m[ 3 ][ 3 ];

	m[ 3 ][ 0 ] = a.m[ 3 ][ 0 ] * b.m[ 0 ][ 0 ] + a.m[ 3 ][ 1 ] * b.m[ 1 ][ 0 ] + a.m[ 3 ][ 2 ] * b.m[ 2 ][ 0 ] + a.m[ 3 ][ 3 ] * b.m[ 3 ][ 0 ];
	m[ 3 ][ 1 ] = a.m[ 3 ][ 0 ] * b.m[ 0 ][ 1 ] + a.m[ 3 ][ 1 ] * b.m[ 1 ][ 1 ] + a.m[ 3 ][ 2 ] * b.m[ 2 ][ 1 ] + a.m[ 3 ][ 3 ] * b.m[ 3 ][ 1 ];
	m[ 3 ][ 2 ] = a.m[ 3 ][ 0 ] * b.m[ 0 ][ 2 ] + a.m[ 3 ][ 1 ] * b.m[ 1 ][ 2 ] + a.m[ 3 ][ 2 ] * b.m[ 2 ][ 2 ] + a.m[ 3 ][ 3 ] * b.m[ 3 ][ 2 ];
	m[ 3 ][ 3 ] = a.m[ 3 ][ 0 ] * b.m[ 0 ][ 3 ] + a.m[ 3 ][ 1 ] * b.m[ 1 ][ 3 ] + a.m[ 3 ][ 2 ] * b.m[ 2 ][ 3 ] + a.m[ 3 ][ 3 ] * b.m[ 3 ][ 3 ];
}

GXVoid GXMat4::Multiply ( GXVec4 &out, const GXVec4 &v ) const
{
	out.SetX ( v.data[ 0 ] * m[ 0 ][ 0 ] + v.data[ 1 ] * m[ 1 ][ 0 ] + v.data[ 2 ] * m[ 2 ][ 0 ] + v.data[ 3 ] * m[ 3 ][ 0 ] );
	out.SetY ( v.data[ 0 ] * m[ 0 ][ 1 ] + v.data[ 1 ] * m[ 1 ][ 1 ] + v.data[ 2 ] * m[ 2 ][ 1 ] + v.data[ 3 ] * m[ 3 ][ 1 ] );
	out.SetZ ( v.data[ 0 ] * m[ 0 ][ 2 ] + v.data[ 1 ] * m[ 1 ][ 2 ] + v.data[ 2 ] * m[ 2 ][ 2 ] + v.data[ 3 ] * m[ 3 ][ 2 ] );
	out.SetW ( v.data[ 0 ] * m[ 0 ][ 3 ] + v.data[ 1 ] * m[ 1 ][ 3 ] + v.data[ 2 ] * m[ 2 ][ 3 ] + v.data[ 3 ] * m[ 3 ][ 3 ] );
}

GXVoid GXMat4::MultiplyAsNormal ( GXVec3 &out, const GXVec3 &v ) const
{
	out.SetX ( v.data[ 0 ] * m[ 0 ][ 0 ] + v.data[ 1 ] * m[ 1 ][ 0 ] + v.data[ 2 ] * m[ 2 ][ 0 ] );
	out.SetY ( v.data[ 0 ] * m[ 0 ][ 1 ] + v.data[ 1 ] * m[ 1 ][ 1 ] + v.data[ 2 ] * m[ 2 ][ 1 ] );
	out.SetZ ( v.data[ 0 ] * m[ 0 ][ 2 ] + v.data[ 1 ] * m[ 1 ][ 2 ] + v.data[ 2 ] * m[ 2 ][ 2 ] );
}

GXVoid GXMat4::MultiplyAsPoint ( GXVec3 &out, const GXVec3 &v ) const
{
	out.SetX ( v.data[ 0 ] * m[ 0 ][ 0 ] + v.data[ 1 ] * m[ 1 ][ 0 ] + v.data[ 2 ] * m[ 2 ][ 0 ] + m[ 3 ][ 0 ] );
	out.SetY ( v.data[ 0 ] * m[ 0 ][ 1 ] + v.data[ 1 ] * m[ 1 ][ 1 ] + v.data[ 2 ] * m[ 2 ][ 1 ] + m[ 3 ][ 1 ] );
	out.SetZ ( v.data[ 0 ] * m[ 0 ][ 2 ] + v.data[ 1 ] * m[ 1 ][ 2 ] + v.data[ 2 ] * m[ 2 ][ 2 ] + m[ 3 ][ 2 ] );
}

GXVoid GXMat4::GetPerspectiveParams ( GXFloat &fieldOfViewYRadiands, GXFloat &aspectRatio, GXFloat &zNear, GXFloat &zFar )
{
	fieldOfViewYRadiands = 2.0f * atanf ( 1.0f / m[ 1 ][ 1 ] );
	aspectRatio = m[ 1 ][ 1 ] / m[ 0 ][ 0 ];

	zNear = -m[ 3 ][ 2 ] / ( m[ 2 ][ 2 ] + 1.0f );
	zFar = ( zNear * m[ 3 ][ 2 ] ) / ( m[ 3 ][ 2 ] + 2.0f * zNear );
}

GXVoid GXMat4::GetOrthoParams ( GXFloat &width, GXFloat &height, GXFloat &zNear, GXFloat &zFar )
{
	width = 2.0f / m[ 0 ][ 0 ];
	height = 2.0f / m[ 1 ][ 1 ];

	GXFloat factor = 1.0f / m[ 2 ][ 2 ];

	zNear = -( 1.0f + m[ 3 ][ 2 ] ) * factor;
	zFar = ( 2.0f + zNear * m[ 2 ][ 2 ] ) * factor;
}

GXVoid GXMat4::GetRayPerspective ( GXVec3 &rayView, const GXVec2 &mouseCVV ) const
{
	rayView.SetX ( mouseCVV.GetX () / m[ 0 ][ 0 ] );
	rayView.SetY ( mouseCVV.GetY () / m[ 1 ][ 1 ] );
	rayView.SetZ ( 1.0f );
}

GXMat4& GXMat4::operator = ( const GXMat4 &matrix )
{
	memcpy ( this, &matrix, sizeof ( GXMat4 ) );
	return *this;
}

//------------------------------------------------------------------

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
	vertices = 0;

	min = GXVec3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	max = GXVec3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
}

GXVoid GXAABB::Transform ( GXAABB &bounds, const GXMat4 &transform ) const
{
	GXVec3 verticesLocal[ 8 ];
	verticesLocal[ 0 ].Init ( min.data[ 0 ], min.data[ 1 ], min.data[ 2 ] );
	verticesLocal[ 1 ].Init ( max.data[ 0 ], min.data[ 1 ], min.data[ 2 ] );
	verticesLocal[ 2 ].Init ( max.data[ 0 ], min.data[ 1 ], max.data[ 2 ] );
	verticesLocal[ 3 ].Init ( min.data[ 0 ], min.data[ 1 ], max.data[ 2 ] );

	verticesLocal[ 4 ].Init ( min.data[ 0 ], max.data[ 1 ], min.data[ 2 ] );
	verticesLocal[ 5 ].Init ( max.data[ 0 ], max.data[ 1 ], min.data[ 2 ] );
	verticesLocal[ 6 ].Init ( max.data[ 0 ], max.data[ 1 ], max.data[ 2 ] );
	verticesLocal[ 7 ].Init ( min.data[ 0 ], max.data[ 1 ], max.data[ 2 ] );

	bounds.Empty ();

	for ( GXUByte i = 0u; i < 8u; i++ )
	{
		GXVec3 vertex;
		transform.MultiplyAsPoint ( vertex, verticesLocal[ i ] );
		bounds.AddVertex ( vertex );
	}
}

GXVoid GXAABB::AddVertex ( const GXVec3 &vertex )
{
	AddVertex ( vertex.data[ 0 ], vertex.data[ 1 ], vertex.data[ 2 ] );
}

GXVoid GXAABB::AddVertex ( GXFloat x, GXFloat y, GXFloat z )
{
	if ( vertices > 1 )
	{
		if ( min.data[ 0 ] > x )
			min.data[ 0 ] = x;
		else if ( max.data[ 0 ] < x )
			max.data[ 0 ] = x;

		if ( min.data[ 1 ] > y )
			min.data[ 1 ] = y;
		else if ( max.data[ 1 ] < y )
			max.data[ 1 ] = y;

		if ( min.data[ 2 ] > z )
			min.data[ 2 ] = z;
		else if ( max.data[ 2 ] < z )
			max.data[ 2 ] = z;

		return;
	}
	else if ( vertices == 0 )
	{
		vertices++;

		min.Init ( x, y, z );
		return;
	}

	vertices++;

	if ( min.data[ 0 ] > x )
	{
		max.data[ 0 ] = min.data[ 0 ];
		min.data[ 0 ] = x;
	}
	else
		max.data[ 0 ] = x;

	if ( min.data[ 1 ] > y )
	{
		max.data[ 1 ] = min.data[ 1 ];
		min.data[ 1 ] = y;
	}
	else
		max.data[ 1 ] = y;

	if ( min.data[ 2 ]  > z )
	{
		max.data[ 2 ] = min.data[ 2 ];
		min.data[ 2 ] = z;
	}
	else
		max.data[ 2 ] = z;
}

GXBool GXAABB::IsOverlaped ( const GXAABB &other ) const
{
	if ( min.data[ 0 ] > other.max.data[ 0 ] ) return GX_FALSE;
	if ( min.data[ 1 ] > other.max.data[ 1 ] ) return GX_FALSE;
	if ( min.data[ 2 ] > other.max.data[ 2 ] ) return GX_FALSE;

	if ( other.min.data[ 0 ] > max.data[ 0 ] ) return GX_FALSE;
	if ( other.min.data[ 1 ] > max.data[ 1 ] ) return GX_FALSE;
	if ( other.min.data[ 2 ] > max.data[ 2 ] ) return GX_FALSE;

	return GX_TRUE;
}

GXBool GXAABB::IsOverlaped ( const GXVec3 &point ) const
{
	return IsOverlaped ( point.data[ 0 ], point.data[ 1 ], point.data[ 2 ] );
}

GXBool GXAABB::IsOverlaped ( GXFloat x, GXFloat y, GXFloat z ) const
{
	if ( min.data[ 0 ] > x ) return GX_FALSE;
	if ( min.data[ 1 ] > y ) return GX_FALSE;
	if ( min.data[ 2 ] > z ) return GX_FALSE;

	if ( max.data[ 0 ] < x ) return GX_FALSE;
	if ( max.data[ 1 ] < y ) return GX_FALSE;
	if ( max.data[ 2 ] < z ) return GX_FALSE;

	return GX_TRUE;
}

GXVoid GXAABB::GetCenter ( GXVec3 &center ) const
{
	center.SetX ( ( min.data[ 0 ] + max.data[ 0 ] ) * 0.5f );
	center.SetY ( ( min.data[ 1 ] + max.data[ 1 ] ) * 0.5f );
	center.SetZ ( ( min.data[ 2 ] + max.data[ 2 ] ) * 0.5f );
}

GXFloat GXAABB::GetWidth () const
{
	if ( vertices < 2 )
		return -1.0f;

	return max.data[ 0 ] - min.data[ 0 ];
}

GXFloat GXAABB::GetHeight () const
{
	if ( vertices < 2 )
		return -1.0f;

	return max.data[ 1 ] - min.data[ 1 ];
}

GXFloat GXAABB::GetDepth () const
{
	if ( vertices < 2 )
		return -1.0f;

	return max.data[ 2 ] - min.data[ 2 ];
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

	this->a = normal.data[ 0 ];
	this->b = normal.data[ 1 ];
	this->c = normal.data[ 2 ];
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

	a = normal.data[ 0 ];
	b = normal.data[ 1 ];
	c = normal.data[ 3 ];
	d = -a * lineStart.data[ 0 ] - b * lineStart.data[ 1 ] - c * lineStart.data[ 2 ];

	if ( ClassifyVertex ( point ) == eGXPlaneClassifyVertex::Behind )
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
	return ClassifyVertex ( vertex.data[ 0 ], vertex.data[ 1 ], vertex.data[ 2 ] );
}

eGXPlaneClassifyVertex GXPlane::ClassifyVertex ( GXFloat x, GXFloat y, GXFloat z ) const
{
	GXFloat test = a * x + b * y + c * z + d;

	if ( test < 0.0f ) return eGXPlaneClassifyVertex::Behind;
	if ( test > 0.0f ) return eGXPlaneClassifyVertex::InFront;
	return eGXPlaneClassifyVertex::On;
}

GXPlane& GXPlane::operator = ( const GXPlane &other )
{
	memcpy ( this, &other, sizeof ( GXPlane ) );
	return *this;
}

//------------------------------------------------------------------

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
	planes[ 0 ].a = src.m[ 0 ][ 3 ] + src.m[ 0 ][ 0 ];
	planes[ 0 ].b = src.m[ 1 ][ 3 ] + src.m[ 1 ][ 0 ];
	planes[ 0 ].c = src.m[ 2 ][ 3 ] + src.m[ 2 ][ 0 ];
	planes[ 0 ].d = src.m[ 3 ][ 3 ] + src.m[ 3 ][ 0 ];
	
	// Right clipping plane
	planes[ 1 ].a = src.m[ 0 ][ 3 ] - src.m[ 0 ][ 0 ];
	planes[ 1 ].b = src.m[ 1 ][ 3 ] - src.m[ 1 ][ 0 ];
	planes[ 1 ].c = src.m[ 2 ][ 3 ] - src.m[ 2 ][ 0 ];
	planes[ 1 ].d = src.m[ 3 ][ 3 ] - src.m[ 3 ][ 0 ];
	
	// Top clipping plane
	planes[ 2 ].a = src.m[ 0 ][ 3 ] - src.m[ 0 ][ 1 ];
	planes[ 2 ].b = src.m[ 1 ][ 3 ] - src.m[ 1 ][ 1 ];
	planes[ 2 ].c = src.m[ 2 ][ 3 ] - src.m[ 2 ][ 1 ];
	planes[ 2 ].d = src.m[ 3 ][ 3 ] - src.m[ 3 ][ 1 ];
	
	// Bottom clipping plane
	planes[ 3 ].a = src.m[ 0 ][ 3 ] + src.m[ 0 ][ 1 ];
	planes[ 3 ].b = src.m[ 1 ][ 3 ] + src.m[ 1 ][ 1 ];
	planes[ 3 ].c = src.m[ 2 ][ 3 ] + src.m[ 2 ][ 1 ];
	planes[ 3 ].d = src.m[ 3 ][ 3 ] + src.m[ 3 ][ 1 ];
	
	// Near clipping plane
	planes[ 4 ].a = src.m[ 0 ][ 3 ] + src.m[ 0 ][ 2 ];
	planes[ 4 ].b = src.m[ 1 ][ 3 ] + src.m[ 1 ][ 2 ];
	planes[ 4 ].c = src.m[ 2 ][ 3 ] + src.m[ 2 ][ 2 ];
	planes[ 4 ].d = src.m[ 3 ][ 3 ] + src.m[ 3 ][ 2 ];
	
	// Far clipping plane
	planes[ 5 ].a = src.m[ 0 ][ 3 ] - src.m[ 0 ][ 2 ];
	planes[ 5 ].b = src.m[ 1 ][ 3 ] - src.m[ 1 ][ 2 ];
	planes[ 5 ].c = src.m[ 2 ][ 3 ] - src.m[ 2 ][ 2 ];
	planes[ 5 ].d = src.m[ 3 ][ 3 ] - src.m[ 3 ][ 2 ];
}

GXBool GXProjectionClipPlanes::IsVisible ( const GXAABB &bounds )
{
	GXInt flags = (GXInt)PlaneTest ( bounds.min.data[ 0 ], bounds.min.data[ 1 ], bounds.min.data[ 2 ] );
	flags &= (GXInt)PlaneTest ( bounds.min.data[ 0 ], bounds.max.data[ 1 ], bounds.min.data[ 2 ] );
	flags &= (GXInt)PlaneTest ( bounds.max.data[ 0 ], bounds.max.data[ 1 ], bounds.min.data[ 2 ] );
	flags &= (GXInt)PlaneTest ( bounds.max.data[ 0 ], bounds.min.data[ 1 ], bounds.min.data[ 2 ] );

	flags &= (GXInt)PlaneTest ( bounds.min.data[ 0 ], bounds.min.data[ 1 ], bounds.max.data[ 2 ] );
	flags &= (GXInt)PlaneTest ( bounds.min.data[ 0 ], bounds.max.data[ 1 ], bounds.max.data[ 2 ] );
	flags &= (GXInt)PlaneTest ( bounds.max.data[ 0 ], bounds.max.data[ 1 ], bounds.max.data[ 2 ] );
	flags &= (GXInt)PlaneTest ( bounds.max.data[ 0 ], bounds.min.data[ 1 ], bounds.max.data[ 2 ] );

	return ( flags > 0 ) ? GX_FALSE : GX_TRUE;
}

GXProjectionClipPlanes& GXProjectionClipPlanes::operator = ( const GXProjectionClipPlanes &clipPlanes )
{
	memcpy ( this, &clipPlanes, sizeof ( GXProjectionClipPlanes ) );
	return *this;
}

GXUByte GXProjectionClipPlanes::PlaneTest ( GXFloat x, GXFloat y, GXFloat z )
{
	GXUByte flags = 0;

	for ( GXUByte i = 0; i < 6; i++ )
	{
		if ( planes[ i ].ClassifyVertex ( x, y, z ) == eGXPlaneClassifyVertex::Behind )
			flags |= (GXUByte)( 1 << i );
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
	srand ( (GXUInt)time ( 0 ) );
}

GXFloat GXCALL GXRandomNormalize ()
{
	return (GXFloat)rand () * INVERSE_RAND_MAX;
}

GXFloat GXCALL GXRandomBetween ( GXFloat from, GXFloat to )
{
	GXFloat delta = to - from;
	return from + delta * GXRandomNormalize ();
}

GXVoid GXCALL GXRandomBetween ( GXVec3 &out, const GXVec3 &from, const GXVec3 &to )
{
	out.data[ 0 ] = GXRandomBetween ( from.data[ 0 ], to.data[ 0 ] );
	out.data[ 1 ] = GXRandomBetween ( from.data[ 1 ], to.data[ 1 ] );
	out.data[ 2 ] = GXRandomBetween ( from.data[ 2 ], to.data[ 2 ] );
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
		case 0:
		{
			v0 = (const GXVec3*)vertices;
			v1 = (const GXVec3*)( vertices + vertexStride );
			v2 = (const GXVec3*)( vertices + 2 * vertexStride );

			uv0 = (const GXVec2*)uvs;
			uv1 = (const GXVec2*)( uvs + uvStride );
			uv2 = (const GXVec2*)( uvs + 2 * uvStride );
		}
		break;

		case 1:
		{
			v0 = (const GXVec3*)( vertices + vertexStride );
			v1 = (const GXVec3*)( vertices + 2 * vertexStride );
			v2 = (const GXVec3*)vertices;

			uv0 = (const GXVec2*)( uvs + uvStride );
			uv1 = (const GXVec2*)( uvs + 2 * uvStride );
			uv2 = (const GXVec2*)uvs;
		}
		break;

		case 2:
		default:
		{
			v0 = (const GXVec3*)( vertices + 2 * vertexStride );
			v1 = (const GXVec3*)vertices;
			v2 = (const GXVec3*)( vertices + vertexStride );

			uv0 = (const GXVec2*)( uvs + 2 * uvStride );
			uv1 = (const GXVec2*)uvs;
			uv2 = (const GXVec2*)( uvs + uvStride );
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

	GXFloat factor = 1.0f / ( dUVa.data[ 0 ] * dUVb.data[ 1 ] - dUVb.data[ 0 ] * dUVa.data[ 1 ] );

	outTangent.data[ 0 ] = factor * ( dUVb.data[ 1 ] * a.data[ 0 ] - dUVa.data[ 1 ] * b.data[ 0 ] );
	outTangent.data[ 1 ] = factor * ( dUVb.data[ 1 ] * a.data[ 1 ] - dUVa.data[ 1 ] * b.data[ 1 ] );
	outTangent.data[ 2 ] = factor * ( dUVb.data[ 1 ] * a.data[ 2 ] - dUVa.data[ 1 ] * b.data[ 2 ] );
	outTangent.Normalize ();

	outBitangent.data[ 0 ] = factor * ( -dUVb.data[ 0 ] * a.data[ 0 ] + dUVa.data[ 0 ] * b.data[ 0 ] );
	outBitangent.data[ 1 ] = factor * ( -dUVb.data[ 0 ] * a.data[ 1 ] + dUVa.data[ 0 ] * b.data[ 1 ] );
	outBitangent.data[ 2 ] = factor * ( -dUVb.data[ 0 ] * a.data[ 2 ] + dUVa.data[ 0 ] * b.data[ 2 ] );
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
	out.data[ 0 ] = 1.0f - out.data[ 1 ] - out.data[ 2 ];
}

GXVoid GXCALL GXGetRayFromViewer ( GXVec3 &origin, GXVec3 &direction, GXUShort x, GXUShort y, GXUShort viewportWidth, GXUShort viewportHeight, const GXVec3& viewerLocation, const GXMat4 &viewProjectionMatrix )
{
	GXFloat halfWidth = viewportWidth * 0.5f;
	GXFloat halfHeight = viewportHeight * 0.5f;

	GXVec4 pointCVV ( ( (GXFloat)x - halfWidth ) / halfWidth, ( (GXFloat)y - halfHeight ) / halfHeight, 1.0f, 1.0f );

	GXMat4 inverseViewProjectionMatrix;
	inverseViewProjectionMatrix.Inverse ( viewProjectionMatrix );

	GXVec4 pointWorld;
	inverseViewProjectionMatrix.Multiply ( pointWorld, pointCVV );
	GXFloat alpha = 1.0f / pointWorld.data[ 3 ];

	pointWorld.data[ 0 ] *= alpha;
	pointWorld.data[ 1 ] *= alpha;
	pointWorld.data[ 2 ] *= alpha;

	direction.Substract ( GXVec3 ( pointWorld.data[ 0 ], pointWorld.data[ 1 ], pointWorld.data[ 2 ] ), viewerLocation );
	direction.Normalize ();

	origin = viewerLocation;
}
