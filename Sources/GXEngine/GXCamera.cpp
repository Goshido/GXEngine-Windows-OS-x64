//version 1.8

#include <GXEngine/GXCamera.h>


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

const GXMat4& GXCamera::GetViewProjectionMatrix ()
{
	return view_proj_mat;
}

const GXMat4& GXCamera::GetProjectionMatrix ()
{
	return proj_mat;
}

const GXMat4& GXCamera::GetModelMatrix ()
{
	return mod_mat;
}

const GXMat4& GXCamera::GetViewMatrix ()
{
	return view_mat;
}

const GXMat4* GXCamera::GetViewProjectionMatrixPtr ()
{
	return &view_proj_mat;
}

const GXMat4* GXCamera::GetProjectionMatrixPtr ()
{
	return &proj_mat;
}

const GXMat4* GXCamera::GetModelMatrixPtr ()
{
	return &mod_mat;
}

const GXMat4* GXCamera::GetViewMatrixPtr ()
{
	return &view_mat;
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

GXVoid GXCamera::GetLocation ( GXVec3& outLocation )
{
	memcpy ( &outLocation, trans_mat.wv.v, sizeof ( GXVec3 ) );
}

GXVec3* GXCamera::GetLocation ()
{
	return &trans_mat.wv;
}

GXVoid GXCamera::GetRotation ( GXMat4 &out )
{
	out = rot_mat;
}

GXVoid GXCamera::GetRotation ( GXQuat &out )
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

GXVoid GXCamera::UpdateClipPlanes ()
{
	clipPlanesWorld.From ( view_proj_mat );
}