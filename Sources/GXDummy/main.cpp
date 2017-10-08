#include <GXCommon/GXMath.h>
#include <GXCommon/GXLogger.h>


int wmain ( int /*argc*/, const wchar_t** /*argv*/ )
{
	GXMat4 m;
	m.m[ 0 ][ 0 ] = 1.0f;
	m.m[ 0 ][ 1 ] = -0.0f;
	m.m[ 0 ][ 2 ] = -0.0f;
	m.m[ 0 ][ 3 ] = 0.0f;

	m.m[ 1 ][ 0 ] = 0.0f;
	m.m[ 1 ][ 1 ] = 0.994846f;
	m.m[ 1 ][ 2 ] = 0.101396f;
	m.m[ 1 ][ 3 ] = 0.0f;

	m.m[ 2 ][ 0 ] = 0.0f;
	m.m[ 2 ][ 1 ] = -0.101396f;
	m.m[ 2 ][ 2 ] = 0.994846f;
	m.m[ 2 ][ 3 ] = 0.0f;

	m.m[ 3 ][ 0 ] = 13.349850f;
	m.m[ 3 ][ 1 ] = -0.000023f;
	m.m[ 3 ][ 2 ] = 0.000008f;
	m.m[ 3 ][ 3 ] = 1.0f;

	GXQuat q ( m );

	GXMat4 m1;
	m1.From ( q, GXVec3 ( 13.349850f, -0.000023f, 0.000008f ) );

	GXQuat q1 (q);
	q1.Normalize ();

	GXMat4 m2;
	m2.From ( q1, GXVec3 ( 13.349850f, -0.000023f, 0.000008f ) );

	return 0;
}
