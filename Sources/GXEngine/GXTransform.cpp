// version 1.5

#include <GXEngine/GXTransform.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXMemory.h>


GXTransform GXTransform::_nullTransform;

GXTransform::GXTransform ()
{
    _currentFrameLocation.Init ( 0.0f, 0.0f, 0.0f );
    _currentFrameScale.Init ( 1.0f, 1.0f, 1.0f );
    _currentFrameRotationMatrix.Identity ();
    _currentFrameModelMatrix.Identity ();
    _lastFrameModelMatrix.Identity ();
}

GXTransform::GXTransform ( const GXTransform &other )
{
    memcpy ( this, &other, sizeof ( GXTransform ) );
}

GXTransform::~GXTransform ()
{
    // NOTHING
}

GXVoid GXTransform::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
    _currentFrameLocation.Init ( x, y, z );
    _currentFrameModelMatrix.SetW ( _currentFrameLocation );

    TransformUpdated ();
}

GXVoid GXTransform::SetLocation ( const GXVec3 &loc )
{
    _currentFrameLocation = loc;
    _currentFrameModelMatrix.SetW ( _currentFrameLocation );

    TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXEuler &rotation  )
{
    SetRotation ( rotation._pitchRadians, rotation._yawRadians, rotation._rollRadians );
}

GXVoid GXTransform::SetRotation ( const GXMat4 &rot )
{
    _currentFrameRotationMatrix = rot;

    GXVec3 tmpA;
    GXVec3 tmpB;

    _currentFrameRotationMatrix.GetX ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 0u ] );
    _currentFrameModelMatrix.SetX ( tmpB );

    _currentFrameRotationMatrix.GetY ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 1u ] );
    _currentFrameModelMatrix.SetY ( tmpB );

    _currentFrameRotationMatrix.GetZ ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 2u ] );
    _currentFrameModelMatrix.SetZ ( tmpB );

    TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
    _currentFrameRotationMatrix.RotationXYZ ( pitchRadians, yawRadians, rollRadians );

    GXVec3 tmpA;
    GXVec3 tmpB;

    _currentFrameRotationMatrix.GetX ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 0u ] );
    _currentFrameModelMatrix.SetX ( tmpB );

    _currentFrameRotationMatrix.GetY ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 1u ] );
    _currentFrameModelMatrix.SetY ( tmpB );

    _currentFrameRotationMatrix.GetZ ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 2u ] );
    _currentFrameModelMatrix.SetZ ( tmpB );

    TransformUpdated ();
}

GXVoid GXTransform::SetRotation ( const GXQuat &quaternion )
{
    _currentFrameRotationMatrix.SetRotation ( quaternion );

    GXVec3 tmpA;
    GXVec3 tmpB;

    _currentFrameRotationMatrix.GetX ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 0u ] );
    _currentFrameModelMatrix.SetX ( tmpB );

    _currentFrameRotationMatrix.GetY ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 1u ] );
    _currentFrameModelMatrix.SetY ( tmpB );

    _currentFrameRotationMatrix.GetZ ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 2u ] );
    _currentFrameModelMatrix.SetZ ( tmpB );

    TransformUpdated ();
}

GXVoid GXTransform::SetScale ( GXFloat x, GXFloat y, GXFloat z )
{
    _currentFrameScale.Init ( x, y, z );

    GXVec3 tmpA;
    GXVec3 tmpB;

    _currentFrameRotationMatrix.GetX ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 0u ] );
    _currentFrameModelMatrix.SetX ( tmpB );

    _currentFrameRotationMatrix.GetY ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 1u ] );
    _currentFrameModelMatrix.SetY ( tmpB );

    _currentFrameRotationMatrix.GetZ ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 2u ] );
    _currentFrameModelMatrix.SetZ ( tmpB );

    TransformUpdated ();
}

GXVoid GXTransform::SetScale ( const GXVec3 &scale )
{
    _currentFrameScale = scale;

    GXVec3 tmpA;
    GXVec3 tmpB;

    _currentFrameRotationMatrix.GetX ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 0u ] );
    _currentFrameModelMatrix.SetX ( tmpB );

    _currentFrameRotationMatrix.GetY ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 1u ] );
    _currentFrameModelMatrix.SetY ( tmpB );

    _currentFrameRotationMatrix.GetZ ( tmpA );
    tmpB.Multiply ( tmpA, _currentFrameScale._data[ 2u ] );
    _currentFrameModelMatrix.SetZ ( tmpB );

    TransformUpdated ();
}

GXVoid GXTransform::GetLocation ( GXVec3 &loc ) const
{
    loc = _currentFrameLocation;
}

const GXVec3& GXTransform::GetLocation () const
{
    return _currentFrameLocation;
}

GXVoid GXTransform::GetRotation ( GXMat4 &rot ) const
{
    rot = _currentFrameRotationMatrix;
}

GXVoid GXTransform::GetRotation ( GXQuat &rot ) const
{
    rot.From ( _currentFrameRotationMatrix );
}

const GXMat4& GXTransform::GetRotation () const
{
    return _currentFrameRotationMatrix;
}

GXVoid GXTransform::GetScale ( GXVec3 &scale ) const
{
    scale = _currentFrameScale;
}

const GXVec3& GXTransform::GetScale () const
{
    return _currentFrameScale;
}

const GXMat4& GXTransform::GetCurrentFrameModelMatrix () const
{
    return _currentFrameModelMatrix;
}

GXVoid GXTransform::SetCurrentFrameModelMatrix ( const GXMat4 &matrix )
{
    _currentFrameModelMatrix = matrix;

    matrix.GetW ( _currentFrameLocation );
    _currentFrameRotationMatrix.ClearRotation ( matrix );
    matrix.ClearScale ( _currentFrameScale );
}

const GXMat4& GXTransform::GetLastFrameModelMatrix () const
{
    return _lastFrameModelMatrix;
}

GXVoid GXTransform::UpdateLastFrameModelMatrix ()
{
    _lastFrameModelMatrix = _currentFrameModelMatrix;
}

const GXTransform& GXTransform::GetNullTransform ()
{
    return _nullTransform;
}

GXTransform& GXTransform::operator = ( const GXTransform &other )
{
    memcpy ( this, &other, sizeof ( GXTransform ) );
    return *this;
}

GXVoid GXTransform::TransformUpdated ()
{
    // NOTHING
}
