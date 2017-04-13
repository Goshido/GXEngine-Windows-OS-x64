//version 1.7

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

	GXSetMat4Perspective ( currentProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	currentViewProjectionMatrix = currentProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameViewMatrix ();
}

GXCameraPerspective::GXCameraPerspective ( GXFloat fovy_rad, GXFloat aspectRatio, GXFloat znear, GXFloat zfar )
{
	this->fovy_rad = fovy_rad;
	this->aspectRatio = aspectRatio;
	this->znear = znear;
	this->zfar = zfar;

	GXSetMat4Perspective ( currentProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	currentViewProjectionMatrix = currentProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameViewMatrix ();
}

GXCameraPerspective::~GXCameraPerspective ()
{
	//NOTHING
}

GXVoid GXCameraPerspective::SetFov ( GXFloat fovy_rad )
{
	this->fovy_rad = fovy_rad;

	GXSetMat4Perspective ( currentProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	currentViewProjectionMatrix = currentProjectionMatrix;

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetAspectRatio ( GXFloat aspectRatio )
{
	this->aspectRatio = aspectRatio;

	GXSetMat4Perspective ( currentProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	currentViewProjectionMatrix = currentProjectionMatrix;

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZnear ( GXFloat znear )
{
	this->znear = znear;

	GXSetMat4Perspective ( currentProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	currentViewProjectionMatrix = currentProjectionMatrix;

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZfar	( GXFloat zfar )
{
	this->zfar = zfar;

	GXSetMat4Perspective ( currentProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentInverseProjectionMatrix, currentProjectionMatrix );
	currentViewProjectionMatrix = currentProjectionMatrix;

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
