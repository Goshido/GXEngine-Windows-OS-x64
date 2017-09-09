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
	zNear = DEFAULT_Z_NEAR;
	zFar = DEFAULT_Z_FAR;

	currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
	currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameMatrices ();
}

GXCameraOrthographic::GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar )
{
	this->width = width;
	this->height = height;
	this->zNear = zNear;
	this->zFar = zFar;

	currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );
	
	currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
	currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameMatrices ();
}

GXCameraOrthographic::~GXCameraOrthographic ()
{
	//NOTHING
}

GXVoid GXCameraOrthographic::SetZNear ( GXFloat newZNear )
{
	zNear = newZNear;

	currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetZFar ( GXFloat newZFar )
{
	zFar = newZFar;

	currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetProjection ( GXFloat newWidth, GXFloat newHeight, GXFloat newZNear, GXFloat newZFar )
{
	width = newWidth;
	height = newHeight;
	zNear = newZNear;
	zFar = newZFar;

	currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}
