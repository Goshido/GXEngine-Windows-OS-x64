//version 1.4

#ifndef GX_CAMERA_ORHTOGRAPHIC
#define GX_CAMERA_ORHTOGRAPHIC


#include "GXCamera.h"


class GXCameraOrthographic : public GXCamera
{
	protected:
		GXFloat		width;
		GXFloat		height;

	public:
		GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar );

		virtual GXVoid SetZnear ( GXFloat znear );
		virtual GXVoid SetZfar	( GXFloat zfar );

		virtual GXFloat GetZnear ();
		virtual GXFloat GetZfar ();

		GXVoid SetProjection ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar );
};


#endif //GX_CAMERA_ORHTOGRAPHIC