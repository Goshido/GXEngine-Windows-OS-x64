//version 1.1

#ifndef GX_COLLISION_DETECTOR
#define GX_COLLISION_DETECTOR


#include "GXSphereShape.h"
#include "GXPlaneShape.h"
#include "GXBoxShape.h"
#include "GXCollisionData.h"
#include <GXCommon/GXMemory.h>


struct GXSupportPoint;
struct GXEdge;
class GXCollisionDetector
{
	private:
		GXDynamicArray					supportPoints;
		GXUInt							totalSupportPoints;

		GXDynamicArray					edges;
		GXUInt							totalEdges;

		GXDynamicArray					faces;
		GXUInt							totalFaces;

		static GXCollisionDetector*		instance;

	public:
		static GXCollisionDetector& GXCALL GetInstance ();
		~GXCollisionDetector ();

		GXVoid Check ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData );

		GXUInt GetAllocatedSupportPoints () const;
		GXUInt GetAllocatedEdges () const;
		GXUInt GetAllocatedFaces () const;

	private:
		GXCollisionDetector ();
		GXVoid CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction );
		GXVoid ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edgeArrayPointer, GXUInt &totalEdges );
};


#endif GX_COLLISION_DETECTOR
