//version 1.10

#include <GXEngine/GXCameraPerspective.h>


#define DEFAULT_FIELD_OF_VIEW_Y_DEGREES		60.0f
#define DEFAULT_PROJECTION_ASPECT_RATIO		1.7777f		//16 : 9
#define DEFAULT_Z_NEAR						0.01f
#define DEFAULT_Z_FAR						1000.0f


GXCameraPerspective::GXCameraPerspective ()
{
	fovy_rad = GXDegToRad ( DEFAULT_FIELD_OF_VIEW_Y_DEGREES );
	aspectRatio = DEFAULT_PROJECTION_ASPECT_RATIO;
	znear = DEFAULT_Z_NEAR;
	zfar = DEFAULT_Z_FAR;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	
	currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
	currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameMatrices ();
}

GXCameraPerspective::GXCameraPerspective ( GXFloat fieldOfViewYRadians, GXFloat projectionAspectRatio, GXFloat zNear, GXFloat zFar )
{
	fovy_rad = fieldOfViewYRadians;
	aspectRatio = projectionAspectRatio;
	znear = zNear;
	zfar = zFar;

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

GXVoid GXCameraPerspective::SetFov ( GXFloat radians )
{
	fovy_rad = radians;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetAspectRatio ( GXFloat projectionAspectRatio )
{
	aspectRatio = projectionAspectRatio;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZnear ( GXFloat zNear )
{
	znear = zNear;

	GXSetMat4Perspective ( currentFrameProjectionMatrix, fovy_rad, aspectRatio, znear, zfar );
	GXSetMat4Inverse ( currentFrameInverseProjectionMatrix, currentFrameProjectionMatrix );
	GXMulMat4Mat4 ( currentFrameViewProjectionMatrix, currentFrameViewMatrix, currentFrameProjectionMatrix );
	GXSetMat4Inverse ( currentFrameInverseViewProjectionMatrix, currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZfar	( GXFloat zFar )
{
	zfar = zFar;

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
