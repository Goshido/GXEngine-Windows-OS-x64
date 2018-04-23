// version 1.10

#ifndef GX_CAMERA_PERSPECTIVE
#define GX_CAMERA_PERSPECTIVE


#include "GXCamera.h"


class GXCameraPerspective : public GXCamera
{
	protected:
		GXFloat		fieldOfViewYRadians;
		GXFloat		aspectRatio;

	public:
		GXCameraPerspective ();
		explicit GXCameraPerspective ( GXFloat fieldOfViewYRadians, GXFloat aspectRatio, GXFloat zNear, GXFloat zFar );
		~GXCameraPerspective () override;

		GXVoid SetFieldOfViewY ( GXFloat radians );
		GXVoid SetAspectRatio ( GXFloat newAspectRatio );
		GXVoid SetZNear ( GXFloat newZNear ) override;
		GXVoid SetZFar ( GXFloat newZFar ) override;

		GXFloat GetFieldOFViewYRadians () const;
		GXFloat GetAspectRatio () const;
};


#endif // GX_CAMERA_PERSPECTIVE
