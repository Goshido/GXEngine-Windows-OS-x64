// version 1.50

#ifndef GX_MATH
#define GX_MATH


#include "GXTypes.h"
#include "GXWarning.h"

GX_DISABLE_COMMON_WARNINGS

#include <math.h>
#include <limits.h>
#include <float.h>

GX_RESTORE_WARNING_STATE


#define GX_MATH_HALF_PI		1.5707963f
#define GX_MATH_PI			3.1415927f
#define GX_MATH_DOUBLE_PI	6.2831853f


// By convention it is row-vertex.
struct GXVec2
{
	// Stores vector components in x, y order.
	GXFloat		data[ 2 ];

	GXVec2 ();
	GXVec2 ( const GXVec2 &other );
	explicit GXVec2 ( GXFloat x, GXFloat y );

	GXVoid SetX ( GXFloat x );
	GXFloat GetX () const;

	GXVoid SetY ( GXFloat y );
	GXFloat GetY () const;

	GXVoid Init ( GXFloat x, GXFloat y );
	GXVoid Normalize ();

	GXVoid CalculateNormalFast ( const GXVec2 &a, const GXVec2 &b );	// No normalization
	GXVoid CalculateNormal ( const GXVec2 &a, const GXVec2 &b );

	GXVoid Sum ( const GXVec2 &a, const GXVec2 &b );
	GXVoid Sum ( const GXVec2 &a, GXFloat bScale, const GXVec2 &b );
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

// By convention it is row-vector.
struct GXVec3
{
	// Stores vector components in x, y, z order.
	GXFloat		data[ 3 ];

	GXVec3 ();
	GXVec3 ( const GXVec3 &other );
	explicit GXVec3 ( GXFloat x, GXFloat y, GXFloat z );

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
	GXVoid Sum ( const GXVec3 &a, GXFloat bScale, const GXVec3 &b );
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

GXBool GXCALL GXRayTriangleIntersection3D ( GXFloat &outT, const GXVec3 &origin, const GXVec3 &direction, GXFloat length, const GXVec3 &a, const GXVec3 &b, const GXVec3 &c );

//-------------------------------------------------------------

struct GXEuler
{
	GXFloat		pitchRadians;
	GXFloat		yawRadians;
	GXFloat		rollRadians;

	GXEuler ();
	GXEuler ( const GXEuler &other );
	explicit GXEuler ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );

	GXEuler& operator = ( const GXEuler &other );
};

//-------------------------------------------------------------

// By convention it is row-vector.
struct GXVec4
{
	// Stores vector components in x, y, z, w order.
	GXFloat		data[ 4 ];

	GXVec4 ();
	GXVec4 ( const GXVec4 &other );
	explicit GXVec4 ( const GXVec3& vector, GXFloat w );
	explicit GXVec4 ( GXFloat x, GXFloat y, GXFloat z, GXFloat w );

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
	GXVoid Sum ( const GXVec4 &a, GXFloat bScale, const GXVec4 &b );
	GXVoid Substract ( const GXVec4 &a, const GXVec4 &b );

	GXFloat DotProduct ( const GXVec4 &other ) const;

	GXFloat Length () const;
	GXFloat SquaredLength () const;

	GXVec4& operator = ( const GXVec4 &vector );
};

//-------------------------------------------------------------

struct GXVec6
{
	GXFloat		data[ 6 ];

	GXVec6 ();
	GXVec6 ( const GXVec6 &other );
	explicit GXVec6 ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 );

	GXVoid Init ( GXFloat a1, GXFloat a2, GXFloat a3, GXFloat a4, GXFloat a5, GXFloat a6 );
	GXVoid From ( const GXVec3 &v1, const GXVec3 &v2 );

	GXFloat DotProduct ( const GXVec6 &other ) const;
	GXVoid Sum ( const GXVec6 &a, const GXVec6 &b );
	GXVoid Sum ( const GXVec6 &a, GXFloat bScale, const GXVec6 &b );
	GXVoid Multiply ( const GXVec6 &a, GXFloat factor );

	GXVec6& operator = ( const GXVec6 &other );
};

//-------------------------------------------------------------

struct GXColorHSV;
struct GXColorRGB
{
	// Stores components in red, green, blue, alpha order.
	GXFloat		data[ 4 ];

	GXColorRGB ();
	GXColorRGB ( const GXColorRGB &other );
	explicit GXColorRGB ( GXFloat red, GXFloat green, GXFloat blue, GXFloat alpha );
	explicit GXColorRGB ( GXUByte red, GXUByte green, GXUByte blue, GXFloat alpha );
	explicit GXColorRGB ( const GXColorHSV &color );

	GXVoid Init ( GXFloat red, GXFloat green, GXFloat blue, GXFloat alpha );

	// [0.0f +inf)
	GXVoid SetRed ( GXFloat red );
	GXFloat GetRed () const;

	// [0.0f +inf)
	GXVoid SetGreen ( GXFloat green );
	GXFloat GetGreen () const;

	// [0.0f +inf)
	GXVoid SetBlue ( GXFloat blue );
	GXFloat GetBlue () const;

	// [0.0f 1.0f]
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
	// Stores components in hue, saturation, value, alpha order.
	GXFloat		data[ 4 ];

	GXColorHSV ();
	GXColorHSV ( const GXColorHSV &other );
	explicit GXColorHSV ( GXFloat hue, GXFloat saturation, GXFloat value, GXFloat alpha );
	explicit GXColorHSV ( const GXColorRGB &color );

	// [0.0f 360.0f]
	GXVoid SetHue ( GXFloat hue );
	GXFloat GetHue () const;

	// [0.0f 100.0f]
	GXVoid SetSaturation ( GXFloat saturation );
	GXFloat GetSaturation () const;

	// [0.0f 100.0f]
	GXVoid SetValue ( GXFloat value );
	GXFloat GetValue () const;

	// [0.0f 100.0f]
	GXVoid SetAlpha ( GXFloat alpha );
	GXFloat GetAlpha () const;

	GXVoid From ( const GXColorRGB &color );

	GXVoid operator = ( const GXColorHSV &other );
};

//-------------------------------------------------------------

struct GXMat3;
struct GXMat4;

// Quaternion representation: r + ai + bj + ck.
// By convention stores only orientation without any scale.
struct GXQuat
{
	// Stores quaternion components in r, a, b, c order.
	GXFloat		data[ 4 ];

	GXQuat ();
	GXQuat ( const GXQuat &other );
	explicit GXQuat ( GXFloat r, GXFloat a, GXFloat b, GXFloat c );

	// Result is valid if rotationMatrix is rotation matrix. Any scale will be ignored.
	explicit GXQuat ( const GXMat3 &rotationMatrix );

	// Result is valid if rotationMatrix is rotation matrix. Any scale will be ignored.
	explicit GXQuat ( const GXMat4 &rotationMatrix );

	GXVoid Init ( GXFloat r, GXFloat a, GXFloat b, GXFloat c );

	GXVoid SetR ( GXFloat r );
	GXFloat GetR () const;

	GXVoid SetA ( GXFloat a );
	GXFloat GetA () const;

	GXVoid SetB ( GXFloat b );
	GXFloat GetB () const;

	GXVoid SetC ( GXFloat c );
	GXFloat GetC () const;

	GXVoid Identity ();
	GXVoid Normalize ();
	GXVoid Inverse ( const GXQuat &sourceQuaternion );
	GXVoid FromAxisAngle ( GXFloat x, GXFloat y, GXFloat z, GXFloat angle );
	GXVoid FromAxisAngle ( const GXVec3 &axis, GXFloat angle );

	// Result is valid if rotationMatrix is rotation matrix. Any scale will be ignored.
	GXVoid From ( const GXMat3& rotationMatrix );

	// Result is valid if rotationMatrix is rotation matrix. Any scale will be ignored.
	GXVoid From ( const GXMat4& rotationMatrix );

	// Result is valid if pureRotationMatrix is not scaled rotation matrix.
	GXVoid FromFast ( const GXMat3& pureRotationMatrix );

	// Result is valid if pureRotationMatrix is not scaled rotation matrix.
	GXVoid FromFast ( const GXMat4& pureRotationMatrix );

	GXVoid Multiply ( const GXQuat &a, const GXQuat &b );
	GXVoid Multiply ( const GXQuat &q, GXFloat scale );
	GXVoid Sum ( const GXQuat &a, const GXQuat &b );
	GXVoid Substract ( const GXQuat &a, const GXQuat &b );

	GXVoid SphericalLinearInterpolation ( const GXQuat &start, const GXQuat &finish, GXFloat interpolationFactor );
	
	GXVoid GetAxisAngle ( GXVec3 &axis, GXFloat &angle ) const;
	GXVoid Transform ( GXVec3 &out, const GXVec3 &v ) const;

	// Result is valid if quaternion is normalized.
	GXVoid TransformFast ( GXVec3 &out, const GXVec3 &v ) const;

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
	GXMat3 ( const GXMat3 &other );
	explicit GXMat3 ( const GXMat4 &matrix );

	GXVoid From ( const GXQuat &quaternion );
	GXVoid From ( const GXMat4 &matrix );

	// Constructs orthonormal basis. Result is valid if zDirection is unit vector.
	GXVoid From ( const GXVec3 &zDirection );

	// Result is valid if quaternion is normalized.
	GXVoid FromFast ( const GXQuat &quaternion );

	GXVoid SetX ( const GXVec3& x );
	GXVoid GetX ( GXVec3 &x ) const;

	GXVoid SetY ( const GXVec3 &y );
	GXVoid GetY ( GXVec3 &y ) const;

	GXVoid SetZ ( const GXVec3 &z );
	GXVoid GetZ ( GXVec3 &z ) const;

	GXVoid Identity ();
	GXVoid Zeros ();

	GXVoid Inverse ( const GXMat3 &sourceMatrix );
	GXVoid Transponse ( const GXMat3 &sourceMatrix );
	GXVoid ClearRotation ( const GXMat3 &sourceMatrix );
	GXVoid ClearRotation ( const GXMat4 &sourceMatrix );

	GXVoid SkewSymmetric ( const GXVec3 &base );

	GXVoid Sum ( const GXMat3 &a, const GXMat3 &b );
	GXVoid Substract ( const GXMat3 &a, const GXMat3 &b );
	GXVoid Multiply ( const GXMat3 &a, const GXMat3 &b );

	GXVoid MultiplyVectorMatrix ( GXVec3 &out, const GXVec3 &v ) const;
	GXVoid MultiplyMatrixVector ( GXVec3 &out, const GXVec3 &v ) const;

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

	// Result is valid if quaternion is normalized.
	GXVoid SetRotationFast ( const GXQuat &quaternion );

	GXVoid SetOrigin ( const GXVec3 &origin );
	GXVoid From ( const GXQuat &quaternion, const GXVec3 &origin );
	GXVoid From ( const GXMat3 &rotation, const GXVec3 &origin );
	GXVoid From ( const GXVec3 &zDirection, const GXVec3 &origin );

	// Result is valid if quaternion is normalized.
	GXVoid FromFast ( const GXQuat &quaternion, const GXVec3 &origin );

	GXVoid SetX ( const GXVec3 &x );
	GXVoid GetX ( GXVec3 &x ) const;

	GXVoid SetY ( const GXVec3 &y );
	GXVoid GetY ( GXVec3 &y ) const;
	
	GXVoid SetZ ( const GXVec3 &z );
	GXVoid GetZ ( GXVec3 &z ) const;
	
	GXVoid SetW ( const GXVec3 &w );
	GXVoid GetW ( GXVec3 &w ) const;

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
	GXVoid ClearRotation ( const GXMat3 &sourceMatrix );
	GXVoid ClearRotation ( const GXMat4 &sourceMatrix );

	GXVoid Scale ( GXFloat x, GXFloat y, GXFloat z );
	GXVoid ClearScale ( GXVec3 &scale ) const;

	GXVoid Inverse ( const GXMat4 &sourceMatrix );

	GXVoid Multiply ( const GXMat4 &a, const GXMat4 &b );

	// Multiply row-vector [1x4] by own matrix [4x4].
	GXVoid MultiplyVectorMatrix ( GXVec4 &out, const GXVec4 &v ) const;

	// Multiply own matrix [4x4] by column-vector [4x1].
	GXVoid MultiplyMatrixVector ( GXVec4 &out, const GXVec4 &v ) const;

	// Multiply row-vector [1x3] by own matrix sub matrix [3x3].
	GXVoid MultiplyAsNormal ( GXVec3 &out, const GXVec3 &v ) const;

	// Multiply row-vector [1x3] by own matrix sub matrix [3x3] and add own w-vector.
	GXVoid MultiplyAsPoint ( GXVec3 &out, const GXVec3 &v ) const;

	// Result is valid if own matrix is perspective matrix.
	GXVoid GetPerspectiveParams ( GXFloat &fieldOfViewYRadiands, GXFloat &aspectRatio, GXFloat &zNear, GXFloat &zFar );

	// Result is valid if own matrix is ortho matrix.
	GXVoid GetOrthoParams ( GXFloat &width, GXFloat &height, GXFloat &zNear, GXFloat &zFar );

	// Result is valid if own matrix is perspective matrix.
	GXVoid GetRayPerspective ( GXVec3 &rayView, const GXVec2 &mouseCVV ) const;

	GXMat4& operator = ( const GXMat4 &other );
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
		explicit GXProjectionClipPlanes ( const GXMat4 &src );

		// Normals will be directed inside view volume.
		GXVoid From ( const GXMat4 &src );

		// Trivial invisibility test.
		GXBool IsVisible ( const GXAABB &bounds );

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

GXVoid GXCALL GXGetTangentBitangent ( GXVec3 &outTangent, GXVec3 &outBitangent, GXUByte vertexID, const GXUByte* vertices, GXUPointer vertexStride, const GXUByte* uvs, GXUPointer uvStride );

GXFloat GXCALL GXClampf ( GXFloat value, GXFloat minValue, GXFloat maxValue );
GXInt GXCALL GXClampi ( GXInt value, GXInt minValue, GXInt maxValue );

GXFloat GXCALL GXMinf ( GXFloat a, GXFloat b );
GXFloat GXCALL GXMaxf ( GXFloat a, GXFloat b );

GXVoid GXCALL GXGetBarycentricCoords ( GXVec3 &out, const GXVec3 &point, const GXVec3 &a, const GXVec3 &b, const GXVec3 &c );

GXVoid GXCALL GXGetRayFromViewer ( GXVec3 &origin, GXVec3 &direction, GXUShort x, GXUShort y, GXUShort viewportWidth, GXUShort viewportHeight, const GXVec3& viewerLocation, const GXMat4 &viewProjectionMatrix );


#endif // GX_MATH
