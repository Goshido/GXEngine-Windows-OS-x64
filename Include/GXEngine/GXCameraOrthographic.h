// version 1.10

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
		explicit GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar );
		~GXCameraOrthographic () override;

		GXVoid SetZNear ( GXFloat newZNear ) override;
		GXVoid SetZFar ( GXFloat newZFar ) override;

		GXVoid SetProjection ( GXFloat newWidth, GXFloat newHeight, GXFloat newZNear, GXFloat newZFar );

	private:
		GXCameraOrthographic ( const GXCameraOrthographic &other ) = delete;
		GXCameraOrthographic& operator = ( const GXCameraOrthographic &other ) = delete;
};


#endif // GX_CAMERA_ORHTOGRAPHIC
