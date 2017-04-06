//version 1.6

#include <GXEngine/GXCameraPerspective.h>


#define DEFAULT_FOVY_DEGREES	60.0f
#define DEFAULT_ASPECT_RATIO	1.7777f		//16 : 9
#define DEFAULT_Z_NEAR			0.01f
#define DEFAULT_Z_FAR			1000.0f


GXCameraPerspective::GXCameraPerspective ()
{
	fov_rad = GXDegToRad ( DEFAULT_FOVY_DEGREES );
	aspectRatio = DEFAULT_ASPECT_RATIO;
	znear = DEFAULT_Z_NEAR;
	zfar = DEFAULT_Z_FAR;

	GXSetMat4Perspective ( proj_mat, fov_rad, aspectRatio, znear, zfar );
	view_proj_mat = proj_mat;

	UpdateClipPlanes ();
}

GXCameraPerspective::GXCameraPerspective ( GXFloat fovy_rad, GXFloat aspectRatio, GXFloat znear, GXFloat zfar )
{
	this->fov_rad = fov_rad;
	this->aspectRatio = aspectRatio;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Perspective ( proj_mat, fov_rad, aspectRatio, znear, zfar );
	view_proj_mat = proj_mat;

	UpdateClipPlanes ();
}

GXCameraPerspective::~GXCameraPerspective ()
{
	//NOTHING
}

GXVoid GXCameraPerspective::SetFov ( GXFloat fov_rad )
{
	this->fov_rad = fov_rad;
	GXSetMat4Perspective ( proj_mat, fov_rad, aspectRatio, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetAspectRatio ( GXFloat aspectRatio )
{
	this->aspectRatio = aspectRatio;
	GXSetMat4Perspective ( proj_mat, fov_rad, aspectRatio, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZnear ( GXFloat znear )
{
	this->znear = znear;
	GXSetMat4Perspective ( proj_mat, fov_rad, aspectRatio, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZfar	( GXFloat zfar )
{
	this->zfar = zfar;
	GXSetMat4Perspective ( proj_mat, fov_rad, aspectRatio, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXFloat GXCameraPerspective::GetFov ()
{
	return fov_rad;
}

GXFloat GXCameraPerspective::GetAspectRatio ()
{
	return aspectRatio;
}

GXFloat GXCameraPerspective::GetZnear () const
{
	return znear;
}

GXFloat GXCameraPerspective::GetZfar () const
{
	return zfar;
}