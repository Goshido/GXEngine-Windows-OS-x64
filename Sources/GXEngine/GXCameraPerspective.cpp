// version 1.15

#include <GXEngine/GXCameraPerspective.h>


#define DEFAULT_FIELD_OF_VIEW_Y_DEGREES     60.0f
#define DEFAULT_PROJECTION_ASPECT_RATIO     1.7777f     // 16 : 9
#define DEFAULT_Z_NEAR                      0.01f
#define DEFAULT_Z_FAR                       1000.0f

//---------------------------------------------------------------------------------------------------------------------

GXCameraPerspective::GXCameraPerspective ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXCameraPerspective" )
    GXCamera ( DEFAULT_Z_NEAR, DEFAULT_Z_FAR ),
    _fieldOfViewYRadians ( GXDegToRad ( DEFAULT_FIELD_OF_VIEW_Y_DEGREES ) ),
    _aspectRatio ( DEFAULT_PROJECTION_ASPECT_RATIO )
{
    _currentFrameProjectionMatrix.Perspective ( _fieldOfViewYRadians, _aspectRatio, _zNear, _zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix = _currentFrameProjectionMatrix;
    _currentFrameInverseViewProjectionMatrix = _currentFrameInverseProjectionMatrix;

    UpdateClipPlanes ();
    UpdateLastFrameMatrices ();
}

GXCameraPerspective::GXCameraPerspective ( GXFloat fieldOfViewYRadians, GXFloat aspectRatio, GXFloat zNear, GXFloat zFar )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXCameraPerspective" )
    GXCamera ( zNear, zFar ),
    _fieldOfViewYRadians ( fieldOfViewYRadians ),
    _aspectRatio ( aspectRatio )
{
    _currentFrameProjectionMatrix.Perspective ( _fieldOfViewYRadians, _aspectRatio, zNear, zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix = _currentFrameProjectionMatrix;
    _currentFrameInverseViewProjectionMatrix = _currentFrameInverseProjectionMatrix;

    UpdateClipPlanes ();
    UpdateLastFrameMatrices ();
}

GXCameraPerspective::~GXCameraPerspective ()
{
    // NOTHING
}

GXVoid GXCameraPerspective::SetFieldOfViewY ( GXFloat radians )
{
    _fieldOfViewYRadians = radians;

    _currentFrameProjectionMatrix.Perspective ( _fieldOfViewYRadians, _aspectRatio, _zNear, _zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetAspectRatio ( GXFloat aspectRatio )
{
    _aspectRatio = aspectRatio;

    _currentFrameProjectionMatrix.Perspective ( _fieldOfViewYRadians, _aspectRatio, _zNear, _zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZNear ( GXFloat zNear )
{
    _zNear = zNear;

    _currentFrameProjectionMatrix.Perspective ( _fieldOfViewYRadians, _aspectRatio, _zNear, _zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCameraPerspective::SetZFar ( GXFloat zFar )
{
    _zFar = zFar;

    _currentFrameProjectionMatrix.Perspective ( _fieldOfViewYRadians, _aspectRatio, _zNear, _zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXFloat GXCameraPerspective::GetFieldOFViewYRadians () const
{
    return _fieldOfViewYRadians;
}

GXFloat GXCameraPerspective::GetAspectRatio () const
{
    return _aspectRatio;
}
