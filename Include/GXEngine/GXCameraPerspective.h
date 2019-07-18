// version 1.15

#ifndef GX_CAMERA_PERSPECTIVE
#define GX_CAMERA_PERSPECTIVE


#include "GXCamera.h"
#include <GXCommon/GXMemory.h>


class GXCameraPerspective final : public GXMemoryInspector, public GXCamera
{
    protected:
        GXFloat     _fieldOfViewYRadians;
        GXFloat     _aspectRatio;

    public:
        GXCameraPerspective ();
        explicit GXCameraPerspective ( GXFloat fieldOfViewYRadians, GXFloat aspectRatio, GXFloat zNear, GXFloat zFar );
        ~GXCameraPerspective () override;

        GXVoid SetFieldOfViewY ( GXFloat radians );
        GXVoid SetAspectRatio ( GXFloat aspectRatio );
        GXVoid SetZNear ( GXFloat zNear ) override;
        GXVoid SetZFar ( GXFloat zFar ) override;

        GXFloat GetFieldOFViewYRadians () const;
        GXFloat GetAspectRatio () const;

    private:
        GXCameraPerspective ( const GXCameraPerspective &other ) = delete;
        GXCameraPerspective& operator = ( const GXCameraPerspective &other ) = delete;
};


#endif // GX_CAMERA_PERSPECTIVE
