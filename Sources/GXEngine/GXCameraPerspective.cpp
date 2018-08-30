// version 1.12

#include <GXEngine/GXCameraPerspective.h>


#define DEFAULT_FIELD_OF_VIEW_Y_DEGREES		60.0f
#define DEFAULT_PROJECTION_ASPECT_RATIO		1.7777f		// 16 : 9
#define DEFAULT_Z_NEAR						0.01f
#define DEFAULT_Z_FAR						1000.0f


GXCameraPerspective::GXCameraPerspective ():
	GXCamera ( DEFAULT_Z_NEAR, DEFAULT_Z_FAR )
{
	fieldOfViewYRadians = GXDegToRad ( DEFAULT_FIELD_OF_VIEW_Y_DEGREES );
	aspectRatio = DEFAULT_PROJECTION_ASPECT_RATIO;

	currentFrameProjectionMatrix.Perspective ( fieldOfViewYRadians, aspectRatio, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );
	
	currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
	currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameMatrices ();
}

GXCameraPerspective::GXCameraPerspective ( GXFloat fieldOfViewYRadians, GXFloat aspectRatio, GXFloat zNear, GXFloat zFar ):
	GXCamera ( zNear, zFar )
{
	this->fieldOfViewYRadians = fieldOfViewYRadians;
	this->aspectRatio = aspectRatio;

	currentFrameProjectionMatrix.Perspective ( fieldOfViewYRadians, aspectRatio, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
	currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

	UpdateClipPlanes ();
	UpdateLastFrameMatrices ();
}

GXCameraPerspective::~GXCameraPerspective ()
{
	// NOTHING
}

GXVoid GXCameraPerspective::SetFieldOfViewY ( GXFloat radians )
{
	fieldOfViewYRadians = radians;

	currentFrameProjectionMatrix.Perspective ( fieldOfViewYRadians, aspectRatio, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetAspectRatio ( GXFloat projectionAspectRatio )
{
	aspectRatio = projectionAspectRatio;

	currentFrameProjectionMatrix.Perspective ( fieldOfViewYRadians, aspectRatio, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZNear ( GXFloat newZNear )
{
	zNear = newZNear;

	currentFrameProjectionMatrix.Perspective ( fieldOfViewYRadians, aspectRatio, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZFar ( GXFloat newZFar )
{
	zFar = newZFar;

	currentFrameProjectionMatrix.Perspective ( fieldOfViewYRadians, aspectRatio, zNear, zFar );
	currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

	currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
	currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

	UpdateClipPlanes ();
}

GXFloat GXCameraPerspective::GetFieldOFViewYRadians () const
{
	return fieldOfViewYRadians;
}

GXFloat GXCameraPerspective::GetAspectRatio () const
{
	return aspectRatio;
}
