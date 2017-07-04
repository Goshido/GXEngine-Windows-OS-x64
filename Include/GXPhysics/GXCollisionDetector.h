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
		GXUInt CheckSphereAndSphere ( const GXSphereShape &sphereA, const GXSphereShape &sphereB, GXCollisionData &collisionData );
		GXUInt CheckSphereAndHalfSpace ( const GXSphereShape &sphere, const GXPlaneShape &plane, GXCollisionData &collisionData );
		GXUInt CheckSphereAndTruePlane ( const GXSphereShape &sphere, const GXPlaneShape &plane, GXCollisionData &collisionData );
		GXUInt CheckBoxAndHalfSpace ( const GXBoxShape &box, const GXPlaneShape &plane, GXCollisionData &collisionData );
		GXUInt CheckBoxAndSphere ( const GXBoxShape &box, const GXSphereShape &sphere, GXCollisionData &collisionData );
		GXUInt CheckBoxAndBox ( const GXBoxShape &boxA, const GXBoxShape &boxB, GXCollisionData &collisionData );

		GXVoid CheckViaGJK ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData );

	private:
		GXBool TryAxis ( const GXBoxShape &boxA, const GXBoxShape &boxB, GXVec3 axis, const GXVec3 &toCentre, GXUInt index, GXFloat smallestPenetration, GXUInt &smallestCase );
		GXFloat PenetrationOnAxis ( const GXBoxShape &one, const GXBoxShape &two, const GXVec3 &axis, const GXVec3 &toCentre );
		GXFloat TransformToAxis ( const GXBoxShape &box, const GXVec3 &axis );
		GXVoid FillPointFaceBoxBox ( const GXBoxShape &one, const GXBoxShape &two, const GXVec3 &toCentre, GXCollisionData &collisionData, GXUInt best, GXFloat pen );
		GXVoid GetAxis ( GXVec3 &axis, const GXMat4 &transform, GXUInt index );
		GXVec3 ContactPoint ( const GXVec3 &pOne, const GXVec3 &dOne, GXFloat oneSize, const GXVec3 &pTwo, const GXVec3 &dTwo, GXFloat twoSize, GXBool useOne );

		GXVoid CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction );
		GXVoid ModifySimplex ( GXSimplex &simplex, GXVec3 &direction );
		GXVoid GetContacts ( GXTriangle &triangle, const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData);
		GXBool SimpleWindingCheck ( const GXTriangle &triangle, const GXVec3 &point );
};


#endif GX_COLLISION_DETECTOR
