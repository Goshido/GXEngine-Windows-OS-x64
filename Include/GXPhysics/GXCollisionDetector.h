//version 1.2

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
		GXUInt							totalShapeAContactGeometryPoints;

		GXVec3*							shapeBContactGeometry;
		GXUInt							totalShapeBContactGeometryPoints;

		GXVec3*							shapeAProjectedContactGeometry;
		GXVec3*							shapeBProjectedContactGeometry;

		GXVec2*							shapeAPlanarContactGeometry;
		GXVec3*							shapeAPlanarContactGeometryDebug;

		GXVec2*							shapeBPlanarContactGeometry;
		GXVec3*							shapeBPlanarContactGeometryDebug;

		GXDynamicArray					alphaPlanarIntersectionGeometry;
		GXDynamicArray					bettaPlanarIntersectionGeometry;
		GXDynamicArray					planarIntersectionGeometry;
		GXDynamicArray					intersectionGeometry;
		GXUInt							totalIntersectionPoints;

		static GXCollisionDetector*		instance;

	public:
		static GXCollisionDetector& GXCALL GetInstance ();
		~GXCollisionDetector ();

		GXVoid Check ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData );

		GXVoid EnableDebugData ();
		GXVoid DisableDebugData ();

		GXVoid SetDeviationAxes ( GXUShort axes );
		GXUShort GetDeviationAxes () const;

		GXVoid SetDeviationAngle ( GXFloat radians );
		GXFloat GetDeviationAngle () const;

		const GXVec3* GetShapeAContactGeometry () const;
		GXUInt GetTotalShapeAContactGeometryPoints () const;

		const GXVec3* GetShapeBContactGeometry () const;
		GXUInt GetTotalShapeBContactGeometryPoints () const;

		const GXVec3* GetShapeAPlanarContactGeometry () const;
		GXUInt GetTotalShapeAPlanarContactGeometryPoints () const;

		const GXVec3* GetShapeBPlanarContactGeometry () const;
		GXUInt GetTotalShapeBPlanarContactGeometryPoints () const;

		const GXVec3* GetPlanarIntersectionGeometry () const;
		GXUInt GetTotalPlanarIntersectionPoints () const;

		GXUInt GetAllocatedSupportPoints () const;
		GXUInt GetAllocatedEdges () const;
		GXUInt GetAllocatedFaces () const;

	private:
		GXCollisionDetector ();

		GXVoid CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction );
		GXVoid ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edgeArrayPointer, GXUInt &totalEdgeNumber );
		GXVoid ProjectContactGeometry ( GXVec3* contactGeometryProjection, const GXVec3* contactGeometryWorld, GXUShort totalContactGeometryPoints, const GXVec3 &contactNormal );
		GXVoid CalculatePlanarContactGeometryCoordinates ( GXVec2* planarContactGeometry, const GXVec3* projectedContactGeometry, GXUShort totalContactGeometryPoints, const GXVec3 &xAxis, const GXVec3 &yAxis );
		GXVoid GetClippedPlanarContactGeometry ( GXVec2** clippedGeometryArrayPointer, GXUInt &totalPoints, const GXVec2* clipGeometry, GXUInt totalClipGeometryPoints, const GXVec2* subjectGeometry, GXUInt totalSubjectGeometryPoints );
		GXVoid GetIntersectionGeometryCoordinates ( GXVec3* out, const GXVec2* planarIntersectionGeometryArray, GXUInt totalIntersectionPointNumber, const GXVec3 &xAxis, const GXVec3 &yAxis );
		GXVoid GetCentroid ( GXVec3 &centroid, const GXVec3* points, GXUInt totalPoints );
		GXVoid UpdateDeviationAxes ();
		GXVoid UpdateDebugData ( const GXVec2* planarIntersectionGeometry );
};


#endif GX_COLLISION_DETECTOR
