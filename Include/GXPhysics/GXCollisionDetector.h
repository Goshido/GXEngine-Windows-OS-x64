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

		GXVec3*							deviationAxesLocal;
		GXUShort						totalDeviationAxes;
		GXFloat							deviationAngle;

		GXVec3*							shapeAContactGeometry;
		GXUShort						totalShapeAContactGeometryPoints;

		GXVec3*							shapeBContactGeometry;
		GXUShort						totalShapeBContactGeometryPoints;

		GXVec3*							shapeAProjectedContactGeometry;
		GXUShort						totalShapeAProjectedContactGeometryPoints;

		GXVec3*							shapeBProjectedContactGeometry;
		GXUShort						totalShapeBProjectedContactGeometryPoints;

		GXVec3*							shapeAPlanarContactGeometry;
		GXUShort						totalShapeAPlanarContactGeometryPoints;

		GXVec3*							shapeBPlanarContactGeometry;
		GXUShort						totalShapeBPlanarContactGeometryPoints;

		GXDynamicArray					alphaPlanarIntersectionGeometry;
		GXDynamicArray					bettaPlanarIntersectionGeometry;
		GXUInt							totalPlanarIntersectionPoints;

		static GXCollisionDetector*		instance;

	public:
		static GXCollisionDetector& GXCALL GetInstance ();
		~GXCollisionDetector ();

		GXVoid Check ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData );

		GXVoid SetDeviationAxes ( GXUShort axes );
		GXUShort GetDeviationAxes () const;

		GXVoid SetDeviationAngle ( GXFloat radians );
		GXFloat GetDeviationAngle () const;

		const GXVec3* GetShapeAContactGeometry () const;
		GXUShort GetTotalShapeAContactGeometryPoints () const;

		const GXVec3* GetShapeBContactGeometry () const;
		GXUShort GetTotalShapeBContactGeometryPoints () const;

		const GXVec3* GetShapeAProjectedContactGeometry () const;
		GXUShort GetTotalShapeAProjectedContactGeometryPoints () const;

		const GXVec3* GetShapeBProjectedContactGeometry () const;
		GXUShort GetTotalShapeBProjectedContactGeometryPoints () const;

		const GXVec3* GetShapeAPlanarContactGeometry () const;
		GXUShort GetTotalShapeAPlanarContactGeometryPoints () const;

		const GXVec3* GetShapeBPlanarContactGeometry () const;
		GXUShort GetTotalShapeBPlanarContactGeometryPoints () const;

		GXUInt GetAllocatedSupportPoints () const;
		GXUInt GetAllocatedEdges () const;
		GXUInt GetAllocatedFaces () const;

	private:
		GXCollisionDetector ();

		GXVoid CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction );
		GXVoid ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edgeArrayPointer, GXUInt &totalEdges );
		GXVoid ProjectContactGeometry ( GXVec3* contactGeometryProjection, const GXVec3* contactGeometryWorld, GXUShort totalContactGeometryPoints, const GXVec3 &contactNormal );
		GXVoid CalculatePlanarContactGeometryCoordinates ( GXVec3* planarContactGeometry, const GXVec3* projectedContactGeometry, GXUShort totalContactGeometryPoints, const GXVec3 &xAxis, const GXVec3 &yAxis );
		GXVoid UpdateDeviationAxes ();
};


#endif GX_COLLISION_DETECTOR
