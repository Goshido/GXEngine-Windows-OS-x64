//version 1.0

#include <GXPhysics/GXWorld.h>
#include <GXPhysics/GXContactGenerator.h>
#include <GXCommon/GXLogger.h>


struct GXRigidBodyRegistration
{
	GXRigidBodyRegistration*		next;
	GXRigidBodyRegistration*		prev;

	GXRigidBody*					body;
};

//----------------------------------------------------------------

struct GXContactGeneratorsRegistration
{
	GXContactGeneratorsRegistration*	next;
	GXContactGeneratorsRegistration*	prev;

	GXContactGenerator*					generator;
};

//----------------------------------------------------------------

struct GXForceGeneratorsRegistration
{
	GXForceGeneratorsRegistration*		next;
	GXForceGeneratorsRegistration*		prev;

	GXForceGenerator*					generator;
	GXRigidBody*						body;
};

//----------------------------------------------------------------

GXWorld::GXWorld ( GXUInt maxContacts, GXUInt iterations ):
contactResolver ( iterations ), collisions ( maxContacts )
{
	GXCollisionDetector::GetInstance ();

	bodies = nullptr;
	contactGenerators = nullptr;
	forceGenerators = nullptr;
	isCalculateIterations = iterations == 0;
}

GXWorld::~GXWorld ()
{
	delete &( GXCollisionDetector::GetInstance () );

	ClearRigidBodyRegistrations ();
	ClearForceGeneratorRegistrations ();
}

GXVoid GXWorld::RegisterRigidBody ( GXRigidBody &body )
{
	GXRigidBodyRegistration* reg = new GXRigidBodyRegistration ();
	reg->body = &body;
	reg->next = nullptr;
	reg->prev = nullptr;

	if ( bodies )
		bodies->prev = reg;

	reg->next = bodies;
	bodies = reg;
}


GXVoid GXWorld::UnregisterRigidBody ( GXRigidBody &body )
{
	GXRigidBodyRegistration* reg = FindRigidBodyRegistration ( body );
	if ( !reg )
	{
		GXLogW ( L"GXWorld::UnregisterRigidBody::Error - Can't find rigid body!" );
		return;
	}

	if ( reg == bodies )
	{
		if ( !reg->next )
		{
			bodies = nullptr;
		}
		else
		{
			bodies = bodies->next;
			bodies->prev = nullptr;
		}

		delete reg;
	}
	else if ( !reg->next )
	{
		reg->prev->next = nullptr;
		delete reg;
	}
	else
	{
		reg->next->prev = reg->prev;
		reg->prev->next = reg->next;

		delete reg;
	}
}

GXVoid GXWorld::ClearRigidBodyRegistrations ()
{
	while ( bodies )
	{
		GXRigidBodyRegistration* reg = bodies;
		bodies = bodies->next;
		delete reg;
	}
}

GXVoid GXWorld::RegisterForceGenerator ( GXRigidBody &body, GXForceGenerator &generator )
{
	GXForceGeneratorsRegistration* reg = new GXForceGeneratorsRegistration ();
	reg->body = &body;
	reg->generator = &generator;
	reg->next = nullptr;
	reg->prev = nullptr;

	if ( forceGenerators )
		forceGenerators->prev = reg;

	reg->next = forceGenerators;
	forceGenerators = reg;
}

GXVoid GXWorld::UnregisterForceGenerator ( GXRigidBody &body, GXForceGenerator &generator )
{
	GXForceGeneratorsRegistration* reg = FindForceGeneratorRegistration ( body, generator );
	if ( !reg )
	{
		GXLogW ( L"GXWorld::UnregisterForceGenerator::Error - Can't find force generator!" );
		return;
	}

	if ( reg == forceGenerators )
	{
		if ( !reg->next )
		{
			forceGenerators = nullptr;
		}
		else
		{
			forceGenerators = forceGenerators->next;
			forceGenerators->prev = nullptr;
		}

		delete reg;
	}
	else if ( !reg->next )
	{
		reg->prev->next = nullptr;
		delete reg;
	}
	else
	{
		reg->next->prev = reg->prev;
		reg->prev->next = reg->next;

		delete reg;
	}
}

GXVoid GXWorld::ClearForceGeneratorRegistrations ()
{
	while ( forceGenerators )
	{
		GXForceGeneratorsRegistration* reg = forceGenerators;
		forceGenerators = forceGenerators->next;
		delete reg;
	}
}

GXVoid GXWorld::RunPhysics ( GXFloat deltaTime )
{
	collisions.Reset ();

	// We do not do anything unless 2 rigid bodies were registered
	if ( !bodies || !bodies->next ) return;

	for ( GXForceGeneratorsRegistration* p = forceGenerators; p; p = p->next )
		p->generator->UpdateForce ( *p->body, deltaTime );

	for ( GXRigidBodyRegistration* p = bodies; p; p = p->next )
		p->body->Integrate ( deltaTime );

	for ( GXContactGeneratorsRegistration* reg = contactGenerators; reg; reg = reg->next )
	{
		reg->generator->AddContact ( collisions );
		if ( !collisions.HasMoreContacts () ) break;
	}

	GXCollisionDetector& collisionDetector = GXCollisionDetector::GetInstance ();

	for ( GXRigidBodyRegistration* p = bodies; p->next; p = p->next )
	{
		for ( GXRigidBodyRegistration* q = p->next; q; q = q->next )
		{
			if ( p->body->IsKinematic () && q->body->IsKinematic () ) continue;

			GXShape& shapeA = p->body->GetShape ();
			GXShape& shapeB = q->body->GetShape ();

			collisionDetector.Check ( shapeA, shapeB, collisions );
		}
	}

	contactResolver.ResolveContacts ( collisions.GetAllContacts (), collisions.GetTotalContacts (), deltaTime );
}

const GXCollisionData& GXWorld::GetCollisionData () const
{
	return collisions;
}

GXRigidBodyRegistration* GXWorld::FindRigidBodyRegistration ( GXRigidBody &body )
{
	for ( GXRigidBodyRegistration* p = bodies; p; p = p->next )
	{
		if ( p->body == &body )
			return p;
	}

	return nullptr;
}

GXForceGeneratorsRegistration* GXWorld::FindForceGeneratorRegistration ( GXRigidBody &body, GXForceGenerator &generator )
{
	for ( GXForceGeneratorsRegistration* p = forceGenerators; p; p = p->next )
	{
		if ( p->body == &body && p->generator == &generator )
			return p;
	}

	return nullptr;
}
