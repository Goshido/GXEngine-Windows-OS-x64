#include <GXCommon/GXMath.h>
#include <GXCommon/GXLogger.h>


int wmain ( int /*argc*/, const wchar_t** /*argv*/ )
{
	GXVec3 v ( 1.0f, 0.0f, 0.0f );

	GXVec3 rotationAxis ( 0.0f, 1.0f, 0.0f );
	rotationAxis.Normalize ();

	GXQuat rotationQuaternion;
	rotationQuaternion.FromAxisAngle ( rotationAxis, GXDegToRad ( 180.0f ) );

	GXVec3 rotatedV0;
	rotationQuaternion.Transform ( rotatedV0, v );

	GXMat3 rotationMatrix;
	rotationMatrix.From ( rotationQuaternion );

	GXVec3 rotatedV1;
	rotationMatrix.Multiply ( rotatedV1, v );

	GXQuat q ( rotationMatrix );

	GXFloat vl = v.Length ();
	GXFloat rotatedV0l = rotatedV0.Length ();
	GXFloat rotatedV1l = rotatedV1.Length ();

	GXLogA ( "%f %f %f\n", vl, rotatedV0l, rotatedV1l );

	return 0;
}
