//version 1.5

#ifndef GX_CAMERA_PERSPECTIVE
#define GX_CAMERA_PERSPECTIVE


#include "GXCamera.h"


class GXCameraPerspective : public GXCamera
{
	protected:
		GXFloat	fov_rad;
		GXFloat aspect_ratio;

	public:
		GXCameraPerspective ( GXFloat fov_rad, GXFloat aspect_ratio, GXFloat znear, GXFloat zfar );
		~GXCameraPerspective () override;

		GXVoid SetFov ( GXFloat fov_rad );
		GXVoid SetAspectRatio ( GXFloat aspect_ratio );
		GXVoid SetZnear ( GXFloat znear ) override;
		GXVoid SetZfar ( GXFloat zfar ) override;

		GXFloat GetFov ();
		GXFloat GetAspectRatio ();
		GXFloat GetZnear () const override;
		GXFloat GetZfar () const override;
};


#endif //GX_CAMERA_PERSPECTIVE