// version 1.0

#ifndef GX_SPRING
#define GX_SPRING


#include "GXForceGenerator.h"


class GXSpring : public GXForceGenerator
{
	private:
		GXVec3			connectionPointLocal;
		GXVec3			otherConnectionPointLocal;

		GXRigidBody*	otherBody;

		GXFloat			hardness;
		GXFloat			restLength;

	public:
		GXSpring (const GXVec3 &connectionPointLocal, GXRigidBody &otherBody, const GXVec3 &otherConnectionPointLocal, GXFloat hardness, GXFloat restLength );
		GXVoid UpdateForce ( GXRigidBody &body, GXFloat deltaTime ) override;
};


#endif // GX_SPRING
