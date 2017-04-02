//version 1.5

#include <GXEngine/GXCameraOrthographic.h>


GXCameraOrthographic::GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar )
{
	this->width = width;
	this->height = height;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Ortho ( proj_mat, width, height, znear, zfar );
	view_proj_mat = proj_mat;

	UpdateClipPlanes ();
}

GXCameraOrthographic::~GXCameraOrthographic ()
{
	//NOTHING
}

GXVoid GXCameraOrthographic::SetZnear ( GXFloat znear )
{
	this->znear = znear;
	GXSetMat4Ortho ( proj_mat, width, height, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetZfar ( GXFloat zfar )
{
	this->zfar = zfar;
	GXSetMat4Ortho ( proj_mat, width, height, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXFloat GXCameraOrthographic::GetZnear () const
{
	return znear;
}

GXFloat GXCameraOrthographic::GetZfar () const
{
	return zfar;
}

GXVoid GXCameraOrthographic::SetProjection ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar )
{
	this->width = width;
	this->height = height;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Ortho ( proj_mat, width, height, znear, zfar );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}