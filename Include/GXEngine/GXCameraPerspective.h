//version 1.3

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

		GXVoid SetFov ( GXFloat fov_rad );
		GXVoid SetAspectRatio ( GXFloat aspect_ratio );
		virtual GXVoid SetZnear ( GXFloat znear );
		virtual GXVoid SetZfar ( GXFloat zfar );

		GXFloat GetFov ();
		GXFloat GetAspectRatio ();
		virtual GXFloat GetZnear ();
		virtual GXFloat GetZfar ();
};


#endif //GX_CAMERA_PERSPECTIVE