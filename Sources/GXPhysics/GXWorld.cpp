// version 1.0

#include <GXPhysics/GXWorld.h>
#include <GXPhysics/GXContactGenerator.h>
#include <GXPhysics/GXSphereShape.h>
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
collisions ( maxContacts )
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

	for ( GXRigidBodyRegistration* p = bodies; p; p = p->next )
		p->body->ClearAccumulators ();

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

	contactResolver.ResolveContacts ( collisions.GetAllContacts (), collisions.GetTotalContacts () );
}

const GXCollisionData& GXWorld::GetCollisionData () const
{
	return collisions;
}

GXBool GXWorld::Raycast ( const GXVec3 &origin, const GXVec3 &direction, GXFloat length, GXVec3 &contactLocation, GXVec3 &contactNormal, const GXShape** contactShape ) const
{
	GXFloat inverseX = 1.0f / direction.GetX ();
	GXFloat inverseY = 1.0f / direction.GetY ();
	GXFloat inverseZ = 1.0f / direction.GetZ ();

	GXBool foundIntersection = GX_FALSE;
	GXFloat intersectionFactor = 0.0f;

	for ( GXRigidBodyRegistration* p = bodies; p != nullptr; p = p->next )
	{
		GXShape& shape = p->body->GetShape ();
		const GXAABB& shapeBounds = shape.GetBoundsWorld ();

		GXFloat t1 = ( shapeBounds.min.GetX () - origin.GetX () ) * inverseX;
		GXFloat t2 = ( shapeBounds.max.GetX () - origin.GetX () ) * inverseX;
		GXFloat t3 = ( shapeBounds.min.GetY () - origin.GetY () ) * inverseY;
		GXFloat t4 = ( shapeBounds.max.GetY () - origin.GetY () ) * inverseY;
		GXFloat t5 = ( shapeBounds.min.GetZ () - origin.GetZ () ) * inverseZ;
		GXFloat t6 = ( shapeBounds.max.GetZ () - origin.GetZ () ) * inverseZ;

		GXFloat tMin = GXMaxf ( GXMaxf ( GXMinf ( t1, t2 ), GXMinf ( t3, t4 ) ), GXMinf ( t5, t6 ) );
		GXFloat tMax = GXMinf ( GXMinf ( GXMaxf ( t1, t2 ), GXMaxf ( t3, t4 ) ), GXMaxf ( t5, t6 ) );

		if ( tMax < 0.0f ) continue;

		if ( tMin > tMax || tMin > length ) continue;

		const GXMat4& shapeTransform = shape.GetTransformWorld ();

		switch ( shape.GetType () )
		{
			case eGXShapeType::Sphere:
			{
				GXSphereShape& sphereShape = (GXSphereShape&)shape;

				GXVec3 sphereCenter;
				shapeTransform.GetW ( sphereCenter );

				GXVec3 alpha;
				alpha.Substract ( sphereCenter, origin );

				GXFloat betta = alpha.DotProduct ( direction );

				if ( betta < 0.0f ) break;

				GXFloat gamma = alpha.DotProduct ( alpha ) - betta * betta;
				GXFloat radiusSquare = sphereShape.GetRadius ();
				radiusSquare *= radiusSquare;

				if ( gamma > radiusSquare ) break;

				GXFloat yotta = sqrtf ( radiusSquare - gamma );
				GXFloat intersection0 = betta - yotta;
				GXFloat intersection1 = betta + yotta;

				if ( intersection0 > intersection1 )
				{
					GXFloat tmp = intersection0;
					intersection0 = intersection1;
					intersection1 = tmp;
				}

				if ( intersection0 < 0.0f )
				{
					intersection0 = intersection1;

					if ( intersection0 < 0.0f ) break;
				}

				if ( !foundIntersection )
				{
					foundIntersection = GX_TRUE;
					intersectionFactor = intersection0;

					contactLocation.Sum ( origin, intersection0, direction );
					contactNormal.Substract ( contactLocation, sphereCenter );
					contactNormal.Normalize ();
					*contactShape = &shape;

					break;
				}

				if ( intersectionFactor < intersection0 ) break;

				intersectionFactor = intersection0;

				contactLocation.Sum ( origin, intersection0, direction );
				contactNormal.Substract ( contactLocation, sphereCenter );
				contactNormal.Normalize ();
				*contactShape = &shape;
			}
			break;

			case eGXShapeType::Box:
			{
				const GXAABB boundsLocal ( shape.GetBoundsLocal () );

				GXVec3 geometryLocal[ 8 ];
				geometryLocal[ 0 ] = boundsLocal.min;
				geometryLocal[ 1 ].Init ( boundsLocal.min.GetX (), boundsLocal.min.GetY (), boundsLocal.max.GetZ () );
				geometryLocal[ 2 ].Init ( boundsLocal.max.GetX (), boundsLocal.min.GetY (), boundsLocal.max.GetZ () );
				geometryLocal[ 3 ].Init ( boundsLocal.max.GetX (), boundsLocal.min.GetY (), boundsLocal.min.GetZ () );
				geometryLocal[ 4 ].Init ( boundsLocal.min.GetX (), boundsLocal.max.GetY (), boundsLocal.min.GetZ () );
				geometryLocal[ 5 ].Init ( boundsLocal.min.GetX (), boundsLocal.max.GetY (), boundsLocal.max.GetZ () );
				geometryLocal[ 6 ] = boundsLocal.max;
				geometryLocal[ 7 ].Init ( boundsLocal.max.GetX (), boundsLocal.max.GetY (), boundsLocal.min.GetZ () );

				const GXUByte indices[ 36 ] =
				{
					0u, 3u, 1u,
					1u, 3u, 2u,
					1u, 5u, 0u,
					0u, 5u, 4u,
					2u, 6u, 1u,
					1u, 6u, 5u,
					3u, 7u, 2u,
					2u, 7u, 6u,
					0u, 4u, 3u,
					3u, 4u, 7u,
					4u, 5u, 7u,
					7u, 5u, 6u
				};

				GXVec3 geometryWorld[ 8 ];

				for ( GXUByte i = 0u; i < 8u; i++ )
					shapeTransform.MultiplyAsPoint ( geometryWorld[ i ], geometryLocal[ i ] );

				const GXUByte totalTriangles = 12u;
				GXUByte offset = 0u;

				for ( GXUByte i = 0u; i < totalTriangles; i++ )
				{
					GXFloat t;
					const GXVec3& a = geometryWorld[ indices[ offset ] ];
					const GXVec3& b = geometryWorld[ indices[ offset + 1 ] ];
					const GXVec3& c = geometryWorld[ indices[ offset + 2 ] ];

					offset += 3u;

					if ( !GXRayTriangleIntersection3D ( t, origin, direction, length, a, b, c ) ) continue;

					if ( !foundIntersection )
					{
						foundIntersection = GX_TRUE;
						intersectionFactor = t;

						contactLocation.Sum ( origin, t, direction );

						GXPlane plane;
						plane.From ( a, b, c );

						contactNormal.Init ( plane.a, plane.b, plane.c );
						*contactShape = &shape;

						continue;
					}

					if ( t > intersectionFactor ) continue;

					intersectionFactor = t;
					contactLocation.Sum ( origin, t, direction );

					GXPlane plane;
					plane.From ( a, b, c );

					contactNormal.Init ( plane.a, plane.b, plane.c );
					*contactShape = &shape;
				}
			}
			break;

			case eGXShapeType::Rectangle:
				// TODO
			break;
		}
	}

	return foundIntersection;
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
