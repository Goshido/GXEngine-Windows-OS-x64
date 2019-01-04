// version 1.13

#ifndef GX_CAMERA_ORHTOGRAPHIC
#define GX_CAMERA_ORHTOGRAPHIC


#include "GXCamera.h"
#include <GXCommon/GXMemory.h>


class GXCameraOrthographic final : public GXMemoryInspector, public GXCamera
{
    protected:
        GXFloat     width;
        GXFloat     height;

    public:
        GXCameraOrthographic ();
        explicit GXCameraOrthographic ( GXFloat widthUnuts, GXFloat heightUnits, GXFloat zNear, GXFloat zFar );
        ~GXCameraOrthographic () override;

        GXVoid SetZNear ( GXFloat newZNear ) override;
        GXVoid SetZFar ( GXFloat newZFar ) override;

        GXVoid SetProjection ( GXFloat newWidth, GXFloat newHeight, GXFloat newZNear, GXFloat newZFar );

    private:
        GXCameraOrthographic ( const GXCameraOrthographic &other ) = delete;
        GXCameraOrthographic& operator = ( const GXCameraOrthographic &other ) = delete;
};


#endif // GX_CAMERA_ORHTOGRAPHIC
