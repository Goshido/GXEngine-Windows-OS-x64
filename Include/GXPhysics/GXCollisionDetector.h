//version 1.7

#ifndef GX_COLLISION_DETECTOR
#define GX_COLLISION_DETECTOR


#include "GXCollisionData.h"
#include "GXShape.h"
#include <GXCommon/GXMemory.h>


struct GXEdge;
struct GXSimplex;
struct GXSupportPoint;

class GXCollisionDetector final : public GXMemoryInspector
{
    private:
        GXDynamicArray                  _supportPoints;
        GXUInt                          _totalSupportPoints;

        GXDynamicArray                  _edges;
        GXUInt                          _totalEdges;

        GXDynamicArray                  _faces;
        GXUInt                          _totalFaces;

        GXVec3*                         _deviationAxesLocal;
        GXUShort                        _totalDeviationAxes;
        GXFloat                         _deviationAngle;

        GXVec3*                         _shapeAContactGeometry;
        GXUInt                          _totalShapeAContactGeometryPoints;

        GXVec3*                         _shapeBContactGeometry;
        GXUInt                          _totalShapeBContactGeometryPoints;

        GXVec3*                         _shapeAProjectedContactGeometry;
        GXVec3*                         _shapeBProjectedContactGeometry;

        GXVec2*                         _shapeAPlanarContactGeometry;
        GXVec3*                         _shapeAPlanarContactGeometryDebug;

        GXVec2*                         _shapeBPlanarContactGeometry;
        GXVec3*                         _shapeBPlanarContactGeometryDebug;

        GXDynamicArray                  _alphaPlanarIntersectionGeometry;
        GXDynamicArray                  _bettaPlanarIntersectionGeometry;
        GXDynamicArray                  _planarIntersectionGeometry;
        GXDynamicArray                  _intersectionGeometry;
        GXUInt                          _totalIntersectionPoints;

        static GXCollisionDetector*     _instance;

    public:
        static GXCollisionDetector& GXCALL GetInstance ();
        ~GXCollisionDetector ();

        // Method will append contact if it exists.
        // Note two kinematic bodies will cause undefined behaviour.
        GXVoid Check ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData );

        GXBool DoesCollide ( const GXShape &shapeA, const GXShape &shapeB );

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

        GXBool RunGJK ( GXSimplex &simplex, GXUInt &usedIterations, const GXShape &shapeA, const GXShape &shapeB );

        // Note contactNormal will contain direction to move body with shapeA by convention.
        GXBool RunEPA ( GXVec3 &contactNormal, GXFloat &contactPenetration, GXUInt &usedEdges, GXUInt &usedIterations, const GXShape &shapeA, const GXShape &shapeB, GXSimplex &simplex );

        GXVoid CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction );
        GXVoid ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edgeArrayPointer, GXUInt &totalEdgeNumber );
        GXVoid ProjectContactGeometry ( GXVec3* contactGeometryProjection, const GXVec3* contactGeometryWorld, GXUShort totalContactGeometryPoints, const GXVec3 &contactNormal );
        GXVoid CalculatePlanarContactGeometryCoordinates ( GXVec2* planarContactGeometry, const GXVec3* projectedContactGeometry, GXUShort totalContactGeometryPoints, const GXVec3 &xAxis, const GXVec3 &yAxis );
        GXVoid GetClippedPlanarContactGeometry ( GXVec2** clippedGeometryArrayPointer, GXUInt &totalPoints, const GXVec2* clipGeometry, GXUInt totalClipGeometryPoints, const GXVec2* subjectGeometry, GXUInt totalSubjectGeometryPoints );
        GXVoid GetIntersectionGeometryCoordinates ( GXVec3* out, const GXVec2* planarIntersectionGeometryArray, GXUInt totalIntersectionPointNumber, const GXVec3 &xAxis, const GXVec3 &yAxis );
        GXVoid GetCentroid ( GXVec3 &centroid, const GXVec3* points, GXUInt totalPoints );
        GXVoid UpdateDeviationAxes ();
        GXVoid UpdateDebugData ( const GXVec2* planarIntersectionGeometry );

        // Note contact resolver implements constraint based method.
        // This method has some limitations. So there are two rules:
        // 1) Kinematic body MUST be first body
        // 2) Body with less mass MUST be fisrt body if two bodies are dynamics bodies
        //
        // See details in paper "Iterative Dynamics with Temporal Coherence" by Erin Catto,
        // Paragraph: 9.2 Extreme Mass Ratios
        GXVoid NormalizeShapes ( const GXShape** normalizedShapeA, const GXShape** normalizedShapeB, const GXShape &shapeA, const GXShape &shapeB ) const;

        GXCollisionDetector ( const GXCollisionDetector &other ) = delete;
        GXCollisionDetector& operator = ( const GXCollisionDetector &other ) = delete;
};


#endif GX_COLLISION_DETECTOR
