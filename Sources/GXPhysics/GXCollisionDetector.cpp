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

#define INITIAL_EPA_DISTANCE						FLT_MAX
#define EPA_DISTANCE_EPSILON						1.0e-5f

#define DEFAULT_DEVIATION_AXES						8
#define DEFAULT_DEVIATION_ANGLE						0.174533f	// 10 degrees


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
	normal = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
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

	GXReverseVec3 ( normal );
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
	free ( deviationAxesLocal );
	free ( shapeAContactGeometry );
	free ( shapeBContactGeometry );
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
	GXUInt gjkIterations = 0;

	CalculateSupportPoint ( supportPoint, shapeA, shapeB, initialDirection );
	simplex.PushPoint ( supportPoint );
	GXSubVec3Vec3 ( direction, origin, supportPoint.difference );

	while ( isLoop )
	{
		gjkIterations++;

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

	if ( !isIntersected )
	{
		totalShapeAContactGeometryPoints = 0;
		totalShapeBContactGeometryPoints = 0;

		return;
	}

	GXSupportPoint* supportPointArray = (GXSupportPoint*)supportPoints.GetData ();
	GXEdge* edgeArray = (GXEdge*)edges.GetData ();
	GXFace* faceArray = (GXFace*)faces.GetData ();

	memcpy ( supportPointArray, simplex.supportPoints, 4 * sizeof ( GXSupportPoint ) );
	totalSupportPoints = 4;

	totalFaces = 0;

	GXFace& face = simplex.faces[ 0 ];
	GXPlane plane;
	plane.From ( supportPointArray[ face.pointA ].difference, supportPointArray[ face.pointB ].difference, supportPointArray[ face.pointC ].difference );
	face.normal = GXCreateVec3 ( plane.a, plane.b, plane.c );
	face.next = INVALID_INDEX;
	face.prev = INVALID_INDEX;
	GXUInt lastFaceIndex = 0;
	faceArray[ lastFaceIndex ] = face;
	totalFaces++;

	for ( GXUInt i = 1; i < 4; i++ )
	{
		GXFace& face = simplex.faces[ i ];
		plane.From ( supportPointArray[ face.pointA ].difference, supportPointArray[ face.pointB ].difference, supportPointArray[ face.pointC ].difference );
		face.normal = GXCreateVec3 ( plane.a, plane.b, plane.c );
		face.next = INVALID_INDEX;
		face.prev = lastFaceIndex;
		lastFaceIndex = totalFaces;
		faceArray[ lastFaceIndex ] = face;
		faceArray[ face.prev ].next = lastFaceIndex;
		totalFaces++;
	}

	static const GXFace voidFace;

	GXFloat contactPenetration = INITIAL_EPA_DISTANCE;
	GXVec3 contactNormal;
	GXUInt epaIterations = 0;
	GXUInt maximumEdgesUsed = 0;

	while ( GX_TRUE )
	{
		epaIterations++;

		GXFloat smallestDistance = INITIAL_EPA_DISTANCE;
		GXUInt smallestDistanceFaceIndex = 0;
		GXVec3 smallestDistanceFaceNormal;

		for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faceArray[ i ].prev )
		{
			const GXFace& face = faceArray[ i ];

			GXVec3 gamma;
			GXSubVec3Vec3 ( gamma, origin, supportPointArray[ face.pointA ].difference );

			GXFloat newDistance = GXDotVec3Fast ( gamma, face.normal );

			if ( newDistance >= smallestDistance ) continue;

			smallestDistance = newDistance;
			smallestDistanceFaceIndex = i;
			smallestDistanceFaceNormal = face.normal;
		}

		if ( fabsf ( smallestDistance - contactPenetration ) <= EPA_DISTANCE_EPSILON )
		{
			contactPenetration = smallestDistance;
			contactNormal = smallestDistanceFaceNormal;
			GXReverseVec3 ( contactNormal );

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
		GXReverseVec3 ( direction );

		CalculateSupportPoint ( supportPoint, shapeA, shapeB, direction );

		GXUInt newSupportPointIndex = totalSupportPoints;
		supportPointArray[ newSupportPointIndex ] = supportPoint;
		totalSupportPoints++;

		GXUInt lastEdgeIndex = INVALID_INDEX;
		totalEdges = 0;

		for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faceArray[ i ].prev )
		{
			GXFace& face = faceArray[ i ];

			GXVec3 omega;
			GXSubVec3Vec3 ( omega, supportPointArray[ face.pointA ].difference, supportPoint.difference );

			if ( GXDotVec3Fast ( omega, face.normal ) <= 0.0f ) continue;

			ProceedEdge ( GXEdge ( face.pointA, face.pointB ), lastEdgeIndex, &edgeArray, totalEdges );
			ProceedEdge ( GXEdge ( face.pointB, face.pointC ), lastEdgeIndex, &edgeArray, totalEdges );
			ProceedEdge ( GXEdge ( face.pointC, face.pointA ), lastEdgeIndex, &edgeArray, totalEdges );

			if ( i == lastFaceIndex )
				lastFaceIndex = face.prev;

			if ( face.prev != INVALID_INDEX )
				faceArray[ face.prev ].next = face.next;

			if ( face.next != INVALID_INDEX )
				faceArray[ face.next ].prev = face.prev;
		}

		if ( totalEdges > maximumEdgesUsed )
			maximumEdgesUsed = totalEdges;

		for ( GXUInt i = lastEdgeIndex; i != INVALID_INDEX; i = edgeArray[ i ].prev )
		{
			const GXEdge& edge = edgeArray[ i ];

			plane.From ( supportPointArray[ face.pointA ].difference, supportPointArray[ face.pointB ].difference, supportPointArray[ face.pointC ].difference );
			GXFace face ( newSupportPointIndex, edge.startPoint, edge.endPoint, GXCreateVec3 ( plane.a, plane.b, plane.c ) );

			if ( GXPlaneClassifyVertex ( plane, origin ) == eGXPlaneClassifyVertex::Behind )
				face.ChangeOrder ();

			if ( faces.GetLength () < totalFaces + 1 )
			{
				faces.SetValue ( faces.GetLength () + EPA_FACE_ALLOCATIONG_STEP - 1, &voidFace );
				faceArray = (GXFace*)faces.GetData ();
			}

			face.next = INVALID_INDEX;
			face.prev = lastFaceIndex;
			lastFaceIndex = totalFaces;
			faceArray[ lastFaceIndex ] = face;
			faceArray[ face.prev ].next = lastFaceIndex;
			totalFaces++;
		}
	}

	GXContact* contact = collisionData.GetContactsBegin ();
	contact->SetNormal ( contactNormal );
	contact->SetPenetration ( contactPenetration );
	contact->SetGJKIterations ( gjkIterations );
	contact->SetEPAIterations ( epaIterations );
	contact->SetSupportPoints ( totalSupportPoints );
	contact->SetEdges ( maximumEdgesUsed );
	contact->SetFaces ( totalFaces );

	GXMat3 tbn;
	GXVec3 zetta;
	GXVec3 tangent;
	GXVec3 bitangent;

	if ( contactNormal.y > contactNormal.x && contactNormal.y > contactNormal.z )
		GXCrossVec3Vec3 ( zetta, GXVec3::GetAbsoluteX (), contactNormal );
	else
		GXCrossVec3Vec3 ( zetta, GXVec3::GetAbsoluteY (), contactNormal );

	GXCrossVec3Vec3 ( bitangent, contactNormal, zetta );
	GXNormalizeVec3 ( bitangent );
	GXCrossVec3Vec3 ( tangent, bitangent, contactNormal );

	tbn.SetX ( tangent );
	tbn.SetY ( bitangent );
	tbn.SetZ ( contactNormal );

	GXVec3 lastShapeAPoint ( FLT_MAX, FLT_MAX, FLT_MAX );
	GXVec3 lastShapeBPoint ( FLT_MAX, FLT_MAX, FLT_MAX );

	totalShapeAContactGeometryPoints = totalShapeBContactGeometryPoints = 0;

	for ( GXUShort i = 0; i < totalDeviationAxes; i++ )
	{
		GXMulVec3Mat3 ( direction, deviationAxesLocal[ i ], tbn );

		GXVec3 shapeAPoint;
		shapeA.GetExtremePoint ( shapeAPoint, direction );

		GXReverseVec3 ( direction );
		GXVec3 shapeBPoint;
		shapeB.GetExtremePoint ( shapeBPoint, direction );

		if ( !GXIsEqualVec3Vec3 ( lastShapeAPoint, shapeAPoint ) )
		{
			shapeAContactGeometry[ totalShapeAContactGeometryPoints ] = shapeAPoint;
			lastShapeAPoint = shapeAPoint;
			totalShapeAContactGeometryPoints++;
		}

		if ( !GXIsEqualVec3Vec3 ( lastShapeBPoint, shapeBPoint ) )
		{
			shapeBContactGeometry[ totalShapeBContactGeometryPoints ] = shapeBPoint;
			lastShapeBPoint = shapeBPoint;
			totalShapeBContactGeometryPoints++;
		}
	}

	if ( !GXIsEqualVec3Vec3 ( lastShapeAPoint, shapeAContactGeometry[ 0 ] ) )
	{
		shapeAContactGeometry[ totalShapeAContactGeometryPoints ] = shapeAContactGeometry[ 0 ];
		totalShapeAContactGeometryPoints++;
	}

	if ( !GXIsEqualVec3Vec3 ( lastShapeBPoint, shapeBContactGeometry[ 0 ] ) )
	{
		shapeBContactGeometry[ totalShapeBContactGeometryPoints ] = shapeBContactGeometry[ 0 ];
		totalShapeBContactGeometryPoints++;
	}

	collisionData.AddContacts ( 1 );
}

GXVoid GXCollisionDetector::SetDeviationAxes ( GXUShort axes )
{
	if ( axes == totalDeviationAxes ) return;

	GXSafeFree ( deviationAxesLocal );
	deviationAxesLocal = (GXVec3*)malloc ( axes * sizeof ( GXVec3 ) );
	totalDeviationAxes = axes;

	GXUPointer size = ( axes + 1 ) * sizeof ( GXVec3 );
	GXSafeFree ( shapeAContactGeometry );
	GXSafeFree ( shapeBContactGeometry );

	shapeAContactGeometry = (GXVec3*)malloc ( size );
	shapeBContactGeometry = (GXVec3*)malloc ( size );

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
	return shapeAContactGeometry;
}

GXUShort GXCollisionDetector::GetTotalShapeAContactGeometryPoints () const
{
	return totalShapeAContactGeometryPoints;
}

const GXVec3* GXCollisionDetector::GetShapeBContactGeometry () const
{
	return shapeBContactGeometry;
}

GXUShort GXCollisionDetector::GetTotalShapeBContactGeometryPoints () const
{
	return totalShapeBContactGeometryPoints;
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

GXCollisionDetector::GXCollisionDetector ():
supportPoints ( sizeof ( GXSupportPoint ) ), edges ( sizeof ( GXEdge ) ), faces ( sizeof ( GXFace ) )
{
	static const GXSupportPoint voidSupportPoint;
	static const GXEdge voidEdge;
	static const GXFace voidFace;

	supportPoints.SetValue ( EPA_INITIAL_SUPPORT_POINT_ARRAY_CAPACITY - 1, &voidSupportPoint );
	totalSupportPoints = 0;

	edges.SetValue ( EPA_INITIAL_EDGE_ARRAY_CAPACITY - 1, &voidEdge );
	totalEdges = 0;

	faces.SetValue ( EPA_INITIAL_FACE_ARRAY_CAPACITY - 1, &voidFace );
	totalFaces = 0;

	deviationAxesLocal = nullptr;

	shapeAContactGeometry = nullptr;
	totalShapeAContactGeometryPoints = 0;

	shapeBContactGeometry = nullptr;
	totalShapeBContactGeometryPoints = 0;

	deviationAngle = DEFAULT_DEVIATION_ANGLE;
	SetDeviationAxes ( DEFAULT_DEVIATION_AXES );
}

GXVoid GXCollisionDetector::CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction )
{
	shapeA.GetExtremePoint ( supportPoint.extremeA, direction );
	GXVec3 reverseDirection = direction;
	GXReverseVec3 ( reverseDirection );
	shapeB.GetExtremePoint ( supportPoint.extremeB, reverseDirection );
	GXSubVec3Vec3 ( supportPoint.difference, supportPoint.extremeA, supportPoint.extremeB );
}

GXVoid GXCollisionDetector::ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edgeArrayPointer, GXUInt &totalEdges )
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

	if ( edges.GetLength () < totalEdges + 1 )
	{
		static const GXEdge voidEdge;
		edges.SetValue ( edges.GetLength () + EPA_EDGE_ALLOCATIONG_STEP - 1, &voidEdge );
		*edgeArrayPointer = (GXEdge*)edges.GetData ();
	}

	edge.next = INVALID_INDEX;
	edge.prev = lastEdgeIndex;
	lastEdgeIndex = totalEdges;
	edgeArray[ lastEdgeIndex ] = edge;

	if ( lastEdgeIndex > 0 )
		edgeArray[ edge.prev ].next = lastEdgeIndex;

	totalEdges++;
}

GXVoid GXCollisionDetector::UpdateDeviationAxes ()
{
	GXFloat step = GX_MATH_DOUBLE_PI / (GXFloat)totalDeviationAxes;
	GXFloat limit = GX_MATH_DOUBLE_PI - 0.5f * step;
	GXUShort axisIndex = 0;
	GXFloat deviation = sinf ( deviationAngle );
	GXFloat normalizeScale = 1.0f / GXLengthVec3 ( GXCreateVec3 ( deviation, 0.0f, 1.0f ) );

	for ( GXFloat angle = 0.0f; angle < limit; angle += step )
	{
		deviationAxesLocal[ axisIndex ] = GXCreateVec3 ( cosf ( angle ) * deviation, sinf ( angle ) * deviation, 1.0f );
		GXMulVec3Scalar ( deviationAxesLocal[ axisIndex ], deviationAxesLocal[ axisIndex ], normalizeScale );
		axisIndex++;
	}
}
