//version 1.24

#include <GXCommon/GXMath.h>
#include <math.h>
#include <cstdlib>
#include <cfloat>
#include <cwchar>


GXVec2 GXCALL GXCreateVec2 ( GXFloat component_1, GXFloat component_2 )
{
	GXVec2 ans;
	ans.x = component_1;
	ans.y = component_2;
	return ans;
}

GXVoid GXCALL GXSubVec2Vec2 ( GXVec2 &out, const GXVec2 &a, const GXVec2 &b )
{
	out.x = a.x - b.x;
	out.y = a.y - b.y;
}

//------------------------------------------------------------------------------------------------

GXVec3::GXVec3 ()
{
	//NOTHING
}

GXVec3::GXVec3 ( GXFloat component_1, GXFloat component_2, GXFloat component_3 )
{
	x = component_1;
	y = component_2;
	z = component_3;
}

GXVec3 GXVec3::operator * ( GXFloat& factor )
{
	GXVec3 ans;
	ans.x = x * factor;
	ans.y = y * factor;
	ans.z = z * factor;
	return ans;
}

GXVec3 GXVec3::operator + ( GXVec3& V )
{
	GXVec3 ans;
	ans.x = x + V.x;
	ans.y = y + V.y;
	ans.z = z + V.z;
	return ans;
}

GXVec3 GXVec3::operator - ( GXVec3& V )
{
	GXVec3 ans;
	ans.x = x - V.x;
	ans.y = y - V.y;
	ans.z = z - V.z;
	return ans;
}

GXVoid GXVec3::operator = ( const GXVec3 &V )
{
	memcpy ( v, V.v, sizeof ( GXVec3 ) );
}

//----------------------------------------------------------------------------

GXVoid GXCALL GXNormalizeVec3 ( GXVec3 &inOut )
{
	GXFloat inv_a = 1.0f / sqrtf ( inOut.x * inOut.x + inOut.y * inOut.y + inOut.z * inOut.z );
	inOut.x *= inv_a;
	inOut.y *= inv_a;
	inOut.z *= inv_a;
}

GXVec3 GXCALL GXCreateVec3 ( GXFloat component_1, GXFloat component_2, GXFloat component_3 )
{
	GXVec3 ans;
	ans.x = component_1;
	ans.y = component_2;
	ans.z = component_3;
	return ans;
}

GXVoid GXCALL GXSumVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b )
{
	out.x = a.x + b.x;
	out.y = a.y + b.y;
	out.z = a.z + b.z;
}

GXVoid GXCALL GXSubVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b )
{
	out.x = a.x - b.x;
	out.y = a.y - b.y;
	out.z = a.z - b.z;
}

GXVoid GXCALL GXMulVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b )
{
	out.x = a.x * b.x;
	out.y = a.y * b.y;
	out.z = a.z * b.z;
}

GXVoid GXCALL GXMulVec3Scalar ( GXVec3 &out, const GXVec3 &v, GXFloat factor )
{
	out.x = v.x * factor;
	out.y = v.y * factor;
	out.z = v.z * factor;
}

GXFloat GXCALL GXDotVec3Fast ( const GXVec3 &a, const GXVec3 &b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

GXVoid GXCALL GXCrossVec3Vec3 ( GXVec3 &out, const GXVec3 &a, const GXVec3 &b )
{
	out.x = a.y * b.z - a.z * b.y;
	out.y = a.z * b.x - a.x * b.z;
	out.z = a.x * b.y - a.y * b.x;
}

GXFloat GXCALL GXLengthVec3 ( const GXVec3 &v )
{
	return sqrtf ( v.x * v.x + v.y * v.y + v.z * v.z );
}

GXFloat GXCALL GXDistanceVec3Vec3 ( const GXVec3 &a, const GXVec3 &b )
{
	GXVec3 diff;
	GXSubVec3Vec3 ( diff, a, b );
	return GXLengthVec3 ( diff );
}

GXVoid GXCALL GXProjectVec3Vec3 ( GXVec3 &projection, const GXVec3 &vector, const GXVec3 &unitVector )
{
	GXVec3 normalVector = vector;
	GXNormalizeVec3 ( normalVector );

	GXFloat factor = GXLengthVec3 ( vector ) * GXDotVec3Fast ( normalVector, unitVector );

	projection.x = unitVector.x * factor;
	projection.y = unitVector.y * factor;
	projection.z = unitVector.z * factor;
}

//----------------------------------------------------------------------------------------

GXVec4::GXVec4 ()
{
	//NOTHING
}

GXVec4::GXVec4 ( GXVec3& V, GXFloat component_4 )
{
	memcpy ( v, V.v, sizeof ( GXVec3 ) );
	w = component_4;
}

GXVec4::GXVec4 ( GXFloat component_1, GXFloat component_2, GXFloat component_3, GXFloat component_4 )
{
	x = component_1;
	y = component_2;
	z = component_3;
	w = component_4;
}

GXVec4 GXVec4::operator * ( GXFloat& factor )
{
	GXVec4 ans;
	ans.x = x * factor;
	ans.y = y * factor;
	ans.z = z * factor;
	ans.w = w * factor;
	return ans;
}

GXVec4 GXVec4::operator + ( GXVec4& V )
{
	GXVec4 ans;
	ans.x = x + V.x;
	ans.y = y + V.y;
	ans.z = z + V.z;
	ans.w = w + V.w;
	return ans;
}

GXVec4 GXVec4::operator - ( GXVec4& V )
{
	GXVec4 ans;
	ans.x = x - V.x;
	ans.y = y - V.y;
	ans.z = z - V.z;
	ans.w = w - V.w;
	return ans;
}

GXVoid GXVec4::operator = ( const GXVec4 &V )
{
	memcpy ( v, V.v, sizeof ( GXVec4 ) );
}

GXVec4 GXCALL GXCreateVec4 ( GXFloat component_1, GXFloat component_2, GXFloat component_3, GXFloat component_4 )
{
	return GXVec4 ( component_1, component_2, component_3, component_4 );
}

//------------------------------------------------------------------

GXMat4::GXMat4 ()
{
	memset ( A, 0, sizeof ( GXMat4 ) );
}

GXMat4::GXMat4 ( GXFloat* v16 )
{
	memcpy ( this->A, v16, 16*sizeof ( GXFloat ) );
}

GXVoid GXMat4::operator = ( const GXMat4& M )
{
	memcpy ( this->A, M.A, 16 * sizeof ( GXFloat ) );
}

const GXMat4 &GXMat4::SetRotation ( const GXQuat& q )
{
	GXFloat	wx, wy, wz;
	GXFloat	xx, yy, yz;
	GXFloat	xy, xz, zz;
	GXFloat	x2, y2, z2;

	x2 = q.x + q.x;
	y2 = q.y + q.y;
	z2 = q.z + q.z;

	xx = q.x * x2;
	xy = q.x * y2;
	xz = q.x * z2;

	yy = q.y * y2;
	yz = q.y * z2;
	zz = q.z * z2;

	wx = q.w * x2;
	wy = q.w * y2;
	wz = q.w * z2;

	m[ 0 ][ 0 ] = 1.0f - ( yy + zz );
	m[ 0 ][ 1 ] = xy - wz;
	m[ 0 ][ 2 ] = xz + wy;

	m[ 1 ][ 0 ] = xy + wz;
	m[ 1 ][ 1 ] = 1.0f - ( xx + zz );
	m[ 1 ][ 2 ] = yz - wx;

	m[ 2 ][ 0 ] = xz - wy;
	m[ 2 ][ 1 ] = yz + wx;
	m[ 2 ][ 2 ] = 1.0f - ( xx + yy );

	return *this;
}

const GXMat4 &GXMat4::SetOrigin ( const GXVec3& origin )
{ 
	memcpy ( A + 12, origin.v, sizeof ( GXVec3 ) );
	return *this;
}

const GXMat4 &GXMat4::From ( const GXQuat &quat, const GXVec3 &origin )
{
	SetRotation ( quat );
	SetOrigin ( origin );
	m14 = m24 = m34 = 0.0f;
	m44 = 1.0f;
	return *this;
}

GXVoid GXCALL GXSetMat4Identity ( GXMat4 &out )
{
	out.m12 = out.m13 = out.m14 = 0.0f;
	out.m21 = out.m23 = out.m24 = 0.0f;
	out.m31 = out.m32 = out.m34 = 0.0f;
	out.m41 = out.m42 = out.m43 = 0.0f;

	out.m11 = out.m22 = out.m33 = out.m44 = 1.0f;
}

GXVoid GXCALL GXSetMat4Perspective ( GXMat4 &out, GXFloat fovy_rad, GXFloat aspect, GXFloat znear, GXFloat zfar )
{
	GXFloat halfFovy = fovy_rad * 0.5f;
	GXFloat ctan = cosf ( halfFovy ) / sinf ( halfFovy );
	GXFloat invRange = 1.0f / ( zfar - znear );

	out.m11 = ctan / aspect;
	out.m22 = ctan;
	out.m33 = ( zfar + znear ) * invRange;
	out.m34 = 1.0f;
	out.m43 = -2.0f * zfar * znear * invRange;

	out.m12 = out.m13 = out.m14 = 0.0f;
	out.m21 = out.m23 = out.m24 = 0.0f;
	out.m31 = out.m32 = 0.0f;
	out.m41 = out.m42 = out.m44 = 0.0f;
}

GXVoid GXCALL GXSetMat4Ortho ( GXMat4 &out, GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar )
{
	GXFloat invRange = 1.0f / ( zfar - znear );

	out.m11 = 2.0f / width;
	out.m22 = 2.0f / height;
	out.m33 = 2.0f * invRange;
	out.m43 = -invRange * ( zfar + znear );

	out.m12 = out.m13 = out.m14 = 0.0f;
	out.m21 = out.m23 = out.m24 = 0.0f;
	out.m31 = out.m32 = out.m34 = 0.0f;
	out.m41 = out.m42 = 0.0f;
	out.m44 = 1.0f;
}

GXVoid GXCALL GXSetMat4Translation ( GXMat4 &out, GXFloat x, GXFloat y, GXFloat z )
{
	out.m12 = out.m13 = out.m14 = 0.0f;
	out.m21 = out.m23 = out.m24 = 0.0f;
	out.m31 = out.m32 = out.m34 = 0.0f;

	out.m11 = out.m22 = out.m33 = out.m44 = 1.0f;

	out.m41 = x;
	out.m42 = y;
	out.m43 = z;
}

GXVoid GXCALL GXSetMat4TranslateTo ( GXMat4 &out, GXFloat x, GXFloat y, GXFloat z )
{
	out.wv = GXCreateVec3 ( x, y, z );
}

GXVoid GXCALL GXSetMat4TranslateTo ( GXMat4 &out, const GXVec3 &location )
{
	memcpy ( out.wv.v, location.v, sizeof ( GXVec3 ) );
}

GXVoid GXCALL GXSetMat4RotationX ( GXMat4 &out, GXFloat angle )
{
	out.m12 = out.m13 = 0.0f;
	out.m21 = 0.0f;
	out.m31 = 0.0f;

	GXFloat c;
	GXFloat s;

	c = cosf ( angle );
	s = sinf ( angle );

	out.m22 = c;	out.m23 = s;
	out.m32 = -s;	out.m33 = c;

	out.m11 = 1.0f;

	out.m44 = 1.0f;
	out.m41 = out.m42 = out.m43 = out.m14 = out.m24 = out.m34 = 0;
}

GXVoid GXCALL GXSetMat4RotationY ( GXMat4 &out, GXFloat angle )
{
	out.m12 = 0.0f;
	out.m21 = out.m23 = 0.0f;
	out.m32 = 0.0f;

	GXFloat c;
	GXFloat s;

	c = cosf ( angle );
	s = sinf ( angle );

	out.m11 = c;	out.m13 = -s;
	out.m31 = s;	out.m33 = c;

	out.m22 = 1.0f;

	out.m44 = 1.0f;
	out.m41 = out.m42 = out.m43 = out.m14 = out.m24 = out.m34 = 0.0f;
}

GXVoid GXCALL GXSetMat4RotationZ ( GXMat4 &out, GXFloat angle )
{
	out.m13 = 0.0f;
	out.m23 = 0.0f;
	out.m31 = out.m32 = 0.0f;

	GXFloat c;
	GXFloat s;

	c = cosf ( angle );
	s = sinf ( angle );

	out.m11 = c;	out.m12 = s;
	out.m21 = -s;	out.m22 = c;

	out.m33 = 1.0f;

	out.m44 = 1.0f;
	out.m41 = out.m42 = out.m43 = out.m14 = out.m24 = out.m34 = 0.0f;
}

GXVoid GXCALL GXSetMat4RotationXY ( GXMat4 &out, GXFloat pitch_rad, GXFloat yaw_rad )
{
	GXMat4 X;
	GXMat4 Y;

	GXSetMat4RotationX ( X, pitch_rad );
	GXSetMat4RotationY ( Y, yaw_rad );

	GXMulMat4Mat4 ( out, X, Y );
}

GXVoid GXCALL GXSetMat4RotationXYZ ( GXMat4 &out, GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXMat4 X;
	GXMat4 Y;
	GXMat4 Z;

	GXSetMat4RotationX ( X, pitch_rad );
	GXSetMat4RotationY ( Y, yaw_rad );
	GXSetMat4RotationZ ( Z, roll_rad );

	GXMat4 temp;
	GXMulMat4Mat4 ( temp, X, Y );
	GXMulMat4Mat4 ( out, temp, Z );
}

GXVoid GXCALL GXSetMat4RotationRelative ( GXMat4 &out, GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad, GXFloat angle )
{
	GXFloat c;
	GXFloat s;

	c = cosf ( angle );
	s = sinf ( angle );

	GXFloat ic = 1.0f - c;

	out.m11 = ic * pitch_rad * pitch_rad + c;
	out.m12 = ic * pitch_rad * yaw_rad - s * roll_rad;
	out.m13 = ic * pitch_rad * roll_rad + s * yaw_rad;

	out.m21 = ic * yaw_rad * pitch_rad + s * roll_rad;
	out.m22 = ic * yaw_rad * yaw_rad + c;
	out.m23 = ic * yaw_rad * roll_rad - s * pitch_rad;

	out.m31 = ic * roll_rad * pitch_rad - s * yaw_rad;
	out.m32 = ic * roll_rad * yaw_rad + s * pitch_rad;
	out.m33 = ic * roll_rad * roll_rad + c;

	out.m44 = 1.0f;
	out.m41 = out.m42 = out.m43 = out.m14 = out.m24 = out.m34 = 0.0f;
}

GXVoid GXCALL GXSetMat4ClearRotation ( GXMat4 &out, const GXMat4 &mod_mat )
{
	GXVec3 scaleFactor;
	scaleFactor.x = 1.0f / GXLengthVec3 ( mod_mat.xv );
	scaleFactor.y = 1.0f / GXLengthVec3 ( mod_mat.yv );
	scaleFactor.z = 1.0f / GXLengthVec3 ( mod_mat.zv );

	GXMulVec3Scalar ( out.xv, mod_mat.xv, scaleFactor.x );
	GXMulVec3Scalar ( out.yv, mod_mat.yv, scaleFactor.y );
	GXMulVec3Scalar ( out.zv, mod_mat.zv, scaleFactor.z );

	out.m14 = out.m24 = out.m34 = 0.0f;
	out.m41 = out.m42 = out.m43 = 0.0f;

	out.m44 = 1.0f;
}

GXVoid GXCALL GXSetMat4Scale ( GXMat4 &out, GXFloat x, GXFloat y, GXFloat z )
{
	out.m12 = out.m13 = 0.0f;
	out.m21 = out.m23 = 0.0f;
	out.m31 = out.m32 = 0.0f;

	out.m11 = x;
	out.m22 = y;
	out.m33 = z;

	out.m44 = 1.0f;
	out.m41 = out.m42 = out.m43 = out.m14 = out.m24 = out.m34 = 0.0f;
}


GXVoid GXCALL GXSetMat4Inverse ( GXMat4 &out, const GXMat4 &src )
{
	GXFloat det, invDet;

	// 2x2 sub-determinants required to calculate 4x4 determinant
	GXFloat det2_01_01 = src.m[0][0] * src.m[1][1] - src.m[1][0] * src.m[0][1];
	GXFloat det2_01_02 = src.m[0][0] * src.m[2][1] - src.m[2][0] * src.m[0][1];
	GXFloat det2_01_03 = src.m[0][0] * src.m[3][1] - src.m[3][0] * src.m[0][1];
	GXFloat det2_01_12 = src.m[1][0] * src.m[2][1] - src.m[2][0] * src.m[1][1];
	GXFloat det2_01_13 = src.m[1][0] * src.m[3][1] - src.m[3][0] * src.m[1][1];
	GXFloat det2_01_23 = src.m[2][0] * src.m[3][1] - src.m[3][0] * src.m[2][1];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	GXFloat det3_201_012 = src.m[0][2] * det2_01_12 - src.m[1][2] * det2_01_02 + src.m[2][2] * det2_01_01;
	GXFloat det3_201_013 = src.m[0][2] * det2_01_13 - src.m[1][2] * det2_01_03 + src.m[3][2] * det2_01_01;
	GXFloat det3_201_023 = src.m[0][2] * det2_01_23 - src.m[2][2] * det2_01_03 + src.m[3][2] * det2_01_02;
	GXFloat det3_201_123 = src.m[1][2] * det2_01_23 - src.m[2][2] * det2_01_13 + src.m[3][2] * det2_01_12;

	det = ( - det3_201_123 * src.m[0][3] + det3_201_023 * src.m[1][3] - det3_201_013 * src.m[2][3] + det3_201_012 * src.m[3][3] );

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	GXFloat det2_03_01 = src.m[0][0] * src.m[1][3] - src.m[1][0] * src.m[0][3];
	GXFloat det2_03_02 = src.m[0][0] * src.m[2][3] - src.m[2][0] * src.m[0][3];
	GXFloat det2_03_03 = src.m[0][0] * src.m[3][3] - src.m[3][0] * src.m[0][3];
	GXFloat det2_03_12 = src.m[1][0] * src.m[2][3] - src.m[2][0] * src.m[1][3];
	GXFloat det2_03_13 = src.m[1][0] * src.m[3][3] - src.m[3][0] * src.m[1][3];
	GXFloat det2_03_23 = src.m[2][0] * src.m[3][3] - src.m[3][0] * src.m[2][3];

	GXFloat det2_13_01 = src.m[0][1] * src.m[1][3] - src.m[1][1] * src.m[0][3];
	GXFloat det2_13_02 = src.m[0][1] * src.m[2][3] - src.m[2][1] * src.m[0][3];
	GXFloat det2_13_03 = src.m[0][1] * src.m[3][3] - src.m[3][1] * src.m[0][3];
	GXFloat det2_13_12 = src.m[1][1] * src.m[2][3] - src.m[2][1] * src.m[1][3];
	GXFloat det2_13_13 = src.m[1][1] * src.m[3][3] - src.m[3][1] * src.m[1][3];
	GXFloat det2_13_23 = src.m[2][1] * src.m[3][3] - src.m[3][1] * src.m[2][3];

	// remaining 3x3 sub-determinants
	GXFloat det3_203_012 = src.m[0][2] * det2_03_12 - src.m[1][2] * det2_03_02 + src.m[2][2] * det2_03_01;
	GXFloat det3_203_013 = src.m[0][2] * det2_03_13 - src.m[1][2] * det2_03_03 + src.m[3][2] * det2_03_01;
	GXFloat det3_203_023 = src.m[0][2] * det2_03_23 - src.m[2][2] * det2_03_03 + src.m[3][2] * det2_03_02;
	GXFloat det3_203_123 = src.m[1][2] * det2_03_23 - src.m[2][2] * det2_03_13 + src.m[3][2] * det2_03_12;

	GXFloat det3_213_012 = src.m[0][2] * det2_13_12 - src.m[1][2] * det2_13_02 + src.m[2][2] * det2_13_01;
	GXFloat det3_213_013 = src.m[0][2] * det2_13_13 - src.m[1][2] * det2_13_03 + src.m[3][2] * det2_13_01;
	GXFloat det3_213_023 = src.m[0][2] * det2_13_23 - src.m[2][2] * det2_13_03 + src.m[3][2] * det2_13_02;
	GXFloat det3_213_123 = src.m[1][2] * det2_13_23 - src.m[2][2] * det2_13_13 + src.m[3][2] * det2_13_12;

	GXFloat det3_301_012 = src.m[0][3] * det2_01_12 - src.m[1][3] * det2_01_02 + src.m[2][3] * det2_01_01;
	GXFloat det3_301_013 = src.m[0][3] * det2_01_13 - src.m[1][3] * det2_01_03 + src.m[3][3] * det2_01_01;
	GXFloat det3_301_023 = src.m[0][3] * det2_01_23 - src.m[2][3] * det2_01_03 + src.m[3][3] * det2_01_02;
	GXFloat det3_301_123 = src.m[1][3] * det2_01_23 - src.m[2][3] * det2_01_13 + src.m[3][3] * det2_01_12;

	out.m[0][0] = - det3_213_123 * invDet;
	out.m[0][1] = + det3_213_023 * invDet;
	out.m[0][2] = - det3_213_013 * invDet;
	out.m[0][3] = + det3_213_012 * invDet;

	out.m[1][0] = + det3_203_123 * invDet;
	out.m[1][1] = - det3_203_023 * invDet;
	out.m[1][2] = + det3_203_013 * invDet;
	out.m[1][3] = - det3_203_012 * invDet;

	out.m[2][0] = + det3_301_123 * invDet;
	out.m[2][1] = - det3_301_023 * invDet;
	out.m[2][2] = + det3_301_013 * invDet;
	out.m[2][3] = - det3_301_012 * invDet;

	out.m[3][0] = - det3_201_123 * invDet;
	out.m[3][1] = + det3_201_023 * invDet;
	out.m[3][2] = - det3_201_013 * invDet;
	out.m[3][3] = + det3_201_012 * invDet;
}

GXVoid GXCALL GXMulMat4Mat4 ( GXMat4 &out, const GXMat4 &a, const GXMat4 &b )
{
	out.m11 = a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31 + a.m14 * b.m41;
	out.m12 = a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32 + a.m14 * b.m42;
	out.m13 = a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33 + a.m14 * b.m43;
	out.m14 = a.m11 * b.m14 + a.m12 * b.m24 + a.m13 * b.m34 + a.m14 * b.m44;

	out.m21 = a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31 + a.m24 * b.m41;
	out.m22 = a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32 + a.m24 * b.m42;
	out.m23 = a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33 + a.m24 * b.m43;
	out.m24 = a.m21 * b.m14 + a.m22 * b.m24 + a.m23 * b.m34 + a.m24 * b.m44;

	out.m31 = a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31 + a.m34 * b.m41;
	out.m32 = a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32 + a.m34 * b.m42;
	out.m33 = a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33 + a.m34 * b.m43;
	out.m34 = a.m31 * b.m14 + a.m32 * b.m24 + a.m33 * b.m34 + a.m34 * b.m44;

	out.m41 = a.m41 * b.m11 + a.m42 * b.m21 + a.m43 * b.m31 + a.m44 * b.m41;
	out.m42 = a.m41 * b.m12 + a.m42 * b.m22 + a.m43 * b.m32 + a.m44 * b.m42;
	out.m43 = a.m41 * b.m13 + a.m42 * b.m23 + a.m43 * b.m33 + a.m44 * b.m43;
	out.m44 = a.m41 * b.m14 + a.m42 * b.m24 + a.m43 * b.m34 + a.m44 * b.m44;
}

GXVoid GXCALL GXMulVec4Mat4 ( GXVec4 &out, const GXVec4 &V, const GXMat4 &M )
{
	out.x = V.x * M.m11 + V.y * M.m21 + V.z * M.m31 + V.w * M.m41;
	out.y = V.x * M.m12 + V.y * M.m22 + V.z * M.m32 + V.w * M.m42;
	out.z = V.x * M.m13 + V.y * M.m23 + V.z * M.m33 + V.w * M.m43;
	out.w = V.x * M.m14 + V.y * M.m24 + V.z * M.m34 + V.w * M.m44;
}

GXVoid GXCALL GXMulVec3Mat4AsNormal ( GXVec3 &out, const GXVec3 &V, const GXMat4 &M )
{
	out.x = M.m11 * V.x + M.m21 * V.y + M.m31 * V.z;
	out.y = M.m12 * V.x + M.m22 * V.y + M.m32 * V.z;
	out.z = M.m13 * V.x + M.m23 * V.y + M.m33 * V.z;
}

GXVoid GXCALL GXMulVec3Mat4AsPoint ( GXVec3 &out, const GXVec3 &V, const GXMat4 &M )
{
	out.x = M.m11 * V.x + M.m21 * V.y + M.m31 * V.z + M.m41;
	out.y = M.m12 * V.x + M.m22 * V.y + M.m32 * V.z + M.m42;
	out.z = M.m13 * V.x + M.m23 * V.y + M.m33 * V.z + M.m43;
}

GXVoid GXCALL GXGetPerspectiveParams ( const GXMat4 &m, GXFloat &fovy_rad, GXFloat &aspect, GXFloat &zNear, GXFloat &zFar )
{
	fovy_rad = 2.0f * atanf ( 1.0f / m.m22 );
	aspect = m.m22 / m.m11;

	zNear = -m.m43 / ( m.m33 + 1.0f );
	zFar = ( zNear * m.m43 ) / ( m.m43 + 2.0f * zNear );
}

GXVoid GXCALL GXGetOrthoParams ( const GXMat4 &m, GXFloat &width, GXFloat &height, GXFloat &zNear, GXFloat &zFar )
{
	width = 2.0f / m.m11;
	height = 2.0f / m.m22;

	GXFloat factor = 1.0f / m.m33;

	zNear = -( 1.0f + m.m43 ) * factor;
	zFar = ( 2.0f + zNear * m.m33 ) * factor;
}

GXVoid GXCALL GXGetRayPerspective ( GXVec3 &rayView, const GXMat4 &proj_mat, const GXVec2 &mouseCVV )
{
	rayView.x = mouseCVV.x / proj_mat.m11;
	rayView.y = mouseCVV.y / proj_mat.m22;
	rayView.z = 1.0f;
}

//-------------------------------------------------------------------

GXMat3::GXMat3 ()
{
	memset ( A, 0, sizeof ( GXMat3 ) );
}

GXMat3::GXMat3 ( GXMat4& M )
{
	memcpy ( A, M.A, 3 * sizeof ( GXFloat ) );
	memcpy ( A + 3, M.A + 4, 3 * sizeof ( GXFloat ) );
	memcpy ( A + 6, M.A + 8, 3 * sizeof ( GXFloat ) );
}

GXMat3 GXMat3::operator ~ ()
{
	GXMat3 ans;
	ans.A [ 3 ] = A [ 1 ]; ans.A [ 1 ]	= A [ 3 ];  
	ans.A [ 6 ] = A [ 2 ]; ans.A [ 2 ]	= A [ 6 ]; 
	ans.A [ 7 ] = A [ 5 ]; ans.A [ 5 ]	= A [ 7 ]; 
	ans.A [ 0 ] = A [ 0 ]; ans.A [ 4 ] = A [ 4 ]; ans.A [ 8 ] = A [ 8 ];
	return ans;	
}

GXVoid GXMat3::operator = ( GXMat3& M )
{
	memcpy ( this->A, M.A, 9 * sizeof ( GXFloat ) );
}

GXVoid GXCALL GXTransponseMat3 ( GXMat3& out, GXMat3& a )
{
	out.A [ 3 ] = a.A [ 1 ]; out.A [ 1 ] = a.A [ 3 ];
	out.A [ 6 ] = a.A [ 2 ]; out.A [ 2 ] = a.A [ 6 ];
	out.A [ 7 ] = a.A [ 5 ]; out.A [ 5 ] = a.A [ 7 ];
	out.A [ 0 ] = a.A [ 0 ]; out.A [ 4 ] = a.A [ 4 ]; out.A [ 8 ] = a.A [ 8 ];
}

//----------------------------------------------------------------------------

GXQuat GXCALL GXCreateQuat ( const GXMat4 &mat )
{
	GXFloat		trace;
	GXFloat		s;
	GXFloat		t;
	GXInt		i;
	GXInt		j;
	GXInt		k;

	GXQuat v;

	static GXInt next[ 3 ] = { 1, 2, 0 };

	trace = mat.m[ 0 ][ 0 ] + mat.m[ 1 ][ 1 ] + mat.m[ 2 ][ 2 ];

	if ( trace > 0.0f ) 
	{
		t = trace + 1.0f;
		s = 1.0f / sqrtf ( t ) * 0.5f;

		v.v[ 3 ] = s * t;
		v.v[ 0 ] = ( mat.m[ 2 ][ 1 ] - mat.m[ 1 ][ 2 ] ) * s;
		v.v[ 1 ] = ( mat.m[ 0 ][ 2 ] - mat.m[ 2 ][ 0 ] ) * s;
		v.v[ 2 ] = ( mat.m[ 1 ][ 0 ] - mat.m[ 0 ][ 1 ] ) * s;
	}
	else
	{

		i = 0;
		if ( mat.m[ 1 ][ 1 ] > mat.m[ 0 ][ 0 ] ) 
			i = 1;

		if ( mat.m[ 2 ][ 2 ] > mat.m[ i ][ i ] ) 
			i = 2;

		j = next[ i ];
		k = next[ j ];

		t = ( mat.m[ i ][ i ] - ( mat.m[ j ][ j ] + mat.m[ k ][ k ] ) ) + 1.0f;
		s = 1.0f / sqrtf ( t ) * 0.5f;

		v.v[ i ] = s * t;
		v.v[ 3 ] = ( mat.m[ k ][ j ] - mat.m[ j ][ k ] ) * s;
		v.v[ j ] = ( mat.m[ j ][ i ] + mat.m[ i ][ j ] ) * s;
		v.v[ k ] = ( mat.m[ k ][ i ] + mat.m[ i ][ k ] ) * s;
	}

	return v;
}

GXVoid GXCALL GXQuatRehandCoordinateSystem ( GXQuat &out, const GXQuat &src )
{
	out = src;
	out.w = -src.w;
}

GXVoid GXCALL GXQuatToEulerAngles ( GXVec3 &out_rad, const GXQuat &q )
{
	//TODO
	GXFloat factor = q.z * q.z;

	out_rad.x = atan2f ( 2.0f * ( q.x * q.y + q.z * q.w ), 1.0f - 2.0f * ( q.y * q.y + factor ) );
	out_rad.y = asinf ( 2.0f * ( q.x * q.z - q.w * q.y ) );
	out_rad.z = atan2f ( 2.0f * ( q.x * q.w + q.y * q.z ), 1.0f - 2.0f * ( factor + q.w * q.w ) );
}

GXVoid GXCALL GXSetQuatIdentity ( GXQuat &out )
{
	out.x = out.y = out.z = 0.0f;
	out.w = 1.0f;
}

GXVoid GXCALL GXSetQuatRotationAxis ( GXQuat &out, GXFloat x, GXFloat y, GXFloat z, GXFloat angle )
{
	GXFloat sn = sinf ( angle * 0.5f );
	GXFloat cs = cosf ( angle * 0.5f );

	out.x = x * sn;
	out.y = y * sn;
	out.z = z * sn;
	out.w = cs;
}

GXVoid GXCALL GXSetQuatRotationAxis ( GXQuat &out, const GXVec3 &axis, GXFloat angle )
{
	GXFloat sn = sinf ( angle * 0.5f );
	GXFloat cs = cosf ( angle * 0.5f );

	out.x = axis.x * sn;
	out.y = axis.y * sn;
	out.z = axis.z * sn;
	out.w = cs;
}

GXVoid GXCALL GXMulQuatQuat ( GXQuat &out, const GXQuat &a, const GXQuat &b )
{
	GXFloat A, B, C, D, E, F, G, H;

	A = ( a.w + a.x ) * ( b.w + b.x );
	B = ( a.z - a.y ) * ( b.y - b.z );
	C = ( a.x - a.w ) * ( b.y + b.z );
	D = ( a.y + a.z ) * ( b.x - b.w );
	E = ( a.x + a.z ) * ( b.x + b.y );
	F = ( a.x - a.z ) * ( b.x - b.y );
	G = ( a.w + a.y ) * ( b.w - b.z );
	H = ( a.w - a.y ) * ( b.w + b.z );

	out.w =  B + ( -E - F + G + H ) * 0.5f;
	out.x =  A - (  E + F + G + H ) * 0.5f; 
	out.y = -C + (  E - F + G - H ) * 0.5f;
	out.z = -D + (  E - F - G + H ) * 0.5f;
}

GXVoid GXCALL GXSumQuatQuat ( GXQuat& out, const GXQuat &a, const GXQuat &b )
{
	out.x = a.x + b.x;
	out.y = a.y + b.y;
	out.z = a.z + b.z;
	out.w = a.w + b.w;
}

GXVoid GXCALL GXSubQuatQuat ( GXQuat &out, const GXQuat &a, const GXQuat &b )
{
	GXQuat bi;
	GXInverseQuat ( bi, b );
	GXMulQuatQuat ( out, bi, a );
}

GXVoid GXCALL GXQuatSLerp ( GXQuat &out, const GXQuat &a, const GXQuat &b, GXFloat k )
{
	GXQuat temp;
	GXFloat omega;
	GXFloat cosom;
	GXFloat sinom;
	GXFloat scale0;
	GXFloat scale1;

	if ( k <= 0.0f )
	{
		out = a;
		return;
	}

	if ( k >= 1.0f )
	{
		out = b;
		return;
	}

	cosom = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	if ( cosom < 0.0f )
	{
		temp.x = -b.x;
		temp.y = -b.y;
		temp.z = -b.z;
		temp.w = -b.w;
		cosom = -cosom;
	}
	else
	{
		temp = b;
	}

	if ( ( 1.0f - cosom ) > 1e-6f )
	{
		omega = acosf( cosom );
		sinom = 1.0f / sinf ( omega );
		scale0 = sinf ( ( 1.0f - k ) * omega ) * sinom;
		scale1 = sinf ( k * omega ) * sinom;
	}
	else
	{
		scale0 = 1.0f - k;
		scale1 = k;
	}

	out.x = a.x * scale0 + temp.x * scale1;
	out.y = a.y * scale0 + temp.y * scale1;
	out.z = a.z * scale0 + temp.z * scale1;
	out.w = a.w * scale0 + temp.w * scale1;
}

GXVoid GXCALL GXQuatTransform ( GXVec3 &out, const GXQuat &q, const GXVec3 &v )
{
	GXFloat xxzz = q.x * q.x - q.z * q.z;
	GXFloat wwyy = q.w * q.w - q.y * q.y;

	GXFloat xw2 = q.x * q.w * 2.0f;
	GXFloat xy2 = q.x * q.y * 2.0f;
	GXFloat xz2 = q.x * q.z * 2.0f;
	GXFloat yw2 = q.y * q.w * 2.0f;
	GXFloat yz2 = q.y * q.z * 2.0f;
	GXFloat zw2 = q.z * q.w * 2.0f;

	out.x = ( xxzz + wwyy ) *v.x	+ ( xy2 + zw2 ) * v.y										+ ( xz2 - yw2 ) * v.z;
	out.y = ( xy2 - zw2 ) * v.x		+ ( q.y * q.y + q.w * q.w - q.x * q.x - q.z * q.z ) * v.y	+ ( yz2 + xw2 ) * v.z;
	out.z = ( xz2 + yw2 ) * v.x		+ ( yz2 - xw2 ) * v.y										+ ( wwyy - xxzz ) * v.z;
}

GXVoid GXCALL GXInverseQuat ( GXQuat &out, const GXQuat &q )
{
	GXFloat lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if ( lenSq > 0.0f )
	{
		GXFloat invLenSq = 1.0f / lenSq;
		out.x = -q.x * invLenSq;
		out.y = -q.y * invLenSq;
		out.z = -q.z * invLenSq;
		out.w = q.w * invLenSq;
		return;
	}
	else
	{
		wprintf ( L"Œ¯Ë·Í‡ GXInverseQuat\n" );
		out.x = out.y = out.z = 0.0f;
		out.w = 1.0f;
	}
}

GXVoid GXCALL GXNormalizeQuat ( GXQuat &out, const GXQuat &q )
{
	GXFloat len = sqrtf ( q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w );
	if ( len )
	{
		GXFloat ilength = 1.0f / len;
		out.x *= ilength;
		out.y *= ilength;
		out.z *= ilength;
		out.w *= ilength;
	}
}

//------------------------------------------------------------------

GXAABB::GXAABB ()
{
	vertices = 0;

	min = GXVec3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	max = GXVec3 ( FLT_MIN, FLT_MIN, FLT_MIN );
}

GXFloat GXCALL GXGetAABBWidth ( const GXAABB &bounds )
{
	if ( bounds.vertices < 2 ) 
		return -1.0f;

	return bounds.max.x - bounds.min.x;
}

GXFloat GXCALL GXGetAABBHeight ( const GXAABB &bounds )
{
	if ( bounds.vertices < 2 ) 
		return -1.0f;

	return bounds.max.y - bounds.min.y;
}

GXFloat GXCALL GXGetAABBDepth ( const GXAABB &bounds )
{
	if ( bounds.vertices < 2 ) 
		return -1.0f;

	return bounds.max.z - bounds.min.z;
}

GXVoid GXCALL GXSetAABBEmpty ( GXAABB &inOut )
{
	inOut.vertices = 0;

	inOut.min = GXVec3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	inOut.max = GXVec3 ( FLT_MIN, FLT_MIN, FLT_MIN );
}

GXVoid GXCALL GXAddVertexToAABB ( GXAABB &inOut, const GXVec3 &vertex )
{
	GXAddVertexToAABB ( inOut, vertex.x, vertex.y, vertex.z );
}

GXVoid GXCALL GXAddVertexToAABB ( GXAABB &inOut, GXFloat x, GXFloat y, GXFloat z )
{
	if ( inOut.vertices > 1 )
	{
		if ( inOut.min.x > x )
			inOut.min.x = x;
		else if ( inOut.max.x < x )
			inOut.max.x = x;

		if ( inOut.min.y > y )
			inOut.min.y = y;
		else if ( inOut.max.y < y )
			inOut.max.y = y;

		if ( inOut.min.z > z )
			inOut.min.z = z;
		else if ( inOut.max.z < z )
			inOut.max.z = z;

		return;
	}
	else if ( inOut.vertices == 0 )
	{
		inOut.vertices++;

		inOut.min = GXCreateVec3 ( x, y, z );
		return;
	}

	inOut.vertices++;

	if ( inOut.min.x > x )
	{
		inOut.max.x = inOut.min.x;
		inOut.min.x = x;
	}
	else
		inOut.max.x = x;

	if ( inOut.min.y > y )
	{
		inOut.max.y = inOut.min.y;
		inOut.min.y = y;
	}
	else
		inOut.max.y = y;

	if ( inOut.min.z > z )
	{
		inOut.max.z = inOut.min.z;
		inOut.min.z = z;
	}
	else
		inOut.max.z = z;
}

GXBool GXCALL GXIsOverlapedAABBAABB ( const GXAABB& a, const GXAABB& b )
{
	if ( a.min.x > b.max.x ) return GX_FALSE;
	if ( a.min.y > b.max.y ) return GX_FALSE;
	if ( a.min.z > b.max.z ) return GX_FALSE;

	if ( b.min.x > a.max.x ) return GX_FALSE;
	if ( b.min.y > a.max.y ) return GX_FALSE;
	if ( b.min.z > a.max.z ) return GX_FALSE;

	return GX_TRUE;
}

GXBool GXCALL GXIsOverlapedAABBVec3 ( const GXAABB &a, const GXVec3 &b )
{
	if ( a.min.x > b.x ) return GX_FALSE;
	if ( a.min.y > b.y ) return GX_FALSE;
	if ( a.min.z > b.z ) return GX_FALSE;

	if ( a.max.x < b.x ) return GX_FALSE;
	if ( a.max.y < b.y ) return GX_FALSE;
	if ( a.max.z < b.z ) return GX_FALSE;

	return GX_TRUE;
}

GXBool GXCALL GXIsOverlapedAABBVec3 ( const GXAABB &a, GXFloat x, GXFloat y, GXFloat z )
{
	if ( a.min.x > x ) return GX_FALSE;
	if ( a.min.y > y ) return GX_FALSE;
	if ( a.min.z > z ) return GX_FALSE;

	if ( a.max.x < x ) return GX_FALSE;
	if ( a.max.y < y ) return GX_FALSE;
	if ( a.max.z < z ) return GX_FALSE;

	return GX_TRUE;
}

GXVoid GXCALL GXGetAABBCenter ( GXVec3 &center, const GXAABB &bounds )
{
	center.x = ( bounds.min.x + bounds.max.x ) * 0.5f;
	center.y = ( bounds.min.y + bounds.max.y ) * 0.5f;
	center.z = ( bounds.min.z + bounds.max.z ) * 0.5f;
}

GXFloat GXCALL GXGetAABBSphereRadius ( const GXAABB &bounds )
{
	GXVec3 center;
	GXGetAABBCenter ( center, bounds );
	return GXDistanceVec3Vec3 ( center, bounds.min );
}

//------------------------------------------------------------------

eGXPlaneClassifyVertex GXCALL GXPlaneClassifyVertex ( const GXPlane &plane, const GXVec3 &vertex )
{
	GXFloat test = plane.a * vertex.x + plane.b * vertex.y + plane.c * vertex.z + plane.d;

	if ( test < 0.0f ) return GX_CLASSIFY_BEHIND;
	if ( test > 0.0f ) return GX_CLASSIFY_IN_FRONT;
	return GX_CLASSIFY_ON;
}

eGXPlaneClassifyVertex GXCALL GXPlaneClassifyVertex ( const GXPlane &plane, GXFloat x, GXFloat y, GXFloat z )
{
	GXFloat test = plane.a * x + plane.b * y + plane.c * z + plane.d;

	if ( test < 0.0f ) return GX_CLASSIFY_BEHIND;
	if ( test > 0.0f ) return GX_CLASSIFY_IN_FRONT;
	return GX_CLASSIFY_ON;
}

GXVoid GXCALL GXNormalizePlane ( GXPlane &inOut )
{
	GXFloat length = sqrtf ( inOut.a * inOut.a + inOut.b * inOut.b + inOut.c * inOut.c );

	inOut.a /= length;
	inOut.b /= length;
	inOut.c /= length;
	inOut.d /= length;
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

GXVoid GXProjectionClipPlanes::From ( const GXMat4 &m )
{
	//Left clipping plane
	planes[ 0 ].a = m.m14 + m.m11;
	planes[ 0 ].b = m.m24 + m.m21;
	planes[ 0 ].c = m.m34 + m.m31;
	planes[ 0 ].d = m.m44 + m.m41;
	
	//Right clipping plane
	planes[ 1 ].a = m.m14 - m.m11;
	planes[ 1 ].b = m.m24 - m.m21;
	planes[ 1 ].c = m.m34 - m.m31;
	planes[ 1 ].d = m.m44 - m.m41;
	
	//Top clipping plane
	planes[ 2 ].a = m.m14 - m.m12;
	planes[ 2 ].b = m.m24 - m.m22;
	planes[ 2 ].c = m.m34 - m.m32;
	planes[ 2 ].d = m.m44 - m.m42;
	
	//Bottom clipping plane
	planes[ 3 ].a = m.m14 + m.m12;
	planes[ 3 ].b = m.m24 + m.m22;
	planes[ 3 ].c = m.m34 + m.m32;
	planes[ 3 ].d = m.m44 + m.m42;
	
	//Near clipping plane
	planes[ 4 ].a = m.m14 + m.m13;
	planes[ 4 ].b = m.m24 + m.m23;
	planes[ 4 ].c = m.m34 + m.m33;
	planes[ 4 ].d = m.m44 + m.m43;
	
	//Far clipping plane
	planes[ 5 ].a = m.m14 - m.m13;
	planes[ 5 ].b = m.m24 - m.m23;
	planes[ 5 ].c = m.m34 - m.m33;
	planes[ 5 ].d = m.m44 - m.m43;
}

GXBool GXProjectionClipPlanes::IsVisible ( const GXAABB &bounds )
{
	//TODO
	GXUByte flags = PlaneTest ( bounds.min.x, bounds.min.y, bounds.min.z );
	flags &= PlaneTest ( bounds.min.x, bounds.max.y, bounds.min.z );
	flags &= PlaneTest ( bounds.max.x, bounds.max.y, bounds.min.z );
	flags &= PlaneTest ( bounds.max.x, bounds.min.y, bounds.min.z );

	flags &= PlaneTest ( bounds.min.x, bounds.min.y, bounds.max.z );
	flags &= PlaneTest ( bounds.min.x, bounds.max.y, bounds.max.z );
	flags &= PlaneTest ( bounds.max.x, bounds.max.y, bounds.max.z );
	flags &= PlaneTest ( bounds.max.x, bounds.min.y, bounds.max.z );

	return ( flags > 0 ) ? GX_FALSE : GX_TRUE;
}

GXUByte	GXProjectionClipPlanes::PlaneTest ( GXFloat x, GXFloat y, GXFloat z )
{
	GXUByte flags = 0;

	for ( GXUByte i = 0; i < 6; i++ )
	{
		if ( GXPlaneClassifyVertex ( planes[ i ], x, y, z ) == GX_CLASSIFY_BEHIND )
			flags |= (GXUByte)( 1 << i );
	}

	return flags;
}

//------------------------------------------------------------------

GXFloat GXCALL GXDegToRad ( GXFloat degrees )
{
	return degrees * 0.0174533f;	//degrees * pi / 180
}

GXVoid GXCALL GXColorToVec4 ( GXVec4 &out, GXUChar r, GXUChar g, GXUChar b, GXUChar a )
{
	#define GX_FACTOR 0.00392157f

	out.r = r * GX_FACTOR;
	out.g = g * GX_FACTOR;
	out.b = b * GX_FACTOR;
	out.a = a * GX_FACTOR;

	#undef GX_FACTOR
}

GXVoid GXCALL GXConvert3DSMaxToGXEngine ( GXVec3 &gx_out, GXFloat max_x, GXFloat max_y, GXFloat max_z )
{
	gx_out.x = -max_x;
	gx_out.y = max_z;
	gx_out.z = -max_y;
}

GXFloat GXCALL GXRandomNormalize ()
{
	#define INV_RAND_MAX		3.05185e-5f;

	return rand () * INV_RAND_MAX;

	#undef INV_RAND_MAX
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
	GXSubVec3Vec3 ( a, *v1, *v0 );
	GXSubVec3Vec3 ( b, *v2, *v0 );

	GXVec2 dUVa;
	GXVec2 dUVb;
	GXSubVec2Vec2 ( dUVa, *uv1, *uv0 );
	GXSubVec2Vec2 ( dUVb, *uv2, *uv0 );

	GXFloat factor = 1.0f / ( dUVa.u * dUVb.v - dUVb.u * dUVa.v );

	outTangent.x = factor * ( dUVb.v * a.x - dUVa.v * b.x );
	outTangent.y = factor * ( dUVb.v * a.y - dUVa.v * b.y );
	outTangent.z = factor * ( dUVb.v * a.z - dUVa.v * b.z );
	GXNormalizeVec3 ( outTangent );

	outBitangent.x = factor * ( -dUVb.u * a.x + dUVa.u * b.x );
	outBitangent.y = factor * ( -dUVb.u * a.y + dUVa.u * b.y );
	outBitangent.z = factor * ( -dUVb.u * a.z + dUVa.u * b.z );
	GXNormalizeVec3 ( outBitangent );
}

GXFloat GXCALL GXClampf ( GXFloat value, GXFloat minValue, GXFloat maxValue )
{
	return ( value < minValue ) ? minValue : ( value > maxValue ) ? maxValue : value;
}

GXUInt GXCALL GXClampi ( GXInt value, GXInt minValue, GXInt maxValue )
{
	return ( value < minValue ) ? minValue : ( value > maxValue ) ? maxValue : value;
}
