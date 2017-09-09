//version 1.13

#include <GXEngine/GXCamera.h>


#define DEFAULT_Z_NEAR		-1.0f
#define DEFAULT_Z_FAR		1.0f


GXCamera* GXCamera::activeCamera = nullptr;

GXCamera::GXCamera ()
{
	zNear = DEFAULT_Z_NEAR;
	zFar = DEFAULT_Z_FAR;

	currentFrameModelMatrix.Identity ();
	currentFrameViewMatrix.Identity ();
	currentFrameProjectionMatrix.Identity ();
	currentFrameInverseProjectionMatrix.Identity ();
	currentFrameViewProjectionMatrix.Identity ();
	currentFrameInverseViewProjectionMatrix.Identity ();
	lastFrameModelMatrix.Identity ();
	lastFrameViewMatrix.Identity ();
	lastFrameViewProjectionMatrix.Identity ();

	UpdateClipPlanes ();
}

GXCamera::~GXCamera ()
{
	//NOTHING
}

const GXMat4& GXCamera::GetCurrentFrameViewProjectionMatrix () const
{
	return currentFrameViewProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameInverseViewProjectionMatrix () const
{
	return currentFrameInverseViewProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameProjectionMatrix () const
{
	return currentFrameProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameInverseProjectionMatrix () const
{
	return currentFrameInverseProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameModelMatrix () const
{
	return currentFrameModelMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameViewMatrix () const
{
	return currentFrameViewMatrix;
}

const GXMat4& GXCamera::GetLastFrameModelMatrix () const
{
	return lastFrameModelMatrix;
}

const GXMat4& GXCamera::GetLastFrameViewMatrix () const
{
	return lastFrameViewMatrix;
}

const GXMat4& GXCamera::GetLastFrameViewProjectionMatrix () const
{
	return lastFrameViewProjectionMatrix;
}

GXVoid GXCamera::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	currentFrameModelMatrix.SetW ( GXVec3 ( x, y, z ) );
	currentFrameViewMatrix.Inverse ( currentFrameModelMatrix );
	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetLocation ( const GXVec3 &location )
{
	currentFrameModelMatrix.SetW ( location );
	currentFrameViewMatrix.Inverse ( currentFrameModelMatrix );
	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXVec3 location;
	currentFrameModelMatrix.GetW ( location );

	currentFrameModelMatrix.RotationXYZ ( pitch_rad, yaw_rad, roll_rad );
	currentFrameModelMatrix.SetW ( location );

	currentFrameViewMatrix.Inverse ( currentFrameModelMatrix );
	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXMat4 &rotation )
{
	GXVec3 location;
	currentFrameModelMatrix.GetW ( location );

	currentFrameModelMatrix = rotation;
	currentFrameModelMatrix.SetW ( location );

	currentFrameViewMatrix.Inverse ( currentFrameModelMatrix );
	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXQuat &rotation )
{
	GXVec3 location;
	currentFrameModelMatrix.GetW ( location );
	currentFrameModelMatrix.From ( rotation, location );

	currentFrameViewMatrix.Inverse ( currentFrameModelMatrix );
	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetCurrentFrameModelMatrix ( const GXMat4 &matrix )
{
	currentFrameModelMatrix = matrix;

	currentFrameViewMatrix.Inverse ( currentFrameModelMatrix );
	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::GetLocation ( GXVec3& outLocation ) const
{
	currentFrameModelMatrix.GetW ( outLocation );
}

GXVoid GXCamera::GetRotation ( GXMat4 &out ) const
{
	static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );

	out = currentFrameModelMatrix;
	out.SetW ( origin );
}

GXVoid GXCamera::GetRotation ( GXQuat &out ) const
{
	out.From ( currentFrameModelMatrix );
}

const GXProjectionClipPlanes& GXCamera::GetClipPlanesWorld ()
{
	return clipPlanesWorld;
}

GXBool GXCamera::IsObjectVisible ( const GXAABB objectBoundsWorld )
{
	return clipPlanesWorld.IsVisible ( objectBoundsWorld );
}

GXFloat GXCamera::GetZNear () const
{
	return zNear;
}

GXFloat GXCamera::GetZFar () const
{
	return zFar;
}

GXVoid GXCamera::UpdateLastFrameMatrices ()
{
	lastFrameModelMatrix = currentFrameModelMatrix;
	lastFrameViewMatrix = currentFrameViewMatrix;
	lastFrameViewProjectionMatrix = currentFrameViewProjectionMatrix;
}

GXCamera* GXCALL GXCamera::GetActiveCamera ()
{
	return activeCamera;
}

GXVoid GXCALL GXCamera::SetActiveCamera ( GXCamera* camera )
{
	activeCamera = camera;
}

GXVoid GXCamera::UpdateClipPlanes ()
{
	clipPlanesWorld.From ( currentFrameViewProjectionMatrix );
}