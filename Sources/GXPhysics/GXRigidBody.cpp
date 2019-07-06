// version 1.4

#include <GXPhysics/GXRigidBody.h>
#include <GXPhysics/GXShape.h>
#include <GXPhysics/GXPhysicsEngine.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_MASS                    1.0f

#define DEFAULT_LOCATION_X              0.0f
#define DEFAULT_LOCATION_Y              0.0f
#define DEFAULT_LOCATION_Z              0.0f

#define DEFAULT_ROTATION_AXIS_X         0.0f
#define DEFAULT_ROTATION_AXIS_Y         0.0f
#define DEFAULT_ROTATION_AXIS_Z         1.0f
#define DEFAULT_ROTATION_ANGLE          0.0f

#define DEFAULT_LINEAR_VELOCITY_X       0.0f
#define DEFAULT_LINEAR_VELOCITY_Y       0.0f
#define DEFAULT_LINEAR_VELOCITY_Z       0.0f

#define DEFAULT_ANGULAR_VELOCITY_X      0.0f
#define DEFAULT_ANGULAR_VELOCITY_Y      0.0f
#define DEFAULT_ANGULAR_VELOCITY_Z      0.0f

#define DEFALUT_LINEAR_DAMPING          0.8f
#define DEFAULT_ANGULAR_DAMPING         0.8f

//---------------------------------------------------------------------------------------------------------------------

GXRigidBody::GXRigidBody ():
    _shape ( nullptr ),
    _handler ( nullptr ),
    _OnTransformChanged ( nullptr )
{
    SetMass ( DEFAULT_MASS );

    constexpr GXVec3 defaultLocation ( DEFAULT_LOCATION_X, DEFAULT_LOCATION_Y, DEFAULT_LOCATION_Z );
    SetLocation ( defaultLocation );

    GXQuat rot;
    constexpr GXVec3 defaultRotationDirection ( DEFAULT_ROTATION_AXIS_X, DEFAULT_ROTATION_AXIS_Y, DEFAULT_ROTATION_AXIS_Z );
    rot.FromAxisAngle ( defaultRotationDirection, DEFAULT_ROTATION_ANGLE );
    SetRotaton ( rot );

    constexpr GXVec3 defaultLinearVelocity ( DEFAULT_LINEAR_VELOCITY_X, DEFAULT_LINEAR_VELOCITY_Y, DEFAULT_LINEAR_VELOCITY_Z );
    SetLinearVelocity ( defaultLinearVelocity );

    constexpr GXVec3 defaultAngularVelocity ( DEFAULT_ANGULAR_VELOCITY_X, DEFAULT_ANGULAR_VELOCITY_Y, DEFAULT_ANGULAR_VELOCITY_Z );
    SetAngularVelocity ( defaultAngularVelocity );

    SetLinearDamping ( DEFALUT_LINEAR_DAMPING );
    SetAngularDamping ( DEFAULT_ANGULAR_DAMPING );

    DisableKinematic ();
    ClearAccumulators ();
    
    SetAwake ();
    SetCanSleep ( GX_TRUE );
}

GXRigidBody::~GXRigidBody ()
{
    GXSafeDelete ( _shape );
}

GXVoid GXRigidBody::SetOnTransformChangedCallback ( GXVoid* handlerObject, PFNRIGIDBODYONTRANSFORMCHANGED callback )
{
    _handler = handlerObject;
    _OnTransformChanged = callback;
}

GXVoid GXRigidBody::CalculateCachedData ()
{
    _rotation.Normalize ();
    _transform.FromFast ( _rotation, _location );

    if ( _OnTransformChanged )
        _OnTransformChanged ( _handler, *this );

    GXMat3 alpha ( _transform );
    _inverseTransform.Transponse ( alpha );

    GXMat3 betta;
    betta.Multiply ( alpha, _shape->GetInertiaTensor () );

    GXMat3 inertiaTensorWorld;
    inertiaTensorWorld.Multiply ( betta, _inverseTransform );

    _inverseInertiaTensorWorld.Inverse ( inertiaTensorWorld );

    _shape->CalculateCacheData ();
}

GXVoid GXRigidBody::ClearAccumulators ()
{
    constexpr GXVec3 zero ( 0.0f, 0.0f, 0.0f );

    _totalForce = zero;
    _totalTorque = zero;
}

const GXMat3& GXRigidBody::GetInverseInertiaTensorWorld () const
{
    return _inverseInertiaTensorWorld;
}

GXVoid GXRigidBody::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
    _location.Init ( x, y, z );
    _transform.SetW ( _location );

    if ( _OnTransformChanged )
        _OnTransformChanged ( _handler, *this );

    if ( !_shape ) return;

    _shape->CalculateCacheData ();
}

GXVoid GXRigidBody::SetLocation ( const GXVec3 &newLocation )
{
    _location = newLocation;
    _transform.SetW ( _location );

    if ( _OnTransformChanged )
        _OnTransformChanged ( _handler, *this );

    if ( !_shape ) return;

    _shape->CalculateCacheData ();
}

const GXVec3& GXRigidBody::GetLocation () const
{
    return _location;
}

GXVoid GXRigidBody::SetRotaton ( const GXQuat &newRotation )
{
    _rotation = newRotation;
    _transform.From ( _rotation, _location );

    if ( _OnTransformChanged )
        _OnTransformChanged ( _handler, *this );

    if ( !_shape ) return;

    _shape->CalculateCacheData ();
    CalculateCachedData ();
}

const GXQuat& GXRigidBody::GetRotation () const
{
    return _rotation;
}

GXVoid GXRigidBody::SetLinearVelocity ( const GXVec3 &velocity )
{
    _linearVelocity = velocity;
}

const GXVec3& GXRigidBody::GetLinearVelocity () const
{
    return _linearVelocity;
}

GXVoid GXRigidBody::AddLinearVelocity ( const GXVec3 &velocity )
{
    _linearVelocity.Sum ( _linearVelocity, velocity );
}

GXVoid GXRigidBody::SetAngularVelocity ( const GXVec3 &velocity )
{
    _angularVelocity = velocity;
}

const GXVec3& GXRigidBody::GetAngularVelocity () const
{
    return _angularVelocity;
}

GXVoid GXRigidBody::AddAngularVelocity ( const GXVec3 &velocity )
{
    _angularVelocity.Sum ( _angularVelocity, velocity );
}

GXVoid GXRigidBody::SetMass ( GXFloat newMass )
{
    _mass = newMass;
    _inverseMass = 1.0f / _mass;
}

GXFloat GXRigidBody::GetMass () const
{
    return _mass;
}

GXFloat GXRigidBody::GetInverseMass () const
{
    return _inverseMass;
}

GXVoid GXRigidBody::SetLinearDamping ( GXFloat damping )
{
    _linearDamping = damping;
}

GXFloat GXRigidBody::GetLinearDamping () const
{
    return _linearDamping;
}

GXVoid GXRigidBody::SetAngularDamping ( GXFloat damping )
{
    _angularDamping = damping;
}

GXFloat GXRigidBody::GetAngularDamping () const
{
    return _angularDamping;
}

GXVoid GXRigidBody::SetShape ( GXShape &newShape )
{
    _shape = &newShape;
    _shape->CalculateInertiaTensor ( _mass );
    _shape->CalculateCacheData ();
    CalculateCachedData ();
}

GXShape& GXRigidBody::GetShape ()
{
    return *_shape;
}

const GXVec3& GXRigidBody::GetLastFrameAcceleration () const
{
    return _lastFrameAcceleration;
}

GXVoid GXRigidBody::TranslatePointToWorld ( GXVec3 &out, const GXVec3 &pointLocal )
{
    _transform.MultiplyAsPoint ( out, pointLocal );
}

const GXMat4& GXRigidBody::GetTransform () const
{
    return _transform;
}

GXVoid GXRigidBody::SetAwake ()
{
    _isAwake = GX_TRUE;
    _sleepTimeout = 0.0f;
}

GXVoid GXRigidBody::SetSleep ()
{
    constexpr GXVec3 zero ( 0.0f, 0.0f, 0.0f );

    _isAwake = GX_FALSE;
    _linearVelocity = zero;
    _angularVelocity = zero;
}

GXBool GXRigidBody::IsAwake () const
{
    return _isAwake;
}

GXVoid GXRigidBody::EnableKinematic ()
{
    _isKinematic = GX_TRUE;
}

GXVoid GXRigidBody::DisableKinematic ()
{
    _isKinematic = GX_FALSE;
}

GXBool GXRigidBody::IsKinematic () const
{
    return _isKinematic;
}

GXVoid GXRigidBody::AddForce ( const GXVec3 &forceWorld )
{
    _totalForce.Sum ( _totalForce, forceWorld );
}

GXVoid GXRigidBody::SetCanSleep ( GXBool isCanSleep )
{
    _canSleep = isCanSleep;

    if ( !_canSleep && !_isAwake )
    {
        SetAwake ();
    }
}

GXVoid GXRigidBody::AddForceAtPointLocal ( const GXVec3 &forceWorld, const GXVec3 &pointLocal )
{
    GXVec3 pointWorld;
    TranslatePointToWorld ( pointWorld, pointLocal );
    AddForceAtPointWorld ( forceWorld, pointWorld );
}

GXVoid GXRigidBody::AddForceAtPointWorld ( const GXVec3 &forceWorld, const GXVec3 &pointWorld )
{
    _totalForce.Sum ( _totalForce, forceWorld );

    GXVec3 p;
    p.Substract ( pointWorld, _location );

    GXVec3 torque;
    torque.CrossProduct ( p, forceWorld );
    _totalTorque.Sum ( _totalTorque, torque );

    _isAwake = GX_TRUE;
}

const GXVec3& GXRigidBody::GetTotalForce () const
{
    return _totalForce;
}

const GXVec3& GXRigidBody::GetTotalTorque () const
{
    return _totalTorque;
}

GXVoid GXRigidBody::AddImpulseAtPointWorld ( const GXVec3 &impulseWorld, const GXVec3 &pointWorld )
{
    if ( _isKinematic ) return;

    GXVec3 centerOfMassToPointWorld;
    centerOfMassToPointWorld.Substract ( pointWorld, _location );

    GXVec3 alpha ( centerOfMassToPointWorld );
    alpha.Normalize ();

    GXVec3 betta ( impulseWorld );
    betta.Normalize ();

    GXVec3 gamma;
    gamma.Multiply ( impulseWorld, -alpha.DotProduct ( betta ) * _inverseMass );

    _linearVelocity.Sum ( _linearVelocity, gamma );

    if ( _isAwake ) return;

    _isAwake = GX_TRUE;
    // TODO
}

GXVoid GXRigidBody::Integrate ( GXFloat deltaTime )
{
    if ( _isKinematic )
    {
        _location.Sum ( _location, deltaTime, _linearVelocity );

        GXQuat betta ( 0.0f, _angularVelocity.GetX (), _angularVelocity.GetY (), _angularVelocity.GetZ () );
        betta.Multiply ( betta, deltaTime * 0.5f );

        GXQuat gamma;
        gamma.Multiply ( betta, _rotation );

        _rotation.Sum ( _rotation, gamma );

        CalculateCachedData ();
        return;
    }

    if ( !_isAwake ) return;

    _lastFrameAcceleration.Sum ( _acceleration, _inverseMass, _totalForce );

    GXVec3 angularAcceleration;
    _inverseInertiaTensorWorld.MultiplyMatrixVector ( angularAcceleration, _totalTorque );

    _linearVelocity.Sum ( _linearVelocity, deltaTime, _lastFrameAcceleration );
    _angularVelocity.Sum ( _angularVelocity, deltaTime, angularAcceleration );

    _linearVelocity.Multiply ( _linearVelocity, powf ( _linearDamping, deltaTime ) );
    _angularVelocity.Multiply ( _angularVelocity, powf ( _angularDamping, deltaTime ) );

    _lastFrameLocation = _location;
    _location.Sum ( _location, deltaTime, _linearVelocity );

    GXQuat betta ( 0.0f, _angularVelocity.GetX (), _angularVelocity.GetY (), _angularVelocity.GetZ () );
    betta.Multiply ( betta, deltaTime * 0.5f );

    GXQuat gamma;
    gamma.Multiply ( betta, _rotation );

    _lastFrameRotation = _rotation;
    _rotation.Sum ( _rotation, gamma );
    
    CalculateCachedData ();

    if ( !_canSleep ) return;

    GXPhysicsEngine& physicsEngine = GXPhysicsEngine::GetInstance ();

    GXVec3 locationDifference;
    locationDifference.Substract ( _location, _lastFrameLocation );

    if ( locationDifference.SquaredLength () > physicsEngine.GetMaximumLocationChangeSquaredDeviation () )
    {
        _sleepTimeout = 0.0f;
        return;
    }

    const GXVec4 yotta ( _rotation.GetA (), _rotation.GetB (), _rotation.GetC (), _rotation.GetR () );
    const GXVec4 phi ( _lastFrameRotation.GetA (), _lastFrameRotation.GetB (), _lastFrameRotation.GetC (), _lastFrameRotation.GetR () );

    GXVec4 difference;
    difference.Substract ( yotta, phi );

    if ( difference.SquaredLength () > physicsEngine.GetMaximumRotationChangeSquaredDeviation () )
    {
        _sleepTimeout = 0.0f;
        return;
    }

    _sleepTimeout += deltaTime;

    if ( _sleepTimeout < physicsEngine.GetSleepTimeout () ) return;

    SetSleep ();
}
