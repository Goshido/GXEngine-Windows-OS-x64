//version 1.0

#ifndef GX_COLLISION_DETECTOR
#define GX_COLLISION_DETECTOR


#include "GXSphereShape.h"
#include "GXPlaneShape.h"
#include "GXBoxShape.h"
#include "GXCollisionData.h"


class GXCollisionDetector
{
	public:
		GXUInt CheckSphereAndSphere ( const GXSphereShape &sphereA, const GXSphereShape &sphereB, GXCollisionData* collisionData );
		GXUInt CheckSphereAndHalfSpace ( const GXSphereShape &sphere, const GXPlaneShape &plane, GXCollisionData* collisionData );
		GXUInt CheckSphereAndTruePlane ( const GXSphereShape &sphere, const GXPlaneShape &plane, GXCollisionData* collisionData );
		GXUInt CheckBoxAndHalfSpace ( const GXBoxShape &box, const GXPlaneShape &plane, GXCollisionData* collisionData );
		GXUInt CheckBoxAndSphere ( const GXBoxShape &box, const GXSphereShape &sphere, GXCollisionData* collisionData );
		GXUInt CheckBoxAndBox ( const GXBoxShape &boxA, const GXBoxShape &boxB, GXCollisionData* collisionData );

	private:
		GXBool TryAxis ( const GXBoxShape &boxA, const GXBoxShape &boxB, GXVec3 axis, const GXVec3 &toCentre, GXUInt index, GXFloat smallestPenetration, GXUInt &smallestCase );
		GXFloat PenetrationOnAxis ( const GXBoxShape &one, const GXBoxShape &two, const GXVec3 &axis, const GXVec3 &toCentre );
		GXFloat TransformToAxis ( const GXBoxShape &box, const GXVec3 &axis );
		GXVoid FillPointFaceBoxBox ( const GXBoxShape &one, const GXBoxShape &two, const GXVec3 &toCentre, GXCollisionData* collisionData, GXUInt best, GXFloat pen );
};


#endif GX_COLLISION_DETECTOR