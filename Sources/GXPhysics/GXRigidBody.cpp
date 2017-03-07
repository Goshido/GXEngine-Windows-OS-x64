//version 1.0

#include <GXPhysics/GXRigidBody.h>

GXVoid GXRigidBody::CalculateCachedData ()
{
	transform.From ( rotation, location );

	GXMat3 localToWorld;
	GXSetMat3FromMat4 ( localToWorld, transform );
	GXMat3 worldToLocal;
	GXSetMat3Transponse ( worldToLocal, localToWorld );
	GXMat3 transform1;
	GXMulMat3Mat3 ( transform1, worldToLocal, invInertiaTensorLocal );
	GXMulMat3Mat3 ( invInertiaTensorWorld, transform1, localToWorld );
}

GXVoid GXRigidBody::ClearAccumulators ()
{
	totalForce = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	totalTorque = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
}

GXVoid GXRigidBody::SetInertiaTensor ( const GXMat3 &inertiaTensor )
{
	GXSetMat3Inverse ( invInertiaTensorLocal, inertiaTensor );
}

const GXMat3& GXRigidBody::GetInverseInertiaTensorWorld () const
{
	return invInertiaTensorWorld;
}

GXVoid GXRigidBody::SetLocation ( const GXVec3 &location )
{
	this->location = location;
}

const GXVec3& GXRigidBody::GetLocation () const
{
	return location;
}

GXVoid GXRigidBody::SetRotaton ( const GXQuat &rotation )
{
	this->rotation = rotation;
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

GXVoid GXRigidBody::SetAngularVelocity ( const GXVec3 velocity )
{
	angularVelocity = velocity;
}

const GXVec3& GXRigidBody::GetAngularVelocity () const
{
	return angularVelocity;
}

GXFloat GXRigidBody::GetMass () const
{
	return mass;
}

GXFloat GXRigidBody::GetInverseMass () const
{
	return invMass;
}

GXBool GXRigidBody::HasFiniteMass () const
{
	return invMass >= 0.0f;
}

const GXVec3& GXRigidBody::GetLastFrameAcceleration () const
{
	return lastFrameAcceleration;
}

GXVoid GXRigidBody::TranslatePointToWorld ( GXVec3 &out, const GXVec3 &pointLocal )
{
	GXMulVec3Mat4AsPoint ( out, pointLocal, transform );
}

const GXMat4& GXRigidBody::GetTransform ()
{
	return transform;
}

GXVoid GXRigidBody::SetAwake ()
{
	isAwake = GX_TRUE;
}

GXBool GXRigidBody::IsAwake () const
{
	return isAwake;
}

GXVoid GXRigidBody::AddForce ( const GXVec3 &forceWorld )
{
	GXSumVec3Vec3 ( totalForce, totalForce, forceWorld );
}

GXVoid GXRigidBody::AddForceAtPointLocal ( const GXVec3 &forceWorld, const GXVec3 &pointLocal )
{
	GXVec3 pointWorld;
	TranslatePointToWorld ( pointWorld, pointLocal );
	AddForceAtPointWorld ( forceWorld, pointWorld );
}

GXVoid GXRigidBody::AddForceAtPointWorld ( const GXVec3 &forceWorld, const GXVec3 &pointWorld )
{
	GXSumVec3Vec3 ( totalForce, totalForce, forceWorld );

	GXVec3 p;
	GXSubVec3Vec3 ( p, pointWorld, location );

	GXVec3 torque;
	GXCrossVec3Vec3 ( torque, p, forceWorld );
	GXSumVec3Vec3 ( totalTorque, totalTorque, torque );

	isAwake = GX_TRUE;
}

GXVoid GXRigidBody::Integrate ( GXFloat deltaTime )
{
	lastFrameAcceleration = acceleration;

	GXVec3 forceFactor;
	GXMulVec3Scalar ( forceFactor, totalForce, invMass );
	GXSumVec3Vec3 ( lastFrameAcceleration, lastFrameAcceleration, forceFactor );

	GXVec3 angularAcceleration;
	GXMulVec3Mat3 ( angularAcceleration, totalTorque, invInertiaTensorWorld );

	GXVec3 linearVelocityDelta;
	GXMulVec3Scalar ( linearVelocityDelta, lastFrameAcceleration, deltaTime );
	GXSumVec3Vec3 ( linearVelocity, linearVelocity, linearVelocityDelta );

	GXVec3 angularVelocityDelta;
	GXMulVec3Scalar ( angularVelocityDelta, angularAcceleration, deltaTime );
	GXSumVec3Vec3 ( angularVelocity, angularVelocity, angularVelocityDelta );

	GXMulVec3Scalar ( linearVelocity, linearVelocity, powf ( linearDamping, deltaTime ) );
	GXMulVec3Scalar ( angularVelocity, angularVelocity, powf ( angularDamping, deltaTime ) );

	GXVec3 locationDelta;
	GXMulVec3Scalar ( locationDelta, linearVelocity, deltaTime );
	GXSumVec3Vec3 ( location, location, locationDelta );

	GXSumQuatScaledVec3 ( rotation, rotation, deltaTime, angularVelocity );

	CalculateCachedData ();
	ClearAccumulators ();
}
