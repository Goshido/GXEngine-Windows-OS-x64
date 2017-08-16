//version 1.1

#include <GXPhysics/GXCollisionDetector.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


#define TRY_AXIS_EPSILON							0.0001f
#define CONTACT_POINT_EPSILON						0.0001f
#define DEFAULT_MINKOWSKI_EPSILON					0.001f
#define DEFAULT_MAX_EPA_ITERATIONS					35
#define DEFAULT_MIN_EPA_DOT_DISTANCE				0.001f
#define DEFAULT_EDGE_EPSILON						1.0e-3f
#define INVALID_INDEX								0xFFFFFFFF

#define EPA_INITIAL_SUPPORT_POINT_ARRAY_CAPACITY	64
#define EPA_SUPPORT_POINTS_ALLOCATING_STEP			64

#define EPA_INITIAL_FACE_ARRAY_CAPACITY				128
#define EPA_FACE_ALLOCATIONG_STEP					128

#define EPA_INITIAL_EDGE_ARRAY_CAPACITY				384
#define EPA_EDGE_ALLOCATIONG_STEP					384

#define INITIAL_EPA_DISTANCE						FLT_MAX;
#define EPA_DISTANCE_EPSILON						1.0e-5f


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

	GXFace ();
	explicit GXFace ( GXUInt pointA, GXUInt pointB, GXUInt pointC );

	GXVoid ChangeOrder ();

	GXVoid operator = ( const GXFace &other );
};

GXFace::GXFace ()
{
	next = prev = INVALID_INDEX;
	pointA = pointB = pointC = INVALID_INDEX;
}

GXFace::GXFace ( GXUInt pointA, GXUInt pointB, GXUInt pointC )
{
	next = prev = INVALID_INDEX;

	this->pointA = pointA;
	this->pointB = pointB;
	this->pointC = pointC;
}

GXVoid GXFace::ChangeOrder ()
{
	GXUInt tmp = pointA;
	pointA = pointB;
	pointB = tmp;
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
	
	if ( GXPlaneClassifyVertex ( plane, supportPoints[ 3 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 0 ].ChangeOrder ();

	faces[ 1 ].pointA = 0;
	faces[ 1 ].pointB = 2;
	faces[ 1 ].pointC = 3;
	plane.From ( supportPoints[ faces[ 1 ].pointA ].difference, supportPoints[ faces[ 1 ].pointB ].difference, supportPoints[ faces[ 1 ].pointC ].difference );

	if ( GXPlaneClassifyVertex ( plane, supportPoints[ 1 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 1 ].ChangeOrder ();

	faces[ 2 ].pointA = 0;
	faces[ 2 ].pointB = 3;
	faces[ 2 ].pointC = 1;
	plane.From ( supportPoints[ faces[ 2 ].pointA ].difference, supportPoints[ faces[ 2 ].pointB ].difference, supportPoints[ faces[ 2 ].pointC ].difference );

	if ( GXPlaneClassifyVertex ( plane, supportPoints[ 2 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 2 ].ChangeOrder ();

	faces[ 3 ].pointA = 1;
	faces[ 3 ].pointB = 3;
	faces[ 3 ].pointC = 2;
	plane.From ( supportPoints[ faces[ 3 ].pointA ].difference, supportPoints[ faces[ 3 ].pointB ].difference, supportPoints[ faces[ 3 ].pointC ].difference );

	if ( GXPlaneClassifyVertex ( plane, supportPoints[ 0 ].difference ) == eGXPlaneClassifyVertex::Behind )
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
	instance = nullptr;
}

GXVoid GXCollisionDetector::Check ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData )
{
	static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );
	static const GXVec3 initialDirection ( 0.0f, 0.0f, 1.0f );

	GXSimplex simplex;
	GXSupportPoint supportPoint;
	GXVec3 direction;

	GXBool isLoop = GX_TRUE;
	GXBool isIntersected = GX_FALSE;

	CalculateSupportPoint ( supportPoint, shapeA, shapeB, initialDirection );
	simplex.PushPoint ( supportPoint );
	GXSubVec3Vec3 ( direction, origin, supportPoint.difference );

	while ( isLoop )
	{
		CalculateSupportPoint ( supportPoint, shapeA, shapeB, direction );

		GXFloat t = GXDotVec3Fast ( supportPoint.difference, direction );
		if ( GXDotVec3Fast ( supportPoint.difference, direction ) < 0.0f )
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
				GXSubVec3Vec3 ( simplexLineDirection, simplex.supportPoints[ 0 ].difference, simplex.supportPoints[ 1 ].difference );
				GXNormalizeVec3 ( simplexLineDirection );
				
				GXVec3 toOrigin;
				GXSubVec3Vec3 ( toOrigin, origin, simplex.supportPoints[ 1 ].difference );

				GXVec3 alpha;
				GXProjectVec3Vec3 ( alpha, toOrigin, simplexLineDirection );

				GXVec3 betta;
				GXSumVec3Vec3 ( betta, simplex.supportPoints[ 1 ].difference, alpha );

				GXSubVec3Vec3 ( direction, origin, betta );
			}
			break;

			case 3:
			{
				GXPlane simplexPlane;
				simplexPlane.From ( simplex.supportPoints[ 0 ].difference, simplex.supportPoints[ 1 ].difference, simplex.supportPoints[ 2 ].difference );
				switch ( GXPlaneClassifyVertex ( simplexPlane, origin ) )
				{
					case eGXPlaneClassifyVertex::On:
					case eGXPlaneClassifyVertex::InFront:
						direction = GXCreateVec3 ( simplexPlane.a, simplexPlane.b, simplexPlane.c );
					break;

					case eGXPlaneClassifyVertex::Behind:
						direction = GXCreateVec3 ( -simplexPlane.a, -simplexPlane.b, -simplexPlane.c );
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

					if ( GXPlaneClassifyVertex ( simplexPlane, origin ) != eGXPlaneClassifyVertex::Behind ) continue;

					direction = GXCreateVec3 ( -simplexPlane.a, -simplexPlane.b, -simplexPlane.c );

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

	if ( !isIntersected ) return;

	GXSupportPoint* supportPoints = (GXSupportPoint*)epaSupportPoints.GetData ();
	GXEdge* edges = (GXEdge*)epaEdges.GetData ();
	GXFace* faces = (GXFace*)epaFaces.GetData ();

	memcpy ( supportPoints, simplex.supportPoints, 4 * sizeof ( GXSupportPoint ) );
	numEPASupportPoints = 4;

	numEPAFaces = 0;

	GXFace face = simplex.faces[ 0 ];
	face.next = INVALID_INDEX;
	face.prev = INVALID_INDEX;
	GXUInt lastFaceIndex = 0;
	faces[ lastFaceIndex ] = face;
	numEPAFaces++;

	for ( GXUInt i = 1; i < 4; i++ )
	{
		GXFace face = simplex.faces[ i ];
		face.next = INVALID_INDEX;
		face.prev = lastFaceIndex;
		lastFaceIndex = numEPAFaces;
		faces[ lastFaceIndex ] = face;
		faces[ face.prev ].next = lastFaceIndex;
		numEPAFaces++;
	}

	static const GXFace voidFace;

	GXFloat contactPenetration = INITIAL_EPA_DISTANCE;
	GXVec3 contactNormal;

	while ( GX_TRUE )
	{
		GXFloat smallestDistance = INITIAL_EPA_DISTANCE;
		GXUInt smallestDistanceFaceIndex = 0;
		GXVec3 smallestDistanceFaceNormal;

		for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faces[ i ].prev )
		{
			GXPlane plane;
			plane.From ( supportPoints[ faces[ i ].pointA ].difference, supportPoints[ faces[ i ].pointB ].difference, supportPoints[ faces[ i ].pointC ].difference );

			GXVec3 gamma;
			GXSubVec3Vec3 ( gamma, origin, supportPoints[ faces[ i ].pointA ].difference );

			GXVec3 planeNormal ( plane.a, plane.b, plane.c );
			GXFloat newDistance = GXDotVec3Fast ( gamma, planeNormal );

			if ( newDistance >= smallestDistance ) continue;

			smallestDistance = newDistance;
			smallestDistanceFaceIndex = i;
			smallestDistanceFaceNormal = planeNormal;
		}

		contactPenetration = smallestDistance;

		if ( fabsf ( smallestDistance - contactPenetration ) <= EPA_DISTANCE_EPSILON )
		{
			contactNormal = smallestDistanceFaceNormal;
			GXReverseVec3 ( contactNormal );

			break;
		}

		if ( epaSupportPoints.GetLength () < numEPASupportPoints + 1 )
		{
			static const GXSupportPoint voidSupportPoint;
			epaSupportPoints.SetValue ( epaSupportPoints.GetLength () + EPA_SUPPORT_POINTS_ALLOCATING_STEP - 1, &voidSupportPoint );
			supportPoints = (GXSupportPoint*)epaSupportPoints.GetData ();
		}

		direction = smallestDistanceFaceNormal;
		GXReverseVec3 ( direction );
		GXSupportPoint d;

		CalculateSupportPoint ( d, shapeA, shapeB, direction );

		GXUInt newEPASupportPointIndex = numEPASupportPoints;
		supportPoints[ newEPASupportPointIndex ] = d;
		numEPASupportPoints++;

		GXUInt lastEdgeIndex = INVALID_INDEX;
		numEPAEdges = 0;

		for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faces[ i ].prev )
		{
			GXFace& face = faces[ i ];

			GXPlane plane;
			plane.From ( supportPoints[ face.pointA ].difference, supportPoints[ face.pointB ].difference, supportPoints[ face.pointC ].difference );
			GXVec3 normal ( plane.a, plane.b, plane.c );

			GXVec3 omega;
			GXSubVec3Vec3 ( omega, supportPoints[ face.pointA ].difference, d.difference );

			if ( GXDotVec3Fast ( omega, normal ) <= 0.0f ) continue;

			ProceedEdge ( GXEdge ( face.pointA, face.pointB ), lastEdgeIndex, &edges, numEPAEdges );
			ProceedEdge ( GXEdge ( face.pointB, face.pointC ), lastEdgeIndex, &edges, numEPAEdges );
			ProceedEdge ( GXEdge ( face.pointC, face.pointA ), lastEdgeIndex, &edges, numEPAEdges );

			if ( i == lastFaceIndex )
				lastFaceIndex = face.prev;

			if ( face.prev != INVALID_INDEX )
				faces[ face.prev ].next = face.next;

			if ( face.next != INVALID_INDEX )
				faces[ face.next ].prev = face.prev;
		}

		for ( GXUInt i = lastEdgeIndex; i != INVALID_INDEX; i = edges[ i ].prev )
		{
			const GXEdge& edge = edges[ i ];
			GXFace face ( newEPASupportPointIndex, edge.startPoint, edge.endPoint );

			GXPlane plane;
			plane.From ( supportPoints[ face.pointA ].difference, supportPoints[ face.pointB ].difference, supportPoints[ face.pointC ].difference );
			
			if ( GXPlaneClassifyVertex ( plane, origin ) == eGXPlaneClassifyVertex::Behind )
				face.ChangeOrder ();

			if ( epaFaces.GetLength () < numEPAFaces + 1 )
			{
				epaFaces.SetValue ( epaFaces.GetLength () + EPA_FACE_ALLOCATIONG_STEP - 1, &voidFace );
				faces = (GXFace*)epaFaces.GetData ();
			}

			face.next = INVALID_INDEX;
			face.prev = lastFaceIndex;
			lastFaceIndex = numEPAFaces;
			faces[ lastFaceIndex ] = face;
			faces[ face.prev ].next = lastFaceIndex;
			numEPAFaces++;
		}
	}

	static const GXVec3 voidNormal ( 0.0f, 0.0f, 0.0f );
	static const GXVec3 voidPoint ( 0.0f, 0.0f, 0.0f );

	GXContact* contact = collisionData.GetContactsBegin ();
	contact->SetNormal ( contactNormal );
	contact->SetPenetration ( contactPenetration );
	contact->SetContactPoint ( voidPoint );

	collisionData.AddContacts ( 1 );
}

GXCollisionDetector::GXCollisionDetector ():
epaSupportPoints ( sizeof ( GXSupportPoint ) ), epaEdges ( sizeof ( GXEdge ) ), epaFaces ( sizeof ( GXFace ) )
{
	static const GXSupportPoint voidSupportPoint;
	static const GXEdge voidEdge;
	static const GXFace voidFace;

	epaSupportPoints.SetValue ( EPA_INITIAL_SUPPORT_POINT_ARRAY_CAPACITY - 1, &voidSupportPoint );
	numEPASupportPoints = 0;

	epaEdges.SetValue ( EPA_INITIAL_EDGE_ARRAY_CAPACITY - 1, &voidEdge );
	numEPAEdges = 0;

	epaFaces.SetValue ( EPA_INITIAL_FACE_ARRAY_CAPACITY - 1, &voidFace );
	numEPAFaces = 0;
}

GXVoid GXCollisionDetector::CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction )
{
	shapeA.GetExtremePoint ( supportPoint.extremeA, direction );
	GXVec3 reverseDirection = direction;
	GXReverseVec3 ( reverseDirection );
	shapeB.GetExtremePoint ( supportPoint.extremeB, reverseDirection );
	GXSubVec3Vec3 ( supportPoint.difference, supportPoint.extremeA, supportPoint.extremeB );
}

GXVoid GXCollisionDetector::ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edges, GXUInt &totalEdges )
{
	GXBool allowInsert = GX_TRUE;
	GXEdge* edgeArray = *edges;

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

	if ( epaEdges.GetLength () < numEPAEdges + 1 )
	{
		static const GXEdge voidEdge;
		epaEdges.SetValue ( epaEdges.GetLength () + EPA_EDGE_ALLOCATIONG_STEP - 1, &voidEdge );
		*edges = (GXEdge*)epaEdges.GetData ();
	}

	edge.next = INVALID_INDEX;
	edge.prev = lastEdgeIndex;
	lastEdgeIndex = numEPAEdges;
	edgeArray[ lastEdgeIndex ] = edge;

	if ( lastEdgeIndex > 0 )
		edgeArray[ edge.prev ].next = lastEdgeIndex;

	totalEdges++;
}
