//version 1.6

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

		GXVoid SetZnear ( GXFloat znear ) override;
		GXVoid SetZfar	( GXFloat zfar ) override;

		GXFloat GetZnear () const override;
		GXFloat GetZfar () const override;

		GXVoid SetProjection ( GXFloat width, GXFloat height, GXFloat znear, GXFloat zfar );
};


#endif //GX_CAMERA_ORHTOGRAPHIC