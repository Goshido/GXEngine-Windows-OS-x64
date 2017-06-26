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
	currentFrameModelMatrix.wv = currentFrameLocation;

	TransformUpdated ();
}

GXVoid GXTransform::SetLocation ( const GXVec3 &loc )
{
	currentFrameLocation = loc;
	currentFrameModelMatrix.wv = currentFrameLocation;

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXVec3 &rot_rad  )
{
	GXSetMat4RotationXYZ ( currentFrameRotationMatrix, rot_rad.pitch_rad, rot_rad.yaw_rad, rot_rad.roll_rad );
	
	GXMulVec3Scalar ( currentFrameModelMatrix.xv, currentFrameRotationMatrix.xv, currentFrameScale.x );
	GXMulVec3Scalar ( currentFrameModelMatrix.yv, currentFrameRotationMatrix.yv, currentFrameScale.y );
	GXMulVec3Scalar ( currentFrameModelMatrix.zv, currentFrameRotationMatrix.zv, currentFrameScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXMat4 &rot )
{
	currentFrameRotationMatrix = rot;

	GXMulVec3Scalar ( currentFrameModelMatrix.xv, currentFrameRotationMatrix.xv, currentFrameScale.x );
	GXMulVec3Scalar ( currentFrameModelMatrix.yv, currentFrameRotationMatrix.yv, currentFrameScale.y );
	GXMulVec3Scalar ( currentFrameModelMatrix.zv, currentFrameRotationMatrix.zv, currentFrameScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( currentFrameRotationMatrix, pitch_rad, yaw_rad, roll_rad );

	GXMulVec3Scalar ( currentFrameModelMatrix.xv, currentFrameRotationMatrix.xv, currentFrameScale.x );
	GXMulVec3Scalar ( currentFrameModelMatrix.yv, currentFrameRotationMatrix.yv, currentFrameScale.y );
	GXMulVec3Scalar ( currentFrameModelMatrix.zv, currentFrameRotationMatrix.zv, currentFrameScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXQuat &quaternion )
{
	currentFrameRotationMatrix.SetRotation ( quaternion );

	GXMulVec3Scalar ( currentFrameModelMatrix.xv, currentFrameRotationMatrix.xv, currentFrameScale.x );
	GXMulVec3Scalar ( currentFrameModelMatrix.yv, currentFrameRotationMatrix.yv, currentFrameScale.y );
	GXMulVec3Scalar ( currentFrameModelMatrix.zv, currentFrameRotationMatrix.zv, currentFrameScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetScale ( GXFloat x, GXFloat y, GXFloat z )
{
	currentFrameScale = GXCreateVec3 ( x, y, z );

	GXMulVec3Scalar ( currentFrameModelMatrix.xv, currentFrameRotationMatrix.xv, currentFrameScale.x );
	GXMulVec3Scalar ( currentFrameModelMatrix.yv, currentFrameRotationMatrix.yv, currentFrameScale.y );
	GXMulVec3Scalar ( currentFrameModelMatrix.zv, currentFrameRotationMatrix.zv, currentFrameScale.z );

	TransformUpdated ();
}

GXVoid GXTransform::SetScale ( const GXVec3 &scale )
{
	currentFrameScale = scale;

	GXMulVec3Scalar ( currentFrameModelMatrix.xv, currentFrameRotationMatrix.xv, currentFrameScale.x );
	GXMulVec3Scalar ( currentFrameModelMatrix.yv, currentFrameRotationMatrix.yv, currentFrameScale.y );
	GXMulVec3Scalar ( currentFrameModelMatrix.zv, currentFrameRotationMatrix.zv, currentFrameScale.z );

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
