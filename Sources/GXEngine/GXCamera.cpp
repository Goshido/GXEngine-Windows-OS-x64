//version 1.11

#include <GXEngine/GXCamera.h>


#define DEFAULT_Z_NEAR		-1.0f
#define DEFAULT_Z_FAR		1.0f


GXCamera* GXCamera::activeCamera = nullptr;

GXCamera::GXCamera ()
{
	znear = DEFAULT_Z_NEAR;
	zfar = DEFAULT_Z_FAR;

	GXSetMat4Identity ( currentModelMatrix );
	GXSetMat4Identity ( currentViewMatrix );
	GXSetMat4Identity ( currentProjectionMatrix );
	GXSetMat4Identity ( currentInverseProjectionMatrix );
	GXSetMat4Identity ( currentViewProjectionMatrix );
	GXSetMat4Identity ( lastFrameViewMatrix );

	UpdateClipPlanes ();
}

GXCamera::~GXCamera ()
{
	//NOTHING
}

const GXMat4& GXCamera::GetCurrentViewProjectionMatrix () const
{
	return currentViewProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentProjectionMatrix () const
{
	return currentProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentInverseProjectionMatrix () const
{
	return currentInverseProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentModelMatrix () const
{
	return currentModelMatrix;
}

const GXMat4& GXCamera::GetCurrentViewMatrix () const
{
	return currentViewMatrix;
}

const GXMat4& GXCamera::GetLastFrameViewMatrix () const
{
	return lastFrameViewMatrix;
}

GXVoid GXCamera::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	currentModelMatrix.wv = GXCreateVec3 ( x, y, z );
	GXSetMat4Inverse ( currentViewMatrix, currentModelMatrix );
	GXMulMat4Mat4 ( currentViewProjectionMatrix, currentViewMatrix, currentProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetLocation ( const GXVec3 &location )
{
	currentModelMatrix.wv = location;
	GXSetMat4Inverse ( currentViewMatrix, currentModelMatrix );
	GXMulMat4Mat4 ( currentViewProjectionMatrix, currentViewMatrix, currentProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXVec3 location = currentModelMatrix.wv;

	GXSetMat4RotationXYZ ( currentModelMatrix, pitch_rad, yaw_rad, roll_rad );
	currentModelMatrix.wv = location;

	GXSetMat4Inverse ( currentViewMatrix, currentModelMatrix );
	GXMulMat4Mat4 ( currentViewProjectionMatrix, currentViewMatrix, currentProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXMat4 &rotation )
{
	GXVec3 location = currentModelMatrix.wv;

	currentModelMatrix = rotation;
	currentModelMatrix.wv = location;

	GXSetMat4Inverse ( currentViewMatrix, currentModelMatrix );
	GXMulMat4Mat4 ( currentViewProjectionMatrix, currentViewMatrix, currentProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXQuat &rotation )
{
	GXVec3 location = currentModelMatrix.wv;
	currentModelMatrix.From ( rotation, location );

	GXSetMat4Inverse ( currentViewMatrix, currentModelMatrix );
	GXMulMat4Mat4 ( currentViewProjectionMatrix, currentViewMatrix, currentProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetCurrentModelMatrix ( const GXMat4 &matrix )
{
	currentModelMatrix = matrix;
	GXSetMat4Inverse ( currentViewMatrix, currentModelMatrix );
	GXMulMat4Mat4 ( currentViewProjectionMatrix, currentViewMatrix, currentProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCamera::GetLocation ( GXVec3& outLocation ) const
{
	outLocation = currentViewMatrix.wv;
}

GXVoid GXCamera::GetRotation ( GXMat4 &out ) const
{
	out = currentModelMatrix;
	out.wv = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
}

GXVoid GXCamera::GetRotation ( GXQuat &out ) const
{
	out = GXCreateQuat ( currentModelMatrix );
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

GXVoid GXCamera::UpdateLastFrameViewMatrix ()
{
	lastFrameViewMatrix = currentViewMatrix;
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
	clipPlanesWorld.From ( currentViewProjectionMatrix );
}