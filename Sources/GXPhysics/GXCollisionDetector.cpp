//version 1.0

#include <GXPhysics/GXCollisionDetector.h>
#include <GXCommon/GXLogger.h>


#define TRY_AXIS_EPSILON				0.0001f
#define CONTACT_POINT_EPSILON			0.0001f
#define DEFAULT_MINKOWSKI_EPSILON		0.001f
#define DEFAULT_MAX_EPA_ITERATIONS		35
#define DEFAULT_MIN_EPA_DOT_DISTANCE	0.001f
#define DEFAULT_EDGE_EPSILON			1.0e-3f
#define INVALID_INDEX					0xFF


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

struct GXFace
{
	GXUByte		a;
	GXUByte		b;
	GXUByte		c;

	GXFace ();

	GXVoid ChangeOrder ();
};

GXFace::GXFace ()
{
	a = b = c = INVALID_INDEX;
}

GXVoid GXFace::ChangeOrder ()
{
	GXUByte tmp = a;
	a = b;
	b = tmp;
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

	faces[ 0 ].a = 0;
	faces[ 0 ].b = 1;
	faces[ 0 ].c = 2;

	GXPlane plane;
	plane.From ( supportPoints[ faces[ 0 ].a ].difference, supportPoints[ faces[ 0 ].b ].difference, supportPoints[ faces[ 0 ].c ].difference );
	
	if ( GXPlaneClassifyVertex ( plane, supportPoints[ 3 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 0 ].ChangeOrder ();

	faces[ 1 ].a = 0;
	faces[ 1 ].b = 2;
	faces[ 1 ].c = 3;
	plane.From ( supportPoints[ faces[ 1 ].a ].difference, supportPoints[ faces[ 1 ].b ].difference, supportPoints[ faces[ 1 ].c ].difference );

	if ( GXPlaneClassifyVertex ( plane, supportPoints[ 1 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 1 ].ChangeOrder ();

	faces[ 2 ].a = 0;
	faces[ 2 ].b = 3;
	faces[ 2 ].c = 1;
	plane.From ( supportPoints[ faces[ 2 ].a ].difference, supportPoints[ faces[ 2 ].b ].difference, supportPoints[ faces[ 2 ].c ].difference );

	if ( GXPlaneClassifyVertex ( plane, supportPoints[ 2 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 2 ].ChangeOrder ();

	faces[ 3 ].a = 1;
	faces[ 3 ].b = 3;
	faces[ 3 ].c = 4;
	plane.From ( supportPoints[ faces[ 3 ].a ].difference, supportPoints[ faces[ 3 ].b ].difference, supportPoints[ faces[ 3 ].c ].difference );

	if ( GXPlaneClassifyVertex ( plane, supportPoints[ 0 ].difference ) == eGXPlaneClassifyVertex::Behind )
		faces[ 3 ].ChangeOrder ();
}

GXVoid GXSimplex::Reset ()
{
	numPoints = 0;
}

//------------------------------------------------------------------------------------------

GXVoid GXCollisionDetector::CheckViaGJK ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData )
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
					simplexPlane.From ( simplex.supportPoints[ simplex.faces[ i ].a ].difference, simplex.supportPoints[ simplex.faces[ i ].b ].difference, simplex.supportPoints[ simplex.faces[ i ].c ].difference );

					if ( GXPlaneClassifyVertex ( simplexPlane, origin ) != eGXPlaneClassifyVertex::Behind ) continue;

					direction = GXCreateVec3 ( -simplexPlane.a, -simplexPlane.b, -simplexPlane.c );

					GXSupportPoint a = simplex.supportPoints[ simplex.faces[ i ].a ];
					GXSupportPoint b = simplex.supportPoints[ simplex.faces[ i ].b ];
					GXSupportPoint c = simplex.supportPoints[ simplex.faces[ i ].c ];

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

	static const GXVec3 voidNormal ( 0.0f, 0.0f, 0.0f );
	static const GXVec3 voidPoint ( 0.0f, 0.0f, 0.0f );
	const GXFloat voidPenetration = 0.0f;

	GXContact* contact = collisionData.GetContactsBegin ();
	contact->SetNormal ( voidNormal );
	contact->SetPenetration ( voidPenetration );
	contact->SetContactPoint ( voidPoint );

	collisionData.AddContacts ( 1 );
}

GXVoid GXCollisionDetector::CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction )
{
	shapeA.GetExtremePoint ( supportPoint.extremeA, direction );
	GXVec3 reverseDirection = direction;
	GXReverseVec3 ( reverseDirection );
	shapeB.GetExtremePoint ( supportPoint.extremeB, reverseDirection );
	GXSubVec3Vec3 ( supportPoint.difference, supportPoint.extremeA, supportPoint.extremeB );
}
