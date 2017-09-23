//version 1.40

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
	//NOTHING
}

GXVec4::GXVec4 ( GXVec3& vector, GXFloat w )
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

	GXUByte selector = (GXUByte)( (GXInt)( correctedHue * HSVA_FACTOR ) % 6 );
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

GXQuat::GXQuat ( GXFloat i, GXFloat j, GXFloat k, GXFloat real )
{
	Init ( i, j, k, real );
}

GXQuat::GXQuat ( const GXMat3& rotationMatrix )
{
	From ( rotationMatrix );
}

GXQuat::GXQuat ( const GXMat4& rotationMatrix )
{
	From ( rotationMatrix );
}

GXQuat::GXQuat ( const GXQuat &other )
{
	memcpy ( this, &other, sizeof ( GXQuat ) );
}

GXVoid GXQuat::Init ( GXFloat i, GXFloat j, GXFloat k, GXFloat real )
{
	data[ 0 ] = i;
	data[ 1 ] = j;
	data[ 2 ] = k;
	data[ 3 ] = real;
}

GXVoid GXQuat::SetI ( GXFloat i )
{
	data[ 0 ] = i;
}

GXFloat GXQuat::GetI () const
{
	return data[ 0 ];
}

GXVoid GXQuat::SetJ ( GXFloat j )
{
	data[ 1 ] = j;
}

GXFloat GXQuat::GetJ () const
{
	return data[ 1 ];
}

GXVoid GXQuat::SetK ( GXFloat k )
{
	data[ 2 ] = k;
}

GXFloat GXQuat::GetK () const
{
	return data[ 2 ];
}

GXVoid GXQuat::SetR ( GXFloat real )
{
	data[ 3 ] = real;
}

GXFloat GXQuat::GetR () const
{
	return data[ 3 ];
}

GXVoid GXQuat::Identity ()
{
	data[ 0 ] = data[ 1 ] = data[ 2 ] = 0.0f;
	data[ 3 ] = 1.0f;
}

GXVoid GXQuat::Normalize ()
{
	GXFloat squaredLength = data[ 0 ] * data[ 0 ] + data[ 1 ] * data[ 1 ] + data[ 2 ] * data[ 2 ] + data[ 3 ] * data[ 3 ];

	if ( squaredLength == 0.0f )
	{
		GXLogW ( L"GXQuat::Normalize - Error\n" );
		return;
	}

	Multiply ( *this, 1.0f / sqrtf ( squaredLength ) );
}

GXVoid GXQuat::Inverse ( const GXQuat &q )
{
	GXFloat squaredLength = q.data[ 0 ] * q.data[ 0 ] + q.data[ 1 ] * q.data[ 1 ] + q.data[ 2 ] * q.data[ 2 ] + q.data[ 3 ] * q.data[ 3 ];

	if ( squaredLength > 0.0f )
	{
		GXFloat inverseSquaredLength = 1.0f / squaredLength;

		data[ 0 ] = -q.data[ 0 ] * inverseSquaredLength;
		data[ 1 ] = -q.data[ 1 ] * inverseSquaredLength;
		data[ 2 ] = -q.data[ 2 ] * inverseSquaredLength;
		data[ 3 ] = q.data[ 3 ] * inverseSquaredLength;

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
	GXFloat sn = sinf ( angle * 0.5f );
	GXFloat cs = cosf ( angle * 0.5f );

	data[ 0 ] = x * sn;
	data[ 1 ] = y * sn;
	data[ 2 ] = z * sn;
	data[ 3 ] = cs;
}

GXVoid GXQuat::FromAxisAngle ( const GXVec3 &axis, GXFloat angle )
{
	FromAxisAngle ( axis.GetX (), axis.GetY (), axis.GetZ (), angle );
}

GXVoid GXQuat::From ( const GXMat4& rotationMatrix )
{
	static const GXInt next[ 3 ] = { 1, 2, 0 };

	GXFloat trace = rotationMatrix.m[ 0 ][ 0 ] + rotationMatrix.m[ 1 ][ 1 ] + rotationMatrix.m[ 2 ][ 2 ];

	if ( trace > 0.0f )
	{
		GXFloat t = trace + 1.0f;
		GXFloat s = 1.0f / sqrtf ( t ) * 0.5f;

		data[ 3 ] = s * t;
		data[ 0 ] = ( rotationMatrix.m[ 2 ][ 1 ] - rotationMatrix.m[ 1 ][ 2 ] ) * s;
		data[ 1 ] = ( rotationMatrix.m[ 0 ][ 2 ] - rotationMatrix.m[ 2 ][ 0 ] ) * s;
		data[ 2 ] = ( rotationMatrix.m[ 1 ][ 0 ] - rotationMatrix.m[ 0 ][ 1 ] ) * s;

		return;
	}

	GXInt i = 0;
	if ( rotationMatrix.m[ 1 ][ 1 ] > rotationMatrix.m[ 0 ][ 0 ] )
		i = 1;

	if ( rotationMatrix.m[ 2 ][ 2 ] > rotationMatrix.m[ i ][ i ] )
		i = 2;

	GXInt j = next[ i ];
	GXInt k = next[ j ];

	GXFloat t = ( rotationMatrix.m[ i ][ i ] - ( rotationMatrix.m[ j ][ j ] + rotationMatrix.m[ k ][ k ] ) ) + 1.0f;
	GXFloat s = 1.0f / sqrtf ( t ) * 0.5f;

	data[ i ] = s * t;
	data[ 3 ] = ( rotationMatrix.m[ k ][ j ] - rotationMatrix.m[ j ][ k ] ) * s;
	data[ j ] = ( rotationMatrix.m[ j ][ i ] + rotationMatrix.m[ i ][ j ] ) * s;
	data[ k ] = ( rotationMatrix.m[ k ][ i ] + rotationMatrix.m[ i ][ k ] ) * s;
}

GXVoid GXQuat::From ( const GXMat3& rotationMatrix )
{
	static const GXInt next[ 3 ] = { 1, 2, 0 };

	GXFloat trace = rotationMatrix.m[ 0 ][ 0 ] + rotationMatrix.m[ 1 ][ 1 ] + rotationMatrix.m[ 2 ][ 2 ];

	if ( trace > 0.0f )
	{
		GXFloat t = trace + 1.0f;
		GXFloat s = 1.0f / sqrtf ( t ) * 0.5f;

		data[ 3 ] = s * t;
		data[ 0 ] = ( rotationMatrix.m[ 2 ][ 1 ] - rotationMatrix.m[ 1 ][ 2 ] ) * s;
		data[ 1 ] = ( rotationMatrix.m[ 0 ][ 2 ] - rotationMatrix.m[ 2 ][ 0 ] ) * s;
		data[ 2 ] = ( rotationMatrix.m[ 1 ][ 0 ] - rotationMatrix.m[ 0 ][ 1 ] ) * s;

		return;
	}

	GXInt i = 0;
	if ( rotationMatrix.m[ 1 ][ 1 ] > rotationMatrix.m[ 0 ][ 0 ] )
		i = 1;

	if ( rotationMatrix.m[ 2 ][ 2 ] > rotationMatrix.m[ i ][ i ] )
		i = 2;

	GXInt j = next[ i ];
	GXInt k = next[ j ];

	GXFloat t = ( rotationMatrix.m[ i ][ i ] - ( rotationMatrix.m[ j ][ j ] + rotationMatrix.m[ k ][ k ] ) ) + 1.0f;
	GXFloat s = 1.0f / sqrtf ( t ) * 0.5f;

	data[ i ] = s * t;
	data[ 3 ] = ( rotationMatrix.m[ k ][ j ] - rotationMatrix.m[ j ][ k ] ) * s;
	data[ j ] = ( rotationMatrix.m[ j ][ i ] + rotationMatrix.m[ i ][ j ] ) * s;
	data[ k ] = ( rotationMatrix.m[ k ][ i ] + rotationMatrix.m[ i ][ k ] ) * s;
}

GXVoid GXQuat::Rehand ()
{
	data[ 3 ] = -data[ 3 ];
}

GXVoid GXQuat::Multiply ( const GXQuat &a, const GXQuat &b )
{
	data[ 0 ] = a.data[ 3 ] * b.data[ 0 ] + a.data[ 0 ] * b.data[ 3 ] + a.data[ 1 ] * b.data[ 2 ] - a.data[ 2 ] * b.data[ 1 ];
	data[ 1 ] = a.data[ 3 ] * b.data[ 1 ] - a.data[ 0 ] * b.data[ 2 ] + a.data[ 1 ] * b.data[ 3 ] + a.data[ 2 ] * b.data[ 0 ];
	data[ 2 ] = a.data[ 3 ] * b.data[ 2 ] + a.data[ 0 ] * b.data[ 1 ] - a.data[ 1 ] * b.data[ 0 ] + a.data[ 2 ] * b.data[ 3 ];
	data[ 3 ] = a.data[ 3 ] * b.data[ 3 ] - a.data[ 0 ] * b.data[ 0 ] - a.data[ 1 ] * b.data[ 1 ] - a.data[ 2 ] * b.data[ 2 ];
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

GXVoid GXQuat::Sum ( const GXQuat &q, GXFloat s, const GXVec3 &v )
{
	GXQuat qs ( v.GetX () * s, v.GetY () * s, v.GetZ () * s, 0.0f );

	GXQuat q1;
	q1.Multiply ( qs, q );

	data[ 0 ] = q.data[ 0 ] + q1.data[ 0 ] * 0.5f;
	data[ 1 ] = q.data[ 1 ] + q1.data[ 1 ] * 0.5f;
	data[ 2 ] = q.data[ 2 ] + q1.data[ 2 ] * 0.5f;
	data[ 3 ] = q.data[ 3 ] + q1.data[ 3 ] * 0.5f;
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

	if ( ( 1.0f - cosom ) > 1e-6f )
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

	if ( q.data[ 3 ] > 1.0f )
		q.Normalize ();

	angle = 2.0f * acosf ( q.data[ 3 ] );

	axis.SetX ( q.data[ 0 ] );
	axis.SetY ( q.data[ 1 ] );
	axis.SetZ ( q.data[ 2 ] );

	GXFloat s = sqrtf ( 1.0f - q.data[ 3 ] * q.data[ 3 ] );

	if ( s < 0.001 ) return;

	axis.Multiply ( axis, 1.0f / s );
}

GXVoid GXQuat::Transform ( GXVec3 &out, const GXVec3 &v ) const
{
	GXFloat xxzz = data[ 0 ] * data[ 0 ] - data[ 2 ] * data[ 2 ];
	GXFloat wwyy = data[ 3 ] * data[ 3 ] - data[ 1 ] * data[ 1 ];

	GXFloat xw2 = data[ 0 ] * data[ 3 ] * 2.0f;
	GXFloat xy2 = data[ 0 ] * data[ 1 ] * 2.0f;
	GXFloat xz2 = data[ 0 ] * data[ 2 ] * 2.0f;
	GXFloat yw2 = data[ 1 ] * data[ 3 ] * 2.0f;
	GXFloat yz2 = data[ 1 ] * data[ 2 ] * 2.0f;
	GXFloat zw2 = data[ 2 ] * data[ 3 ] * 2.0f;

	out.SetX ( ( xxzz + wwyy ) * v.GetX () + ( xy2 + zw2 ) * v.GetY () + ( xz2 - yw2 ) * v.GetZ () );
	out.SetY ( ( xy2 - zw2 ) * v.GetX () + ( data[ 1 ] * data[ 1 ] + data[ 3 ] * data[ 3 ] - data[ 0 ] * data[ 0 ] - data[ 2 ] * data[ 2 ] ) * v.GetY () + ( yz2 + xw2 ) * v.GetZ () );
	out.SetZ ( ( xz2 + yw2 ) * v.GetX () + ( yz2 - xw2 ) * v.GetY () + ( wwyy - xxzz ) * v.GetZ () );
}

GXVoid GXQuat::TransformTest ( GXVec3 &out, const GXVec3 &v ) const
{
	GXQuat vQuaternion ( v.data[ 0 ], v.data[ 1 ], v.data[ 2 ], 0.0f );

	GXQuat inverseQuaternion;
	inverseQuaternion.Inverse ( *this );

	GXQuat alpha;
	alpha.Multiply ( *this, vQuaternion );

	GXQuat betta;
	betta.Multiply ( alpha, inverseQuaternion );

	out.Init ( betta.data[ 0 ], betta.data[ 1 ], betta.data[ 2 ] );
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

GXMat3::GXMat3 ( const GXMat4& matrix )
{
	From ( matrix );
}

GXMat3::GXMat3 ( const GXMat3 &other )
{
	memcpy ( this, &other, sizeof ( GXMat3 ) );
}

GXVoid GXMat3::From ( const GXQuat &quaternion )
{
	GXFloat xx = quaternion.GetI () * quaternion.GetI ();
	GXFloat xy = quaternion.GetI () * quaternion.GetJ ();
	GXFloat xz = quaternion.GetI () * quaternion.GetK ();
	GXFloat xw = quaternion.GetI () * quaternion.GetR ();

	GXFloat yy = quaternion.GetJ () * quaternion.GetJ ();
	GXFloat yz = quaternion.GetJ () * quaternion.GetK ();
	GXFloat yw = quaternion.GetJ () * quaternion.GetR ();

	GXFloat zz = quaternion.GetK () * quaternion.GetK ();
	GXFloat zw = quaternion.GetK () * quaternion.GetR ();

	m[ 0 ][ 0 ] = 1.0f - 2.0f * ( yy + zz );
	m[ 0 ][ 1 ] = 2.0f * ( xy - zw );
	m[ 0 ][ 2 ] = 2.0f * ( xz + yw );

	m[ 1 ][ 0 ] = 2.0f * ( xy + zw );
	m[ 1 ][ 1 ] = 1.0f - 2.0f * ( xx + zz );
	m[ 1 ][ 2 ] = 2.0f * ( yz - xw );

	m[ 2 ][ 0 ] = 2.0f * ( xz - yw );
	m[ 2 ][ 1 ] = 2.0f * ( yz + xw );
	m[ 2 ][ 2 ] = 1.0f - 2.0f * ( xx + yy );
}

GXVoid GXMat3::FromTest ( const GXQuat &quaternion )
{
	GXVec3 x;
	quaternion.TransformTest ( x, GXVec3::GetAbsoluteX () );

	GXVec3 y;
	quaternion.TransformTest ( y, GXVec3::GetAbsoluteY () );

	GXVec3 z;
	quaternion.TransformTest ( z, GXVec3::GetAbsoluteZ () );

	SetX ( x );
	SetY ( y );
	SetZ ( z );
}

GXVoid GXMat3::From ( const GXMat4 &matrix )
{
	GXUPointer lineSize = 3 * sizeof ( GXFloat );

	memcpy ( data, matrix.data, lineSize );
	memcpy ( data + 3, matrix.data + 4, lineSize );
	memcpy ( data + 6, matrix.data + 8, lineSize );
}

GXVoid GXMat3::GetX ( GXVec3& x ) const
{
	memcpy ( &x, data, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::SetX ( const GXVec3& x )
{
	memcpy ( data, &x, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::GetY ( GXVec3& y ) const
{
	memcpy ( &y, data + 3, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::SetY ( const GXVec3& y )
{
	memcpy ( data + 3, &y, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::GetZ ( GXVec3& z ) const
{
	memcpy ( &z, data + 6, sizeof ( GXVec3 ) );
}

GXVoid GXMat3::SetZ ( const GXVec3& z )
{
	memcpy ( data + 6, &z, sizeof ( GXVec3 ) );
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

GXVoid GXMat3::Inverse ( const GXMat3 &src )
{
	GXFloat determinant = src.m[ 0 ][ 0 ] * ( src.m[ 1 ][ 1 ] * src.m[ 2 ][ 2 ] - src.m[ 2 ][ 1 ] * src.m[ 1 ][ 2 ] );
	determinant -= src.m[ 0 ][ 1 ] * ( src.m[ 1 ][ 0 ] * src.m[ 2 ][ 2 ] - src.m[ 2 ][ 0 ] * src.m[ 1 ][ 2 ] );
	determinant += src.m[ 0 ][ 2 ] * ( src.m[ 1 ][ 0 ] * src.m[ 2 ][ 1 ] - src.m[ 2 ][ 0 ] * src.m[ 1 ][ 1 ] );

	GXFloat invDeterminant = 1.0f / determinant;

	m[ 0 ][ 0 ] = invDeterminant * ( src.m[ 1 ][ 1 ] * src.m[ 2 ][ 2 ] - src.m[ 2 ][ 1 ] * src.m[ 1 ][ 2 ] );
	m[ 0 ][ 1 ] = invDeterminant * ( src.m[ 0 ][ 2 ] * src.m[ 2 ][ 1 ] - src.m[ 2 ][ 2 ] * src.m[ 0 ][ 1 ] );
	m[ 0 ][ 2 ] = invDeterminant * ( src.m[ 0 ][ 1 ] * src.m[ 1 ][ 2 ] - src.m[ 1 ][ 1 ] * src.m[ 0 ][ 2 ] );

	m[ 1 ][ 0 ] = invDeterminant * ( src.m[ 1 ][ 2 ] * src.m[ 2 ][ 0 ] - src.m[ 2 ][ 2 ] * src.m[ 1 ][ 0 ] );
	m[ 1 ][ 1 ] = invDeterminant * ( src.m[ 0 ][ 0 ] * src.m[ 2 ][ 2 ] - src.m[ 2 ][ 0 ] * src.m[ 0 ][ 2 ] );
	m[ 1 ][ 2 ] = invDeterminant * ( src.m[ 0 ][ 2 ] * src.m[ 1 ][ 0 ] - src.m[ 1 ][ 2 ] * src.m[ 0 ][ 0 ] );

	m[ 2 ][ 0 ] = invDeterminant * ( src.m[ 1 ][ 0 ] * src.m[ 2 ][ 1 ] - src.m[ 2 ][ 0 ] * src.m[ 1 ][ 1 ] );
	m[ 2 ][ 1 ] = invDeterminant * ( src.m[ 0 ][ 1 ] * src.m[ 2 ][ 0 ] - src.m[ 2 ][ 1 ] * src.m[ 0 ][ 0 ] );
	m[ 2 ][ 2 ] = invDeterminant * ( src.m[ 0 ][ 0 ] * src.m[ 1 ][ 1 ] - src.m[ 1 ][ 0 ] * src.m[ 0 ][ 1 ] );
}

GXVoid GXMat3::Transponse ( const GXMat3 &src )
{
	m[ 0 ][ 0 ] = src.m[ 0 ][ 0 ];
	m[ 0 ][ 1 ] = src.m[ 1 ][ 0 ];
	m[ 0 ][ 2 ] = src.m[ 2 ][ 0 ];

	m[ 1 ][ 0 ] = src.m[ 0 ][ 1 ];
	m[ 1 ][ 1 ] = src.m[ 1 ][ 1 ];
	m[ 1 ][ 2 ] = src.m[ 2 ][ 1 ];

	m[ 2 ][ 0 ] = src.m[ 0 ][ 2 ];
	m[ 2 ][ 1 ] = src.m[ 1 ][ 2 ];
	m[ 2 ][ 2 ] = src.m[ 2 ][ 2 ];
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

GXVoid GXMat3::Multiply ( GXVec3 &out, const GXVec3 &v ) const
{
	out.SetX ( v.data[ 0 ] * m[ 0 ][ 0 ] + v.data[ 1 ] * m[ 1 ][ 0 ] + v.data[ 2 ] * m[ 2 ][ 0 ] );
	out.SetY ( v.data[ 0 ] * m[ 0 ][ 1 ] + v.data[ 1 ] * m[ 1 ][ 1 ] + v.data[ 2 ] * m[ 2 ][ 1 ] );
	out.SetZ ( v.data[ 0 ] * m[ 0 ][ 2 ] + v.data[ 1 ] * m[ 1 ][ 2 ] + v.data[ 2 ] * m[ 2 ][ 2 ] );
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

GXVoid GXMat4::SetRotation ( const GXQuat& quaternion )
{
	GXFloat ii = quaternion.GetI () * quaternion.GetI ();
	GXFloat ij = quaternion.GetI () * quaternion.GetJ ();
	GXFloat ik = quaternion.GetI () * quaternion.GetK ();
	GXFloat ir = quaternion.GetI () * quaternion.GetR ();

	GXFloat jj = quaternion.GetJ () * quaternion.GetJ ();
	GXFloat jk = quaternion.GetJ () * quaternion.GetK ();
	GXFloat jr = quaternion.GetJ () * quaternion.GetR ();

	GXFloat kk = quaternion.GetK () * quaternion.GetK ();
	GXFloat kr = quaternion.GetK () * quaternion.GetR ();

	m[ 0 ][ 0 ] = 1.0f - 2.0f * ( jj + kk );
	m[ 0 ][ 1 ] = 2.0f * ( ij - kr );
	m[ 0 ][ 2 ] = 2.0f * ( ik + jr );

	m[ 1 ][ 0 ] = 2.0f * ( ij + kr );
	m[ 1 ][ 1 ] = 1.0f - 2.0f * ( ii + kk );
	m[ 1 ][ 2 ] = 2.0f * ( jk - ir );

	m[ 2 ][ 0 ] = 2.0f * ( ik - jr );
	m[ 2 ][ 1 ] = 2.0f * ( jk + ir );
	m[ 2 ][ 2 ] = 1.0f - 2.0f * ( ii + jj );
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

GXVoid GXMat4::SetX ( const GXVec3& x )
{
	memcpy ( data, &x, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetX ( GXVec3& x ) const
{
	memcpy ( &x, data, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::SetY ( const GXVec3& y )
{
	memcpy ( data + 4, &y, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetY ( GXVec3& y ) const
{
	memcpy ( &y, data + 4, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::SetZ ( const GXVec3& z )
{
	memcpy ( data + 8, &z, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetZ ( GXVec3& z ) const
{
	memcpy ( &z, data + 8, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::SetW ( const GXVec3& w )
{
	memcpy ( data + 12, &w, sizeof ( GXVec3 ) );
}

GXVoid GXMat4::GetW ( GXVec3& w ) const
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

GXVoid GXMat4::ClearRotation ( const GXMat4 &matrix )
{
	GXVec3 outX;
	GXVec3 outY;
	GXVec3 outZ;

	GXVec3 modelX;
	GXVec3 modelY;
	GXVec3 modelZ;

	matrix.GetX ( modelX );
	matrix.GetY ( modelY );
	matrix.GetZ ( modelZ );

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

GXVoid GXMat4::Inverse ( const GXMat4& src )
{
	// 2x2 sub-determinants required to calculate 4x4 determinant
	GXFloat det2_01_01 = src.m[ 0 ][ 0 ] * src.m[ 1 ][ 1 ] - src.m[ 1 ][ 0 ] * src.m[ 0 ][ 1 ];
	GXFloat det2_01_02 = src.m[ 0 ][ 0 ] * src.m[ 2 ][ 1 ] - src.m[ 2 ][ 0 ] * src.m[ 0 ][ 1 ];
	GXFloat det2_01_03 = src.m[ 0 ][ 0 ] * src.m[ 3 ][ 1 ] - src.m[ 3 ][ 0 ] * src.m[ 0 ][ 1 ];
	GXFloat det2_01_12 = src.m[ 1 ][ 0 ] * src.m[ 2 ][ 1 ] - src.m[ 2 ][ 0 ] * src.m[ 1 ][ 1 ];
	GXFloat det2_01_13 = src.m[ 1 ][ 0 ] * src.m[ 3 ][ 1 ] - src.m[ 3 ][ 0 ] * src.m[ 1 ][ 1 ];
	GXFloat det2_01_23 = src.m[ 2 ][ 0 ] * src.m[ 3 ][ 1 ] - src.m[ 3 ][ 0 ] * src.m[ 2 ][ 1 ];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	GXFloat det3_201_012 = src.m[ 0 ][ 2 ] * det2_01_12 - src.m[ 1 ][ 2 ] * det2_01_02 + src.m[ 2 ][ 2 ] * det2_01_01;
	GXFloat det3_201_013 = src.m[ 0 ][ 2 ] * det2_01_13 - src.m[ 1 ][ 2 ] * det2_01_03 + src.m[ 3 ][ 2 ] * det2_01_01;
	GXFloat det3_201_023 = src.m[ 0 ][ 2 ] * det2_01_23 - src.m[ 2 ][ 2 ] * det2_01_03 + src.m[ 3 ][ 2 ] * det2_01_02;
	GXFloat det3_201_123 = src.m[ 1 ][ 2 ] * det2_01_23 - src.m[ 2 ][ 2 ] * det2_01_13 + src.m[ 3 ][ 2 ] * det2_01_12;

	GXFloat inverseDeterminant = 1.0f / ( -det3_201_123 * src.m[ 0 ][ 3 ] + det3_201_023 * src.m[ 1 ][ 3 ] - det3_201_013 * src.m[ 2 ][ 3 ] + det3_201_012 * src.m[ 3 ][ 3 ] );

	// remaining 2x2 sub-determinants
	GXFloat det2_03_01 = src.m[ 0 ][ 0 ] * src.m[ 1 ][ 3 ] - src.m[ 1 ][ 0 ] * src.m[ 0 ][ 3 ];
	GXFloat det2_03_02 = src.m[ 0 ][ 0 ] * src.m[ 2 ][ 3 ] - src.m[ 2 ][ 0 ] * src.m[ 0 ][ 3 ];
	GXFloat det2_03_03 = src.m[ 0 ][ 0 ] * src.m[ 3 ][ 3 ] - src.m[ 3 ][ 0 ] * src.m[ 0 ][ 3 ];
	GXFloat det2_03_12 = src.m[ 1 ][ 0 ] * src.m[ 2 ][ 3 ] - src.m[ 2 ][ 0 ] * src.m[ 1 ][ 3 ];
	GXFloat det2_03_13 = src.m[ 1 ][ 0 ] * src.m[ 3 ][ 3 ] - src.m[ 3 ][ 0 ] * src.m[ 1 ][ 3 ];
	GXFloat det2_03_23 = src.m[ 2 ][ 0 ] * src.m[ 3 ][ 3 ] - src.m[ 3 ][ 0 ] * src.m[ 2 ][ 3 ];

	GXFloat det2_13_01 = src.m[ 0 ][ 1 ] * src.m[ 1 ][ 3 ] - src.m[ 1 ][ 1 ] * src.m[ 0 ][ 3 ];
	GXFloat det2_13_02 = src.m[ 0 ][ 1 ] * src.m[ 2 ][ 3 ] - src.m[ 2 ][ 1 ] * src.m[ 0 ][ 3 ];
	GXFloat det2_13_03 = src.m[ 0 ][ 1 ] * src.m[ 3 ][ 3 ] - src.m[ 3 ][ 1 ] * src.m[ 0 ][ 3 ];
	GXFloat det2_13_12 = src.m[ 1 ][ 1 ] * src.m[ 2 ][ 3 ] - src.m[ 2 ][ 1 ] * src.m[ 1 ][ 3 ];
	GXFloat det2_13_13 = src.m[ 1 ][ 1 ] * src.m[ 3 ][ 3 ] - src.m[ 3 ][ 1 ] * src.m[ 1 ][ 3 ];
	GXFloat det2_13_23 = src.m[ 2 ][ 1 ] * src.m[ 3 ][ 3 ] - src.m[ 3 ][ 1 ] * src.m[ 2 ][ 3 ];

	// remaining 3x3 sub-determinants
	GXFloat det3_203_012 = src.m[ 0 ][ 2 ] * det2_03_12 - src.m[ 1 ][ 2 ] * det2_03_02 + src.m[ 2 ][ 2 ] * det2_03_01;
	GXFloat det3_203_013 = src.m[ 0 ][ 2 ] * det2_03_13 - src.m[ 1 ][ 2 ] * det2_03_03 + src.m[ 3 ][ 2 ] * det2_03_01;
	GXFloat det3_203_023 = src.m[ 0 ][ 2 ] * det2_03_23 - src.m[ 2 ][ 2 ] * det2_03_03 + src.m[ 3 ][ 2 ] * det2_03_02;
	GXFloat det3_203_123 = src.m[ 1 ][ 2 ] * det2_03_23 - src.m[ 2 ][ 2 ] * det2_03_13 + src.m[ 3 ][ 2 ] * det2_03_12;

	GXFloat det3_213_012 = src.m[ 0 ][ 2 ] * det2_13_12 - src.m[ 1 ][ 2 ] * det2_13_02 + src.m[ 2 ][ 2 ] * det2_13_01;
	GXFloat det3_213_013 = src.m[ 0 ][ 2 ] * det2_13_13 - src.m[ 1 ][ 2 ] * det2_13_03 + src.m[ 3 ][ 2 ] * det2_13_01;
	GXFloat det3_213_023 = src.m[ 0 ][ 2 ] * det2_13_23 - src.m[ 2 ][ 2 ] * det2_13_03 + src.m[ 3 ][ 2 ] * det2_13_02;
	GXFloat det3_213_123 = src.m[ 1 ][ 2 ] * det2_13_23 - src.m[ 2 ][ 2 ] * det2_13_13 + src.m[ 3 ][ 2 ] * det2_13_12;

	GXFloat det3_301_012 = src.m[ 0 ][ 3 ] * det2_01_12 - src.m[ 1 ][ 3 ] * det2_01_02 + src.m[ 2 ][ 3 ] * det2_01_01;
	GXFloat det3_301_013 = src.m[ 0 ][ 3 ] * det2_01_13 - src.m[ 1 ][ 3 ] * det2_01_03 + src.m[ 3 ][ 3 ] * det2_01_01;
	GXFloat det3_301_023 = src.m[ 0 ][ 3 ] * det2_01_23 - src.m[ 2 ][ 3 ] * det2_01_03 + src.m[ 3 ][ 3 ] * det2_01_02;
	GXFloat det3_301_123 = src.m[ 1 ][ 3 ] * det2_01_23 - src.m[ 2 ][ 3 ] * det2_01_13 + src.m[ 3 ][ 3 ] * det2_01_12;

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

GXVoid GXMat4::Multiply ( const GXMat4& a, const GXMat4& b )
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

GXMat4& GXMat4::operator = ( const GXMat4& matrix )
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
	verticesLocal[ 0 ].Init ( bounds.min.data[ 0 ], bounds.min.data[ 1 ], bounds.min.data[ 2 ] );
	verticesLocal[ 1 ].Init ( bounds.max.data[ 0 ], bounds.min.data[ 1 ], bounds.min.data[ 2 ] );
	verticesLocal[ 2 ].Init ( bounds.max.data[ 0 ], bounds.min.data[ 1 ], bounds.max.data[ 2 ] );
	verticesLocal[ 3 ].Init ( bounds.min.data[ 0 ], bounds.min.data[ 1 ], bounds.max.data[ 2 ] );

	verticesLocal[ 4 ].Init ( bounds.min.data[ 0 ], bounds.max.data[ 1 ], bounds.min.data[ 2 ] );
	verticesLocal[ 5 ].Init ( bounds.max.data[ 0 ], bounds.max.data[ 1 ], bounds.min.data[ 2 ] );
	verticesLocal[ 6 ].Init ( bounds.max.data[ 0 ], bounds.max.data[ 1 ], bounds.max.data[ 2 ] );
	verticesLocal[ 7 ].Init ( bounds.min.data[ 0 ], bounds.max.data[ 1 ], bounds.max.data[ 2 ] );

	bounds.Empty ();

	for ( GXUByte i = 0; i < 8; i++ )
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
	//Left clipping plane
	planes[ 0 ].a = src.m[ 0 ][ 3 ] + src.m[ 0 ][ 0 ];
	planes[ 0 ].b = src.m[ 1 ][ 3 ] + src.m[ 1 ][ 0 ];
	planes[ 0 ].c = src.m[ 2 ][ 3 ] + src.m[ 2 ][ 0 ];
	planes[ 0 ].d = src.m[ 3 ][ 3 ] + src.m[ 3 ][ 0 ];
	
	//Right clipping plane
	planes[ 1 ].a = src.m[ 0 ][ 3 ] - src.m[ 0 ][ 0 ];
	planes[ 1 ].b = src.m[ 1 ][ 3 ] - src.m[ 1 ][ 0 ];
	planes[ 1 ].c = src.m[ 2 ][ 3 ] - src.m[ 2 ][ 0 ];
	planes[ 1 ].d = src.m[ 3 ][ 3 ] - src.m[ 3 ][ 0 ];
	
	//Top clipping plane
	planes[ 2 ].a = src.m[ 0 ][ 3 ] - src.m[ 0 ][ 1 ];
	planes[ 2 ].b = src.m[ 1 ][ 3 ] - src.m[ 1 ][ 1 ];
	planes[ 2 ].c = src.m[ 2 ][ 3 ] - src.m[ 2 ][ 1 ];
	planes[ 2 ].d = src.m[ 3 ][ 3 ] - src.m[ 3 ][ 1 ];
	
	//Bottom clipping plane
	planes[ 3 ].a = src.m[ 0 ][ 3 ] + src.m[ 0 ][ 1 ];
	planes[ 3 ].b = src.m[ 1 ][ 3 ] + src.m[ 1 ][ 1 ];
	planes[ 3 ].c = src.m[ 2 ][ 3 ] + src.m[ 2 ][ 1 ];
	planes[ 3 ].d = src.m[ 3 ][ 3 ] + src.m[ 3 ][ 1 ];
	
	//Near clipping plane
	planes[ 4 ].a = src.m[ 0 ][ 3 ] + src.m[ 0 ][ 2 ];
	planes[ 4 ].b = src.m[ 1 ][ 3 ] + src.m[ 1 ][ 2 ];
	planes[ 4 ].c = src.m[ 2 ][ 3 ] + src.m[ 2 ][ 2 ];
	planes[ 4 ].d = src.m[ 3 ][ 3 ] + src.m[ 3 ][ 2 ];
	
	//Far clipping plane
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
	#define INV_RAND_MAX		3.05185e-5f;

	return (GXFloat)rand () * INV_RAND_MAX;

	#undef INV_RAND_MAX
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

GXVoid GXCALL GXGetTangentBitangent ( GXVec3 &outTangent, GXVec3 &outBitangent, GXUByte vertexID, const GXUByte* vertices, GXUInt vertexStride, const GXUByte* uvs, GXUInt uvStride )
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
