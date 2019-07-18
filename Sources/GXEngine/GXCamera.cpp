// version 1.16

#include <GXEngine/GXCamera.h>


GXCamera* GXCamera::_activeCamera = nullptr;

const GXMat4& GXCamera::GetCurrentFrameViewProjectionMatrix () const
{
    return _currentFrameViewProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameInverseViewProjectionMatrix () const
{
    return _currentFrameInverseViewProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameProjectionMatrix () const
{
    return _currentFrameProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameInverseProjectionMatrix () const
{
    return _currentFrameInverseProjectionMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameModelMatrix () const
{
    return _currentFrameModelMatrix;
}

const GXMat4& GXCamera::GetCurrentFrameViewMatrix () const
{
    return _currentFrameViewMatrix;
}

const GXMat4& GXCamera::GetLastFrameModelMatrix () const
{
    return _lastFrameModelMatrix;
}

const GXMat4& GXCamera::GetLastFrameViewMatrix () const
{
    return _lastFrameViewMatrix;
}

const GXMat4& GXCamera::GetLastFrameViewProjectionMatrix () const
{
    return _lastFrameViewProjectionMatrix;
}

GXVoid GXCamera::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
    _currentFrameModelMatrix.SetW ( GXVec3 ( x, y, z ) );
    _currentFrameViewMatrix.Inverse ( _currentFrameModelMatrix );
    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCamera::SetLocation ( const GXVec3 &location )
{
    _currentFrameModelMatrix.SetW ( location );
    _currentFrameViewMatrix.Inverse ( _currentFrameModelMatrix );
    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
    GXVec3 location;
    _currentFrameModelMatrix.GetW ( location );

    _currentFrameModelMatrix.RotationXYZ ( pitch_rad, yaw_rad, roll_rad );
    _currentFrameModelMatrix.SetW ( location );

    _currentFrameViewMatrix.Inverse ( _currentFrameModelMatrix );
    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXMat4 &rotation )
{
    GXVec3 location;
    _currentFrameModelMatrix.GetW ( location );

    _currentFrameModelMatrix = rotation;
    _currentFrameModelMatrix.SetW ( location );

    _currentFrameViewMatrix.Inverse ( _currentFrameModelMatrix );
    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCamera::SetRotation ( const GXQuat &rotation )
{
    GXVec3 location;
    _currentFrameModelMatrix.GetW ( location );
    _currentFrameModelMatrix.From ( rotation, location );

    _currentFrameViewMatrix.Inverse ( _currentFrameModelMatrix );
    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCamera::SetCurrentFrameModelMatrix ( const GXMat4 &matrix )
{
    _currentFrameModelMatrix = matrix;

    _currentFrameViewMatrix.Inverse ( _currentFrameModelMatrix );
    _currentFrameViewProjectionMatrix.Multiply ( _currentFrameViewMatrix, _currentFrameProjectionMatrix );
    _currentFrameInverseViewProjectionMatrix.Inverse ( _currentFrameViewProjectionMatrix );

    UpdateClipPlanes ();
}

GXVoid GXCamera::GetLocation ( GXVec3 &location ) const
{
    _currentFrameModelMatrix.GetW ( location );
}

GXVoid GXCamera::GetRotation ( GXMat4 &rotation ) const
{
    static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );

    rotation = _currentFrameModelMatrix;
    rotation.SetW ( origin );
}

GXVoid GXCamera::GetRotation ( GXQuat &rotation ) const
{
    rotation.From ( _currentFrameModelMatrix );
}

const GXProjectionClipPlanes& GXCamera::GetClipPlanesWorld ()
{
    return _clipPlanesWorld;
}

GXBool GXCamera::IsObjectVisible ( const GXAABB objectBoundsWorld )
{
    return _clipPlanesWorld.IsVisible ( objectBoundsWorld );
}

GXFloat GXCamera::GetZNear () const
{
    return _zNear;
}

GXFloat GXCamera::GetZFar () const
{
    return _zFar;
}

GXVoid GXCamera::UpdateLastFrameMatrices ()
{
    _lastFrameModelMatrix = _currentFrameModelMatrix;
    _lastFrameViewMatrix = _currentFrameViewMatrix;
    _lastFrameViewProjectionMatrix = _currentFrameViewProjectionMatrix;
}

GXCamera* GXCALL GXCamera::GetActiveCamera ()
{
    return _activeCamera;
}

GXVoid GXCALL GXCamera::SetActiveCamera ( GXCamera* camera )
{
    _activeCamera = camera;
}

GXCamera::GXCamera ( GXFloat zNear, GXFloat zFar ):
    _zNear ( zNear ),
    _zFar ( zFar )
{
    _currentFrameModelMatrix.Identity ();
    _currentFrameViewMatrix.Identity ();
    _currentFrameProjectionMatrix.Identity ();
    _currentFrameInverseProjectionMatrix.Identity ();
    _currentFrameViewProjectionMatrix.Identity ();
    _currentFrameInverseViewProjectionMatrix.Identity ();
    _lastFrameModelMatrix.Identity ();
    _lastFrameViewMatrix.Identity ();
    _lastFrameViewProjectionMatrix.Identity ();

    UpdateClipPlanes ();
}

GXCamera::~GXCamera ()
{
    // NOTHING
}

GXVoid GXCamera::UpdateClipPlanes ()
{
    _clipPlanesWorld.From ( _currentFrameViewProjectionMatrix );
}
