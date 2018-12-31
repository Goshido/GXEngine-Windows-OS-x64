// version 1.12

#include <GXEngine/GXCameraOrthographic.h>


#define DEFAULT_WIDTH       1920
#define DEFAULT_HEIGHT      1080
#define DEFAULT_Z_NEAR      0.01f
#define DEFAULT_Z_FAR       1000.0f

//---------------------------------------------------------------------------------------------------------------------

GXCameraOrthographic::GXCameraOrthographic ():
    GXCamera ( DEFAULT_Z_NEAR, DEFAULT_Z_FAR )
{
    width = DEFAULT_WIDTH;
    height = DEFAULT_HEIGHT;

    currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
    currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

    currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
    currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

    UpdateClipPlanes ();
    UpdateLastFrameMatrices ();
}

GXCameraOrthographic::GXCameraOrthographic ( GXFloat width, GXFloat height, GXFloat zNear, GXFloat zFar ):
    GXCamera ( zNear, zFar )
{
    this->width = width;
    this->height = height;

    currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
    currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );
    
    currentFrameViewProjectionMatrix = currentFrameProjectionMatrix;
    currentFrameInverseViewProjectionMatrix = currentFrameInverseProjectionMatrix;

    UpdateClipPlanes ();
    UpdateLastFrameMatrices ();
}

GXCameraOrthographic::~GXCameraOrthographic ()
{
    // NOTHING
}

GXVoid GXCameraOrthographic::SetZNear ( GXFloat newZNear )
{
    zNear = newZNear;

    currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
    currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

    currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
    currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetZFar ( GXFloat newZFar )
{
    zFar = newZFar;

    currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
    currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

    currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
    currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCameraOrthographic::SetProjection ( GXFloat newWidth, GXFloat newHeight, GXFloat newZNear, GXFloat newZFar )
{
    width = newWidth;
    height = newHeight;
    zNear = newZNear;
    zFar = newZFar;

    currentFrameProjectionMatrix.Ortho ( width, height, zNear, zFar );
    currentFrameInverseProjectionMatrix.Inverse ( currentFrameProjectionMatrix );

    currentFrameViewProjectionMatrix.Multiply ( currentFrameViewMatrix, currentFrameProjectionMatrix );
    currentFrameInverseViewProjectionMatrix.Inverse ( currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}
