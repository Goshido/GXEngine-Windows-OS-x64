//version 1.0

#include <GXPhysics/GXCollisionDetector.h>
#include <GXCommon/GXLogger.h>


#define TRY_AXIS_EPSILON				0.0001f
#define CONTACT_POINT_EPSILON			0.0001f
#define DEFAULT_MINKOWSKI_EPSILON		0.001f
#define DEFAULT_MAX_EPA_ITERATIONS		35
#define DEFAULT_MIN_EPA_DOT_DISTANCE	0.001f
#define DEFAULT_EDGE_EPSILON			1.0e-3f


struct GXSupportPoint
{
	GXVec3		difference;

	GXVec3		extremeA;
	GXVec3		extremeB;

	GXVoid operator = ( const GXSupportPoint &point );
};

GXVoid GXSupportPoint::operator = ( const GXSupportPoint &point )
{
	memcpy ( this, &point, sizeof ( GXSupportPoint ) );
}

//------------------------------------------------------------------------------------------

struct GXSimplex
{
	GXSupportPoint		a;
	GXSupportPoint		b;
	GXSupportPoint		c;
	GXSupportPoint		d;

	GXUByte				numPoints;

	GXSimplex ();
	GXVoid PushPoint ( const GXSupportPoint &point );
	GXVoid Reset ();
	GXBool DotTestWithAO ( const GXVec3 &v );
};

GXSimplex::GXSimplex ()
{
	numPoints = 0;
}

GXVoid GXSimplex::PushPoint ( const GXSupportPoint &point )
{
	d = c;
	c = b;
	b = a;
	a = point;

	numPoints++;
}

GXVoid GXSimplex::Reset ()
{
	numPoints = 0;
}

GXBool GXSimplex::DotTestWithAO ( const GXVec3 &v )
{
	GXVec3 ao ( -a.difference.x, -a.difference.y, -a.difference.z );
	return GXDotVec3Fast ( v, ao ) > 0.0f;
}

//------------------------------------------------------------------------------------------

struct GXTriangle
{
	GXSupportPoint	points[ 3 ];
	GXVec3			normal;

	GXTriangle ();
	GXTriangle ( const GXSupportPoint &a, const GXSupportPoint &b, const GXSupportPoint &c );

	GXVoid Init ( const GXSupportPoint &a, const GXSupportPoint &b, const GXSupportPoint &c );
};

GXTriangle::GXTriangle ()
{
	memset ( points, 0, 3 * sizeof ( GXSupportPoint ) );
}

GXTriangle::GXTriangle ( const GXSupportPoint &a, const GXSupportPoint &b, const GXSupportPoint &c )
{
	Init ( a, b, c );
}

GXVoid GXTriangle::Init ( const GXSupportPoint &a, const GXSupportPoint &b, const GXSupportPoint &c )
{
	points[ 0 ] = a;
	points[ 1 ] = b;
	points[ 2 ] = c;
}

//------------------------------------------------------------------------------------------

struct GXTriangleNode
{
	GXTriangleNode*	next;
	GXTriangleNode*	prev;

	GXTriangle*		triangle;
};

struct GXTriangleVector
{
	GXTriangleNode*	head;
	GXTriangleNode*	tail;

	GXTriangleVector ();

	GXVoid PushBack ( GXTriangle &triangle );
	GXVoid Remove ( GXTriangle &triangle );
	GXTriangleNode* Find ( GXTriangle &triangle );
	GXTriangle& operator [] ( GXUInt index );
};

GXTriangleVector::GXTriangleVector ()
{
	head = nullptr;
	tail = nullptr;
}

GXVoid GXTriangleVector::PushBack ( GXTriangle &triangle )
{
	GXTriangleNode* node;
	node->triangle = &triangle;
	node->next = nullptr;

	if ( !tail )
	{
		tail = node;
		node->prev = nullptr;
		head = tail;
	}
	else
	{
		tail->next = node;
		node->prev = tail;
		tail = node;
	}
}

GXVoid GXTriangleVector::Remove ( GXTriangle &triangle )
{
	GXTriangleNode* node = Find ( triangle );
	if ( !node )
	{
		GXLogW ( L"GXTriangleVector::Remove::Error - Can't find triangle!" );
		return;
	}

	if ( head == node )
	{
		if ( !node->next )
		{
			head = tail = nullptr;
		}
		else
		{
			head = node->next;
			head->prev = nullptr;
		}

		delete node;
	}
	else if ( tail == node )
	{
		tail = node->prev;
		tail->next = nullptr;

		delete node;
	}
	else
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;

		delete node;
	}
}

GXTriangleNode* GXTriangleVector::Find ( GXTriangle &triangle )
{
	for ( GXTriangleNode* p = head; p; p = p->next )
	{
		if ( p->triangle == &triangle )
			return p;
	}

	return nullptr;
}

GXTriangle& GXTriangleVector::operator [] ( GXUInt index )
{
	GXUInt i = 0;
	for ( GXTriangleNode* p = head; p; p = p->next )
	{
		if ( i == index )
			return *p->triangle;

		i++;
	}

	GXLogW ( L"GXTriangleVector::Error - Triangle with %i index does not exist\n", index );
	return GXTriangle ();
}

//------------------------------------------------------------------------------------------

struct GXEdge
{
	GXSupportPoint		points[ 2 ];

	GXEdge ();
	GXEdge ( const GXSupportPoint &a, const GXSupportPoint &b );
};

GXEdge::GXEdge ()
{
	memset ( points, 0, 2 * sizeof ( GXSupportPoint ) );
}

GXEdge::GXEdge ( const GXSupportPoint &a, const GXSupportPoint &b )
{
	points[ 0 ] = a;
	points[ 1 ] = b;
}

//------------------------------------------------------------------------------------------

struct GXEdgeNode
{
	GXEdgeNode*		next;
	GXEdgeNode*		prev;

	GXEdge*			edge;
};

struct GXEdgeVector
{
	GXEdgeNode*		head;
	GXEdgeNode*		tail;

	GXEdgeVector ();

	GXVoid PushBack ( GXEdge &edge );
	GXVoid Remove ( GXEdge &edge );
	GXVoid Clear ();
	GXEdgeNode* Find ( GXEdge &edge );
	GXEdge& operator [] ( GXUInt index );

	GXVoid AddEdgeWithChecking ( const GXSupportPoint &edgeStart, const GXSupportPoint &edgeEnd );
};

GXEdgeVector::GXEdgeVector ()
{
	head = nullptr;
	tail = nullptr;
}

GXVoid GXEdgeVector::PushBack ( GXEdge &edge )
{
	GXEdgeNode* node;
	node->edge = &edge;
	node->next = nullptr;

	if ( !tail )
	{
		tail = node;
		node->prev = nullptr;
		head = tail;
	}
	else
	{
		tail->next = node;
		node->prev = tail;
		tail = node;
	}
}

GXVoid GXEdgeVector::Remove ( GXEdge &edge )
{
	GXEdgeNode* node = Find ( edge );
	if ( !node )
	{
		GXLogW ( L"GXEdgeVector::Remove::Error - Can't find edge!" );
		return;
	}

	if ( head == node )
	{
		if ( !node->next )
		{
			head = tail = nullptr;
		}
		else
		{
			head = node->next;
			head->prev = nullptr;
		}

		delete node;
	}
	else if ( tail == node )
	{
		tail = node->prev;
		tail->next = nullptr;

		delete node;
	}
	else
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;

		delete node;
	}
}

GXVoid GXEdgeVector::Clear ()
{
	while ( head )
	{
		GXEdgeNode* node = head;
		head = head->next;
		delete node;
	}
}

GXEdgeNode* GXEdgeVector::Find ( GXEdge &edge )
{
	for ( GXEdgeNode* p = head; p; p = p->next )
	{
		if ( p->edge == &edge )
			return p;
	}

	return nullptr;
}

GXEdge& GXEdgeVector::operator [] ( GXUInt index )
{
	GXUInt i = 0;
	for ( GXEdgeNode* p = head; p; p = p->next )
	{
		if ( i == index )
			return *p->edge;

		i++;
	}

	GXLogW ( L"GXEdgeVector::Error - Edge with %i index does not exist\n", index );
	return GXEdge ();
}

GXVoid GXEdgeVector::AddEdgeWithChecking ( const GXSupportPoint &edgeStart, const GXSupportPoint &edgeEnd )
{
	for (GXEdgeNode* p = head; p; p = p->next )
	{
		GXEdge* edge = p->edge;

		GXBool testA = GXDistanceVec3Vec3 ( edge->points[ 0 ].difference, edgeEnd.difference ) < DEFAULT_EDGE_EPSILON;
		GXBool testB = GXDistanceVec3Vec3 ( edge->points[ 1 ].difference, edgeStart.difference ) < DEFAULT_EDGE_EPSILON;

		if (testA && testB)
		{
			Remove(*edge);
			return;
		}
	}

	PushBack ( GXEdge ( edgeStart, edgeEnd ) );
}

//------------------------------------------------------------------------------------------

GXUInt GXCollisionDetector::CheckSphereAndSphere ( const GXSphereShape &sphereA, const GXSphereShape &sphereB, GXCollisionData &collisionData )
{
	if ( !collisionData.HasMoreContacts () ) return 0;

	const GXMat4& transformA = sphereA.GetTransformWorld ();
	const GXMat4& transformB = sphereB.GetTransformWorld ();

	GXVec3 midline;
	GXSubVec3Vec3 ( midline, transformA.wv, transformB.wv );
	GXFloat size = GXLengthVec3 ( midline );

	GXFloat radiusSum = sphereA.GetRadius () + sphereB.GetRadius ();

	if ( size <= 0.0f || size >= radiusSum ) return 0;

	GXVec3 normal;
	GXMulVec3Scalar ( normal, midline, 1.0f / size );

	GXContact* contact = collisionData.GetContactsBegin ();
	contact->SetNormal ( normal );

	GXVec3 contactPoint;
	GXSumVec3ScaledVec3 ( contactPoint, transformA.wv, 0.5f, midline );
	contact->SetContactPoint ( contactPoint );

	contact->SetPenetration ( radiusSum - size );
	contact->SetData ( sphereA, &sphereB );

	collisionData.AddContacts ( 1 );
	return 1;
}

GXUInt GXCollisionDetector::CheckSphereAndHalfSpace ( const GXSphereShape &sphere, const GXPlaneShape &plane, GXCollisionData &collisionData )
{
	if ( !collisionData.HasMoreContacts () ) return 0;

	const GXMat4& sphereTransform = sphere.GetTransformWorld ();
	GXVec3 position = sphereTransform.wv;

	GXPlane p = plane.GetPlane ();
	GXNormalizePlane ( p );
	GXVec3 planeNormal ( p.a, p.b, p.c );

	GXFloat ballDistance = GXDotVec3Fast ( planeNormal, position ) - sphere.GetRadius () - p.d;

	if ( ballDistance >= 0 ) return 0;

	GXContact* contact = collisionData.GetContactsBegin ();

	contact->SetNormal ( planeNormal );
	contact->SetPenetration ( -ballDistance );

	GXVec3 contactPoint;
	GXSubVec3ScaledVec3 ( contactPoint, position, ballDistance + sphere.GetRadius (), planeNormal );
	contact->SetContactPoint ( contactPoint );

	contact->SetData ( sphere, nullptr );

	collisionData.AddContacts ( 1 );
	return 1;
}

GXUInt GXCollisionDetector::CheckSphereAndTruePlane ( const GXSphereShape &sphere, const GXPlaneShape &plane, GXCollisionData &collisionData )
{
	if ( !collisionData.HasMoreContacts () ) return 0;

	const GXMat4& sphereTransform = sphere.GetTransformWorld ();
	GXPlane p = plane.GetPlane ();
	GXNormalizePlane ( p );
	GXVec3 planeNormal ( p.a, p.b, p.c );

	GXFloat centerDistance = GXDotVec3Fast ( planeNormal, sphereTransform.wv ) - p.d;

	GXFloat sphereRadius = sphere.GetRadius ();

	if ( centerDistance * centerDistance > sphereRadius * sphereRadius ) return 0;

	GXVec3 normal = planeNormal;
	GXFloat penetration = -centerDistance;
	if ( centerDistance < 0.0f )
	{
		GXReverseVec3 ( normal );
		penetration = -penetration;
	}

	penetration += sphereRadius;

	GXContact* contact = collisionData.GetContactsBegin ();
	contact->SetNormal ( normal );
	contact->SetPenetration ( penetration );

	GXVec3 contactPoint;
	GXSubVec3ScaledVec3 ( contactPoint, sphereTransform.wv, centerDistance, planeNormal );
	contact->SetContactPoint ( contactPoint );

	contact->SetData ( sphere, nullptr );
	
	collisionData.AddContacts ( 1 );
	return 1;
}

GXUInt GXCollisionDetector::CheckBoxAndHalfSpace ( const GXBoxShape &box, const GXPlaneShape &plane, GXCollisionData &collisionData )
{
	if ( !collisionData.HasMoreContacts () ) return 0;

	GXBoxShapeVertices boxVertices;
	box.GetVecticesWorld ( boxVertices );

	GXContact* contact = collisionData.GetContactsBegin ();
	GXUInt contactsUsed = 0;

	GXPlane p = plane.GetPlane ();
	GXNormalizePlane ( p );
	GXVec3 planeNormal ( p.a, p.b, p.c );

	for ( GXUByte i = 0; i < 8; i++ )
	{
		GXFloat vertexDistance = GXDotVec3Fast ( boxVertices.vertices[ i ], planeNormal );

		if ( vertexDistance <= p.d )
		{
			contact->SetNormal ( planeNormal );
			GXVec3 contactPoint;
			GXSumVec3ScaledVec3 ( contactPoint, boxVertices.vertices[ i ], vertexDistance - p.d, planeNormal );
			contact->SetContactPoint ( contactPoint );
			contact->SetPenetration ( p.d - vertexDistance );

			contact->SetData ( box, nullptr );

			contact++;
			contactsUsed++;

			if ( contactsUsed == collisionData.GetContactsLeft () ) break;
		}
	}

	collisionData.AddContacts ( contactsUsed );
	return contactsUsed;
}

GXUInt GXCollisionDetector::CheckBoxAndSphere ( const GXBoxShape &box, const GXSphereShape &sphere, GXCollisionData &collisionData )
{
	const GXMat4& sphereTransform = sphere.GetTransformWorld ();
	GXMat4 transformToBoxSpace;
	GXSetMat4Inverse ( transformToBoxSpace, box.GetTransformWorld () );

	GXVec3 relativeCenter;
	GXMulVec3Mat4AsPoint ( relativeCenter, sphereTransform.wv, transformToBoxSpace );

	GXFloat w = box.GetWidth () * 0.5f;
	GXFloat h = box.GetHeight () * 0.5f;
	GXFloat d = box.GetDepth () * 0.5f;

	GXFloat radius = sphere.GetRadius ();

	if ( fabsf ( relativeCenter.x ) - radius > w || fabsf ( relativeCenter.y ) - radius > h || fabsf ( relativeCenter.z ) - radius > d )
		return 0;

	GXVec3 closestPoint;
	closestPoint.x = GXClampf ( relativeCenter.x, -w, w );
	closestPoint.y = GXClampf ( relativeCenter.y, -h, h );
	closestPoint.z = GXClampf ( relativeCenter.z, -d, d );

	GXFloat squareDistance = GXSquareDistanceVec3Vec3 ( closestPoint, relativeCenter );
	if ( squareDistance > radius * radius ) return 0;

	GXVec3 closestPointWorld;
	GXMulVec3Mat4AsPoint ( closestPointWorld, closestPoint, box.GetTransformWorld () );

	GXContact* contact = collisionData.GetContactsBegin ();

	GXVec3 contactNormal;
	GXSubVec3Vec3 ( contactNormal, closestPointWorld, sphereTransform.wv );
	GXNormalizeVec3 ( contactNormal );
	contact->SetNormal ( contactNormal );

	contact->SetContactPoint ( closestPointWorld );
	contact->SetPenetration ( radius - sqrtf ( squareDistance ) );

	contact->SetData ( box, &sphere );

	collisionData.AddContacts ( 1 );
	return 1;
}

GXUInt GXCollisionDetector::CheckBoxAndBox ( const GXBoxShape &boxA, const GXBoxShape &boxB, GXCollisionData &collisionData )
{
	const GXMat4& transformA = boxA.GetTransformWorld ();
	const GXMat4& transformB = boxB.GetTransformWorld ();

	GXVec3 toCentre;
	GXSubVec3Vec3 ( toCentre, transformB.wv, transformA.wv );

	GXFloat pen = FLT_MAX;

	GXUInt best = 0xFFFFFF;

	if ( TryAxis ( boxA, boxB, transformA.xv, toCentre, 0, pen, best ) ) return 0;
	if ( TryAxis ( boxA, boxB, transformA.yv, toCentre, 1, pen, best ) ) return 0;
	if ( TryAxis ( boxA, boxB, transformA.zv, toCentre, 2, pen, best ) ) return 0;

	if ( TryAxis ( boxA, boxB, transformB.xv, toCentre, 3, pen, best ) ) return 0;
	if ( TryAxis ( boxA, boxB, transformB.yv, toCentre, 4, pen, best ) ) return 0;
	if ( TryAxis ( boxA, boxB, transformB.zv, toCentre, 5, pen, best ) ) return 0;

	GXUInt bestSingleAxis = best;
	GXVec3 v;

	GXCrossVec3Vec3 ( v, transformA.xv, transformB.xv );
	if ( TryAxis ( boxA, boxB, v, toCentre, 6, pen, best ) ) return 0;

	GXCrossVec3Vec3 ( v, transformA.xv, transformB.yv );
	if ( TryAxis ( boxA, boxB, v, toCentre, 7, pen, best ) ) return 0;

	GXCrossVec3Vec3 ( v, transformA.xv, transformB.zv );
	if ( TryAxis ( boxA, boxB, v, toCentre, 8, pen, best ) ) return 0;


	GXCrossVec3Vec3 ( v, transformA.yv, transformB.xv );
	if ( TryAxis ( boxA, boxB, v, toCentre, 9, pen, best ) ) return 0;

	GXCrossVec3Vec3 ( v, transformA.yv, transformB.yv );
	if ( TryAxis ( boxA, boxB, v, toCentre, 10, pen, best ) ) return 0;

	GXCrossVec3Vec3 ( v, transformA.yv, transformB.zv );
	if ( TryAxis ( boxA, boxB, v, toCentre, 11, pen, best ) ) return 0;


	GXCrossVec3Vec3 ( v, transformA.zv, transformB.xv );
	if ( TryAxis ( boxA, boxB, v, toCentre, 12, pen, best ) ) return 0;

	GXCrossVec3Vec3 ( v, transformA.zv, transformB.yv );
	if ( TryAxis ( boxA, boxB, v, toCentre, 13, pen, best ) ) return 0;

	GXCrossVec3Vec3 ( v, transformA.zv, transformB.zv );
	if ( TryAxis ( boxA, boxB, v, toCentre, 14, pen, best ) ) return 0;

	if ( best == 0xFFFFFF ) return 0;

	if ( best < 3 )
	{
		FillPointFaceBoxBox ( boxA, boxB, toCentre, collisionData, best, pen );
		collisionData.AddContacts ( 1 );
		return 1;
	}
	else if ( best < 6 )
	{
		GXReverseVec3 ( toCentre );
		FillPointFaceBoxBox ( boxA, boxB, toCentre, collisionData, best - 3, pen );
		collisionData.AddContacts ( 1 );
		return 1;
	}
	else
	{
		best -= 6;
		GXUInt oneAxisIndex = best / 3;
		GXUInt twoAxisIndex = best % 3;

		GXVec3 oneAxis = GetAxis ( transformA, oneAxisIndex );
		GXVec3 twoAxis = GetAxis ( transformB, twoAxisIndex );

		GXVec3 axis;
		GXCrossVec3Vec3 ( axis, oneAxis, twoAxis );
		GXNormalizeVec3 ( axis );

		if ( GXDotVec3Fast ( axis, toCentre ) > 0.0f )
			GXReverseVec3 ( axis );

		GXVec3 ptOnOneEdge ( boxA.GetWidth () * 0.5f, boxA.GetHeight () * 0.5f, boxA.GetDepth () * 0.5f );
		GXVec3 ptOnTwoEdge ( boxB.GetWidth () * 0.5f, boxB.GetHeight () * 0.5f, boxB.GetDepth () * 0.5f );

		for ( GXUInt i = 0; i < 3; i++ )
		{
			if ( i == oneAxisIndex )
				ptOnOneEdge.arr[ i ] = 0.0f;
			else if ( GXDotVec3Fast ( GetAxis ( transformA, i ), axis ) > 0.0f )
				ptOnOneEdge.arr[ i ] = -ptOnOneEdge.arr[ i ];

			if ( i == twoAxisIndex )
				ptOnTwoEdge.arr[ i ] = 0.0f;
			else if ( GXDotVec3Fast ( GetAxis ( transformB, i ), axis ) < 0.0f )
				ptOnTwoEdge.arr[ i ] = -ptOnTwoEdge.arr[ i ];
		}

		GXVec3 onOneEdge;
		GXVec3 onTwoEdge;

		GXMulVec3Mat4AsPoint ( onOneEdge, ptOnOneEdge, transformA );
		GXMulVec3Mat4AsPoint ( onTwoEdge, ptOnTwoEdge, transformB );

		GXVec3 vertex = ContactPoint ( onOneEdge, oneAxis, ptOnOneEdge.arr[ oneAxisIndex ], onTwoEdge, twoAxis, ptOnTwoEdge.arr[ twoAxisIndex ], bestSingleAxis > 2 );

		GXContact* contact = collisionData.GetContactsBegin ();

		contact->SetPenetration ( pen );
		contact->SetNormal ( axis );
		contact->SetContactPoint ( vertex );
		contact->SetData ( boxA, &boxB );

		collisionData.AddContacts ( 1 );
		return 1;
	}

	return 0;
}

GXVoid GXCollisionDetector::CheckViaGJK ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData )
{
	GXSimplex simplex;

	GXSupportPoint a;
	GXVec3 direction ( 0.0f, 1.0f, 0.0f );
	CalculateSupportPoint ( a, shapeA, shapeB, direction );

	simplex.PushPoint ( a );

	direction = GXCreateVec3 ( -simplex.a.difference.x, -simplex.a.difference.y, -simplex.a.difference.z );
	
	GXBool isLoop = GX_TRUE;
	GXBool isIntersected = GX_FALSE;

	while ( isLoop )
	{
		CalculateSupportPoint ( a, shapeA, shapeB, direction );

		if ( GXDotVec3Fast ( a.difference, direction ) < 0.0f )
		{
			isLoop = GX_FALSE;
			isIntersected = GX_FALSE;
			continue;
		}

		simplex.PushPoint ( a );

		switch ( simplex.numPoints )
		{
			case 2:
			{
				GXVec3 ab;
				GXSubVec3Vec3 ( ab, simplex.b.difference, simplex.a.difference );

				GXVec3 crossABxAO;
				GXCrossVec3Vec3 ( crossABxAO, ab, GXCreateVec3 ( -simplex.a.difference.x, -simplex.a.difference.y, -simplex.a.difference.z ) );

				GXCrossVec3Vec3 ( direction, crossABxAO, ab );
			}
			break;

			case 3:
			{
				GXSupportPoint a = simplex.a;
				GXSupportPoint b = simplex.b;
				GXSupportPoint c = simplex.c;

				GXVec3 ab;
				GXVec3 ac;
				GXVec3 crossABxAC;

				GXSubVec3Vec3 ( ab, b.difference, a.difference );
				GXSumVec3Vec3 ( ac, c.difference, a.difference );
				GXCrossVec3Vec3 ( crossABxAC, ac, ac );

				GXVec3 v;
				GXCrossVec3Vec3 ( v, ab, crossABxAC );

				if ( simplex.DotTestWithAO ( v ) )
				{
					// origin is outside the triangle, near the edge ab
					// reset the simplex to the line ab and continue
					// search direction is perpendicular to ab and parallel to ao

					simplex.a = a;
					simplex.b = b;
					simplex.numPoints = 2;

					GXVec3 crossABxAO;
					GXCrossVec3Vec3 ( crossABxAO, ab, GXCreateVec3 ( -a.difference.x, -a.difference.y, -a.difference.z ) );
					GXCrossVec3Vec3 ( direction, crossABxAO, ab );

					continue;
				}

				GXCrossVec3Vec3 ( v, crossABxAC, ac );
				if ( simplex.DotTestWithAO ( v ) )
				{
					// origin is outside the triangle, near the edge ac
					// reset the simplex to the line ac and continue
					// search direction is perpendicular to ac and parallel to ao

					simplex.a = a;
					simplex.b = c;
					simplex.numPoints = 2;

					GXVec3 crossACxAO;
					GXCrossVec3Vec3 ( crossACxAO, ac, GXCreateVec3 ( -a.difference.x, -a.difference.y, -a.difference.z ) );
					GXCrossVec3Vec3 ( direction, crossACxAO, ac );

					continue;
				}

				// origin is within the triangular prism defined by the triangle
				// determine if it is above or below
				if ( simplex.DotTestWithAO ( crossABxAC ) )
				{
					// origin is above the triangle, so the simplex is not modified,
					// the search direction is the triangle's face normal

					direction = crossABxAC;
					continue;
				}

				// origin is below the triangle, so the simplex is rewound the oposite direction
				// the search direction is the new triangle's face normal

				simplex.a = a;
				simplex.b = c;
				simplex.c = b;
				simplex.numPoints = 3;

				direction = GXCreateVec3 ( -crossABxAC.x, -crossABxAC.y, -crossABxAC.z );
			}
			break;

			case 4:
			{
				GXSupportPoint a = simplex.a;
				GXSupportPoint b = simplex.b;
				GXSupportPoint c = simplex.c;
				GXSupportPoint d = simplex.d;

				GXVec3 ab;
				GXSubVec3Vec3 ( ab, b.difference, a.difference );

				GXVec3 ac;
				GXSubVec3Vec3 ( ac, c.difference, a.difference );

				GXVec3 cross;
				GXCrossVec3Vec3 ( cross, ab, ac );

				if ( simplex.DotTestWithAO ( cross ) )
				{
					ModifySimplex ( simplex, direction );
					continue;
				}

				GXVec3 ad;
				GXSubVec3Vec3 ( ad, d.difference, a.difference );
				GXCrossVec3Vec3 ( cross, ac, ad );
				if ( simplex.DotTestWithAO ( cross ) )
				{
					simplex.a = a;
					simplex.b = c;
					simplex.c = d;
					simplex.numPoints = 3;

					ModifySimplex ( simplex, direction );
					continue;
				}

				GXCrossVec3Vec3 ( cross, ad, ab );
				if ( simplex.DotTestWithAO ( cross ) )
				{
					simplex.a = a;
					simplex.b = d;
					simplex.c = b;
					simplex.numPoints = 3;

					ModifySimplex ( simplex, direction );
					continue;
				}

				isIntersected = true;
				isLoop = false;
			}
			break;
		}
	}

	if ( !isIntersected ) return;

	GXTriangleVector triangles;
	GXEdgeVector edges;

	triangles.PushBack ( GXTriangle ( simplex.a, simplex.b, simplex.c ) );
	triangles.PushBack ( GXTriangle ( simplex.a, simplex.c, simplex.d ) );
	triangles.PushBack ( GXTriangle ( simplex.a, simplex.d, simplex.b ) );
	triangles.PushBack ( GXTriangle ( simplex.b, simplex.d, simplex.c ) );

	GXInt iteration = 0;

	while ( GX_TRUE )
	{
		iteration++;

		if ( iteration > DEFAULT_MAX_EPA_ITERATIONS )
		{
			GXLogW ( L"GXCollisionDetector::CheckViaGJK::Warning - Max EPA iterations %i", iteration );
			return;
		}

		GXFloat minDotDistance = FLT_MAX;
		GXUByte nearFace = 0;

		for ( GXUByte i = 0; i < 4; i++ )
		{
			GXTriangle& triangle = triangles[ i ];
			float dotDistance = GXDotVec3Fast ( triangle.normal, triangle.points[ 0 ].difference );
			if ( dotDistance < minDotDistance )
			{
				nearFace = i;
				minDotDistance = dotDistance;
			}
		}

		// Now we know that triangle with index "nearFace" is the closest to the origin
		GXTriangle& closestTriangle = triangles[ nearFace ];

		// Find support point in normal direction of the closest triangle
		CalculateSupportPoint ( a, shapeA, shapeB, closestTriangle.normal );
		float newDotDistance = GXDotVec3Fast ( closestTriangle.normal, a.difference );
		float difference = newDotDistance - minDotDistance;

		if (difference < DEFAULT_MIN_EPA_DOT_DISTANCE)
		{
			GXLogW( L"EPA: exit growth: iteration %i, distance %f\n", iteration, difference );
			GetContacts ( closestTriangle, shapeA, shapeB, collisionData );
			return;
		}

		for (GXTriangleNode* p = triangles.head; p; p = p->next )
		{
			GXTriang* triangle = p->triangle;
			if (simpleWindingCheck(triangle, a.difference))
			{
				addEdgeWithChecking(edges, triangle.points[0], triangle.points[1]);
				addEdgeWithChecking(edges, triangle.points[1], triangle.points[2]);
				addEdgeWithChecking(edges, triangle.points[2], triangle.points[0]);

				trianglesToDelete.push_back(i);
			}
		}

		// TODO
	}
}

GXBool GXCollisionDetector::TryAxis ( const GXBoxShape &boxA, const GXBoxShape &boxB, GXVec3 axis, const GXVec3 &toCentre, GXUInt index, GXFloat smallestPenetration, GXUInt &smallestCase )
{
	if ( GXSquareLengthVec3 ( axis ) < TRY_AXIS_EPSILON ) return GX_TRUE;

	GXNormalizeVec3 ( axis );

	GXFloat penetration = PenetrationOnAxis ( boxA, boxB, axis, toCentre );

	if ( penetration < 0.0f ) return GX_FALSE;

	if ( penetration < smallestPenetration )
	{
		smallestPenetration = penetration;
		smallestCase = index;
	}

	return GX_TRUE;
}

GXFloat GXCollisionDetector::PenetrationOnAxis ( const GXBoxShape &one, const GXBoxShape &two, const GXVec3 &axis, const GXVec3 &toCentre )
{
	GXFloat oneProject = TransformToAxis ( one, axis );
	GXFloat twoProject = TransformToAxis ( two, axis );

	GXFloat distance = fabsf ( GXDotVec3Fast ( toCentre, axis ) );

	return oneProject + twoProject - distance;
}

GXFloat GXCollisionDetector::TransformToAxis ( const GXBoxShape &box, const GXVec3 &axis )
{
	GXFloat w = box.GetWidth () * 0.5f;
	GXFloat h = box.GetHeight () * 0.5f;
	GXFloat d = box.GetDepth () * 0.5f;

	const GXMat4& transform = box.GetTransformWorld ();

	GXFloat ans = w * fabsf ( GXDotVec3Fast ( axis, transform.xv ) );
	ans += h * fabsf ( GXDotVec3Fast ( axis, transform.yv ) );
	return  ans + d * fabsf ( GXDotVec3Fast ( axis, transform.zv ) );
}

GXVoid GXCollisionDetector::FillPointFaceBoxBox ( const GXBoxShape &one, const GXBoxShape &two, const GXVec3 &toCentre, GXCollisionData &collisionData, GXUInt best, GXFloat pen )
{
	GXContact* contact = collisionData.GetContactsBegin ();

	const GXMat4& transformOne = one.GetTransformWorld ();
	const GXMat4& transformTwo = two.GetTransformWorld ();
	GXVec3 normal = GetAxis ( transformOne, best );

	if ( GXDotVec3Fast ( normal, toCentre ) > 0.0f )
		GXReverseVec3 ( normal );

	GXVec3 vertex ( two.GetWidth () * 0.5f, two.GetHeight () * 0.5f, two.GetDepth () * 0.5f );
	if ( GXDotVec3Fast ( transformTwo.xv, normal ) < 0.0f )
		vertex.x = -vertex.x;

	if ( GXDotVec3Fast ( transformTwo.yv, normal ) < 0.0f )
		vertex.y = -vertex.y;

	if ( GXDotVec3Fast ( transformTwo.zv, normal ) < 0.0f )
		vertex.z = -vertex.z;

	contact->SetNormal ( normal );
	contact->SetPenetration ( pen );

	GXVec3 contactPoint;
	GXMulVec3Mat4AsPoint ( contactPoint, vertex, transformTwo );
	contact->SetContactPoint ( contactPoint );

	contact->SetData ( one, &two );
}

const GXVec3& GXCollisionDetector::GetAxis ( const GXMat4 &transform, GXUInt index )
{
	switch ( index )
	{
		case 0:
		return transform.xv;

		case 1:
		return transform.yv;

		case 2:
		return transform.zv;

		default:
			GXLogW ( L"GXCollisionDetector::GetAxis::Warning - Wnong index %i\n", index );
		break;
	}

	return transform.wv;
}

GXVec3 GXCollisionDetector::ContactPoint ( const GXVec3 &pOne, const GXVec3 &dOne, GXFloat oneSize, const GXVec3 &pTwo, const GXVec3 &dTwo, GXFloat twoSize, GXBool useOne )
{
	GXFloat smOne = GXSquareLengthVec3 ( dOne );
	GXFloat smTwo = GXSquareLengthVec3 ( dTwo );
	GXFloat dpOneTwo = GXDotVec3Fast ( dTwo, dOne );

	GXVec3 toSt;
	GXSubVec3Vec3 ( toSt, pOne, pTwo );
	GXFloat dpStaOne = GXDotVec3Fast ( dOne, toSt );
	GXFloat dpStaTwo = GXDotVec3Fast ( dTwo, toSt );

	GXFloat denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	if ( fabsf ( denom ) < CONTACT_POINT_EPSILON )
		return useOne ? pOne : pTwo;

	GXFloat mua = ( dpOneTwo * dpStaTwo - smTwo * dpStaOne ) / denom;
	GXFloat mub = ( smOne * dpStaTwo - dpOneTwo * dpStaOne ) / denom;

	if ( mua > oneSize || mua < -oneSize || mub > twoSize || mub < -twoSize )
		return useOne ? pOne : pTwo;
	else
	{
		GXVec3 cOne;
		GXVec3 cTwo;

		GXSumVec3ScaledVec3 ( cOne, pOne, mua, dOne );
		GXSumVec3ScaledVec3 ( cTwo, pTwo, mub, dTwo );

		GXMulVec3Scalar ( cOne, cOne, 0.5f );
		GXMulVec3Scalar ( cTwo, cTwo, 0.5f );

		GXVec3 ans;
		GXSumVec3Vec3 ( ans, cOne, cTwo );
		return ans;
	}
}

GXVoid GXCollisionDetector::CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction )
{
	shapeA.GetExtremePoint ( supportPoint.extremeA, direction );
	GXVec3 invDirection ( -direction.x, -direction.y, -direction.z );
	shapeB.GetExtremePoint ( supportPoint.extremeB, invDirection );
	GXSubVec3Vec3 ( supportPoint.difference, supportPoint.extremeA, supportPoint.extremeB );
}

GXVoid GXCollisionDetector::ModifySimplex ( GXSimplex &simplex, GXVec3 &direction )
{
	GXSupportPoint a = simplex.a;
	GXSupportPoint b = simplex.b;
	GXSupportPoint c = simplex.c;

	GXVec3 ab;
	GXVec3 ac;
	GXVec3 ao ( -a.difference.x, -a.difference.y, -a.difference.z );
	GXSubVec3Vec3 ( ab, b.difference, a.difference );
	GXSubVec3Vec3 ( ac, c.difference, a.difference );

	GXVec3 crossABxAC;
	GXCrossVec3Vec3 ( crossABxAC, ab, ac );

	GXVec3 v;
	GXCrossVec3Vec3 ( v, ab, crossABxAC );
	if ( simplex.DotTestWithAO ( v ) )
	{
		simplex.a = a;
		simplex.b = b;
		simplex.numPoints = 2;

		GXVec3 crossABxAO;
		GXCrossVec3Vec3 ( crossABxAO, ab, ao );
		GXCrossVec3Vec3 ( direction, crossABxAO, ab );
		return;
	}

	GXCrossVec3Vec3 ( v, crossABxAC, ac );
	if ( simplex.DotTestWithAO ( v ) ) 
	{
		simplex.a = a;
		simplex.b = c;
		simplex.numPoints = 2;

		GXVec3 crossACxAO;
		GXCrossVec3Vec3 ( crossACxAO, ac, ao );
		GXCrossVec3Vec3 ( direction, crossACxAO, ac );
		return;
	}

	simplex.a = a;
	simplex.b = b;
	simplex.c = c;
	simplex.numPoints = 3;

	direction = crossABxAC;
}

GXVoid GXCollisionDetector::GetContacts ( GXTriangle &triangle, const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData )
{
	GXFloat distanceFromOrigin = GXDotVec3Fast ( triangle.normal, triangle.points[ 0 ].difference );

	GXVec3 barycentricCoords;
	GXVec3 point;
	GXMulVec3Scalar ( point, triangle.normal, distanceFromOrigin );

	GXGetBarycentricCoords ( barycentricCoords, point, triangle.points[0].difference, triangle.points[1].difference, triangle.points[2].difference );

	if ( fabsf ( barycentricCoords.x ) > 1.0f || fabsf ( barycentricCoords.y ) > 1.0f || fabs ( barycentricCoords.z ) > 1.0f)
		return;

	GXVec3 v1;
	GXMulVec3Scalar ( v1, triangle.points[0].extremeA, barycentricCoords.x );

	GXVec3 v2;
	GXMulVec3Scalar ( v2, triangle.points[1].extremeA, barycentricCoords.y );

	GXVec3 v3;
	GXMulVec3Scalar ( v3, triangle.points[2].extremeA, barycentricCoords.z );

	GXContact* contact = collisionData.GetContactsBegin ();

	GXVec3 location;
	GXSumVec3Vec3(location, v1, v2);
	GXSumVec3Vec3(location, location, v3);

	contact->SetContactPoint ( location );
	contact->SetNormal ( GXCreateVec3 ( -triangle.normal.x, -triangle.normal.y, -triangle.normal.z ) );
	contact->SetPenetration ( distanceFromOrigin );

	contact->SetData ( shapeA, &shapeB );
	
	collisionData.AddContacts ( 1 );
}
