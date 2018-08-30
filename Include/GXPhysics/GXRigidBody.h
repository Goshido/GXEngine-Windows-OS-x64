// version 1.3

#ifndef GX_RIGID_BODY
#define GX_RIGID_BODY


#include <GXCommon/GXMath.h>


class GXRigidBody;
typedef GXVoid ( GXCALL* PFNRIGIDBODYONTRANSFORMCHANGED ) ( GXVoid* handler, const GXRigidBody &rigidBody );

class GXShape;
class GXRigidBody final
{
	private:
		GXFloat							mass;
		GXFloat							inverseMass;

		GXVec3							location;
		GXVec3							lastFrameLocation;

		GXQuat							rotation;
		GXQuat							lastFrameRotation;

		GXVec3							linearVelocity;
		GXVec3							angularVelocity;

		GXVec3							totalForce;
		GXVec3							totalTorque;

		GXFloat							linearDamping;
		GXFloat							angularDamping;

		GXBool							isAwake;
		GXBool							canSleep;
		GXFloat							sleepTimeout;
		GXBool							isKinematic;

		GXVec3							acceleration;
		GXVec3							lastFrameAcceleration;

		GXMat4							transform;
		GXMat3							inverseTransform;
		GXMat3							inverseInertiaTensorWorld;

		GXShape*						shape;

		GXVoid*							handler;
		PFNRIGIDBODYONTRANSFORMCHANGED	OnTransformChanged;

	public:
		GXRigidBody ();
		~GXRigidBody ();

		GXVoid SetOnTransformChangedCallback ( GXVoid* handlerObject, PFNRIGIDBODYONTRANSFORMCHANGED callback );

		GXVoid CalculateCachedData ();
		GXVoid ClearAccumulators ();

		const GXMat3& GetInverseInertiaTensorWorld () const;

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetLocation ( const GXVec3 &newLocation );
		const GXVec3& GetLocation () const;

		GXVoid SetRotaton ( const GXQuat &newRotation );
		const GXQuat& GetRotation () const;

		GXVoid SetLinearVelocity ( const GXVec3 &velocity );
		const GXVec3& GetLinearVelocity () const;
		GXVoid AddLinearVelocity ( const GXVec3 &velocity );

		GXVoid SetAngularVelocity ( const GXVec3 &velocity );
		const GXVec3& GetAngularVelocity () const;
		GXVoid AddAngularVelocity ( const GXVec3 &velocity );
		
		GXVoid SetMass ( GXFloat newMass );
		GXFloat GetMass () const;
		GXFloat GetInverseMass () const;

		GXVoid SetLinearDamping ( GXFloat damping );
		GXFloat GetLinearDamping () const;

		GXVoid SetAngularDamping ( GXFloat damping );
		GXFloat GetAngularDamping () const;

		GXVoid SetShape ( GXShape &newShape );
		GXShape& GetShape ();

		const GXVec3& GetLastFrameAcceleration () const;

		GXVoid TranslatePointToWorld ( GXVec3 &out, const GXVec3 &pointLocal );
		const GXMat4& GetTransform () const;

		GXVoid SetAwake ();
		GXVoid SetSleep ();
		GXBool IsAwake () const;
		GXVoid SetCanSleep ( GXBool isCanSleep );

		GXVoid EnableKinematic ();
		GXVoid DisableKinematic ();
		GXBool IsKinematic () const;

		GXVoid AddForce ( const GXVec3 &forceWorld );
		GXVoid AddForceAtPointLocal ( const GXVec3 &forceWorld, const GXVec3 &pointLocal );
		GXVoid AddForceAtPointWorld ( const GXVec3 &forceWorld, const GXVec3 &pointWorld );
		const GXVec3& GetTotalForce () const;

		const GXVec3& GetTotalTorque () const;

		GXVoid AddImpulseAtPointWorld ( const GXVec3 &impulseWorld, const GXVec3 &pointWorld );

		GXVoid Integrate ( GXFloat deltaTime );
};


#endif // GX_RIGID_BODY
