// version 1.0

#ifndef GX_FORCE_GENERATOR
#define GX_FORCE_GENERATOR


#include "GXRigidBody.h"


class GXForceGenerator
{
	public:
		virtual GXVoid UpdateForce ( GXRigidBody &body, GXFloat deltaTime ) = 0;
};


#endif // GX_FORCE_GENERATOR
