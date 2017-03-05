//version 1.0

#ifndef GX_GRAVITY
#define GX_GRAVITY


#include "GXForceGenerator.h"


class GXGravity : public GXForceGenerator
{
	private:
		GXVec3		gravity;

	public:
		GXGravity ( const GXVec3 &gravity );
		GXVoid UpdateForce ( GXRigidBody &body, GXFloat deltaTime ) override;
};


#endif GX_GRAVITY
