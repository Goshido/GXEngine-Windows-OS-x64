//version 1.0

#include <GXEngine/GXTransform.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXMemory.h>


GXTransform	GXTransform::nullTransform;

GXTransform::GXTransform ()
{
	currentLocation = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	currentScale = GXCreateVec3 ( 1.0f, 1.0f, 1.0f );
	GXSetMat4Identity ( currentRotationMatrix );
	GXSetMat4Identity ( currentModelMatrix );
	GXSetMat4Identity ( lastFrameModelMatrix );
}

GXTransform::~GXTransform ()
{
	// NOTHING
}

GXVoid GXTransform::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	currentLocation = GXCreateVec3 ( x, y, z );
	currentModelMatrix.wv = currentLocation;

	TransformUpdated ();
}

GXVoid GXTransform::SetLocation ( const GXVec3 &loc )
{
	currentLocation = loc;
	currentModelMatrix.wv = currentLocation;

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXVec3 &rot_rad  )
{
	GXSetMat4RotationXYZ ( currentRotationMatrix, rot_rad.pitch_rad, rot_rad.yaw_rad, rot_rad.roll_rad );
	
	GXMulVec3Scalar ( currentModelMatrix.xv, currentRotationMatrix.xv, currentScale.x );
	GXMulVec3Scalar ( currentModelMatrix.yv, currentRotationMatrix.yv, currentScale.y );
	GXMulVec3Scalar ( currentModelMatrix.zv, currentRotationMatrix.zv, currentScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXMat4 &rot )
{
	currentRotationMatrix = rot;

	GXMulVec3Scalar ( currentModelMatrix.xv, currentRotationMatrix.xv, currentScale.x );
	GXMulVec3Scalar ( currentModelMatrix.yv, currentRotationMatrix.yv, currentScale.y );
	GXMulVec3Scalar ( currentModelMatrix.zv, currentRotationMatrix.zv, currentScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( currentRotationMatrix, pitch_rad, yaw_rad, roll_rad );

	GXMulVec3Scalar ( currentModelMatrix.xv, currentRotationMatrix.xv, currentScale.x );
	GXMulVec3Scalar ( currentModelMatrix.yv, currentRotationMatrix.yv, currentScale.y );
	GXMulVec3Scalar ( currentModelMatrix.zv, currentRotationMatrix.zv, currentScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXQuat &quaternion )
{
	currentRotationMatrix.SetRotation ( quaternion );

	GXMulVec3Scalar ( currentModelMatrix.xv, currentRotationMatrix.xv, currentScale.x );
	GXMulVec3Scalar ( currentModelMatrix.yv, currentRotationMatrix.yv, currentScale.y );
	GXMulVec3Scalar ( currentModelMatrix.zv, currentRotationMatrix.zv, currentScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetScale ( GXFloat x, GXFloat y, GXFloat z )
{
	currentScale = GXCreateVec3 ( x, y, z );

	GXMulVec3Scalar ( currentModelMatrix.xv, currentRotationMatrix.xv, currentScale.x );
	GXMulVec3Scalar ( currentModelMatrix.yv, currentRotationMatrix.yv, currentScale.y );
	GXMulVec3Scalar ( currentModelMatrix.zv, currentRotationMatrix.zv, currentScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetScale ( const GXVec3 &scale )
{
	currentScale = scale;

	GXMulVec3Scalar ( currentModelMatrix.xv, currentRotationMatrix.xv, currentScale.x );
	GXMulVec3Scalar ( currentModelMatrix.yv, currentRotationMatrix.yv, currentScale.y );
	GXMulVec3Scalar ( currentModelMatrix.zv, currentRotationMatrix.zv, currentScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::GetLocation ( GXVec3 &loc ) const
{
	loc = currentLocation;
} 

GXVoid GXTransform::GetRotation ( GXMat4 &rot ) const
{
	rot = currentRotationMatrix;
}

GXVoid GXTransform::GetRotation ( GXQuat &rot ) const
{
	rot = GXCreateQuat ( currentRotationMatrix );
}

GXVoid GXTransform::GetScale ( GXVec3 &scale ) const
{
	scale = currentScale;
}

const GXMat4& GXTransform::GetCurrentModelMatrix () const
{
	return currentModelMatrix;
}

const GXMat4& GXTransform::GetLastFrameModelMatrix () const
{
	return lastFrameModelMatrix;
}

GXVoid GXTransform::UpdateLastFrameModelMatrix ()
{
	lastFrameModelMatrix = currentModelMatrix;
}

const GXTransform& GXTransform::GetNullTransform ()
{
	return nullTransform;
}

GXVoid GXTransform::TransformUpdated ()
{
	//NOTHING
}
