//version 1.9

#include <GXEngine/GXCamera.h>

GXCamera* GXCamera::activeCamera = nullptr;

GXCamera::GXCamera ()
{
	znear = -1.0f;
	zfar = 1.0f;

	GXSetMat4Identity ( trans_mat );
	GXSetMat4Identity ( rot_mat );
	GXSetMat4Identity ( mod_mat );
	GXSetMat4Identity ( view_mat );
	GXSetMat4Identity ( proj_mat );
	GXSetMat4Identity ( view_proj_mat );

	UpdateClipPlanes ();
}

GXCamera::~GXCamera ()
{
	//NOTHING
}

const GXMat4& GXCamera::GetViewProjectionMatrix () const
{
	return view_proj_mat;
}

const GXMat4& GXCamera::GetProjectionMatrix () const
{
	return proj_mat;
}

const GXMat4& GXCamera::GetModelMatrix () const
{
	return mod_mat;
}

const GXMat4& GXCamera::GetViewMatrix () const
{
	return view_mat;
}

GXVoid GXCamera::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	GXSetMat4Translation ( trans_mat, x, y, z );
	GXMulMat4Mat4 ( mod_mat, rot_mat, trans_mat );
	GXSetMat4Inverse ( view_mat, mod_mat );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetLocation ( const GXVec3 &location )
{
	GXSetMat4Translation ( trans_mat, location.x, location.y, location.z );
	GXMulMat4Mat4 ( mod_mat, rot_mat, trans_mat );
	GXSetMat4Inverse ( view_mat, mod_mat );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( rot_mat, pitch_rad, yaw_rad, roll_rad );
	GXMulMat4Mat4 ( mod_mat, rot_mat, trans_mat );
	GXSetMat4Inverse ( view_mat, mod_mat );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXMat4 &rotation )
{
	rot_mat = rotation;
	GXMulMat4Mat4 ( mod_mat, rot_mat, trans_mat );
	GXSetMat4Inverse ( view_mat, mod_mat );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXQuat &rotation )
{
	rot_mat.SetRotation ( rotation );

	UpdateClipPlanes ();
}

GXVoid GXCamera::SetModelMatrix ( const GXMat4 &matrix )
{
	GXSetMat4ClearRotation ( rot_mat, matrix );
	trans_mat.wv = matrix.wv;

	mod_mat = matrix;
	GXSetMat4Inverse ( view_mat, mod_mat );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCamera::GetLocation ( GXVec3& outLocation ) const
{
	memcpy ( &outLocation, trans_mat.wv.v, sizeof ( GXVec3 ) );
}

GXVoid GXCamera::GetRotation ( GXMat4 &out ) const
{
	out = rot_mat;
}

GXVoid GXCamera::GetRotation ( GXQuat &out ) const
{
	out = GXCreateQuat ( rot_mat );
}

const GXProjectionClipPlanes& GXCamera::GetClipPlanesWorld ()
{
	return clipPlanesWorld;
}

GXBool GXCamera::IsObjectVisible ( const GXAABB objectBoundsWorld )
{
	return clipPlanesWorld.IsVisible ( objectBoundsWorld );
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
	clipPlanesWorld.From ( view_proj_mat );
}