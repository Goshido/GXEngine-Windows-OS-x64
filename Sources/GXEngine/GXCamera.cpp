//version 1.12

#include <GXEngine/GXCamera.h>


#define DEFAULT_Z_NEAR		-1.0f
#define DEFAULT_Z_FAR		1.0f


GXCamera* GXCamera::activeCamera = nullptr;

GXCamera::GXCamera ()
{
	znear = DEFAULT_Z_NEAR;
	zfar = DEFAULT_Z_FAR;

	GXSetMat4Identity ( currentFrameModelMatrix );
	GXSetMat4Identity ( currentFrameViewMatrix );
	GXSetMat4Identity ( currentFrameProjectionMatrix );
	GXSetMat4Identity ( currentFrameInverseProjectionMatrix );
	GXSetMat4Identity ( currentFrameViewProjectionMatrix );
	GXSetMat4Identity ( lastFrameViewMatrix );
	GXSetMat4Identity ( lastFrameViewProjectionMatrix );

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
	currentFrameModelMatrix.wv = GXCreateVec3 ( x, y, z );
	GXSetMat4Inverse ( currentFrameViewMatrix, currentFrameModelMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetLocation ( const GXVec3 &location )
{
	currentFrameModelMatrix.wv = location;
	GXSetMat4Inverse ( currentFrameViewMatrix, currentFrameModelMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXVec3 location = currentFrameModelMatrix.wv;

	GXSetMat4RotationXYZ ( currentFrameModelMatrix, pitch_rad, yaw_rad, roll_rad );
	currentFrameModelMatrix.wv = location;

	GXSetMat4Inverse ( currentFrameViewMatrix, currentFrameModelMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXMat4 &rotation )
{
	GXVec3 location = currentFrameModelMatrix.wv;

	currentFrameModelMatrix = rotation;
	currentFrameModelMatrix.wv = location;

	GXSetMat4Inverse ( currentFrameViewMatrix, currentFrameModelMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXQuat &rotation )
{
	GXVec3 location = currentFrameModelMatrix.wv;
	currentFrameModelMatrix.From ( rotation, location );

	GXSetMat4Inverse ( currentFrameViewMatrix, currentFrameModelMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetCurrentFrameModelMatrix ( const GXMat4 &matrix )
{
	currentFrameModelMatrix = matrix;
	GXSetMat4Inverse ( currentFrameViewMatrix, currentFrameModelMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::GetLocation ( GXVec3& outLocation ) const
{
	outLocation = currentFrameViewMatrix.wv;
}

GXVoid GXCamera::GetRotation ( GXMat4 &out ) const
{
	out = currentFrameModelMatrix;
	out.wv = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
}

GXVoid GXCamera::GetRotation ( GXQuat &out ) const
{
	out = GXCreateQuat ( currentFrameModelMatrix );
}

const GXProjectionClipPlanes& GXCamera::GetClipPlanesWorld ()
{
	return clipPlanesWorld;
}

GXBool GXCamera::IsObjectVisible ( const GXAABB objectBoundsWorld )
{
	return clipPlanesWorld.IsVisible ( objectBoundsWorld );
}

GXFloat GXCamera::GetZnear () const
{
	return znear;
}

GXFloat GXCamera::GetZfar () const
{
	return zfar;
}

GXVoid GXCamera::UpdateLastFrameMatrices ()
{
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