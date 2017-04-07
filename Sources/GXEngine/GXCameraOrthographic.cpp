//version 1.6

#include <GXEngine/GXCameraOrthographic.h>


#define DEFAULT_WIDTH		1920
#define DEFAULT_HEIGHT		1080
#define DEFAULT_Z_NEAR		0.01f
#define DEFAULT_Z_FAR		1000.0f


GXCameraOrthographic::GXCameraOrthographic ()
{
	width = DEFAULT_WIDTH;
	height = DEFAULT_HEIGHT;
	znear = DEFAULT_Z_NEAR;
	zfar = DEFAULT_Z_FAR;

	GXSetMat4Ortho ( proj_mat, width, height, znear, zfar );
	GXSetMat4Inverse ( inv_proj_mat, proj_mat );
	view_proj_mat = proj_mat;

	UpdateClipPlanes ();
}

GXCameraOrthographic::GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar )
{
	this->width = width;
	this->height = height;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Ortho ( proj_mat, width, height, znear, zfar );
	GXSetMat4Inverse ( inv_proj_mat, proj_mat );
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
	GXSetMat4Inverse ( inv_proj_mat, proj_mat );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetZfar ( GXFloat zfar )
{
	this->zfar = zfar;
	GXSetMat4Ortho ( proj_mat, width, height, znear, zfar );
	GXSetMat4Inverse ( inv_proj_mat, proj_mat );
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
	GXSetMat4Inverse ( inv_proj_mat, proj_mat );
	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );

	UpdateClipPlanes ();
}