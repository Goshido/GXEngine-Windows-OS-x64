//version 1.9

#ifndef GX_CAMERA_ORHTOGRAPHIC
#define GX_CAMERA_ORHTOGRAPHIC


#include "GXCamera.h"


class GXCameraOrthographic : public GXCamera
{
	protected:
		GXFloat		width;
		GXFloat		height;

	public:
		GXCameraOrthographic ();
		explicit GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar );
		~GXCameraOrthographic () override;

		GXVoid SetZnear ( GXFloat zNear ) override;
		GXVoid SetZfar ( GXFloat zFar ) override;

		GXVoid SetProjection ( GXFloat projectionWidth, GXFloat progectionHeight, GXFloat zNear, GXFloat zFar );
};


#endif //GX_CAMERA_ORHTOGRAPHIC
