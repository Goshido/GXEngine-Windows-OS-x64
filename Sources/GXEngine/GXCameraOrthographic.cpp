//version 1.7

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

	GXSetMat4Ortho ( currentProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	currentViewProjectionMatrix = currentProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameViewMatrix ();
}

GXCameraOrthographic::GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar )
{
	this->width = width;
	this->height = height;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Ortho ( currentProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	currentViewProjectionMatrix = currentProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameViewMatrix ();
}

GXCameraOrthographic::~GXCameraOrthographic ()
{
	//NOTHING
}

GXVoid GXCameraOrthographic::SetZnear ( GXFloat znear )
{
	this->znear = znear;
	GXSetMat4Ortho ( currentProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	GXMulMat4Mat4 ( currentViewProjectionMatrix, currentViewMatrix, currentProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetZfar ( GXFloat zfar )
{
	this->zfar = zfar;
	GXSetMat4Ortho ( currentProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	GXMulMat4Mat4 ( currentViewProjectionMatrix, currentViewMatrix, currentProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetProjection ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar )
{
	this->width = width;
	this->height = height;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Ortho ( currentProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	GXMulMat4Mat4 ( currentViewProjectionMatrix, currentViewMatrix, currentProjectionMatrix );

	UpdateClipPlanes ();
}
