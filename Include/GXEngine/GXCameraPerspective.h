//version 1.7

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
		explicit GXCameraPerspective ( GXFloat fovy_rad, GXFloat aspectRatio, GXFloat znear, GXFloat zfar );
		~GXCameraPerspective () override;

		GXVoid SetFov ( GXFloat fovy_rad );
		GXVoid SetAspectRatio ( GXFloat aspecRatio );
		GXVoid SetZnear ( GXFloat znear ) override;
		GXVoid SetZfar ( GXFloat zfar ) override;

		GXFloat GetFovYRadians () const;
		GXFloat GetAspectRatio () const;
};


#endif //GX_CAMERA_PERSPECTIVE
