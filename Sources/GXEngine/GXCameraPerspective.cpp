//version 1.3

#include <GXEngine/GXCameraPerspective.h>


GXCameraPerspective::GXCameraPerspective ( GXFloat fov_rad, GXFloat aspect_ratio, GXFloat znear, GXFloat zfar )
{
	this->fov_rad = fov_rad;
	this->aspect_ratio = aspect_ratio;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Perspective ( proj_mat, fov_rad, aspect_ratio, znear, zfar );
	view_proj_mat = proj_mat;

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetFov ( GXFloat fov_rad )
{
	this->fov_rad = fov_rad;
	GXSetMat4Perspective ( proj_mat, fov_rad, aspect_ratio, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetAspectRatio ( GXFloat aspect_ratio )
{
	this->aspect_ratio = aspect_ratio;
	GXSetMat4Perspective ( proj_mat, fov_rad, aspect_ratio, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZnear ( GXFloat znear )
{
	this->znear = znear;
	GXSetMat4Perspective ( proj_mat, fov_rad, aspect_ratio, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZfar	( GXFloat zfar )
{
	this->zfar = zfar;
	GXSetMat4Perspective ( proj_mat, fov_rad, aspect_ratio, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXFloat GXCameraPerspective::GetFov ()
{
	return fov_rad;
}

GXFloat GXCameraPerspective::GetAspectRatio ()
{
	return aspect_ratio;
}

GXFloat GXCameraPerspective::GetZnear ()
{
	return znear;
}

GXFloat GXCameraPerspective::GetZfar ()
{
	return zfar;
}