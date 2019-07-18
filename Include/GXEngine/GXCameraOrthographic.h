// version 1.14

#ifndef GX_CAMERA_ORHTOGRAPHIC
#define GX_CAMERA_ORHTOGRAPHIC


#include "GXCamera.h"
#include <GXCommon/GXMemory.h>


class GXCameraOrthographic final : public GXMemoryInspector, public GXCamera
{
    protected:
        GXFloat     _width;
        GXFloat     _height;

    public:
        GXCameraOrthographic ();
        explicit GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar );
        ~GXCameraOrthographic () override;

        GXVoid SetZNear ( GXFloat zNear ) override;
        GXVoid SetZFar ( GXFloat zFar ) override;

        GXVoid SetProjection ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar );

    private:
        GXCameraOrthographic ( const GXCameraOrthographic &other ) = delete;
        GXCameraOrthographic& operator = ( const GXCameraOrthographic &other ) = delete;
};


#endif // GX_CAMERA_ORHTOGRAPHIC
