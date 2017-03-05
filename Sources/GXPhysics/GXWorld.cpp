//version 1.0

#include <GXPhysics/GXWorld.h>

struct GXBodyRegistration
{
	GXBodyRegistration*		next;
	GXBodyRegistration*		prev;

	GXRigidBody*			body;
};

GXVoid GXWorld::BeginFrame ()
{
	for ( GXBodyRegistration* p = bodies; p; p = p->next )
	{
		p->body->ClearAccumulators ();
		p->body->CalculateCachedData ();
	}
}

GXVoid GXWorld::RunPhysics ( GXFloat deltaTime )
{
	for ( GXBodyRegistration* p = bodies; p; p = p->next )
		p->body->Integrate ( deltaTime );
}
