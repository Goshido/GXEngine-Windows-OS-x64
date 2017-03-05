//version 1.0

#ifndef GX_TORQUE_GENERATOR
#define GX_TORQUE_GENERATOR


#include "GXRigidBody.h"


class GXTorgueGenerator
{
	public:
		virtual GXVoid UpdateTorque ( GXRigidBody &body, GXFloat deltaTime ) = 0;
};


#endif GX_TORQUE_GENERATOR
