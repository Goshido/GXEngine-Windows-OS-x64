//version 1.0

#include <GXPhysics/GXCollisionDetector.h>
#include <GXCommon/GXLogger.h>


#define TRY_AXIS_EPSILON		0.0001f
#define CONTACT_POINT_EPSILON	0.0001f


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

	if ( best < 3 )
	{
		FillPointFaceBoxBox ( boxA, boxB, toCentre, collisionData, best, pen );
		collisionData->AddContacts ( 1 );
		return 1;
	}
	else if ( best < 6 )
	{
		GXReverseVec3 ( toCentre );
		FillPointFaceBoxBox ( boxA, boxB, toCentre, collisionData, best - 3, pen );
		collisionData->AddContacts ( 1 );
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

		GXContact* contact = collisionData->GetContactsBegin ();

		contact->SetPenetration ( pen );
		contact->SetNormal ( axis );
		contact->SetContactPoint ( vertex );
		contact->SetData ( *boxA.GetRigidBody (), boxB.GetRigidBody (), collisionData->GetFriction (), collisionData->GetRestitution () );

		collisionData->AddContacts ( 1 );
		return 1;
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

	contact->SetData ( *one.GetRigidBody (), two.GetRigidBody (), collisionData->GetFriction (), collisionData->GetRestitution () );
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
