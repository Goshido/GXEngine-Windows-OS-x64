//version 1.9

#include <GXEngine/GXCameraPerspective.h>


#define DEFAULT_FOVY_DEGREES	60.0f
#define DEFAULT_ASPECT_RATIO	1.7777f		//16 : 9
#define DEFAULT_Z_NEAR			0.01f
#define DEFAULT_Z_FAR			1000.0f


GXCameraPerspective::GXCameraPerspective ()
{
	fovy_rad = GXDegToRad ( DEFAULT_FOVY_DEGREES );
	aspectRatio = DEFAULT_ASPECT_RATIO;
	znear = DEFAULT_Z_NEAR;
	zfar = DEFAULT_Z_FAR;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	
	currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
	currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameMatrices ();
}

GXCameraPerspective::GXCameraPerspective ( GXFloat fovy_rad, GXFloat aspectRatio, GXFloat znear, GXFloat zfar )
{
	this->fovy_rad = fovy_rad;
	this->aspectRatio = aspectRatio;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	
	currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
	currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameMatrices ();
}

GXCameraPerspective::~GXCameraPerspective ()
{
	//NOTHING
}

GXVoid GXCameraPerspective::SetFov ( GXFloat fovy_rad )
{
	this->fovy_rad = fovy_rad;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetAspectRatio ( GXFloat aspectRatio )
{
	this->aspectRatio = aspectRatio;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZnear ( GXFloat znear )
{
	this->znear = znear;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZfar	( GXFloat zfar )
{
	this->zfar = zfar;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXFloat GXCameraPerspective::GetFovYRadians () const
{
	return fovy_rad;
}

GXFloat GXCameraPerspective::GetAspectRatio () const
{
	return aspectRatio;
}
