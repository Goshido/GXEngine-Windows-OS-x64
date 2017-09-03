//version 1.10

#ifndef GX_CAMERA_PERSPECTIVE
#define GX_CAMERA_PERSPECTIVE


#include "GXCamera.h"


class GXCameraPerspective : public GXCamera
{
	protected:
		GXFloat		fovy_rad;
		GXFloat		aspectRatio;

	public:
		GXCameraPerspective ();
		explicit GXCameraPerspective ( GXFloat fieldOfViewYRadians, GXFloat projectionAspectRatio, GXFloat zNear, GXFloat zFar );
		~GXCameraPerspective () override;

		GXVoid SetFov ( GXFloat radians );
		GXVoid SetAspectRatio ( GXFloat projectionAspectRatio );
		GXVoid SetZnear ( GXFloat zNear ) override;
		GXVoid SetZfar ( GXFloat zFar ) override;

		GXFloat GetFovYRadians () const;
		GXFloat GetAspectRatio () const;
};


#endif //GX_CAMERA_PERSPECTIVE
