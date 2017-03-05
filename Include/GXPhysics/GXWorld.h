//version 1.0

#ifndef GX_WORLD
#define GX_WORLD


#include "GXRigidBody.h"


struct GXBodyRegistration;
class GXWorld
{
	GXBodyRegistration* bodies;

	public:
		GXVoid BeginFrame ();
		GXVoid RunPhysics ( GXFloat deltaTime );

};


#endif GX_WORLD
