// version 1.2

#ifndef GX_SPRING
#define GX_SPRING


#include "GXForceGenerator.h"


class GXSpring final : public GXForceGenerator
{
	private:
		GXVec3			connectionPointLocal;
		GXVec3			otherConnectionPointLocal;

		GXRigidBody*	otherBody;

		GXFloat			hardness;
		GXFloat			restLength;

	public:
		explicit GXSpring ( const GXVec3 &connectionPointLocal, GXRigidBody &otherBody, const GXVec3 &otherConnectionPointLocal, GXFloat hardness, GXFloat restLength );
		~GXSpring () override;

		GXVoid UpdateForce ( GXRigidBody &body, GXFloat deltaTime ) override;

	private:
		GXSpring () = delete;
		GXSpring ( const GXSpring &other ) = delete;
		GXSpring& operator = ( const GXSpring &other ) = delete;
};


#endif // GX_SPRING
