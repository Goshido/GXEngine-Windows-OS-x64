//version 1.0

#ifndef GX_COLLISION_DETECTOR
#define GX_COLLISION_DETECTOR


#include "GXSphereShape.h"
#include "GXPlaneShape.h"
#include "GXBoxShape.h"
#include "GXCollisionData.h"


struct GXSupportPoint;
struct GXSimplex;
struct GXTriangle;
class GXCollisionDetector
{
	public:
		GXVoid CheckViaGJK ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData );

	private:
		GXVoid CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction );
};


#endif GX_COLLISION_DETECTOR
