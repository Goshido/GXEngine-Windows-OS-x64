//version 1.1

#include <GXEngine/GXTransform.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXMemory.h>


GXTransform	GXTransform::nullTransform;

GXTransform::GXTransform ()
{
	currentFrameLocation = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	currentFrameScale = GXCreateVec3 ( 1.0f, 1.0f, 1.0f );
	GXSetMat4Identity ( currentFrameRotationMatrix );
	GXSetMat4Identity ( currentFrameModelMatrix );
	GXSetMat4Identity ( lastFrameModelMatrix );
}

GXTransform::~GXTransform ()
{
	// NOTHING
}

GXVoid GXTransform::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	currentFrameLocation = GXCreateVec3 ( x, y, z );
	currentFrameModelMatrix.SetW ( currentFrameLocation );

	TransformUpdated ();
}

GXVoid GXTransform::SetLocation ( const GXVec3 &loc )
{
	currentFrameLocation = loc;
	currentFrameModelMatrix.SetW ( currentFrameLocation );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXVec3 &rot_rad  )
{
	GXSetMat4RotationXYZ ( currentFrameRotationMatrix, rot_rad.pitch_rad, rot_rad.yaw_rad, rot_rad.roll_rad );
	
	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.x );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.y );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.z );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXMat4 &rot )
{
	currentFrameRotationMatrix = rot;

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.x );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.y );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.z );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( currentFrameRotationMatrix, pitch_rad, yaw_rad, roll_rad );

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.x );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.y );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.z );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXQuat &quaternion )
{
	currentFrameRotationMatrix.SetRotation ( quaternion );

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.x );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.y );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.z );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::SetScale ( GXFloat x, GXFloat y, GXFloat z )
{
	currentFrameScale = GXCreateVec3 ( x, y, z );

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.x );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.y );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.z );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::SetScale ( const GXVec3 &scale )
{
	currentFrameScale = scale;

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.x );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.y );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	GXMulVec3Scalar ( tmpB, tmpA, currentFrameScale.z );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::GetLocation ( GXVec3 &loc ) const
{
	loc = currentFrameLocation;
} 

GXVoid GXTransform::GetRotation ( GXMat4 &rot ) const
{
	rot = currentFrameRotationMatrix;
}

GXVoid GXTransform::GetRotation ( GXQuat &rot ) const
{
	rot = GXCreateQuat ( currentFrameRotationMatrix );
}

GXVoid GXTransform::GetScale ( GXVec3 &scale ) const
{
	scale = currentFrameScale;
}

const GXMat4& GXTransform::GetCurrentFrameModelMatrix () const
{
	return currentFrameModelMatrix;
}

const GXMat4& GXTransform::GetLastFrameModelMatrix () const
{
	return lastFrameModelMatrix;
}

GXVoid GXTransform::UpdateLastFrameModelMatrix ()
{
	lastFrameModelMatrix = currentFrameModelMatrix;
}

const GXTransform& GXTransform::GetNullTransform ()
{
	return nullTransform;
}

GXVoid GXTransform::TransformUpdated ()
{
	//NOTHING
}
