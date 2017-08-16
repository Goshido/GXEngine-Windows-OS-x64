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
		GXDynamicArray					epaSupportPoints;
		GXUInt							numEPASupportPoints;

		GXDynamicArray					epaEdges;
		GXUInt							numEPAEdges;

		GXDynamicArray					epaFaces;
		GXUInt							numEPAFaces;

		static GXCollisionDetector*		instance;

	public:
		static GXCollisionDetector& GXCALL GetInstance ();
		~GXCollisionDetector ();

		GXVoid Check ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData );

	private:
		GXCollisionDetector ();
		GXVoid CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction );
		GXVoid ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edges, GXUInt &totalEdges );
};


#endif GX_COLLISION_DETECTOR
