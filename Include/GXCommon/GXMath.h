//version 1.37

#ifndef GX_MATH
#define GX_MATH


#include "GXTypes.h"
#include <math.h>
#include <limits.h>
#include <float.h>


#define GX_MATH_DOUBLE_PI	6.2831853f
#define GX_MATH_PI			3.1415927f
#define GX_MATH_HALF_PI		1.5707963f


union GXVec2
{
	struct
	{
		GXFloat x;
		GXFloat y;
	};
	struct
	{
		GXFloat u;
		GXFloat v;
	};
	GXFloat arr[ 2 ];

	GXVec2 ();
	explicit GXVec2 ( GXFloat component_1, GXFloat component_2 );

	GXVoid operator = ( const GXVec2 &vector );
};

GXVec2 GXCALL GXCreateVec2 ( GXFloat component_1, GXFloat component_2 );
GXVoid GXCALL GXNormalizeVec2 ( GXVec2 &inOut );
GXVoid GXCALL GXCalculateNormalVec2Fast ( GXVec2 &normal, const GXVec2 &a, const GXVec2 &b );	//No normalization
GXVoid GXCALL GXCalculateNormalVec2 ( GXVec2 &normal, const GXVec2 &a, const GXVec2 &b );
GXVoid GXCALL GXSumVec2Vec2 ( GXVec2 &out, const GXVec2 &a, const GXVec2 &b );
GXVoid GXCALL GXSumVec2ScaledVec2 ( GXVec2 &out, const GXVec2 &a, GXFloat s, const GXVec2 &b );
GXVoid GXCALL GXSubVec2Vec2 ( GXVec2 &out, const GXVec2 &a, const GXVec2 &b );
GXVoid GXCALL GXMulVec2Vec2 ( GXVec2 &out, const GXVec2 &a, const GXVec2 &b );
GXVoid GXCALL GXMulVec2Scalar ( GXVec2 &out, const GXVec2 &v, GXFloat a );
GXFloat GXCALL GXDotVec2 ( const GXVec2 &a, const GXVec2 &b );
GXFloat GXCALL GXLengthVec2 ( const GXVec2 &v );
GXBool GXCALL GXIsEqualVec2 ( const GXVec2 &a, const GXVec2 &b );

//-------------------------------------------------------------

enum class eGXLineRelationship : GXUByte
{
	NoIntersection,
	Overlap,
	Intersection
};

eGXLineRelationship GXCALL GXLineIntersection2D ( GXVec2 &intersectionPoint, const GXVec2 &a0, const GXVec2 &a1, const GXVec2 &b0, const GXVec2 &b1 );

//-------------------------------------------------------------

struct GXVec3
{
	union
	{
		struct
		{
			GXFloat x;
			GXFloat y;
			GXFloat z;
		};
		struct
		{
			GXFloat r;
			GXFloat g;
			GXFloat b;
		};
		struct
		{
			GXFloat h;
			GXFloat s;
			GXFloat v;
		};
		struct
		{
			GXFloat pitch_rad;
			GXFloat yaw_rad;
			GXFloat roll_rad;
		};
		GXFloat arr[ 3 ];
	};

	GXVec3 ();
	explicit GXVec3 ( GXFloat component_1, GXFloat component_2, GXFloat component_3 );

	static const GXVec3& GetAbsoluteX ();
	static const GXVec3& GetAbsoluteY ();
	static const GXVec3& GetAbsoluteZ ();

	GXVoid operator = ( const GXVec3 &vector );
};

GXVec3 GXCALL GXCreateVec3 ( GXFloat component_1, GXFloat component_2, GXFloat component_3 );
GXVoid GXCALL GXNormalizeVec3 ( GXVec3 &inOut );
GXVoid GXCALL GXSumVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b );
GXVoid GXCALL GXSumVec3ScaledVec3 ( GXVec3 &out, const GXVec3 &a, GXFloat s, const GXVec3 &b );
GXVoid GXCALL GXSubVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b );
GXVoid GXCALL GXSubVec3ScaledVec3 ( GXVec3 &out, const GXVec3 &a, GXFloat s, const GXVec3 &b );
GXVoid GXCALL GXMulVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b );
GXVoid GXCALL GXMulVec3Scalar ( GXVec3 &out, const GXVec3 &v, GXFloat factor );
GXFloat GXCALL GXDotVec3 ( const GXVec3 &a, const GXVec3 &b );
GXVoid GXCALL GXCrossVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b );
GXFloat GXCALL GXLengthVec3 ( const GXVec3 &v );
GXFloat GXCALL GXSquareLengthVec3 ( const GXVec3 &v );
GXFloat GXCALL GXDistanceVec3Vec3 ( const GXVec3 &a, const GXVec3 &b );
GXFloat GXCALL GXSquareDistanceVec3Vec3 ( const GXVec3 &a, const GXVec3 &b );
GXVoid GXCALL GXReverseVec3 ( GXVec3 &inOut );
GXVoid GXCALL GXLerpVec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b, GXFloat factor );
GXVoid GXCALL GXProjectVec3Vec3 ( GXVec3 &projection, const GXVec3 &vector, const GXVec3 &unitVector );
GXVoid GXCALL GXMakeOrthonormalBasis ( GXVec3 &baseX, GXVec3 &adjustedY, GXVec3 &adjustedZ ); //baseX - correct direction, adjustedY - desirable, adjustedZ - calculated.
GXBool GXCALL GXIsEqualVec3 ( const GXVec3 &a, const GXVec3 &b );

//-------------------------------------------------------------

struct GXVec4
{
	union
	{
		struct
		{
			GXFloat x;
			GXFloat y;
			GXFloat z;
			GXFloat w;
		};
		struct
		{
			union
			{
				struct
				{
					GXFloat r;
					GXFloat g;
					GXFloat b;
				};
				struct
				{
					GXFloat h;
					GXFloat s;
					GXFloat v;
				};
			};
			GXFloat a;
		};
		GXFloat arr[ 4 ];
	};

	GXVec4 ();
	explicit GXVec4 ( GXVec3& vector, GXFloat component_4 );
	explicit GXVec4 ( GXFloat component_1, GXFloat component_2, GXFloat component_3, GXFloat component_4 );

	GXVoid operator = ( const GXVec4& vector );
};

GXVec4 GXCALL GXCreateVec4 ( GXFloat component_1, GXFloat component_2, GXFloat component_3, GXFloat component_4 );

//-------------------------------------------------------------

struct GXMat4;
struct GXMat3;

typedef GXVec4 GXQuat;

GXQuat GXCALL GXCreateQuat ( const GXMat4 &mat );

GXVoid GXCALL GXSetQuatIdentity ( GXQuat &out );
GXVoid GXCALL GXSetQuatFromAxisAngle ( GXQuat &out, GXFloat x, GXFloat y, GXFloat z, GXFloat angle );
GXVoid GXCALL GXSetQuatFromAxisAngle ( GXQuat &out, const GXVec3 &axis, GXFloat angle );

GXVoid GXCALL GXQuatRehandCoordinateSystem ( GXQuat &inOut );
GXVoid GXCALL GXQuatToEulerAngles ( GXVec3 &out_rad, const GXQuat &q );	//TODO
GXVoid GXCALL GXQuatToAxisAngle ( GXVec3 &axis, GXFloat &angle, const GXQuat quaternion );

GXVoid GXCALL GXMulQuatQuat ( GXQuat &out, const GXQuat &a, const GXQuat &b );
GXVoid GXCALL GXSumQuatQuat ( GXQuat &out, const GXQuat &a, const GXQuat &b );
GXVoid GXCALL GXSumQuatScaledVec3 ( GXQuat &out, const GXQuat &q, GXFloat s, const GXVec3 &v );
GXVoid GXCALL GXSubQuatQuat ( GXQuat &out, const GXQuat &a, const GXQuat &b );
GXVoid GXCALL GXQuatSLerp ( GXQuat &out, const GXQuat &a, const GXQuat &b, GXFloat k );
GXVoid GXCALL GXInverseQuat ( GXQuat &out, const GXQuat &q );
GXVoid GXCALL GXNormalizeQuat ( GXQuat &inOut );
GXVoid GXCALL GXQuatTransform ( GXVec3 &out, const GXQuat &q, const GXVec3 &v );

//-------------------------------------------------------------

struct GXMat4
{
	union
	{
		GXFloat arr[ 16 ];
		GXFloat m[ 4 ][ 4 ];
		struct
		{
			GXFloat	m11, m12, m13, m14;
			GXFloat	m21, m22, m23, m24;
			GXFloat	m31, m32, m33, m34;
			GXFloat	m41, m42, m43, m44;
		};
	};

	GXMat4 ();

	GXVoid SetRotation ( const GXQuat &quaternion );
	GXVoid SetOrigin ( const GXVec3 &origin );
	GXVoid From ( const GXQuat &quaternion, const GXVec3 &origin );
	GXVoid From ( const GXMat3 &rotation, const GXVec3 &origin );

	GXVoid GetX ( GXVec3& x ) const;
	GXVoid SetX ( const GXVec3& x );

	GXVoid GetY ( GXVec3& y ) const;
	GXVoid SetY ( const GXVec3& y );

	GXVoid GetZ ( GXVec3& z ) const;
	GXVoid SetZ ( const GXVec3& z );

	GXVoid GetW ( GXVec3& w ) const;
	GXVoid SetW ( const GXVec3& w );

	GXVoid operator = ( const GXMat4& matrix );
};

GXVoid GXCALL GXSetMat4Identity ( GXMat4 &M );
GXVoid GXCALL GXSetMat4Perspective ( GXMat4 &out, GXFloat fovy_rad, GXFloat aspect, GXFloat znear, GXFloat zfar );
GXVoid GXCALL GXSetMat4Ortho ( GXMat4 &out, GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar );
GXVoid GXCALL GXSetMat4Translation ( GXMat4 &out, GXFloat x, GXFloat y, GXFloat z );
GXVoid GXCALL GXSetMat4TranslateTo ( GXMat4 &out, GXFloat x, GXFloat y, GXFloat z );
GXVoid GXCALL GXSetMat4TranslateTo ( GXMat4 &out, const GXVec3 &location );
GXVoid GXCALL GXSetMat4RotationX ( GXMat4 &out, GXFloat angle );
GXVoid GXCALL GXSetMat4RotationY ( GXMat4 &out, GXFloat angle );
GXVoid GXCALL GXSetMat4RotationZ ( GXMat4 &out, GXFloat angle );
GXVoid GXCALL GXSetMat4RotationXY ( GXMat4 &out, GXFloat pitch_rad, GXFloat yaw_rad );
GXVoid GXCALL GXSetMat4RotationXYZ ( GXMat4 &out, GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
GXVoid GXCALL GXSetMat4RotationRelative ( GXMat4 &out, GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad, GXFloat angle );
GXVoid GXCALL GXSetMat4ClearRotation ( GXMat4 &out, const GXMat4 &mod_mat );
GXVoid GXCALL GXSetMat4Scale ( GXMat4 &out, GXFloat x, GXFloat y, GXFloat z );
GXVoid GXCALL GXSetMat4Inverse ( GXMat4& out, const GXMat4& src );

GXVoid GXCALL GXMulMat4Mat4 ( GXMat4& out, const GXMat4& a, const GXMat4& b );
GXVoid GXCALL GXMulVec4Mat4 ( GXVec4 &out, const GXVec4 &V, const GXMat4 &M );
GXVoid GXCALL GXMulVec3Mat4AsNormal ( GXVec3 &out, const GXVec3 &V, const GXMat4 &M );
GXVoid GXCALL GXMulVec3Mat4AsPoint ( GXVec3 &out, const GXVec3 &V, const GXMat4 &M );

GXVoid GXCALL GXGetPerspectiveParams ( const GXMat4 &m, GXFloat &fovy_rad, GXFloat &aspect, GXFloat &zNear, GXFloat &zFar );
GXVoid GXCALL GXGetOrthoParams ( const GXMat4 &m, GXFloat &width, GXFloat &height, GXFloat &zNear, GXFloat &zFar );

GXVoid GXCALL GXGetRayPerspective ( GXVec3 &rayView, const GXMat4 &proj_mat, const GXVec2 &mouseCVV );

//-------------------------------------------------------------

struct GXMat3
{
	union 
	{
		GXFloat arr[ 9 ];
		GXFloat m[ 3 ][ 3 ];
		struct
		{
			GXFloat m11, m12, m13;
			GXFloat m21, m22, m23;
			GXFloat m31, m32, m33;
		};
	};

	GXMat3 ();

	GXVoid From ( const GXQuat &quaternion );

	GXVoid GetX ( GXVec3& x ) const;
	GXVoid SetX ( const GXVec3& x );

	GXVoid GetY ( GXVec3& y ) const;
	GXVoid SetY ( const GXVec3& y );

	GXVoid GetZ ( GXVec3& z ) const;
	GXVoid SetZ ( const GXVec3& z );

	GXVoid operator = ( const GXMat3 &matrix );
};

GXVoid GXCALL GXSetMat3Transponse ( GXMat3 &out, const GXMat3 &a );
GXVoid GXCALL GXSetMat3FromMat4 ( GXMat3 &out, const GXMat4 &m );
GXVoid GXCALL GXSetMat3Identity ( GXMat3 &out);
GXVoid GXCALL GXSetMat3Inverse ( GXMat3 &out, const GXMat3 &m );
GXVoid GXCALL GXSetMat3Zero ( GXMat3 &out );
GXVoid GXCALL GXSetMat3SkewSymmetric ( GXMat3 &out, const GXVec3 &base );
GXVoid GXCALL GXMulMat3Mat3 ( GXMat3 &out, const GXMat3 &a, const GXMat3 &b );
GXVoid GXCALL GXMulVec3Mat3 ( GXVec3 &out, const GXVec3 &v, const GXMat3 &m );
GXVoid GXCALL GXMulMat3Scalar ( GXMat3 &out, const GXMat3 &m, float a);
GXVoid GXCALL GXSumMat3Mat3 ( GXMat3 &out, const GXMat3 &a, const GXMat3 &b);
GXVoid GXCALL GXSubMat3Mat3 ( GXMat3 &out, const GXMat3 &a, const GXMat3 &b);

//-------------------------------------------------------------

struct GXAABB
{
	GXUByte	vertices;

	GXVec3	min;
	GXVec3	max;

	GXAABB ();

	GXVoid operator = ( const GXAABB &aabb );
};

GXVoid GXCALL GXSetAABBEmpty ( GXAABB &inOut );
GXVoid GXCALL GXSetAABBTransformed ( GXAABB &out, const GXAABB &bounds, const GXMat4 &transform );
GXVoid GXCALL GXAddVertexToAABB ( GXAABB &inOut, const GXVec3 &vertex );
GXVoid GXCALL GXAddVertexToAABB ( GXAABB &inOut, GXFloat x, GXFloat y, GXFloat z );
GXBool GXCALL GXIsOverlapedAABBAABB ( const GXAABB& a, const GXAABB& b );
GXBool GXCALL GXIsOverlapedAABBVec3 ( const GXAABB &a, const GXVec3 &b );
GXBool GXCALL GXIsOverlapedAABBVec3 ( const GXAABB &a, GXFloat x, GXFloat y, GXFloat z );
GXVoid GXCALL GXGetAABBCenter ( GXVec3 &c, const GXAABB &bounds );
GXFloat GXCALL GXGetAABBWidth ( const GXAABB &bounds );
GXFloat GXCALL GXGetAABBHeight ( const GXAABB &bounds );
GXFloat GXCALL GXGetAABBDepth ( const GXAABB &bounds );
GXFloat GXCALL GXGetAABBSphereRadius ( const GXAABB &bounds );

//-------------------------------------------------------------

enum class eGXPlaneClassifyVertex
{
	InFront,
	Behind,
	On
};

struct GXPlane
{
	GXFloat a;
	GXFloat b;
	GXFloat c;
	GXFloat d;

	GXPlane ();

	GXVoid From ( const GXVec3 &a, const GXVec3 &b, const GXVec3 &c );
	GXVoid FromLineToPoint ( const GXVec3 &lineStart, const GXVec3 &lineEnd, const GXVec3 &point );

	GXVoid operator = ( const GXPlane &plane );
};

eGXPlaneClassifyVertex GXCALL GXPlaneClassifyVertex ( const GXPlane &plane, const GXVec3 &vertex );
eGXPlaneClassifyVertex GXCALL GXPlaneClassifyVertex ( const GXPlane &plane, GXFloat x, GXFloat y, GXFloat z );
GXVoid GXCALL GXNormalizePlane ( GXPlane &inOut );
GXVoid GXCALL GXFlipPlane ( GXPlane &inOut );

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

		GXVoid operator = ( const GXProjectionClipPlanes &clipPlanes );

	private:
		GXUByte PlaneTest ( GXFloat x, GXFloat y, GXFloat z );
};

//-------------------------------------------------------------

GXFloat GXCALL GXDegToRad ( GXFloat degrees );
GXFloat GXCALL GXRadToDeg ( GXFloat radians );
GXVoid GXCALL GXColorToVec3 ( GXVec3 &out, GXUChar r, GXUChar g, GXUChar b );
GXVoid GXCALL GXColorToVec4 ( GXVec4 &out, GXUChar r, GXUChar g, GXUChar b, GXUChar a );
GXVoid GXCALL GXConvertHSVAToRGBA ( GXUByte &red, GXUByte &green, GXUByte &blue, GXUByte &alpha, const GXVec4 &hsvaColor );
GXVoid GXCALL GXConvertHSVAToRGBA ( GXVec4 &rgbaColor, const GXVec4 &hsvaColor );
GXVoid GXCALL GXConvertRGBAToHSVA ( GXVec4 &hsvaColor, const GXVec4 &rgbaColor );
GXVoid GXCALL GXConvert3DSMaxToGXEngine ( GXVec3 &gx_out, GXFloat max_x, GXFloat max_y, GXFloat max_z );
GXFloat GXCALL GXRandomNormalize ();
GXFloat GXCALL GXRandomBetween ( GXFloat from, GXFloat to );
GXVoid GXCALL GXRandomBetween ( GXVec3 &out, const GXVec3 &from, const GXVec3 &to );
GXVoid GXCALL GXGetTangentBitangent ( GXVec3 &outTangent, GXVec3 &outBitangent, GXUByte vertexID, const GXUByte* vertices, GXUInt vertexStride, const GXUByte* uvs, GXUInt uvStride );

GXFloat GXCALL GXClampf ( GXFloat value, GXFloat minValue, GXFloat maxValue );
GXUInt GXCALL GXClampi ( GXInt value, GXInt minValue, GXInt maxValue );

GXFloat GXCALL GXMinf ( GXFloat a, GXFloat b );
GXFloat GXCALL GXMaxf ( GXFloat a, GXFloat b );

GXVoid GXCALL GXGetBarycentricCoords ( GXVec3 &out, const GXVec3 &point, const GXVec3 &a, const GXVec3 &b, const GXVec3 &c );


#endif //GX_MATH
