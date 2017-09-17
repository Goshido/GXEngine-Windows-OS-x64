//version 1.2

#include <GXPhysics/GXCollisionDetector.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


#define INVALID_INDEX								0xFFFFFFFF

#define EPA_INITIAL_SUPPORT_POINT_ARRAY_CAPACITY	64
#define EPA_SUPPORT_POINTS_ALLOCATING_STEP			64

#define EPA_INITIAL_FACE_ARRAY_CAPACITY				128
#define EPA_FACE_ALLOCATING_STEP					128

#define EPA_INITIAL_EDGE_ARRAY_CAPACITY				384
#define EPA_EDGE_ALLOCATING_STEP					384

#define EPA_DISTANCE_EPSILON						1.0e-5f

#define DEFAULT_DEVIATION_AXES						8
#define DEFAULT_DEVIATION_ANGLE						0.008726f	// 0.5 degrees

#define INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY	128
#define PLANAR_INTERSECTION_ALLOCATING_STEP			128


struct GXSupportPoint
{
	GXVec3		difference;

	GXVoid operator = ( const GXSupportPoint &point );
};

GXVoid GXSupportPoint::operator = ( const GXSupportPoint &point )
{
	memcpy ( this, &point, sizeof ( GXSupportPoint ) );
}

//------------------------------------------------------------------------------------------

struct GXEdge
{
	GXUInt		prev;
	GXUInt		next;

	GXUInt		startPoint;
	GXUInt		endPoint;

	GXEdge ();
	explicit GXEdge ( GXUInt startPoint, GXUInt endPoint );

	static GXBool IsOpposite ( const GXEdge &alpha, const GXEdge &betta );

	GXVoid operator = ( const GXEdge &other );
};

GXEdge::GXEdge ()
{
	prev = next = INVALID_INDEX;
	this->startPoint = INVALID_INDEX;
	this->endPoint = INVALID_INDEX;
}

GXEdge::GXEdge ( GXUInt startPoint, GXUInt endPoint )
{
	prev = next = INVALID_INDEX;
	this->startPoint = startPoint;
	this->endPoint = endPoint;
}

GXBool GXEdge::IsOpposite ( const GXEdge &alpha, const GXEdge &betta )
{
	return ( alpha.startPoint == betta.endPoint && alpha.endPoint == betta.startPoint );
}

GXVoid GXEdge::operator = ( const GXEdge &other )
{
	memcpy ( this, &other, sizeof ( GXEdge ) );
}

//------------------------------------------------------------------------------------------

struct GXFace
{
	GXUInt		next;
	GXUInt		prev;

	GXUInt		pointA;
	GXUInt		pointB;
	GXUInt		pointC;

	GXVec3		normal;

	GXFace ();
	explicit GXFace ( GXUInt pointA, GXUInt pointB, GXUInt pointC, const GXVec3 &normal );

	GXVoid ChangeOrder ();

	GXVoid operator = ( const GXFace &other );
};

GXFace::GXFace ()
{
	next = prev = INVALID_INDEX;
	pointA = pointB = pointC = INVALID_INDEX;
	normal.Init ( 0.0f, 0.0f, 0.0f );
}

GXFace::GXFace ( GXUInt pointA, GXUInt pointB, GXUInt pointC, const GXVec3 &normal )
{
	next = prev = INVALID_INDEX;

	this->pointA = pointA;
	this->pointB = pointB;
	this->pointC = pointC;

	this->normal = normal;
}

GXVoid GXFace::ChangeOrder ()
{
	GXUInt tmp = pointA;
	pointA = pointB;
	pointB = tmp;

	normal.Reverse ();
}

GXVoid GXFace::operator = ( const GXFace &other )
{
	memcpy ( this, &other, sizeof ( GXFace ) );
}

//------------------------------------------------------------------------------------------

struct GXSimplex
{
	GXSupportPoint		supportPoints[ 4 ];
	GXFace				faces[ 4 ];
	GXUByte				numPoints;

	GXSimplex ();

	GXVoid PushPoint ( const GXSupportPoint &point );
	GXVoid Reset ();
};

GXSimplex::GXSimplex ()
{
	numPoints = 0;
}

GXVoid GXSimplex::PushPoint ( const GXSupportPoint &point )
{
	memmove ( supportPoints + 1, supportPoints, 3 * sizeof ( GXSupportPoint ) );
	supportPoints[ 0 ] = point;
	numPoints++;

	if ( numPoints < 4 ) return;

	faces[ 0 ].pointA = 0;
	faces[ 0 ].pointB = 1;
	faces[ 0 ].pointC = 2;

	GXPlane plane;
	plane.From ( supportPoints[ faces[ 0 ].pointA ].difference, supportPoints[ faces[ 0 ].pointB ].difference, supportPoints[ faces[ 0 ].pointC ].difference );
	
	if ( plane.ClassifyVertex ( supportPoints[ 3 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 0 ].ChangeOrder ();

	faces[ 1 ].pointA = 0;
	faces[ 1 ].pointB = 2;
	faces[ 1 ].pointC = 3;
	plane.From ( supportPoints[ faces[ 1 ].pointA ].difference, supportPoints[ faces[ 1 ].pointB ].difference, supportPoints[ faces[ 1 ].pointC ].difference );

	if ( plane.ClassifyVertex ( supportPoints[ 1 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 1 ].ChangeOrder ();

	faces[ 2 ].pointA = 0;
	faces[ 2 ].pointB = 3;
	faces[ 2 ].pointC = 1;
	plane.From ( supportPoints[ faces[ 2 ].pointA ].difference, supportPoints[ faces[ 2 ].pointB ].difference, supportPoints[ faces[ 2 ].pointC ].difference );

	if ( plane.ClassifyVertex ( supportPoints[ 2 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 2 ].ChangeOrder ();

	faces[ 3 ].pointA = 1;
	faces[ 3 ].pointB = 3;
	faces[ 3 ].pointC = 2;
	plane.From ( supportPoints[ faces[ 3 ].pointA ].difference, supportPoints[ faces[ 3 ].pointB ].difference, supportPoints[ faces[ 3 ].pointC ].difference );

	if ( plane.ClassifyVertex ( supportPoints[ 0 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 3 ].ChangeOrder ();
}

GXVoid GXSimplex::Reset ()
{
	numPoints = 0;
}

//------------------------------------------------------------------------------------------

GXCollisionDetector* GXCollisionDetector::instance = nullptr;

GXCollisionDetector& GXCALL GXCollisionDetector::GetInstance ()
{
	if ( !instance )
		instance = new GXCollisionDetector ();

	return *instance;
}

GXCollisionDetector::~GXCollisionDetector ()
{
	free ( deviationAxesLocal );
	free ( shapeAContactGeometry );
	free ( shapeBContactGeometry );
	free ( shapeAProjectedContactGeometry );
	free ( shapeBProjectedContactGeometry );
	free ( shapeAPlanarContactGeometry );
	free ( shapeBPlanarContactGeometry );

	instance = nullptr;

	if ( !shapeAPlanarContactGeometryDebug ) return;

	free ( shapeAPlanarContactGeometryDebug );
	free ( shapeBPlanarContactGeometryDebug );
}

GXVoid GXCollisionDetector::Check ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData )
{
	static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );
	static const GXVec3 initialDirection ( 0.0f, 0.0f, 1.0f );

	//GJK algorithm

	GXSimplex simplex;
	GXSupportPoint supportPoint;
	GXVec3 direction;

	GXBool isLoop = GX_TRUE;
	GXBool isIntersected = GX_FALSE;
	GXUInt gjkIterations = 0;

	CalculateSupportPoint ( supportPoint, shapeA, shapeB, initialDirection );

	simplex.PushPoint ( supportPoint );
	direction.Substract ( origin, supportPoint.difference );

	while ( isLoop )
	{
		gjkIterations++;

		CalculateSupportPoint ( supportPoint, shapeA, shapeB, direction );

		if ( supportPoint.difference.DotProduct ( direction ) < 0.0f )
		{
			isLoop = GX_FALSE;
			isIntersected = GX_FALSE;

			continue;
		}

		simplex.PushPoint ( supportPoint );

		switch ( simplex.numPoints )
		{
			case 2:
			{
				GXVec3 simplexLineDirection;
				simplexLineDirection.Substract ( simplex.supportPoints[ 0 ].difference, simplex.supportPoints[ 1 ].difference );
				simplexLineDirection.Normalize ();
				
				GXVec3 toOrigin;
				toOrigin.Substract ( origin, simplex.supportPoints[ 1 ].difference );

				GXVec3 alpha;
				alpha.Project ( toOrigin, simplexLineDirection );

				GXVec3 betta;
				betta.Sum ( simplex.supportPoints[ 1 ].difference, alpha );

				direction.Substract ( origin, betta );
			}
			break;

			case 3:
			{
				GXPlane simplexPlane;
				simplexPlane.From ( simplex.supportPoints[ 0 ].difference, simplex.supportPoints[ 1 ].difference, simplex.supportPoints[ 2 ].difference );
				switch ( simplexPlane.ClassifyVertex ( origin ) )
				{
					case eGXPlaneClassifyVertex::On:
					case eGXPlaneClassifyVertex::InFront:
						direction.Init ( simplexPlane.a, simplexPlane.b, simplexPlane.c );
					break;

					case eGXPlaneClassifyVertex::Behind:
						direction.Init ( -simplexPlane.a, -simplexPlane.b, -simplexPlane.c );
					break;

					default:
						//NOTHING
					break;
				}
			}
			break;

			case 4:
			{
				isIntersected = GX_TRUE;

				for ( GXUByte i = 0; i < 4; i++ )
				{
					GXPlane simplexPlane;
					simplexPlane.From ( simplex.supportPoints[ simplex.faces[ i ].pointA ].difference, simplex.supportPoints[ simplex.faces[ i ].pointB ].difference, simplex.supportPoints[ simplex.faces[ i ].pointC ].difference );

					if ( simplexPlane.ClassifyVertex ( origin ) != eGXPlaneClassifyVertex::Behind ) continue;

					direction.Init ( -simplexPlane.a, -simplexPlane.b, -simplexPlane.c );

					GXSupportPoint a = simplex.supportPoints[ simplex.faces[ i ].pointA ];
					GXSupportPoint b = simplex.supportPoints[ simplex.faces[ i ].pointB ];
					GXSupportPoint c = simplex.supportPoints[ simplex.faces[ i ].pointC ];

					simplex.Reset ();
					simplex.PushPoint ( a );
					simplex.PushPoint ( b );
					simplex.PushPoint ( c );

					isIntersected = GX_FALSE;

					break;
				}

				if ( isIntersected )
					isLoop = GX_FALSE;
			}
			break;

			default:
				//NOTHING
			break;
		}
	}

	if ( !isIntersected )
	{
		totalShapeAContactGeometryPoints = 0;
		totalShapeBContactGeometryPoints = 0;

		return;
	}

	//EPA algorithm

	GXSupportPoint* supportPointArray = (GXSupportPoint*)supportPoints.GetData ();
	GXEdge* edgeArray = (GXEdge*)edges.GetData ();
	GXFace* faceArray = (GXFace*)faces.GetData ();

	memcpy ( supportPointArray, simplex.supportPoints, 4 * sizeof ( GXSupportPoint ) );
	totalSupportPoints = 4;

	totalFaces = 0;

	GXFace& alphaFace = simplex.faces[ 0 ];
	GXPlane plane;
	plane.From ( supportPointArray[ alphaFace.pointA ].difference, supportPointArray[ alphaFace.pointB ].difference, supportPointArray[ alphaFace.pointC ].difference );
	alphaFace.normal.Init ( plane.a, plane.b, plane.c );
	alphaFace.next = INVALID_INDEX;
	alphaFace.prev = INVALID_INDEX;
	GXUInt lastFaceIndex = 0;
	faceArray[ lastFaceIndex ] = alphaFace;
	totalFaces++;

	for ( GXUInt i = 1; i < 4; i++ )
	{
		GXFace& bettaFace = simplex.faces[ i ];
		plane.From ( supportPointArray[ bettaFace.pointA ].difference, supportPointArray[ bettaFace.pointB ].difference, supportPointArray[ bettaFace.pointC ].difference );
		bettaFace.normal.Init ( plane.a, plane.b, plane.c );
		bettaFace.next = INVALID_INDEX;
		bettaFace.prev = lastFaceIndex;
		lastFaceIndex = totalFaces;
		faceArray[ lastFaceIndex ] = bettaFace;
		faceArray[ bettaFace.prev ].next = lastFaceIndex;
		totalFaces++;
	}

	static const GXFace voidFace;

	GXFloat contactPenetration = FLT_MAX;
	GXVec3 contactNormal;
	GXUInt epaIterations = 0;
	GXUInt maximumEdgesUsed = 0;

	while ( GX_TRUE )
	{
		epaIterations++;

		GXFloat smallestDistance = FLT_MAX;
		GXUInt smallestDistanceFaceIndex = 0;
		GXVec3 smallestDistanceFaceNormal;

		for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faceArray[ i ].prev )
		{
			const GXFace& yottaFace = faceArray[ i ];

			GXVec3 gamma;
			gamma.Substract ( origin, supportPointArray[ yottaFace.pointA ].difference );

			GXFloat newDistance = gamma.DotProduct ( yottaFace.normal );

			if ( newDistance >= smallestDistance ) continue;

			smallestDistance = newDistance;
			smallestDistanceFaceIndex = i;
			smallestDistanceFaceNormal = yottaFace.normal;
		}

		if ( fabsf ( smallestDistance - contactPenetration ) <= EPA_DISTANCE_EPSILON )
		{
			// Disable contacts with penetration depth equal 0.0f or bigger than half FLT_MAX

			if ( contactPenetration == 0.0f || contactPenetration > FLT_MAX * 0.5f )
			{
				totalShapeAContactGeometryPoints = 0;
				totalShapeBContactGeometryPoints = 0;

				return;
			}

			contactPenetration = smallestDistance;
			contactNormal = smallestDistanceFaceNormal;

			if ( shapeA.GetRigidBody ().IsKinematic () )
				contactNormal.Reverse ();

			break;
		}

		contactPenetration = smallestDistance;

		if ( supportPoints.GetLength () < totalSupportPoints + 1 )
		{
			static const GXSupportPoint voidSupportPoint;
			supportPoints.SetValue ( supportPoints.GetLength () + EPA_SUPPORT_POINTS_ALLOCATING_STEP - 1, &voidSupportPoint );
			supportPointArray = (GXSupportPoint*)supportPoints.GetData ();
		}

		direction = smallestDistanceFaceNormal;
		direction.Reverse ();

		CalculateSupportPoint ( supportPoint, shapeA, shapeB, direction );

		GXUInt newSupportPointIndex = totalSupportPoints;
		supportPointArray[ newSupportPointIndex ] = supportPoint;
		totalSupportPoints++;

		GXUInt lastEdgeIndex = INVALID_INDEX;
		totalEdges = 0;

		for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faceArray[ i ].prev )
		{
			GXFace& gammaFace = faceArray[ i ];

			GXVec3 omega;
			omega.Substract ( supportPointArray[ gammaFace.pointA ].difference, supportPoint.difference );

			if ( omega.DotProduct ( gammaFace.normal ) <= 0.0f ) continue;

			GXEdge edge1 = GXEdge ( gammaFace.pointA, gammaFace.pointB );
			ProceedEdge ( edge1, lastEdgeIndex, &edgeArray, totalEdges );

			GXEdge edge2 = GXEdge ( gammaFace.pointB, gammaFace.pointC );
			ProceedEdge ( edge2, lastEdgeIndex, &edgeArray, totalEdges );

			GXEdge edge3 = GXEdge ( gammaFace.pointC, gammaFace.pointA );
			ProceedEdge ( edge3, lastEdgeIndex, &edgeArray, totalEdges );

			if ( i == lastFaceIndex )
				lastFaceIndex = gammaFace.prev;

			if ( lastFaceIndex > 500 )
			{
				GXVec3 wtf;
			}

			if ( gammaFace.prev != INVALID_INDEX )
				faceArray[ gammaFace.prev ].next = gammaFace.next;

			if ( gammaFace.next != INVALID_INDEX )
				faceArray[ gammaFace.next ].prev = gammaFace.prev;
		}

		if ( totalEdges > maximumEdgesUsed )
			maximumEdgesUsed = totalEdges;

		for ( GXUInt i = lastEdgeIndex; i != INVALID_INDEX; i = edgeArray[ i ].prev )
		{
			const GXEdge& edge = edgeArray[ i ];

			plane.From ( supportPointArray[ newSupportPointIndex ].difference, supportPointArray[ edge.startPoint ].difference, supportPointArray[ edge.endPoint ].difference );
			GXFace omegaFace ( newSupportPointIndex, edge.startPoint, edge.endPoint, GXVec3 ( plane.a, plane.b, plane.c ) );

			if ( plane.ClassifyVertex ( origin ) == eGXPlaneClassifyVertex::Behind )
				omegaFace.ChangeOrder ();

			if ( faces.GetLength () < totalFaces + 1 )
			{
				faces.SetValue ( faces.GetLength () + EPA_FACE_ALLOCATING_STEP - 1, &voidFace );
				faceArray = (GXFace*)faces.GetData ();
			}

			omegaFace.next = INVALID_INDEX;
			omegaFace.prev = lastFaceIndex;
			lastFaceIndex = totalFaces;

			faceArray[ lastFaceIndex ] = omegaFace;
			faceArray[ omegaFace.prev ].next = lastFaceIndex;
			totalFaces++;
		}
	}

	//Contact geometry search

	if ( shapeA.GetType () == eGXShapeType::Sphere )
	{
		GXContact* contact = collisionData.GetContactsBegin ();

		if ( shapeA.GetRigidBody ().IsKinematic () )
		{
			contact->SetShapes ( shapeB, shapeA );
			contactNormal.Reverse ();
		}
		else
		{
			shapeA.GetRigidBody ().SetAwake ();
			shapeB.GetRigidBody ().SetAwake ();
			contact->SetShapes ( shapeA, shapeB );
		}

		contact->SetLinkedContacts ( 1 );
		contact->SetNormal ( contactNormal );

		contactNormal.Reverse ();

		GXVec3 contactPoint;
		shapeA.GetExtremePoint ( contactPoint, contactNormal );
		contact->SetContactPoint ( contactPoint );

		contact->SetPenetration ( contactPenetration );
		contact->SetGJKIterations ( gjkIterations );
		contact->SetEPAIterations ( epaIterations );
		contact->SetSupportPoints ( totalSupportPoints );
		contact->SetEdges ( maximumEdgesUsed );
		contact->SetFaces ( totalFaces );

		collisionData.AddContacts ( 1 );

		totalIntersectionPoints = 0;

		return;
	}
	else if ( shapeB.GetType () == eGXShapeType::Sphere )
	{
		GXContact* contact = collisionData.GetContactsBegin ();

		if ( shapeA.GetRigidBody ().IsKinematic () )
		{
			contact->SetShapes ( shapeB, shapeA );
			contact->SetNormal ( contactNormal );

			contactNormal.Reverse ();

			GXVec3 contactPoint;
			shapeB.GetExtremePoint ( contactPoint, contactNormal );
			contact->SetContactPoint ( contactPoint );
		}
		else
		{
			shapeA.GetRigidBody ().SetAwake ();
			shapeB.GetRigidBody ().SetAwake ();
			contact->SetShapes ( shapeA, shapeB );

			GXVec3 contactPoint;
			shapeB.GetExtremePoint ( contactPoint, contactNormal );
			contact->SetContactPoint ( contactPoint );

			contactNormal.Reverse ();
			contact->SetNormal ( contactNormal );
		}

		contact->SetLinkedContacts ( 1 );
		contact->SetPenetration ( contactPenetration );
		contact->SetGJKIterations ( gjkIterations );
		contact->SetEPAIterations ( epaIterations );
		contact->SetSupportPoints ( totalSupportPoints );
		contact->SetEdges ( maximumEdgesUsed );
		contact->SetFaces ( totalFaces );

		collisionData.AddContacts ( 1 );

		totalIntersectionPoints = 0;

		return;
	}

	GXMat3 tbn;
	GXVec3 zetta;
	GXVec3 tangent;
	GXVec3 bitangent;

	if ( fabsf ( contactNormal.GetY () ) > fabsf ( contactNormal.GetX () ) && fabsf ( contactNormal.GetY () ) > fabsf ( contactNormal.GetZ () ) )
		zetta.CrossProduct ( GXVec3::GetAbsoluteX (), contactNormal );
	else
		zetta.CrossProduct ( GXVec3::GetAbsoluteY (), contactNormal );

	bitangent.CrossProduct ( contactNormal, zetta );
	bitangent.Normalize ();
	tangent.CrossProduct ( bitangent, contactNormal );

	tbn.SetX ( tangent );
	tbn.SetY ( bitangent );
	tbn.SetZ ( contactNormal );

	GXVec3 lastShapeAPoint ( FLT_MAX, FLT_MAX, FLT_MAX );
	GXVec3 lastShapeBPoint ( FLT_MAX, FLT_MAX, FLT_MAX );

	totalShapeAContactGeometryPoints = totalShapeBContactGeometryPoints = 0;

	for ( GXUShort i = 0; i < totalDeviationAxes; i++ )
	{
		tbn.Multiply ( direction, deviationAxesLocal[ i ] );

		GXVec3 shapeAPoint;
		shapeA.GetExtremePoint ( shapeAPoint, direction );

		direction.Reverse ();
		GXVec3 shapeBPoint;
		shapeB.GetExtremePoint ( shapeBPoint, direction );

		if ( !lastShapeAPoint.IsEqual ( shapeAPoint ) )
		{
			shapeAContactGeometry[ totalShapeAContactGeometryPoints ] = shapeAPoint;
			lastShapeAPoint = shapeAPoint;
			totalShapeAContactGeometryPoints++;
		}

		if ( !lastShapeBPoint.IsEqual ( shapeBPoint ) )
		{
			shapeBContactGeometry[ totalShapeBContactGeometryPoints ] = shapeBPoint;
			lastShapeBPoint = shapeBPoint;
			totalShapeBContactGeometryPoints++;
		}
	}

	if ( totalShapeAContactGeometryPoints == 1 )
	{
		GXContact* contact = collisionData.GetContactsBegin ();

		if ( shapeA.GetRigidBody ().IsKinematic () )
		{
			contact->SetShapes ( shapeB, shapeA );
		}
		else
		{
			shapeA.GetRigidBody ().SetAwake ();
			shapeB.GetRigidBody ().SetAwake ();

			contact->SetShapes ( shapeA, shapeB );
			contactNormal.Reverse ();
		}

		contact->SetLinkedContacts ( 1 );
		contact->SetNormal ( contactNormal );
		contact->SetPenetration ( contactPenetration );
		contact->SetGJKIterations ( gjkIterations );
		contact->SetEPAIterations ( epaIterations );
		contact->SetSupportPoints ( totalSupportPoints );
		contact->SetEdges ( maximumEdgesUsed );
		contact->SetFaces ( totalFaces );

		contact->SetContactPoint ( shapeAContactGeometry[ 0 ] );
		collisionData.AddContacts ( 1 );

		totalIntersectionPoints = 0;

		return;
	}

	if ( totalShapeBContactGeometryPoints == 1 )
	{
		GXContact* contact = collisionData.GetContactsBegin ();

		if ( shapeA.GetRigidBody ().IsKinematic () )
		{
			contact->SetShapes ( shapeB, shapeA );
		}
		else
		{
			shapeA.GetRigidBody ().SetAwake ();
			shapeB.GetRigidBody ().SetAwake ();

			contact->SetShapes ( shapeA, shapeB );
			contactNormal.Reverse ();
		}

		contact->SetLinkedContacts ( 1 );
		contact->SetNormal ( contactNormal );
		contact->SetPenetration ( contactPenetration );
		contact->SetGJKIterations ( gjkIterations );
		contact->SetEPAIterations ( epaIterations );
		contact->SetSupportPoints ( totalSupportPoints );
		contact->SetEdges ( maximumEdgesUsed );
		contact->SetFaces ( totalFaces );

		contact->SetContactPoint ( shapeBContactGeometry[ 0 ] );
		collisionData.AddContacts ( 1 );

		totalIntersectionPoints = 0;

		return;
	}

	if ( lastShapeAPoint.IsEqual ( shapeAContactGeometry[ 0 ] ) )
		totalShapeAContactGeometryPoints--;

	if ( lastShapeBPoint.IsEqual ( shapeBContactGeometry[ 0 ] ) )
		totalShapeBContactGeometryPoints--;

	//Projecting contact geometry to plane with normal "contactNormal" and "origin"

	ProjectContactGeometry ( shapeAProjectedContactGeometry, shapeAContactGeometry, (GXUShort)totalShapeAContactGeometryPoints, contactNormal );
	ProjectContactGeometry ( shapeBProjectedContactGeometry, shapeBContactGeometry, (GXUShort)totalShapeBContactGeometryPoints, contactNormal );

	GXVec3 xAxis;
	xAxis.Substract ( shapeAProjectedContactGeometry[ 1 ], shapeAProjectedContactGeometry[ 0 ] );
	xAxis.Normalize ();

	GXVec3 yAxis;
	yAxis.CrossProduct ( contactNormal, xAxis );

	CalculatePlanarContactGeometryCoordinates ( shapeAPlanarContactGeometry, shapeAProjectedContactGeometry, (GXUShort)totalShapeAContactGeometryPoints, xAxis, yAxis );
	CalculatePlanarContactGeometryCoordinates ( shapeBPlanarContactGeometry, shapeBProjectedContactGeometry, (GXUShort)totalShapeBContactGeometryPoints, xAxis, yAxis );

	if ( totalShapeAContactGeometryPoints == 2 && totalShapeBContactGeometryPoints == 2 )
	{
		GXVec2 intersection2D;
		eGXLineRelationship lineRelationship = GXLineIntersection2D ( intersection2D, shapeAPlanarContactGeometry[ 0 ], shapeAPlanarContactGeometry[ 1 ], shapeBPlanarContactGeometry[ 0 ], shapeBPlanarContactGeometry[ 1 ] );

		GXContact* contact = collisionData.GetContactsBegin ();

		switch ( lineRelationship )
		{
			case eGXLineRelationship::NoIntersection:
			{
				GXUInt wtf = 0;
				GXLogW ( L"%u\n", wtf );
			}
			break;

			case eGXLineRelationship::Intersection:
			{
				GXVec3 shapeAContactGeometryCentroid;
				GetCentroid ( shapeAContactGeometryCentroid, shapeAContactGeometry, totalShapeAContactGeometryPoints );

				GXVec3 intersection;
				GetIntersectionGeometryCoordinates ( &intersection, &intersection2D, 1, xAxis, yAxis );

				GXVec3 toShapeAContactGeometry;
				toShapeAContactGeometry.Substract ( shapeAContactGeometryCentroid, intersection );

				GXVec3 intersectionShapeAProjection;
				intersectionShapeAProjection.Sum ( intersection, contactNormal.DotProduct ( toShapeAContactGeometry ), contactNormal );

				contact->SetContactPoint ( intersectionShapeAProjection );
			}
			break;

			case eGXLineRelationship::Overlap:
			{
				GXUInt todo = 0;
				GXLogW ( L"%u\n", todo );
			}
			break;

			default:
				//NOTHING
			break;
		}

		if ( shapeA.GetRigidBody ().IsKinematic () )
		{
			contact->SetShapes ( shapeB, shapeA );
			contactNormal.Reverse ();
		}
		else
		{
			shapeA.GetRigidBody ().SetAwake ();
			shapeB.GetRigidBody ().SetAwake ();

			contact->SetShapes ( shapeA, shapeB );
		}

		contact->SetLinkedContacts ( 1 );
		contact->SetNormal ( contactNormal );
		contact->SetPenetration ( contactPenetration );
		contact->SetGJKIterations ( gjkIterations );
		contact->SetEPAIterations ( epaIterations );
		contact->SetSupportPoints ( totalSupportPoints );
		contact->SetEdges ( maximumEdgesUsed );
		contact->SetFaces ( totalFaces );

		collisionData.AddContacts ( 1 );

		if ( shapeAPlanarContactGeometryDebug )
			UpdateDebugData ( nullptr );

		return;
	}

	//Clipping planar shapes

	GXVec2* clipGeometry;
	GXUShort totalClipGeometryPoints;
	GXVec2* subjectGeometry;
	GXUInt totalSubjectGeometryPoints;

	if ( totalShapeAContactGeometryPoints == 2 )
	{
		clipGeometry = shapeBPlanarContactGeometry;
		totalClipGeometryPoints = (GXUShort)totalShapeBContactGeometryPoints;
		subjectGeometry = shapeAPlanarContactGeometry;
		totalSubjectGeometryPoints = totalShapeAContactGeometryPoints;
	}
	else
	{
		clipGeometry = shapeAPlanarContactGeometry;
		totalClipGeometryPoints = (GXUShort)totalShapeAContactGeometryPoints;
		subjectGeometry = shapeBPlanarContactGeometry;
		totalSubjectGeometryPoints = totalShapeBContactGeometryPoints;
	}

	GXVec2* clippedGeometry = nullptr;
	GetClippedPlanarContactGeometry ( &clippedGeometry, totalIntersectionPoints, clipGeometry, totalClipGeometryPoints, subjectGeometry, totalSubjectGeometryPoints );

	if ( shapeAPlanarContactGeometryDebug )
		UpdateDebugData ( clippedGeometry );

	//True contact points

	GXVec3* intersection = (GXVec3*)intersectionGeometry.GetData ();
	GetIntersectionGeometryCoordinates ( intersection, clippedGeometry, totalIntersectionPoints, xAxis, yAxis );

	GXVec3 shapeAContactGeometryCentroid;
	GXVec3 shapeBContactGeometryCentroid;

	GetCentroid ( shapeAContactGeometryCentroid, shapeAContactGeometry, totalShapeAContactGeometryPoints );
	GetCentroid ( shapeBContactGeometryCentroid, shapeBContactGeometry, totalShapeBContactGeometryPoints );

	GXContact* contact = collisionData.GetContactsBegin ();
	GXUInt contactIndex = 0;

	const GXShape* correctedShapeA;
	const GXShape* correctedShapeB;
	GXVec3 correctedContactNormal ( contactNormal );

	if ( shapeA.GetRigidBody ().IsKinematic () )
	{
		correctedShapeA = &shapeB;
		correctedShapeB = &shapeA;
	}
	else
	{
		shapeA.GetRigidBody ().SetAwake ();
		shapeB.GetRigidBody ().SetAwake ();

		correctedShapeA = &shapeA;
		correctedShapeB = &shapeB;
		correctedContactNormal.Reverse ();
	}

	for ( GXUInt i = 0; i < totalIntersectionPoints; i++ )
	{
		const GXVec3& intersectionPoint = intersection[ i ];

		GXVec3 toShapeAContactGeometry;
		toShapeAContactGeometry.Substract ( shapeAContactGeometryCentroid, intersectionPoint );

		GXVec3 toShapeBContactGeometry;
		toShapeBContactGeometry.Substract ( shapeBContactGeometryCentroid, intersectionPoint );

		GXVec3 intersectionShapeAProjection;
		intersectionShapeAProjection.Sum ( intersectionPoint, contactNormal.DotProduct ( toShapeAContactGeometry ), contactNormal );

		GXVec3 intersectionShapeBProjection;
		intersectionShapeBProjection.Sum ( intersectionPoint, contactNormal.DotProduct ( toShapeBContactGeometry ), contactNormal );

		GXVec3 checkDirection;
		checkDirection.Substract ( intersectionShapeAProjection, intersectionShapeBProjection );

		if ( checkDirection.DotProduct ( contactNormal ) < 0.0f ) continue;

		contact[ contactIndex ].SetShapes ( *correctedShapeA, *correctedShapeB );
		contact[ contactIndex ].SetNormal ( correctedContactNormal );
		/*contact[ contactIndex ].SetShapes ( shapeA, shapeB );
		contact[ contactIndex ].SetNormal ( contactNormal );*/
		contact[ contactIndex ].SetContactPoint ( intersectionShapeAProjection );
		contact[ contactIndex ].SetPenetration ( contactPenetration );
		contact[ contactIndex ].SetGJKIterations ( gjkIterations );
		contact[ contactIndex ].SetEPAIterations ( epaIterations );
		contact[ contactIndex ].SetSupportPoints ( totalSupportPoints );
		contact[ contactIndex ].SetEdges ( maximumEdgesUsed );
		contact[ contactIndex ].SetFaces ( totalFaces );

		contactIndex++;
	}

	contact->SetLinkedContacts ( contactIndex );

	collisionData.AddContacts ( contactIndex );
}

GXVoid GXCollisionDetector::EnableDebugData ()
{
	if ( shapeAPlanarContactGeometryDebug ) return;

	GXUPointer size = totalDeviationAxes * sizeof ( GXVec3 );

	shapeAPlanarContactGeometryDebug = (GXVec3*)malloc ( size );
	shapeBPlanarContactGeometryDebug = (GXVec3*)malloc ( size );

	if ( planarIntersectionGeometry.GetLength () >= alphaPlanarIntersectionGeometry.GetLength () ) return;

	static const GXVec3 voidVec3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	planarIntersectionGeometry.SetValue ( alphaPlanarIntersectionGeometry.GetLength () - 1, &voidVec3 );
}

GXVoid GXCollisionDetector::DisableDebugData ()
{
	if ( !shapeAPlanarContactGeometryDebug ) return;

	GXSafeFree ( shapeAPlanarContactGeometryDebug );
	GXSafeFree ( shapeBPlanarContactGeometryDebug );
}

GXVoid GXCollisionDetector::SetDeviationAxes ( GXUShort axes )
{
	if ( axes == totalDeviationAxes ) return;

	GXSafeFree ( deviationAxesLocal );

	GXUPointer sizeAlpha = axes * sizeof ( GXVec3 );

	deviationAxesLocal = (GXVec3*)malloc ( sizeAlpha );
	totalDeviationAxes = axes;

	GXSafeFree ( shapeAContactGeometry );
	GXSafeFree ( shapeBContactGeometry );
	GXSafeFree ( shapeAProjectedContactGeometry );
	GXSafeFree ( shapeBProjectedContactGeometry );

	shapeAContactGeometry = (GXVec3*)malloc ( sizeAlpha );
	shapeBContactGeometry = (GXVec3*)malloc ( sizeAlpha );
	shapeAProjectedContactGeometry = (GXVec3*)malloc ( sizeAlpha );
	shapeBProjectedContactGeometry = (GXVec3*)malloc ( sizeAlpha );

	GXUPointer sizeBetta = axes * sizeof ( GXVec2 );

	GXSafeFree ( shapeAPlanarContactGeometry );
	GXSafeFree ( shapeBPlanarContactGeometry );

	shapeAPlanarContactGeometry = (GXVec2*)malloc ( sizeBetta );
	shapeBPlanarContactGeometry = (GXVec2*)malloc ( sizeBetta );

	UpdateDeviationAxes ();
}

GXUShort GXCollisionDetector::GetDeviationAxes () const
{
	return totalDeviationAxes;
}

GXVoid GXCollisionDetector::SetDeviationAngle ( GXFloat radians )
{
	if ( deviationAngle == radians ) return;

	deviationAngle = radians;
	UpdateDeviationAxes ();
}

GXFloat GXCollisionDetector::GetDeviationAngle () const
{
	return deviationAngle;
}

const GXVec3* GXCollisionDetector::GetShapeAContactGeometry () const
{
	return shapeAPlanarContactGeometryDebug ? shapeAContactGeometry : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeAContactGeometryPoints () const
{
	return shapeAPlanarContactGeometryDebug ? totalShapeAContactGeometryPoints : 0;
}

const GXVec3* GXCollisionDetector::GetShapeBContactGeometry () const
{
	return shapeAPlanarContactGeometryDebug ? shapeBContactGeometry : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeBContactGeometryPoints () const
{
	return shapeAPlanarContactGeometryDebug ? totalShapeBContactGeometryPoints : 0;
}

const GXVec3* GXCollisionDetector::GetShapeAPlanarContactGeometry () const
{
	return shapeAPlanarContactGeometryDebug ? shapeAPlanarContactGeometryDebug : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeAPlanarContactGeometryPoints () const
{
	return shapeAPlanarContactGeometryDebug ? totalShapeAContactGeometryPoints : 0;
}

const GXVec3* GXCollisionDetector::GetShapeBPlanarContactGeometry () const
{
	return shapeAPlanarContactGeometryDebug ? shapeBPlanarContactGeometryDebug : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeBPlanarContactGeometryPoints () const
{
	return shapeAPlanarContactGeometryDebug ? totalShapeBContactGeometryPoints : 0;
}

const GXVec3* GXCollisionDetector::GetPlanarIntersectionGeometry () const
{
	return shapeAPlanarContactGeometryDebug ? (GXVec3*)planarIntersectionGeometry.GetData () : nullptr;
}

GXUInt GXCollisionDetector::GetTotalPlanarIntersectionPoints () const
{
	return shapeAPlanarContactGeometryDebug ? totalIntersectionPoints : 0;
}

GXUInt GXCollisionDetector::GetAllocatedSupportPoints () const
{
	return supportPoints.GetLength ();
}

GXUInt GXCollisionDetector::GetAllocatedEdges () const
{
	return edges.GetLength ();
}

GXUInt GXCollisionDetector::GetAllocatedFaces () const
{
	return faces.GetLength ();
}

GXCollisionDetector::GXCollisionDetector () :
supportPoints ( sizeof ( GXSupportPoint ) ),
edges ( sizeof ( GXEdge ) ),
faces ( sizeof ( GXFace ) ),
alphaPlanarIntersectionGeometry ( sizeof ( GXVec2 ) ),
bettaPlanarIntersectionGeometry ( sizeof ( GXVec2 ) ),
planarIntersectionGeometry ( sizeof ( GXVec3 ) ),
intersectionGeometry ( sizeof ( GXVec3 ) )
{
	static const GXSupportPoint voidSupportPoint;
	static const GXEdge voidEdge;
	static const GXFace voidFace;
	static const GXVec2 voidVec2 ( FLT_MAX, FLT_MAX );
	static const GXVec3 voidVec3 ( FLT_MAX, FLT_MAX, FLT_MAX );

	supportPoints.SetValue ( EPA_INITIAL_SUPPORT_POINT_ARRAY_CAPACITY - 1, &voidSupportPoint );
	totalSupportPoints = 0;

	edges.SetValue ( EPA_INITIAL_EDGE_ARRAY_CAPACITY - 1, &voidEdge );
	totalEdges = 0;

	faces.SetValue ( EPA_INITIAL_FACE_ARRAY_CAPACITY - 1, &voidFace );
	totalFaces = 0;

	alphaPlanarIntersectionGeometry.SetValue ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY - 1, &voidVec2 );
	bettaPlanarIntersectionGeometry.SetValue ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY - 1, &voidVec2 );
	planarIntersectionGeometry.SetValue ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY - 1, &voidVec3 );
	intersectionGeometry.SetValue ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY - 1, &voidVec3 );
	totalIntersectionPoints = 0;

	deviationAxesLocal = nullptr;

	shapeAContactGeometry = nullptr;
	totalShapeAContactGeometryPoints = 0;

	shapeBContactGeometry = nullptr;
	totalShapeBContactGeometryPoints = 0;

	shapeAProjectedContactGeometry = nullptr;
	shapeBProjectedContactGeometry = nullptr;

	shapeAPlanarContactGeometry = nullptr;
	shapeAPlanarContactGeometryDebug = nullptr;

	shapeBPlanarContactGeometry = nullptr;
	shapeBPlanarContactGeometryDebug = nullptr;

	DisableDebugData ();

	deviationAngle = DEFAULT_DEVIATION_ANGLE;
	SetDeviationAxes ( DEFAULT_DEVIATION_AXES );
}

GXVoid GXCollisionDetector::CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction )
{
	GXVec3 extremeA;
	shapeA.GetExtremePoint ( extremeA, direction );

	GXVec3 reverseDirection ( direction );
	reverseDirection.Reverse ();
	GXVec3 extremeB;
	shapeB.GetExtremePoint ( extremeB, reverseDirection );

	supportPoint.difference.Substract ( extremeA, extremeB );
}

GXVoid GXCollisionDetector::ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edgeArrayPointer, GXUInt &totalEdgeNumber )
{
	GXBool allowInsert = GX_TRUE;
	GXEdge* edgeArray = *edgeArrayPointer;

	for ( GXUInt i = lastEdgeIndex; i != INVALID_INDEX; i = edgeArray[ i ].prev )
	{
		if ( !GXEdge::IsOpposite ( edge, edgeArray[ i ] ) ) continue;

		if ( i == lastEdgeIndex )
			lastEdgeIndex = edgeArray[ lastEdgeIndex ].prev;

		if ( edgeArray[ i ].prev != INVALID_INDEX )
			edgeArray[ edgeArray[ i ].prev ].next = edgeArray[ i ].next;

		if ( edgeArray[ i ].next != INVALID_INDEX )
			edgeArray[ edgeArray[ i ].next ].prev = edgeArray[ i ].prev;

		allowInsert = GX_FALSE;
		break;
	}

	if ( !allowInsert ) return;

	if ( edges.GetLength () < totalEdgeNumber + 1 )
	{
		static const GXEdge voidEdge;
		edges.SetValue ( edges.GetLength () + EPA_EDGE_ALLOCATING_STEP - 1, &voidEdge );
		*edgeArrayPointer = (GXEdge*)edges.GetData ();
	}

	edge.next = INVALID_INDEX;
	edge.prev = lastEdgeIndex;
	lastEdgeIndex = totalEdgeNumber;
	edgeArray[ lastEdgeIndex ] = edge;

	if ( lastEdgeIndex > 0 )
		edgeArray[ edge.prev ].next = lastEdgeIndex;

	totalEdgeNumber++;
}

GXVoid GXCollisionDetector::ProjectContactGeometry ( GXVec3* contactGeometryProjection, const GXVec3* contactGeometryWorld, GXUShort totalContactGeometryPoints, const GXVec3 &contactNormal )
{
	static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );

	for ( GXUShort i = 0; i < totalContactGeometryPoints; i++ )
	{
		GXVec3 v;
		v.Substract ( origin, contactGeometryWorld[ i ] );

		GXVec3 offset;
		offset.Multiply ( contactNormal, contactNormal.DotProduct ( v ) );

		contactGeometryProjection[ i ].Sum ( contactGeometryWorld[ i ], offset );
	}
}

GXVoid GXCollisionDetector::CalculatePlanarContactGeometryCoordinates ( GXVec2* planarContactGeometry, const GXVec3* projectedContactGeometry, GXUShort totalContactGeometryPoints, const GXVec3 &xAxis, const GXVec3 &yAxis )
{
	static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );

	for ( GXUShort i = 0; i < totalContactGeometryPoints; i++ )
	{
		GXVec3 v;
		v.Substract ( projectedContactGeometry[ i ], origin );

		planarContactGeometry[ i ].Init ( xAxis.DotProduct ( v ), yAxis.DotProduct ( v ) );
	}
}

GXVoid GXCollisionDetector::GetClippedPlanarContactGeometry ( GXVec2** clippedGeometryArrayPointer, GXUInt &totalPoints, const GXVec2* clipGeometry, GXUInt totalClipGeometryPoints, const GXVec2* subjectGeometry, GXUInt totalSubjectGeometryPoints )
{
	static const GXVec2 voidPVec2 ( FLT_MAX, FLT_MAX );
	static const GXVec3 voidPVec3 ( FLT_MAX, FLT_MAX, FLT_MAX );

	GXVec2* outputGeometry = (GXVec2*)alphaPlanarIntersectionGeometry.GetData ();
	GXUInt totalOutputPoints = totalSubjectGeometryPoints;

	memcpy ( outputGeometry, subjectGeometry, totalSubjectGeometryPoints * sizeof ( GXVec2 ) );

	GXUInt previousClipPointIndex = totalClipGeometryPoints - 1;

	for ( GXUInt currentClipPointIndex = 0; currentClipPointIndex < totalClipGeometryPoints; currentClipPointIndex++ )
	{
		GXVec2* inputGeometry = outputGeometry;
		GXUInt totalInputPoints = totalOutputPoints;

		outputGeometry = ( outputGeometry == (GXVec2*)alphaPlanarIntersectionGeometry.GetData () ) ? (GXVec2*)bettaPlanarIntersectionGeometry.GetData () : (GXVec2*)alphaPlanarIntersectionGeometry.GetData ();
		totalOutputPoints = 0;

		GXVec2 clipEdgeNormal;
		clipEdgeNormal.CalculateNormalFast ( clipGeometry[ previousClipPointIndex ], clipGeometry[ currentClipPointIndex ] );

		GXUInt previousPointIndex = totalInputPoints - 1;
		for ( GXUInt currentPointIndex = 0; currentPointIndex < totalInputPoints; currentPointIndex++ )
		{
			GXVec2 toCurrentPoint;
			toCurrentPoint.Substract ( inputGeometry[ currentPointIndex ], clipGeometry[ previousClipPointIndex ] );

			GXVec2 toPreviousPoint;
			toPreviousPoint.Substract ( inputGeometry[ previousPointIndex ], clipGeometry[ previousClipPointIndex ] );

			if ( clipEdgeNormal.DotProduct ( toPreviousPoint ) >= 0.0f )
			{
				if ( clipEdgeNormal.DotProduct ( toCurrentPoint ) < 0.0f )
				{
					GXVec2 intersectionPoint;
					eGXLineRelationship lineRelationship = GXLineIntersection2D ( intersectionPoint, inputGeometry[ previousPointIndex ], inputGeometry[ currentPointIndex ], clipGeometry[ previousClipPointIndex ], clipGeometry[ currentClipPointIndex ] );

					switch ( lineRelationship )
					{
						case eGXLineRelationship::NoIntersection:
						{
							GXUInt wtf = 0;
							GXLogW ( L"%u\n", wtf );
						}
						break;

						case eGXLineRelationship::Intersection:
						{
							if ( alphaPlanarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
							{
								GXUInt lastIndex = totalOutputPoints + PLANAR_INTERSECTION_ALLOCATING_STEP - 1;

								if ( shapeAPlanarContactGeometryDebug && planarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
									planarIntersectionGeometry.SetValue ( lastIndex, &voidPVec3 );

								intersectionGeometry.SetValue ( lastIndex, &voidPVec3 );

								if ( outputGeometry == (GXVec2*)alphaPlanarIntersectionGeometry.GetData () )
								{
									alphaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );
									bettaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );

									outputGeometry = (GXVec2*)alphaPlanarIntersectionGeometry.GetData ();
									inputGeometry = (GXVec2*)bettaPlanarIntersectionGeometry.GetData ();
								}
								else
								{
									alphaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );
									bettaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );

									outputGeometry = (GXVec2*)bettaPlanarIntersectionGeometry.GetData ();
									inputGeometry = (GXVec2*)alphaPlanarIntersectionGeometry.GetData ();
								}
							}

							outputGeometry[ totalOutputPoints ] = intersectionPoint;
							totalOutputPoints++;
						}
						break;

						case eGXLineRelationship::Overlap:
						{
							GXUInt wtf = 0;
							GXLogW ( L"%u\n", wtf );
						}
						break;

						default:
							//NOTHING
						break;
					}
				}
				else
				{
					if ( alphaPlanarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
					{
						GXUInt lastIndex = totalOutputPoints + PLANAR_INTERSECTION_ALLOCATING_STEP - 1;

						if ( shapeAPlanarContactGeometryDebug && planarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
							planarIntersectionGeometry.SetValue ( lastIndex, &voidPVec3 );

						intersectionGeometry.SetValue ( lastIndex, &voidPVec3 );

						if ( outputGeometry == (GXVec2*)alphaPlanarIntersectionGeometry.GetData () )
						{
							alphaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );
							bettaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );

							outputGeometry = (GXVec2*)alphaPlanarIntersectionGeometry.GetData ();
							inputGeometry = (GXVec2*)bettaPlanarIntersectionGeometry.GetData ();
						}
						else
						{
							alphaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );
							bettaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );

							outputGeometry = (GXVec2*)bettaPlanarIntersectionGeometry.GetData ();
							inputGeometry = (GXVec2*)alphaPlanarIntersectionGeometry.GetData ();
						}
					}

					outputGeometry[ totalOutputPoints ] = inputGeometry[ currentPointIndex ];
					totalOutputPoints++;
				}
			}
			else if ( clipEdgeNormal.DotProduct ( toCurrentPoint ) >= 0.0f )
			{
				GXVec2 intersectionPoint;
				eGXLineRelationship lineRelationship = GXLineIntersection2D ( intersectionPoint, inputGeometry[ previousPointIndex ], inputGeometry[ currentPointIndex ], clipGeometry[ previousClipPointIndex ], clipGeometry[ currentClipPointIndex ] );

				switch ( lineRelationship )
				{
					case eGXLineRelationship::NoIntersection:
					{
						GXUInt wtf = 0;
						GXLogW ( L"%u\n", wtf );
					}
					break;

					case eGXLineRelationship::Intersection:
					{
						if ( alphaPlanarIntersectionGeometry.GetLength () < totalOutputPoints + 2 )
						{
							GXUInt lastIndex = totalOutputPoints + PLANAR_INTERSECTION_ALLOCATING_STEP - 1;

							if ( shapeAPlanarContactGeometryDebug && planarIntersectionGeometry.GetLength () < totalOutputPoints + 2 )
								planarIntersectionGeometry.SetValue ( lastIndex, &voidPVec3 );

							intersectionGeometry.SetValue ( lastIndex, &voidPVec3 );

							if ( outputGeometry == (GXVec2*)alphaPlanarIntersectionGeometry.GetData () )
							{
								alphaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );
								bettaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );

								outputGeometry = (GXVec2*)alphaPlanarIntersectionGeometry.GetData ();
								inputGeometry = (GXVec2*)bettaPlanarIntersectionGeometry.GetData ();
							}
							else
							{
								alphaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );
								bettaPlanarIntersectionGeometry.SetValue ( lastIndex, &voidPVec2 );

								outputGeometry = (GXVec2*)bettaPlanarIntersectionGeometry.GetData ();
								inputGeometry = (GXVec2*)alphaPlanarIntersectionGeometry.GetData ();
							}
						}

						outputGeometry[ totalOutputPoints ] = intersectionPoint;
						totalOutputPoints++;

						outputGeometry[ totalOutputPoints ] = inputGeometry[ currentPointIndex ];
						totalOutputPoints++;
					}
					break;

					case eGXLineRelationship::Overlap:
					{
						GXUInt wtf = 0;
						GXLogW ( L"%u\n", wtf );
					}
					break;

					default:
						//NOTHING
					break;
				}
			}

			previousPointIndex = currentPointIndex;
		}

		previousClipPointIndex = currentClipPointIndex;
	}

	*clippedGeometryArrayPointer = outputGeometry;
	totalPoints = totalOutputPoints;
}

GXVoid GXCollisionDetector::GetIntersectionGeometryCoordinates ( GXVec3* out, const GXVec2* planarIntersectionGeometryArray, GXUInt totalIntersectionPointNumber, const GXVec3 &xAxis, const GXVec3 &yAxis )
{
	for ( GXUInt i = 0; i < totalIntersectionPointNumber; i++ )
	{
		out[ i ].Multiply ( xAxis, planarIntersectionGeometryArray[ i ].GetX () );

		GXVec3 alpha;
		alpha.Multiply ( yAxis, planarIntersectionGeometryArray[ i ].GetY () );

		out[ i ].Sum ( out[ i ], alpha );
	}
}

GXVoid GXCollisionDetector::GetCentroid ( GXVec3 &centroid, const GXVec3* points, GXUInt totalPoints )
{
	GXVec3 sum ( 0.0f, 0.0f, 0.0f );

	for ( GXUInt i = 0; i < totalPoints; i++ )
		sum.Sum ( sum, points[ i ] );

	centroid.Multiply ( sum, 1.0f / (GXFloat)totalPoints );
}

GXVoid GXCollisionDetector::UpdateDeviationAxes ()
{
	GXFloat step = GX_MATH_DOUBLE_PI / (GXFloat)totalDeviationAxes;
	GXFloat limit = GX_MATH_DOUBLE_PI - 0.5f * step;
	GXUShort axisIndex = 0;
	GXFloat deviation = sinf ( deviationAngle );
	GXVec3 deviationVector ( deviation, 0.0f, 1.0f );
	GXFloat normalizeScale = 1.0f / deviationVector.Length ();

	for ( GXFloat angle = 0.0f; angle < limit; angle += step )
	{
		deviationAxesLocal[ axisIndex ].Init ( cosf ( angle ) * deviation, sinf ( angle ) * deviation, 1.0f );
		deviationAxesLocal[ axisIndex ].Multiply ( deviationAxesLocal[ axisIndex ], normalizeScale );
		axisIndex++;
	}
}

GXVoid GXCollisionDetector::UpdateDebugData ( const GXVec2* planarIntersectionGeometryArray )
{
	for ( GXUInt i = 0; i < totalShapeAContactGeometryPoints; i++ )
		shapeAPlanarContactGeometryDebug[ i ].Init ( shapeAPlanarContactGeometry[ i ].GetX (), shapeAPlanarContactGeometry[ i ].GetY (), 0.0f );

	for ( GXUInt i = 0; i < totalShapeBContactGeometryPoints; i++ )
		shapeBPlanarContactGeometryDebug[ i ].Init ( shapeBPlanarContactGeometry[ i ].GetX (), shapeBPlanarContactGeometry[ i ].GetY (), 0.0f );

	if ( !planarIntersectionGeometryArray ) return;

	for ( GXUInt i = 0; i < totalIntersectionPoints; i++ )
	{
		GXVec3 v ( planarIntersectionGeometryArray[ i ].GetX (), planarIntersectionGeometryArray[ i ].GetY (), 0.0f );
		planarIntersectionGeometry.SetValue ( i, &v );
	}
}
