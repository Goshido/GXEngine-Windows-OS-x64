//version 1.0

#include <GXPhysics/GXRigidBody.h>
#include <GXPhysics/GXShape.h>
#include <GXPhysics/GXPhysicsEngine.h>
#include <GXCommon/GXMemory.h>


#define DEFAULT_MASS				1.0f

#define DEFAULT_LOCATION_X			0.0f
#define DEFAULT_LOCATION_Y			0.0f
#define DEFAULT_LOCATION_Z			0.0f

#define DEFAULT_ROTATION_AXIS_X		0.0f
#define DEFAULT_ROTATION_AXIS_Y		0.0f
#define DEFAULT_ROTATION_AXIS_Z		1.0f
#define DEFAULT_ROTATION_ANGLE		0.0f

#define DEFAULT_LINEAR_VELOCITY_X	0.0f
#define DEFAULT_LINEAR_VELOCITY_Y	0.0f
#define DEFAULT_LINEAR_VELOCITY_Z	0.0f

#define DEFAULT_ANGULAR_VELOCITY_X	0.0f
#define DEFAULT_ANGULAR_VELOCITY_Y	0.0f
#define DEFAULT_ANGULAR_VELOCITY_Z	0.0f

#define DEFALUT_LINEAR_DAMPING		0.8f
#define DEFAULT_ANGULAR_DAMPING		0.8f


GXRigidBody::GXRigidBody ()
{
	shape = nullptr;

	handler = nullptr;
	OnTransformChanged = nullptr;

	SetMass ( DEFAULT_MASS );

	GXVec3 vec ( DEFAULT_LOCATION_X, DEFAULT_LOCATION_Y, DEFAULT_LOCATION_Z );
	SetLocation ( vec );

	GXQuat rot;
	vec.Init ( DEFAULT_ROTATION_AXIS_X, DEFAULT_ROTATION_AXIS_Y, DEFAULT_ROTATION_AXIS_Z );
	rot.FromAxisAngle ( vec, DEFAULT_ROTATION_ANGLE );
	SetRotaton ( rot );

	vec.Init ( DEFAULT_LINEAR_VELOCITY_X, DEFAULT_LINEAR_VELOCITY_Y, DEFAULT_LINEAR_VELOCITY_Z );
	SetLinearVelocity ( vec );

	vec.Init ( DEFAULT_ANGULAR_VELOCITY_X, DEFAULT_ANGULAR_VELOCITY_Y, DEFAULT_ANGULAR_VELOCITY_Z );
	SetAngularVelocity ( vec );

	SetLinearDamping ( DEFALUT_LINEAR_DAMPING );
	SetAngularDamping ( DEFAULT_ANGULAR_DAMPING );

	acceleration = GXPhysicsEngine::GetInstance ().GetGravity ();

	DisableKinematic ();
	ClearAccumulators ();
}

GXRigidBody::~GXRigidBody ()
{
	GXSafeDelete ( shape );
}

GXVoid GXRigidBody::SetOnTransformChangedCallback ( GXVoid* handlerObject, PFNRIGIDBODYONTRANSFORMCHANGED callback )
{
	handler = handlerObject;
	OnTransformChanged = callback;
}

GXVoid GXRigidBody::CalculateCachedData ()
{
	rotation.Normalize ();
	transform.From ( rotation, location );

	if ( OnTransformChanged )
		OnTransformChanged ( handler, *this );

	GXMat3 localToWorld;
	localToWorld.From ( transform );
	GXMat3 worldToLocal;
	worldToLocal.Transponse ( localToWorld );
	GXMat3 transform1;
	transform1.Multiply ( worldToLocal, invInertiaTensorLocal );
	invInertiaTensorWorld.Multiply ( transform1, localToWorld );

	shape->CalculateCacheData ();
}

GXVoid GXRigidBody::ClearAccumulators ()
{
	totalForce.Init ( 0.0f, 0.0f, 0.0f );
	totalTorque.Init ( 0.0f, 0.0f, 0.0f );
}

GXVoid GXRigidBody::SetInertiaTensor ( const GXMat3 &inertiaTensor )
{
	invInertiaTensorLocal.Inverse ( inertiaTensor );
	CalculateCachedData ();
}

const GXMat3& GXRigidBody::GetInverseInertiaTensorWorld () const
{
	return invInertiaTensorWorld;
}

GXVoid GXRigidBody::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	location.Init ( x, y, z );
	transform.SetW ( location );

	if ( OnTransformChanged )
		OnTransformChanged ( handler, *this );

	if ( shape )
		shape->CalculateCacheData ();
}

GXVoid GXRigidBody::SetLocation ( const GXVec3 &newLocation )
{
	location = newLocation;
	transform.SetW ( location );

	if ( OnTransformChanged )
		OnTransformChanged ( handler, *this );

	if ( shape )
		shape->CalculateCacheData ();
}

const GXVec3& GXRigidBody::GetLocation () const
{
	return location;
}

GXVoid GXRigidBody::SetRotaton ( const GXQuat &newRotation )
{
	rotation = newRotation;
	transform.From ( rotation, location );

	if ( OnTransformChanged )
		OnTransformChanged ( handler, *this );

	if ( shape )
		shape->CalculateCacheData ();
}

const GXQuat& GXRigidBody::GetRotation () const
{
	return rotation;
}

GXVoid GXRigidBody::SetLinearVelocity ( const GXVec3 &velocity )
{
	linearVelocity = velocity;
}

const GXVec3& GXRigidBody::GetLinearVelocity () const
{
	return linearVelocity;
}

GXVoid GXRigidBody::AddLinearVelocity ( const GXVec3 &velocity )
{
	linearVelocity.Sum ( linearVelocity, velocity );
}

GXVoid GXRigidBody::SetAngularVelocity ( const GXVec3 velocity )
{
	angularVelocity = velocity;
}

const GXVec3& GXRigidBody::GetAngularVelocity () const
{
	return angularVelocity;
}

GXVoid GXRigidBody::AddAngularVelocity ( const GXVec3 &velocity )
{
	angularVelocity.Sum ( angularVelocity, velocity );
}

GXVoid GXRigidBody::SetMass ( GXFloat newMass )
{
	mass = newMass;
	invMass = 1.0f / mass;
}

GXFloat GXRigidBody::GetMass () const
{
	return mass;
}

GXFloat GXRigidBody::GetInverseMass () const
{
	return invMass;
}

GXVoid GXRigidBody::SetLinearDamping ( GXFloat damping )
{
	linearDamping = damping;
}

GXFloat GXRigidBody::GetLinearDamping () const
{
	return linearDamping;
}

GXVoid GXRigidBody::SetAngularDamping ( GXFloat damping )
{
	angularDamping = damping;
}

GXFloat GXRigidBody::GetAngularDamping () const
{
	return angularDamping;
}

GXVoid GXRigidBody::SetShape ( GXShape &newShape )
{
	shape = &newShape;
	shape->CalculateInertiaTensor ( mass );
	SetInertiaTensor ( shape->GetInertialTensor () );
}

GXShape& GXRigidBody::GetShape ()
{
	return *shape;
}

const GXVec3& GXRigidBody::GetLastFrameAcceleration () const
{
	return lastFrameAcceleration;
}

GXVoid GXRigidBody::TranslatePointToWorld ( GXVec3 &out, const GXVec3 &pointLocal )
{
	transform.MultiplyAsPoint ( out, pointLocal );
}

const GXMat4& GXRigidBody::GetTransform ()
{
	return transform;
}

GXVoid GXRigidBody::SetAwake ()
{
	isAwake = GX_TRUE;
	motion = GXPhysicsEngine::GetInstance ().GetSleepEpsilon () * 2.0f;
}

GXVoid GXRigidBody::SetSleep ()
{
	isAwake = GX_FALSE;
	linearVelocity.Init ( 0.0f, 0.0f, 0.0f );
	angularVelocity.Init ( 0.0f, 0.0f, 0.0f );
}

GXBool GXRigidBody::IsAwake () const
{
	return isAwake;
}

GXVoid GXRigidBody::EnableKinematic ()
{
	isKinematic = GX_TRUE;
}

GXVoid GXRigidBody::DisableKinematic ()
{
	isKinematic = GX_FALSE;
}

GXBool GXRigidBody::IsKinematic () const
{
	return isKinematic;
}

GXVoid GXRigidBody::AddForce ( const GXVec3 &forceWorld )
{
	totalForce.Sum ( totalForce, forceWorld );
}

GXVoid GXRigidBody::SetCanSleep ( GXBool isCanSleep )
{
	canSleep = isCanSleep;

	if ( !canSleep && !isAwake )
		SetAwake ();
}

GXVoid GXRigidBody::AddForceAtPointLocal ( const GXVec3 &forceWorld, const GXVec3 &pointLocal )
{
	GXVec3 pointWorld;
	TranslatePointToWorld ( pointWorld, pointLocal );
	AddForceAtPointWorld ( forceWorld, pointWorld );
}

GXVoid GXRigidBody::AddForceAtPointWorld ( const GXVec3 &forceWorld, const GXVec3 &pointWorld )
{
	totalForce.Sum ( totalForce, forceWorld );

	GXVec3 p;
	p.Substract ( pointWorld, location );

	GXVec3 torque;
	torque.CrossProduct ( p, forceWorld );
	totalTorque.Sum ( totalTorque, torque );

	isAwake = GX_TRUE;
}

GXVoid GXRigidBody::Integrate ( GXFloat deltaTime )
{
	if ( isKinematic )
	{
		CalculateCachedData ();
		return;
	}

	if ( !isAwake ) return;

	lastFrameAcceleration.Sum ( acceleration, invMass, totalForce );

	GXVec3 angularAcceleration;
	invInertiaTensorWorld.Multiply ( angularAcceleration, totalTorque );

	linearVelocity.Sum ( linearVelocity, deltaTime, lastFrameAcceleration );
	angularVelocity.Sum ( angularVelocity, deltaTime, angularAcceleration );

	linearVelocity.Multiply ( linearVelocity, powf ( linearDamping, deltaTime ) );
	angularVelocity.Multiply ( angularVelocity, powf ( angularDamping, deltaTime ) );

	location.Sum ( location, deltaTime, linearVelocity );
	rotation.Sum ( rotation, deltaTime, angularVelocity );
	
	ClearAccumulators ();
	CalculateCachedData ();

	return;
	/*
	if ( canSleep )
	{
		GXFloat currentMotion = GXDotVec3 ( linearVelocity, linearVelocity ) + GXDotVec3 ( angularVelocity, angularVelocity );
		GXFloat bias = powf ( 0.5f, deltaTime );

		motion = bias * motion + ( 1.0f - bias ) * currentMotion;
		GXFloat sleepEpsilon = GXPhysicsEngine::GetInstance ().GetSleepEpsilon ();

		if ( motion < sleepEpsilon )
			SetSleep ();
		else if ( motion > 10 * sleepEpsilon )
			motion = 10 * sleepEpsilon;
	}
	*/
}
