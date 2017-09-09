//version 1.1

#include <GXEngine/GXTransform.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXMemory.h>


GXTransform	GXTransform::nullTransform;

GXTransform::GXTransform ()
{
	currentFrameLocation.Init ( 0.0f, 0.0f, 0.0f );
	currentFrameScale.Init ( 1.0f, 1.0f, 1.0f );
	currentFrameRotationMatrix.Identity ();
	currentFrameModelMatrix.Identity ();
	lastFrameModelMatrix.Identity ();
}

GXTransform::~GXTransform ()
{
	// NOTHING
}

GXVoid GXTransform::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	currentFrameLocation.Init ( x, y, z );
	currentFrameModelMatrix.SetW ( currentFrameLocation );

	TransformUpdated ();
}

GXVoid GXTransform::SetLocation ( const GXVec3 &loc )
{
	currentFrameLocation = loc;
	currentFrameModelMatrix.SetW ( currentFrameLocation );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXEuler &rotation  )
{
	SetRotation ( rotation.pitchRadians, rotation.yawRadians, rotation.rollRadians );
}

GXVoid GXTransform::SetRotation ( const GXMat4 &rot )
{
	currentFrameRotationMatrix = rot;

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 0 ] );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 1 ] );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 2 ] );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
	currentFrameRotationMatrix.RotationXYZ ( pitchRadians, yawRadians, rollRadians );

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 0 ] );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 1 ] );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 2 ] );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXQuat &quaternion )
{
	currentFrameRotationMatrix.SetRotation ( quaternion );

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 0 ] );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 1 ] );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 2 ] );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::SetScale ( GXFloat x, GXFloat y, GXFloat z )
{
	currentFrameScale.Init ( x, y, z );

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 0 ] );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 1 ] );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 2 ] );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::SetScale ( const GXVec3 &scale )
{
	currentFrameScale = scale;

	GXVec3 tmpA;
	GXVec3 tmpB;

	currentFrameRotationMatrix.GetX ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 0 ] );
	currentFrameModelMatrix.SetX ( tmpB );

	currentFrameRotationMatrix.GetY ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 1 ] );
	currentFrameModelMatrix.SetY ( tmpB );

	currentFrameRotationMatrix.GetZ ( tmpA );
	tmpB.Multiply ( tmpA, currentFrameScale.data[ 2 ] );
	currentFrameModelMatrix.SetZ ( tmpB );

	TransformUpdated ();
}

GXVoid GXTransform::GetLocation ( GXVec3 &loc ) const
{
	loc = currentFrameLocation;
}

const GXVec3& GXTransform::GetLocation () const
{
	return currentFrameLocation;
}

GXVoid GXTransform::GetRotation ( GXMat4 &rot ) const
{
	rot = currentFrameRotationMatrix;
}

GXVoid GXTransform::GetRotation ( GXQuat &rot ) const
{
	rot.From ( currentFrameRotationMatrix );
}

const GXMat4& GXTransform::GetRotation () const
{
	return currentFrameRotationMatrix;
}

GXVoid GXTransform::GetScale ( GXVec3 &scale ) const
{
	scale = currentFrameScale;
}

const GXVec3& GXTransform::GetScale () const
{
	return currentFrameScale;
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
