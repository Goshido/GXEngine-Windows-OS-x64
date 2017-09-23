//version 1.40

#ifndef GX_MATH
#define GX_MATH


#include "GXTypes.h"
#include "GXDisable3rdPartyWarnings.h"
#include <math.h>
#include <limits.h>
#include <float.h>
#include "GXRestoreWarnings.h"


#define GX_MATH_DOUBLE_PI	6.2831853f
#define GX_MATH_PI			3.1415927f
#define GX_MATH_HALF_PI		1.5707963f


struct GXVec2
{
	GXFloat		data[ 2 ];

	GXVec2 ();
	explicit GXVec2 ( GXFloat x, GXFloat y );
	GXVec2 ( const GXVec2 &other );

	GXVoid SetX ( GXFloat x );
	GXFloat GetX () const;

	GXVoid SetY ( GXFloat y );
	GXFloat GetY () const;

	GXVoid Init ( GXFloat x, GXFloat y );
	GXVoid Normalize ();

	GXVoid CalculateNormalFast ( const GXVec2 &a, const GXVec2 &b );	//No normalization
	GXVoid CalculateNormal ( const GXVec2 &a, const GXVec2 &b );

	GXVoid Sum ( const GXVec2 &a, const GXVec2 &b );
	GXVoid Sum ( const GXVec2 &a, GXFloat scale, const GXVec2 &b );
	GXVoid Substract ( const GXVec2 &a, const GXVec2 &b );
	GXVoid Multiply ( const GXVec2 &a, const GXVec2 &b );
	GXVoid Multiply ( const GXVec2 &v, GXFloat scale );

	GXFloat DotProduct ( const GXVec2 &other ) const;
	GXFloat Length () const;
	GXFloat SquaredLength () const;

	GXBool IsEqual ( const GXVec2 &other ) const;

	GXVec2& operator = ( const GXVec2 &vector );
};

//-------------------------------------------------------------

enum class eGXLineRelationship : GXUByte
{
	NoIntersection = 0,
	Intersection = 1,
	Overlap = 2
};

eGXLineRelationship GXCALL GXLineIntersection2D ( GXVec2 &intersectionPoint, const GXVec2 &a0, const GXVec2 &a1, const GXVec2 &b0, const GXVec2 &b1 );

//-------------------------------------------------------------

struct GXVec3
{
	GXFloat		data[ 3 ];

	GXVec3 ();
	explicit GXVec3 ( GXFloat x, GXFloat y, GXFloat z );
	GXVec3 ( const GXVec3 &other );

	GXVoid SetX ( GXFloat x );
	GXFloat GetX () const;

	GXVoid SetY ( GXFloat y );
	GXFloat GetY () const;

	GXVoid SetZ ( GXFloat z );
	GXFloat GetZ () const;

	GXVoid Init ( GXFloat x, GXFloat y, GXFloat z );
	GXVoid Normalize ();
	GXVoid Reverse ();

	GXVoid Sum ( const GXVec3 &a, const GXVec3 &b );
	GXVoid Sum ( const GXVec3 &a, GXFloat scale, const GXVec3 &b );
	GXVoid Substract ( const GXVec3 &a, const GXVec3 &b );
	GXVoid Multiply ( const GXVec3 &a, GXFloat scale );
	GXVoid Multiply ( const GXVec3 &a, const GXVec3 &b );

	GXFloat DotProduct ( const GXVec3 &other ) const;
	GXVoid CrossProduct ( const GXVec3 &a, const GXVec3 &b );

	GXFloat Length () const;
	GXFloat SquaredLength () const;
	GXFloat Distance ( const GXVec3 &other ) const;
	GXFloat SquaredDistance ( const GXVec3 &other ) const;

	GXVoid LinearInterpolation ( const GXVec3 &start, const GXVec3 &finish, GXFloat interpolationFactor );
	GXVoid Project ( const GXVec3 &vector, const GXVec3 &axis );
	GXBool IsEqual ( const GXVec3 &other );

	static const GXVec3& GetAbsoluteX ();
	static const GXVec3& GetAbsoluteY ();
	static const GXVec3& GetAbsoluteZ ();

	static GXVoid GXCALL MakeOrthonormalBasis ( GXVec3 &baseX, GXVec3 &adjustedY, GXVec3 &adjustedZ );	//baseX - correct direction, adjustedY - desirable, adjustedZ - calculated.

	GXVec3& operator = ( const GXVec3 &vector );
};

//-------------------------------------------------------------

struct GXEuler
{
	GXFloat		pitchRadians;
	GXFloat		yawRadians;
	GXFloat		rollRadians;

	GXEuler ();
	explicit GXEuler ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );
	GXEuler ( const GXEuler &other );

	GXEuler& operator = ( const GXEuler &other );
};

//-------------------------------------------------------------

struct GXVec4
{
	GXFloat		data[ 4 ];

	GXVec4 ();
	explicit GXVec4 ( GXVec3& vector, GXFloat w );
	GXVec4 ( GXFloat x, GXFloat y, GXFloat z, GXFloat w );

	GXVoid Init ( GXFloat x, GXFloat y, GXFloat z, GXFloat w );

	GXVoid SetX ( GXFloat x );
	GXFloat GetX () const;

	GXVoid SetY ( GXFloat y );
	GXFloat GetY () const;

	GXVoid SetZ ( GXFloat z );
	GXFloat GetZ () const;

	GXVoid SetW ( GXFloat w );
	GXFloat GetW () const;

	GXVoid Sum ( const GXVec4 &a, const GXVec4 &b );
	GXVoid Sum ( const GXVec4 &a, GXFloat scale, const GXVec4 &b );
	GXVoid Substract ( const GXVec4 &a, const GXVec4 &b );

	GXFloat DotProduct ( const GXVec4 &other ) const;

	GXFloat Length () const;
	GXFloat SquaredLength () const;

	GXVec4& operator = ( const GXVec4 &vector );
};

//-------------------------------------------------------------

struct GXColorHSV;
struct GXColorRGB
{
	GXFloat		data[ 4 ];

	GXColorRGB ();
	explicit GXColorRGB ( GXFloat red, GXFloat green, GXFloat blue, GXFloat alpha );
	explicit GXColorRGB ( GXUByte red, GXUByte green, GXUByte blue, GXFloat alpha );
	explicit GXColorRGB ( const GXColorHSV &color );
	GXColorRGB ( const GXColorRGB &other );

	GXVoid Init ( GXFloat red, GXFloat green, GXFloat blue, GXFloat alpha );

	//[0.0f +inf)
	GXVoid SetRed ( GXFloat red );
	GXFloat GetRed () const;

	//[0.0f +inf)
	GXVoid SetGreen ( GXFloat green );
	GXFloat GetGreen () const;

	//[0.0f +inf)
	GXVoid SetBlue ( GXFloat blue );
	GXFloat GetBlue () const;

	//[0.0f 1.0f]
	GXVoid SetAlpha ( GXFloat alpha );
	GXFloat GetAlpha () const;

	GXVoid From ( GXUByte red, GXUByte green, GXUByte blue, GXFloat alpha );
	GXVoid From ( const GXColorHSV &color );

	GXVoid ConvertToUByte ( GXUByte &red, GXUByte &green, GXUByte &blue, GXUByte &alpha ) const;

	GXColorRGB& operator = ( const GXColorRGB &other );
};

//-------------------------------------------------------------

struct GXColorHSV
{
	GXFloat		data[ 4 ];

	GXColorHSV ();
	explicit GXColorHSV ( GXFloat hue, GXFloat saturation, GXFloat value, GXFloat alpha );
	explicit GXColorHSV ( const GXColorRGB &color );
	GXColorHSV ( const GXColorHSV &other );

	//[0.0f 360.0f]
	GXVoid SetHue ( GXFloat hue );
	GXFloat GetHue () const;

	//[0.0f 100.0f]
	GXVoid SetSaturation ( GXFloat saturation );
	GXFloat GetSaturation () const;

	//[0.0f 100.0f]
	GXVoid SetValue ( GXFloat value );
	GXFloat GetValue () const;

	//[0.0f 100.0f]
	GXVoid SetAlpha ( GXFloat alpha );
	GXFloat GetAlpha () const;

	GXVoid From ( const GXColorRGB &color );

	GXVoid operator = ( const GXColorHSV &other );
};

//-------------------------------------------------------------

struct GXMat3;
struct GXMat4;
struct GXQuat
{
	//Stores i, j, k, real order.
	GXFloat		data[ 4 ];

	GXQuat ();
	explicit GXQuat ( GXFloat i, GXFloat j, GXFloat k, GXFloat real );
	explicit GXQuat ( const GXMat3& rotationMatrix );
	explicit GXQuat ( const GXMat4& rotationMatrix );
	GXQuat ( const GXQuat &other );

	GXVoid Init ( GXFloat i, GXFloat j, GXFloat k, GXFloat real );

	GXVoid SetI ( GXFloat i );
	GXFloat GetI () const;

	GXVoid SetJ ( GXFloat j );
	GXFloat GetJ () const;

	GXVoid SetK ( GXFloat k );
	GXFloat GetK () const;

	GXVoid SetR ( GXFloat real );
	GXFloat GetR () const;

	GXVoid Identity ();
	GXVoid Normalize ();
	GXVoid Inverse ( const GXQuat &q );
	GXVoid FromAxisAngle ( GXFloat x, GXFloat y, GXFloat z, GXFloat angle );
	GXVoid FromAxisAngle ( const GXVec3 &axis, GXFloat angle );
	GXVoid From ( const GXMat4& rotationMatrix );
	GXVoid From ( const GXMat3& rotationMatrix );
	GXVoid Rehand ();

	GXVoid Multiply ( const GXQuat &a, const GXQuat &b );
	GXVoid Multiply ( const GXQuat &q, GXFloat scale );
	GXVoid Sum ( const GXQuat &a, const GXQuat &b );
	GXVoid Sum ( const GXQuat &q, GXFloat s, const GXVec3 &v );
	GXVoid Substract ( const GXQuat &a, const GXQuat &b );

	GXVoid SphericalLinearInterpolation ( const GXQuat &start, const GXQuat &finish, GXFloat interpolationFactor );
	
	GXVoid GetAxisAngle ( GXVec3 &axis, GXFloat &angle ) const;
	GXVoid Transform ( GXVec3 &out, const GXVec3 &v ) const;
	GXVoid TransformTest ( GXVec3 &out, const GXVec3 &v ) const;

	GXQuat& operator = ( const GXVec4 &other );
};

//-------------------------------------------------------------

struct GXMat3
{
	union
	{
		GXFloat data[ 9 ];
		GXFloat m[ 3 ][ 3 ];
	};

	GXMat3 ();
	explicit GXMat3 ( const GXMat4& matrix );
	GXMat3 ( const GXMat3 &other );

	GXVoid From ( const GXQuat &quaternion );
	GXVoid FromTest ( const GXQuat &quaternion );
	GXVoid From ( const GXMat4 &matrix );

	GXVoid GetX ( GXVec3& x ) const;
	GXVoid SetX ( const GXVec3& x );

	GXVoid GetY ( GXVec3& y ) const;
	GXVoid SetY ( const GXVec3& y );

	GXVoid GetZ ( GXVec3& z ) const;
	GXVoid SetZ ( const GXVec3& z );

	GXVoid Identity ();
	GXVoid Zeros ();

	GXVoid Inverse ( const GXMat3 &src );
	GXVoid Transponse ( const GXMat3 &src );

	GXVoid SkewSymmetric ( const GXVec3 &base );

	GXVoid Sum ( const GXMat3 &a, const GXMat3 &b );
	GXVoid Substract ( const GXMat3 &a, const GXMat3 &b );
	GXVoid Multiply ( const GXMat3 &a, const GXMat3 &b );
	GXVoid Multiply ( GXVec3 &out, const GXVec3 &v ) const;
	GXVoid Multiply ( const GXMat3 &a, GXFloat factor );

	GXMat3& operator = ( const GXMat3 &matrix );
};

//-------------------------------------------------------------

struct GXMat4
{
	union
	{
		GXFloat data[ 16 ];
		GXFloat m[ 4 ][ 4 ];
	};

	GXMat4 ();
	GXMat4 ( const GXMat4 &other );

	GXVoid SetRotation ( const GXQuat &quaternion );
	GXVoid SetOrigin ( const GXVec3 &origin );
	GXVoid From ( const GXQuat &quaternion, const GXVec3 &origin );
	GXVoid From ( const GXMat3 &rotation, const GXVec3 &origin );

	GXVoid SetX ( const GXVec3& x );
	GXVoid GetX ( GXVec3& x ) const;

	GXVoid SetY ( const GXVec3& y );
	GXVoid GetY ( GXVec3& y ) const;
	
	GXVoid SetZ ( const GXVec3& z );
	GXVoid GetZ ( GXVec3& z ) const;
	
	GXVoid SetW ( const GXVec3& w );
	GXVoid GetW ( GXVec3& w ) const;

	GXVoid Identity ();
	GXVoid Perspective ( GXFloat fieldOfViewYRadiands, GXFloat aspectRatio, GXFloat zNear, GXFloat zFar );
	GXVoid Ortho ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar );

	GXVoid Translation ( GXFloat x, GXFloat y, GXFloat z );
	GXVoid TranslateTo ( GXFloat x, GXFloat y, GXFloat z );
	GXVoid TranslateTo ( const GXVec3 &location );

	GXVoid RotationX ( GXFloat angle );
	GXVoid RotationY ( GXFloat angle );
	GXVoid RotationZ ( GXFloat angle );
	GXVoid RotationXY ( GXFloat pitchRadians, GXFloat yawRadians );
	GXVoid RotationXYZ ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );
	GXVoid ClearRotation ( const GXMat4 &matrix );

	GXVoid Scale ( GXFloat x, GXFloat y, GXFloat z );

	GXVoid Inverse ( const GXMat4& src );

	GXVoid Multiply ( const GXMat4& a, const GXMat4& b );
	GXVoid Multiply ( GXVec4 &out, const GXVec4 &v ) const;
	GXVoid MultiplyAsNormal ( GXVec3 &out, const GXVec3 &v ) const;
	GXVoid MultiplyAsPoint ( GXVec3 &out, const GXVec3 &v ) const;

	GXVoid GetPerspectiveParams ( GXFloat &fieldOfViewYRadiands, GXFloat &aspectRatio, GXFloat &zNear, GXFloat &zFar );
	GXVoid GetOrthoParams ( GXFloat &width, GXFloat &height, GXFloat &zNear, GXFloat &zFar );

	GXVoid GetRayPerspective ( GXVec3 &rayView, const GXVec2 &mouseCVV ) const;

	GXMat4& operator = ( const GXMat4& other );
};

//-------------------------------------------------------------

struct GXAABB
{
	GXUByte	vertices;

	GXVec3	min;
	GXVec3	max;

	GXAABB ();
	GXAABB ( const GXAABB &other );

	GXVoid Empty ();

	GXVoid Transform ( GXAABB &bounds, const GXMat4 &transform ) const;
	GXVoid AddVertex ( const GXVec3 &vertex );
	GXVoid AddVertex ( GXFloat x, GXFloat y, GXFloat z );

	GXBool IsOverlaped ( const GXAABB &other ) const;
	GXBool IsOverlaped ( const GXVec3 &point ) const;
	GXBool IsOverlaped ( GXFloat x, GXFloat y, GXFloat z ) const;

	GXVoid GetCenter ( GXVec3 &center ) const;
	GXFloat GetWidth () const;
	GXFloat GetHeight () const;
	GXFloat GetDepth () const;
	GXFloat GetSphereRadius () const;

	GXAABB& operator = ( const GXAABB &other );
};

//-------------------------------------------------------------

enum class eGXPlaneClassifyVertex : GXUByte
{
	InFront = 0,
	On = 1,
	Behind = 2
};

struct GXPlane
{
	GXFloat a;
	GXFloat b;
	GXFloat c;
	GXFloat d;

	GXPlane ();
	GXPlane ( const GXPlane &other );

	GXVoid From ( const GXVec3 &pointA, const GXVec3 &pointB, const GXVec3 &pointC );
	GXVoid FromLineToPoint ( const GXVec3 &lineStart, const GXVec3 &lineEnd, const GXVec3 &point );

	GXVoid Normalize ();
	GXVoid Flip ();

	eGXPlaneClassifyVertex ClassifyVertex ( const GXVec3 &vertex ) const;
	eGXPlaneClassifyVertex ClassifyVertex ( GXFloat x, GXFloat y, GXFloat z ) const;

	GXPlane& operator = ( const GXPlane &other );
};

//-------------------------------------------------------------

class GXProjectionClipPlanes
{
	private:
		GXPlane	planes[ 6 ];

	public:
		GXProjectionClipPlanes ();
		GXProjectionClipPlanes ( const GXMat4 &src );

		GXVoid From ( const GXMat4 &src );			//Normals will be directed inside view volume
		GXBool IsVisible ( const GXAABB &bounds );	//Trivial invisibility test

		GXProjectionClipPlanes& operator = ( const GXProjectionClipPlanes &clipPlanes );

	private:
		GXUByte PlaneTest ( GXFloat x, GXFloat y, GXFloat z );
};

//-------------------------------------------------------------

GXFloat GXCALL GXDegToRad ( GXFloat degrees );
GXFloat GXCALL GXRadToDeg ( GXFloat radians );

GXVoid GXCALL GXConvert3DSMaxToGXEngine ( GXVec3 &gx_out, GXFloat max_x, GXFloat max_y, GXFloat max_z );

GXVoid GXCALL GXRandomize ();
GXFloat GXCALL GXRandomNormalize ();
GXFloat GXCALL GXRandomBetween ( GXFloat from, GXFloat to );
GXVoid GXCALL GXRandomBetween ( GXVec3 &out, const GXVec3 &from, const GXVec3 &to );

GXVoid GXCALL GXGetTangentBitangent ( GXVec3 &outTangent, GXVec3 &outBitangent, GXUByte vertexID, const GXUByte* vertices, GXUInt vertexStride, const GXUByte* uvs, GXUInt uvStride );

GXFloat GXCALL GXClampf ( GXFloat value, GXFloat minValue, GXFloat maxValue );
GXInt GXCALL GXClampi ( GXInt value, GXInt minValue, GXInt maxValue );

GXFloat GXCALL GXMinf ( GXFloat a, GXFloat b );
GXFloat GXCALL GXMaxf ( GXFloat a, GXFloat b );

GXVoid GXCALL GXGetBarycentricCoords ( GXVec3 &out, const GXVec3 &point, const GXVec3 &a, const GXVec3 &b, const GXVec3 &c );


#endif //GX_MATH
