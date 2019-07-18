// version 1.14

#include <GXEngine/GXCameraOrthographic.h>


#define DEFAULT_WIDTH       1920
#define DEFAULT_HEIGHT      1080
#define DEFAULT_Z_NEAR      0.01f
#define DEFAULT_Z_FAR       1000.0f

//---------------------------------------------------------------------------------------------------------------------

GXCameraOrthographic::GXCameraOrthographic ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXCameraOrthographic" )
    GXCamera ( DEFAULT_Z_NEAR, DEFAULT_Z_FAR ),
    _width ( DEFAULT_WIDTH ),
    _height ( DEFAULT_HEIGHT )
{
    _currentFrameProjectionMatrix.Ortho ( _width, _height, _zNear, _zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix = _currentFrameProjectionMatrix;
    _currentFrameInverseViewProjectionMatrix = _currentFrameInverseProjectionMatrix;

    UpdateClipPlanes ();
    UpdateLastFrameMatrices ();
}

GXCameraOrthographic::GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXCameraOrthographic" )
    GXCamera ( zNear, zFar ),
    _width ( width ),
    _height ( height )
{
    _currentFrameProjectionMatrix.Ortho ( _width, _height, zNear, zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );
    
    _currentFrameViewProjectionMatrix = _currentFrameProjectionMatrix;
    _currentFrameInverseViewProjectionMatrix = _currentFrameInverseProjectionMatrix;

    UpdateClipPlanes ();
    UpdateLastFrameMatrices ();
}

GXCameraOrthographic::~GXCameraOrthographic ()
{
    // NOTHING
}

GXVoid GXCameraOrthographic::SetZNear ( GXFloat zNear )
{
    _zNear = zNear;

    _currentFrameProjectionMatrix.Ortho ( _width, _height, _zNear, _zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetZFar ( GXFloat zFar )
{
    _zFar = zFar;

    _currentFrameProjectionMatrix.Ortho ( _width, _height, _zNear, _zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetProjection ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar )
{
    _width = width;
    _height = height;
    _zNear = zNear;
    _zFar = zFar;

    _currentFrameProjectionMatrix.Ortho ( _width, _height, _zNear, _zFar );
    _currentFrameInverseProjectionMatrix.Inverse ( _currentFrameProjectionMatrix );

    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}
