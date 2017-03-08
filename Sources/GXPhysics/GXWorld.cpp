//version 1.0

#include <GXPhysics/GXWorld.h>
#include <GXPhysics/GXContactsGenerator.h>
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

GXWorld::GXWorld ( GXUInt maxContacts, GXUInt iterations )
: contactResolver ( iterations ), collisions ( maxContacts )
{
	bodies = nullptr;
	contactGenerators = nullptr;
	forceGenerators = nullptr;
	isCalculateIterations = iterations == 0;
}

GXWorld::~GXWorld ()
{
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

	// We do not do anything unless 2 rigid bodies were registered
	if ( !bodies || !bodies->next ) return;

	for ( GXRigidBodyRegistration* p = bodies; p; p = p->next )
	{
		p->body->ClearAccumulators ();
		p->body->CalculateCachedData ();
	}

	for ( GXForceGeneratorsRegistration* p = forceGenerators; p; p = p->next )
		p->generator->UpdateForce ( *p->body, deltaTime );

	for ( GXRigidBodyRegistration* p = bodies; p; p = p->next )
		p->body->Integrate ( deltaTime );

	GXContact* nextContact = collisions.GetContactsBegin ();

	for ( GXContactGeneratorsRegistration* reg = contactGenerators; reg; reg = reg->next )
	{
		reg->generator->AddContact ( collisions );
		if ( !collisions.HasMoreContacts () ) break;
	}

	for ( GXRigidBodyRegistration* p = bodies; p->next; p = p->next )
	{
		for ( GXRigidBodyRegistration* q = p->next; q; q = q->next )
		{
			GXShape& shapeA = p->body->GetShape ();
			GXShape& shapeB = q->body->GetShape ();
			switch ( shapeA.GetType () )
			{
				case eGXShapeType::Box:
				{
					switch ( shapeB.GetType () )
					{
						case eGXShapeType::Plane:
							collisionDetector.CheckBoxAndHalfSpace ( (const GXBoxShape&)shapeA, (const GXPlaneShape&)shapeB, collisions );
						break;

						case eGXShapeType::Sphere:
							collisionDetector.CheckBoxAndSphere ( (const GXBoxShape&)shapeA, (const GXSphereShape&)shapeB, collisions );
						break;

						case eGXShapeType::Box:
							collisionDetector.CheckBoxAndBox ( (const GXBoxShape&)shapeA, (const GXBoxShape&)shapeB, collisions );
						break;
					}
				}
				break;

				case eGXShapeType::Plane:
				{
					switch ( shapeB.GetType () )
					{
						case eGXShapeType::Sphere:
							collisionDetector.CheckSphereAndTruePlane ( (const GXSphereShape&)shapeB, (const GXPlaneShape&)shapeA, collisions );
						break;

						case eGXShapeType::Box:
							collisionDetector.CheckBoxAndHalfSpace ( (const GXBoxShape&)shapeB, (const GXPlaneShape&)shapeA, collisions );
						break;
					}
				}
				break;

				case eGXShapeType::Sphere:
				{
					switch ( shapeB.GetType () )
					{
						case eGXShapeType::Plane:
							collisionDetector.CheckSphereAndTruePlane ( (const GXSphereShape&)shapeA, (const GXPlaneShape&)shapeB, collisions );
						break;

						case eGXShapeType::Sphere:
							collisionDetector.CheckSphereAndSphere ( (const GXSphereShape&)shapeA, (const GXSphereShape&)shapeB, collisions );
						break;

						case eGXShapeType::Box:
							collisionDetector.CheckBoxAndSphere ( (const GXBoxShape&)shapeB, (const GXSphereShape&)shapeA, collisions );
						break;
					}
				}
				break;
			}
		}
	}

	if ( isCalculateIterations )
	{
		GXUInt numIterations = collisions.GetTotalContacts () * 4;
		contactResolver.SetPositionIterations ( numIterations );
		contactResolver.SetVelocityIterations ( numIterations );
	}

	contactResolver.ResolveContacts ( collisions.GetAllContacts (), collisions.GetTotalContacts (), deltaTime );
	collisions.Reset ();
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
