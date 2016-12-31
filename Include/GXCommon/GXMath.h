//version 1.25

#ifndef GX_MATH
#define GX_MATH


#include "GXTypes.h"
#include <math.h>


#define GX_MATH_TWO_PI	6.2831853f
#define GX_MATH_PI		3.1415927f
#define GX_MATH_HALFPI	1.5707963f

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
	GXFloat _v[ 2 ];
};

GXVec2 GXCALL GXCreateVec2 ( GXFloat component_1, GXFloat component_2 );
GXVoid GXCALL GXSubVec2Vec2 ( GXVec2 &out, const GXVec2 &a, const GXVec2 &b );

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
			GXFloat pitch_rad;
			GXFloat yaw_rad;
			GXFloat roll_rad;
		};
		GXFloat v[ 3 ];
	};
	 
	GXVec3 ();
	GXVec3 ( GXFloat component_1, GXFloat component_2, GXFloat component_3 );
	GXVec3 operator * ( GXFloat &factor );
	GXVec3 operator + ( GXVec3 &V );
	GXVec3 operator - ( GXVec3 &V );
	GXVoid operator = ( const GXVec3 &V );
};

GXVec3 GXCALL GXCreateVec3 ( GXFloat component_1, GXFloat component_2, GXFloat component_3 );
GXVoid GXCALL GXNormalizeVec3 ( GXVec3 &inOut );
GXVoid GXCALL GXSumVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b );
GXVoid GXCALL GXSubVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b );
GXVoid GXCALL GXMulVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b );
GXVoid GXCALL GXMulVec3Scalar ( GXVec3 &out, const GXVec3 &v, GXFloat factor );
GXFloat GXCALL GXDotVec3Fast ( const GXVec3 &a, const GXVec3 &b );	//Вектора должны быль единичными
GXVoid GXCALL GXCrossVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b );
GXFloat GXCALL GXLengthVec3 ( const GXVec3 &v );
GXFloat GXCALL GXDistanceVec3Vec3 ( const GXVec3 &a, const GXVec3 &b );
GXVoid GXCALL GXProjectVec3Vec3 ( GXVec3 &projection, const GXVec3 &vector, const GXVec3 &unitVector );

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
			GXFloat r;
			GXFloat g;
			GXFloat b;
			GXFloat a;
		};
		GXFloat v[ 4 ];
	};

	GXVec4 ();
	GXVec4 ( GXVec3& V, GXFloat component_4 );
	GXVec4 ( GXFloat component_1, GXFloat component_2, GXFloat component_3, GXFloat component_4 );
	GXVec4 operator * ( GXFloat& factor );
	GXVec4 operator + ( GXVec4& V );
	GXVec4 operator - ( GXVec4& V );
	GXVoid operator = ( const GXVec4& V );
};

GXVec4 GXCALL GXCreateVec4 ( GXFloat component_1, GXFloat component_2, GXFloat component_3, GXFloat component_4 );

//-------------------------------------------------------------

struct GXMat4;

typedef GXVec4 GXQuat;

GXQuat GXCALL GXCreateQuat ( const GXMat4 &mat );

GXVoid GXCALL GXSetQuatIdentity ( GXQuat &out );
GXVoid GXCALL GXSetQuatRotationAxis ( GXQuat &out, GXFloat x, GXFloat y, GXFloat z, GXFloat angle );
GXVoid GXCALL GXSetQuatRotationAxis ( GXQuat &out, const GXVec3 &axis, GXFloat angle );

GXVoid GXCALL GXQuatRehandCoordinateSystem ( GXQuat &out, const GXQuat &src );
GXVoid GXCALL GXQuatToEulerAngles ( GXVec3 &out_rad, const GXQuat &q );	//TODO

GXVoid GXCALL GXMulQuatQuat ( GXQuat& out, const GXQuat &a, const GXQuat &b );
GXVoid GXCALL GXSumQuatQuat ( GXQuat& out, const GXQuat &a, const GXQuat &b );
GXVoid GXCALL GXSubQuatQuat ( GXQuat &out, const GXQuat &a, const GXQuat &b );
GXVoid GXCALL GXQuatSLerp ( GXQuat &out, const GXQuat &a, const GXQuat &b, GXFloat k );
GXVoid GXCALL GXInverseQuat ( GXQuat &out, const GXQuat &q );
GXVoid GXCALL GXNormalizeQuat ( GXQuat &out, GXQuat &q );
GXVoid GXCALL GXQuatTransform ( GXVec3 &out, const GXQuat &q, const GXVec3 &v );

//-------------------------------------------------------------

struct GXMat4
{
	union
	{
		GXFloat A[ 16 ];
		GXFloat m[ 4 ][ 4 ];
		struct
		{
			GXFloat	m11, m12, m13, m14;
			GXFloat	m21, m22, m23, m24;
			GXFloat	m31, m32, m33, m34;
			GXFloat	m41, m42, m43, m44;
		};
		struct
		{
			GXVec3 xv; GXFloat xw;
			GXVec3 yv; GXFloat yw;
			GXVec3 zv; GXFloat zw;
			GXVec3 wv; GXFloat ww;
		};
	};

	GXMat4 ();
	GXMat4 ( GXFloat* v16 );
	GXVoid operator = ( const GXMat4& M );
	const GXMat4 &SetRotation ( const GXQuat &q );
	const GXMat4 &SetOrigin ( const GXVec3 &origin );
	const GXMat4 &From ( const GXQuat &quat, const GXVec3 &origin );
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
	GXFloat A[ 9 ];
	GXMat3 ();
	GXMat3 ( GXMat4& M );
	GXMat3 operator ~ ();
	GXVoid operator = ( GXMat3& M );
};

GXVoid GXCALL GXTransponseMat3 ( GXMat3& out, GXMat3& a );

//-------------------------------------------------------------

struct GXAABB
{
	GXUByte	vertices;

	GXVec3	min;
	GXVec3	max;

	GXAABB ();
};

GXVoid GXCALL GXSetAABBEmpty ( GXAABB &inOut );
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

enum eGXPlaneClassifyVertex
{
	GX_CLASSIFY_IN_FRONT,
	GX_CLASSIFY_BEHIND,
	GX_CLASSIFY_ON
};

struct GXPlane
{
	GXFloat a;
	GXFloat b;
	GXFloat c;

	GXFloat d;
};

eGXPlaneClassifyVertex GXCALL GXPlaneClassifyVertex ( const GXPlane &plane, const GXVec3 &vertex );
eGXPlaneClassifyVertex GXCALL GXPlaneClassifyVertex ( const GXPlane &plane, GXFloat x, GXFloat y, GXFloat z );
GXVoid GXCALL GXNormalizePlane ( GXPlane &inOut );

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

	private:
		GXUByte	PlaneTest ( GXFloat x, GXFloat y, GXFloat z );
};

//-------------------------------------------------------------

GXFloat GXCALL GXDegToRad ( GXFloat degrees );
GXVoid GXCALL GXColorToVec4 ( GXVec4 &out, GXUChar r, GXUChar g, GXUChar b, GXUChar a );
GXVoid GXCALL GXConvert3DSMaxToGXEngine ( GXVec3 &gx_out, GXFloat max_x, GXFloat max_y, GXFloat max_z );
GXFloat GXCALL GXRandomNormalize ();
GXVoid GXCALL GXGetTangentBitangent ( GXVec3 &outTangent, GXVec3 &outBitangent, GXUByte vertexID, const GXUByte* vertices, GXUInt vertexStride, const GXUByte* uvs, GXUInt uvStride );

GXFloat GXCALL GXClampf ( GXFloat value, GXFloat minValue, GXFloat maxValue );
GXUInt GXCALL GXClampi ( GXInt value, GXInt minValue, GXInt maxValue );


#endif //GX_MATH
