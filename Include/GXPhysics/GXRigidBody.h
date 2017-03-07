//version 1.0

#ifndef GX_RIGID_BODY
#define GX_RIGID_BODY


#include <GXCommon/GXMath.h>


class GXRigidBody
{
	private:
		GXFloat		mass;
		GXFloat		invMass;

		GXVec3		location;
		GXQuat		rotation;

		GXVec3		linearVelocity;
		GXVec3		angularVelocity;
		GXMat3		invInertiaTensorLocal;

		GXVec3		totalForce;
		GXVec3		totalTorque;

		GXFloat		linearDamping;
		GXFloat		angularDamping;
		GXBool		isAwake;

		GXVec3		acceleration;
		GXVec3		lastFrameAcceleration;

		GXMat4		transform;
		GXMat3		invInertiaTensorWorld;

	public:
		GXVoid CalculateCachedData ();
		GXVoid ClearAccumulators ();

		GXVoid SetInertiaTensor ( const GXMat3 &inertiaTensor );
		const GXMat3& GetInverseInertiaTensorWorld () const;

		GXVoid SetLocation ( const GXVec3 &location );
		const GXVec3& GetLocation () const;

		GXVoid SetRotaton ( const GXQuat &rotation );
		const GXQuat& GetRotation () const;

		GXVoid SetLinearVelocity ( const GXVec3 &velocity );
		const GXVec3& GetLinearVelocity () const;

		GXVoid SetAngularVelocity ( const GXVec3 velocity );
		const GXVec3& GetAngularVelocity () const;
		
		GXFloat GetMass () const;
		GXFloat GetInverseMass () const;
		GXBool HasFiniteMass () const;

		const GXVec3& GetLastFrameAcceleration () const;

		GXVoid TranslatePointToWorld ( GXVec3 &out, const GXVec3 &pointLocal );
		const GXMat4& GetTransform ();

		GXVoid SetAwake ();
		GXBool IsAwake () const;

		GXVoid AddForce ( const GXVec3 &forceWorld );
		GXVoid AddForceAtPointLocal ( const GXVec3 &forceWorld, const GXVec3 &pointLocal );
		GXVoid AddForceAtPointWorld ( const GXVec3 &forceWorld, const GXVec3 &pointWorld );


		GXVoid Integrate ( GXFloat deltaTime );
};


#endif GX_RIGID_BODY