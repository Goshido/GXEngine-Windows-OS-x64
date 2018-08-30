// version 1.12

#ifndef GX_CAMERA_PERSPECTIVE
#define GX_CAMERA_PERSPECTIVE


#include "GXCamera.h"


class GXCameraPerspective final : public GXCamera
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

	private:
		GXCameraPerspective ( const GXCameraPerspective &other ) = delete;
		GXCameraPerspective& operator = ( const GXCameraPerspective &other ) = delete;
};


#endif // GX_CAMERA_PERSPECTIVE
