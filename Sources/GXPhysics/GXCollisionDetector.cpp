//version 1.0

#include <GXPhysics/GXCollisionDetector.h>


#define TRY_AXIS_EPSILON	0.0001f


GXUInt GXCollisionDetector::CheckSphereAndSphere ( const GXSphereShape &sphereA, const GXSphereShape &sphereB, GXCollisionData* collisionData )
{
	if ( !collisionData->HasMoreContacts () ) return 0;

	const GXMat4& transformA = sphereA.GetTransformWorld ();
	const GXMat4& transformB = sphereB.GetTransformWorld ();

	GXVec3 midline;
	GXSubVec3Vec3 ( midline, transformA.wv, transformB.wv );
	GXFloat size = GXLengthVec3 ( midline );

	GXFloat radiusSum = sphereA.GetRadius () + sphereB.GetRadius ();

	if ( size <= 0.0f || size >= radiusSum ) return 0;

	GXVec3 normal;
	GXMulVec3Scalar ( normal, midline, 1.0f / size );

	GXContact* contact = collisionData->GetContactsBegin ();
	contact->SetNormal ( normal );

	GXVec3 contactPoint;
	GXSumVec3ScaledVec3 ( contactPoint, transformA.wv, 0.5f, midline );
	contact->SetContactPoint ( contactPoint );

	contact->SetPenetration ( radiusSum - size );
	contact->SetData ( *sphereA.GetRigidBody (), sphereB.GetRigidBody (), collisionData->GetFriction (), collisionData->GetRestitution () );

	collisionData->AddContacts ( 1 );
	return 1;
}

GXUInt GXCollisionDetector::CheckSphereAndHalfSpace ( const GXSphereShape &sphere, const GXPlaneShape &plane, GXCollisionData* collisionData )
{
	if ( !collisionData->HasMoreContacts () ) return 0;

	const GXMat4& sphereTransform = sphere.GetTransformWorld ();
	GXVec3 position = sphereTransform.wv;

	GXPlane p = plane.GetPlane ();
	GXNormalizePlane ( p );
	GXVec3 planeNormal ( p.a, p.b, p.c );

	GXFloat ballDistance = GXDotVec3Fast ( planeNormal, position ) - sphere.GetRadius () - p.d;

	if ( ballDistance >= 0 ) return 0;

	GXContact* contact = collisionData->GetContactsBegin ();

	contact->SetNormal ( planeNormal );
	contact->SetPenetration ( -ballDistance );

	GXVec3 contactPoint;
	GXSubVec3ScaledVec3 ( contactPoint, position, ballDistance + sphere.GetRadius (), planeNormal );
	contact->SetContactPoint ( contactPoint );

	contact->SetData ( *sphere.GetRigidBody (), nullptr, collisionData->GetFriction (), collisionData->GetRestitution () );

	collisionData->AddContacts ( 1 );
	return 1;
}

GXUInt GXCollisionDetector::CheckSphereAndTruePlane ( const GXSphereShape &sphere, const GXPlaneShape &plane, GXCollisionData* collisionData )
{
	if ( !collisionData->HasMoreContacts () ) return 0;

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

	GXContact* contact = collisionData->GetContactsBegin ();
	contact->SetNormal ( normal );
	contact->SetPenetration ( penetration );

	GXVec3 contactPoint;
	GXSubVec3ScaledVec3 ( contactPoint, sphereTransform.wv, centerDistance, planeNormal );
	contact->SetContactPoint ( contactPoint );

	contact->SetData ( *sphere.GetRigidBody (), nullptr, collisionData->GetFriction (), collisionData->GetRestitution () );
	
	collisionData->AddContacts ( 1 );
	return 1;
}

GXUInt GXCollisionDetector::CheckBoxAndHalfSpace ( const GXBoxShape &box, const GXPlaneShape &plane, GXCollisionData* collisionData )
{
	if ( !collisionData->HasMoreContacts () ) return 0;

	GXBoxShapeVertices boxVertices;
	box.GetVecticesWorld ( boxVertices );

	GXContact* contact = collisionData->GetContactsBegin ();
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

			contact->SetData ( *box.GetRigidBody (), nullptr, collisionData->GetFriction (), collisionData->GetRestitution () );

			contact++;
			contactsUsed++;

			if ( contactsUsed == collisionData->GetContactsLeft () ) break;
		}
	}

	collisionData->AddContacts ( contactsUsed );
	return contactsUsed;
}

GXUInt GXCollisionDetector::CheckBoxAndSphere ( const GXBoxShape &box, const GXSphereShape &sphere, GXCollisionData* collisionData )
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

	GXContact* contact = collisionData->GetContactsBegin ();

	GXVec3 contactNormal;
	GXSubVec3Vec3 ( contactNormal, closestPointWorld, sphereTransform.wv );
	GXNormalizeVec3 ( contactNormal );
	contact->SetNormal ( contactNormal );

	contact->SetContactPoint ( closestPointWorld );
	contact->SetPenetration ( radius - sqrtf ( squareDistance ) );

	contact->SetData ( *box.GetRigidBody (), sphere.GetRigidBody (), collisionData->GetFriction (), collisionData->GetRestitution () );

	collisionData->AddContacts ( 1 );
	return 1;
}

GXUInt GXCollisionDetector::CheckBoxAndBox ( const GXBoxShape &boxA, const GXBoxShape &boxB, GXCollisionData* collisionData )
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

	// TODO

	if ( best < 3 )
	{

	}

	return 0;
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

GXVoid GXCollisionDetector::FillPointFaceBoxBox ( const GXBoxShape &one, const GXBoxShape &two, const GXVec3 &toCentre, GXCollisionData* collisionData, GXUInt best, GXFloat pen )
{
	GXContact* contact = collisionData->GetContactsBegin ();

	const GXMat4& transformOne = one.GetTransformWorld ();
	GXVec3 normal;
	switch ( best )
	{
		case 0:
			normal = transformOne.xv;
		break;

		case 1:
			normal = transformOne.yv;
		break;

		case 2:
			normal = transformOne.zv;
		break;
	}

	if ( GXDotVec3Fast ( normal, toCentre ) > 0.0f )
		GXReverseVec3 ( normal );

	GXFloat w = two.GetWidth () * 0.5f;
	GXFloat h = two.GetHeight () * 0.5f;
	GXFloat d = two.GetDepth () * 0.5f;

	// TODO
}
