//version 1.9

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

	GXSetMat4Ortho ( currentFrameProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
	currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameMatrices ();
}

GXCameraOrthographic::GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar )
{
	this->width = width;
	this->height = height;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Ortho ( currentFrameProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	
	currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
	currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameMatrices ();
}

GXCameraOrthographic::~GXCameraOrthographic ()
{
	//NOTHING
}

GXVoid GXCameraOrthographic::SetZnear ( GXFloat znear )
{
	this->znear = znear;

	GXSetMat4Ortho ( currentFrameProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetZfar ( GXFloat zfar )
{
	this->zfar = zfar;
	GXSetMat4Ortho ( currentFrameProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetProjection ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar )
{
	this->width = width;
	this->height = height;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Ortho ( currentFrameProjectionMatrix, width, height, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}
