// version 1.6

#include <GXPhysics/GXCollisionDetector.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXLogger.h>


#define INVALID_INDEX                                   0xFFFFFFFFu

#define EPA_INITIAL_SUPPORT_POINT_ARRAY_CAPACITY        64u
#define EPA_SUPPORT_POINTS_ALLOCATING_STEP              64u

#define EPA_INITIAL_FACE_ARRAY_CAPACITY                 128u
#define EPA_FACE_ALLOCATING_STEP                        128u

#define EPA_INITIAL_EDGE_ARRAY_CAPACITY                 384u
#define EPA_EDGE_ALLOCATING_STEP                        384u

#define EPA_DISTANCE_EPSILON                            1.0e-5f

#define DEFAULT_DEVIATION_AXES                          8u
#define DEFAULT_DEVIATION_ANGLE                         0.008726f    // 0.5 degrees

#define INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY      128u
#define PLANAR_INTERSECTION_ALLOCATING_STEP             128u

//---------------------------------------------------------------------------------------------------------------------

struct GXSupportPoint final
{
    GXVec3        difference;

    GXSupportPoint& operator = ( const GXSupportPoint &point );
};

GXSupportPoint& GXSupportPoint::operator = ( const GXSupportPoint &point )
{
    memcpy ( this, &point, sizeof ( GXSupportPoint ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

struct GXEdge final
{
    GXUInt        prev;
    GXUInt        next;

    GXUInt        startPoint;
    GXUInt        endPoint;

    GXEdge ();
    explicit GXEdge ( GXUInt startPoint, GXUInt endPoint );

    static GXBool IsOpposite ( const GXEdge &alpha, const GXEdge &betta );

    GXEdge& operator = ( const GXEdge &other );
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

GXEdge& GXEdge::operator = ( const GXEdge &other )
{
    memcpy ( this, &other, sizeof ( GXEdge ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

struct GXFace final
{
    GXUInt        next;
    GXUInt        prev;

    GXUInt        pointA;
    GXUInt        pointB;
    GXUInt        pointC;

    GXVec3        normal;

    GXFace ();
    explicit GXFace ( GXUInt pointA, GXUInt pointB, GXUInt pointC, const GXVec3 &normal );

    GXVoid ChangeOrder ();

    GXFace& operator = ( const GXFace &other );
};

GXFace::GXFace ()
{
    next = prev = INVALID_INDEX;
    pointA = pointB = pointC = INVALID_INDEX;
    normal.Init ( 0.0f, 0.0f, 0.0f );
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

    normal.Reverse ();
}

GXFace& GXFace::operator = ( const GXFace &other )
{
    memcpy ( this, &other, sizeof ( GXFace ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

struct GXSimplex final
{
    GXUByte                numPoints;
    GXSupportPoint        supportPoints[ 4u ];
    GXFace                faces[ 4u ];

    GXSimplex ();

    GXVoid PushPoint ( const GXSupportPoint &point );
    GXVoid Reset ();
};

GXSimplex::GXSimplex ():
    numPoints ( 0u )
{
    // NOTHING
}

GXVoid GXSimplex::PushPoint ( const GXSupportPoint &point )
{
    memmove ( supportPoints + 1u, supportPoints, 3u * sizeof ( GXSupportPoint ) );
    supportPoints[ 0u ] = point;
    ++numPoints;

    if ( numPoints < 4u ) return;

    faces[ 0u ].pointA = 0u;
    faces[ 0u ].pointB = 1u;
    faces[ 0u ].pointC = 2u;

    GXPlane plane;
    plane.From ( supportPoints[ faces[ 0u ].pointA ].difference, supportPoints[ faces[ 0u ].pointB ].difference, supportPoints[ faces[ 0u ].pointC ].difference );
    
    if ( plane.ClassifyVertex ( supportPoints[ 3 ].difference ) == eGXPlaneClassifyVertex::Behind )
        faces[ 0u ].ChangeOrder ();

    faces[ 1u ].pointA = 0u;
    faces[ 1u ].pointB = 2u;
    faces[ 1u ].pointC = 3u;
    plane.From ( supportPoints[ faces[ 1u ].pointA ].difference, supportPoints[ faces[ 1u ].pointB ].difference, supportPoints[ faces[ 1u ].pointC ].difference );

    if ( plane.ClassifyVertex ( supportPoints[ 1 ].difference ) == eGXPlaneClassifyVertex::Behind )
        faces[ 1u ].ChangeOrder ();

    faces[ 2u ].pointA = 0u;
    faces[ 2u ].pointB = 3u;
    faces[ 2u ].pointC = 1u;
    plane.From ( supportPoints[ faces[ 2u ].pointA ].difference, supportPoints[ faces[ 2u ].pointB ].difference, supportPoints[ faces[ 2u ].pointC ].difference );

    if ( plane.ClassifyVertex ( supportPoints[ 2u ].difference ) == eGXPlaneClassifyVertex::Behind )
        faces[ 2u ].ChangeOrder ();

    faces[ 3u ].pointA = 1u;
    faces[ 3u ].pointB = 3u;
    faces[ 3u ].pointC = 2u;
    plane.From ( supportPoints[ faces[ 3u ].pointA ].difference, supportPoints[ faces[ 3u ].pointB ].difference, supportPoints[ faces[ 3u ].pointC ].difference );

    if ( plane.ClassifyVertex ( supportPoints[ 0u ].difference ) == eGXPlaneClassifyVertex::Behind )
        faces[ 3u ].ChangeOrder ();
}

GXVoid GXSimplex::Reset ()
{
    numPoints = 0u;
}

//---------------------------------------------------------------------------------------------------------------------

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
    free ( shapeAProjectedContactGeometry );
    free ( shapeBProjectedContactGeometry );
    free ( shapeAPlanarContactGeometry );
    free ( shapeBPlanarContactGeometry );

    instance = nullptr;

    if ( !shapeAPlanarContactGeometryDebug ) return;

    free ( shapeAPlanarContactGeometryDebug );
    free ( shapeBPlanarContactGeometryDebug );
}

GXVoid GXCollisionDetector::Check ( const GXShape &shapeA, const GXShape &shapeB, GXCollisionData &collisionData )
{
    const GXShape* normalizedShapeA;
    const GXShape* normalizedShapeB;

    NormalizeShapes ( &normalizedShapeA, &normalizedShapeB, shapeA, shapeB );
    
    GXSimplex simplex;
    GXUInt gjkIterations;

    if ( !RunGJK ( simplex, gjkIterations, *normalizedShapeA, *normalizedShapeB ) ) return;

    GXVec3 contactNormal;
    GXFloat contactPenetration;
    GXUInt epaIterations;
    GXUInt maximumEdgesUsed;

    if ( !RunEPA ( contactNormal, contactPenetration, maximumEdgesUsed, epaIterations, *normalizedShapeA, *normalizedShapeB, simplex ) ) return;

    // Contact geometry search

    if ( normalizedShapeA->GetType () == eGXShapeType::Sphere || normalizedShapeB->GetType () == eGXShapeType::Sphere )
    {
        GXContact* contact = collisionData.GetContactsBegin ();
        contact->SetShapes ( *normalizedShapeA, *normalizedShapeB );

        normalizedShapeA->GetRigidBody ().SetAwake ();
        normalizedShapeB->GetRigidBody ().SetAwake ();

        contact->SetLinkedContacts ( 1u );
        contact->SetNormal ( contactNormal );

        contactNormal.Reverse ();

        GXVec3 contactPoint;
        normalizedShapeA->GetExtremePoint ( contactPoint, contactNormal );
        contact->SetContactPoint ( contactPoint );

        contact->SetPenetration ( contactPenetration );
        contact->SetGJKIterations ( gjkIterations );
        contact->SetEPAIterations ( epaIterations );
        contact->SetSupportPoints ( totalSupportPoints );
        contact->SetEdges ( maximumEdgesUsed );
        contact->SetFaces ( totalFaces );

        collisionData.AddContacts ( 1u );

        totalIntersectionPoints = 0u;
        return;
    }

    GXMat3 tbn;
    GXVec3 zetta;
    GXVec3 tangent;
    GXVec3 bitangent;

    if ( fabsf ( contactNormal.GetY () ) > fabsf ( contactNormal.GetX () ) && fabsf ( contactNormal.GetY () ) > fabsf ( contactNormal.GetZ () ) )
        zetta.CrossProduct ( GXVec3::GetAbsoluteX (), contactNormal );
    else
        zetta.CrossProduct ( GXVec3::GetAbsoluteY (), contactNormal );

    bitangent.CrossProduct ( contactNormal, zetta );
    bitangent.Normalize ();
    tangent.CrossProduct ( bitangent, contactNormal );

    tbn.SetX ( tangent );
    tbn.SetY ( bitangent );
    tbn.SetZ ( contactNormal );

    GXVec3 lastShapeAPoint ( FLT_MAX, FLT_MAX, FLT_MAX );
    GXVec3 lastShapeBPoint ( FLT_MAX, FLT_MAX, FLT_MAX );

    totalShapeAContactGeometryPoints = totalShapeBContactGeometryPoints = 0u;

    for ( GXUShort i = 0u; i < totalDeviationAxes; ++i )
    {
        GXVec3 direction;
        tbn.MultiplyVectorMatrix ( direction, deviationAxesLocal[ i ] );

        GXVec3 shapeBPoint;
        normalizedShapeB->GetExtremePoint ( shapeBPoint, direction );

        GXVec3 shapeAPoint;
        direction.Reverse ();
        normalizedShapeA->GetExtremePoint ( shapeAPoint, direction );

        if ( !lastShapeAPoint.IsEqual ( shapeAPoint ) )
        {
            shapeAContactGeometry[ totalShapeAContactGeometryPoints ] = shapeAPoint;
            lastShapeAPoint = shapeAPoint;
            ++totalShapeAContactGeometryPoints;
        }

        if ( !lastShapeBPoint.IsEqual ( shapeBPoint ) )
        {
            shapeBContactGeometry[ totalShapeBContactGeometryPoints ] = shapeBPoint;
            lastShapeBPoint = shapeBPoint;
            ++totalShapeBContactGeometryPoints;
        }
    }

    if ( totalShapeAContactGeometryPoints == 1u || totalShapeBContactGeometryPoints == 1u )
    {
        GXContact* contact = collisionData.GetContactsBegin ();
        contact->SetShapes ( *normalizedShapeA, *normalizedShapeB );

        normalizedShapeA->GetRigidBody ().SetAwake ();
        normalizedShapeB->GetRigidBody ().SetAwake ();

        contact->SetLinkedContacts ( 1u );
        contact->SetNormal ( contactNormal );
        contact->SetPenetration ( contactPenetration );
        contact->SetGJKIterations ( gjkIterations );
        contact->SetEPAIterations ( epaIterations );
        contact->SetSupportPoints ( totalSupportPoints );
        contact->SetEdges ( maximumEdgesUsed );
        contact->SetFaces ( totalFaces );

        if ( totalShapeAContactGeometryPoints == 1u )
            contact->SetContactPoint ( shapeAContactGeometry[ 0u ] );
        else
            contact->SetContactPoint ( shapeBContactGeometry[ 0u ] );

        collisionData.AddContacts ( 1u );

        totalIntersectionPoints = 0u;

        return;
    }

    if ( lastShapeAPoint.IsEqual ( shapeAContactGeometry[ 0u ] ) )
        --totalShapeAContactGeometryPoints;

    if ( lastShapeBPoint.IsEqual ( shapeBContactGeometry[ 0u ] ) )
        --totalShapeBContactGeometryPoints;

    // Projecting contact geometry onto a plane with normal "contactNormal" and origin at ( 0.0f, 0.0f, 0.0f )

    ProjectContactGeometry ( shapeAProjectedContactGeometry, shapeAContactGeometry, static_cast<GXUShort> ( totalShapeAContactGeometryPoints ), contactNormal );
    ProjectContactGeometry ( shapeBProjectedContactGeometry, shapeBContactGeometry, static_cast<GXUShort> ( totalShapeBContactGeometryPoints ), contactNormal );

    GXVec3 xAxis;
    xAxis.Substract ( shapeAProjectedContactGeometry[ 1u ], shapeAProjectedContactGeometry[ 0u ] );
    xAxis.Normalize ();

    GXVec3 yAxis;
    yAxis.CrossProduct ( contactNormal, xAxis );

    CalculatePlanarContactGeometryCoordinates ( shapeAPlanarContactGeometry, shapeAProjectedContactGeometry, static_cast<GXUShort> ( totalShapeAContactGeometryPoints ), xAxis, yAxis );
    CalculatePlanarContactGeometryCoordinates ( shapeBPlanarContactGeometry, shapeBProjectedContactGeometry, static_cast<GXUShort> ( totalShapeBContactGeometryPoints ), xAxis, yAxis );

    if ( totalShapeAContactGeometryPoints == 2u && totalShapeBContactGeometryPoints == 2u )
    {
        GXVec2 intersection2D;
        eGXLineRelationship lineRelationship = GXLineIntersection2D ( intersection2D, shapeAPlanarContactGeometry[ 0 ], shapeAPlanarContactGeometry[ 1 ], shapeBPlanarContactGeometry[ 0 ], shapeBPlanarContactGeometry[ 1 ] );

        GXContact* contact = collisionData.GetContactsBegin ();

        switch ( lineRelationship )
        {
            case eGXLineRelationship::NoIntersection:
            {
                GXUInt wtf = 0u;
                GXLogA ( "%u\n", wtf );
            }
            break;

            case eGXLineRelationship::Intersection:
            {
                GXVec3 shapeAContactGeometryCentroid;
                GetCentroid ( shapeAContactGeometryCentroid, shapeAContactGeometry, totalShapeAContactGeometryPoints );

                GXVec3 intersection;
                GetIntersectionGeometryCoordinates ( &intersection, &intersection2D, 1u, xAxis, yAxis );

                GXVec3 toShapeAContactGeometry;
                toShapeAContactGeometry.Substract ( shapeAContactGeometryCentroid, intersection );

                GXVec3 intersectionShapeAProjection;
                intersectionShapeAProjection.Sum ( intersection, contactNormal.DotProduct ( toShapeAContactGeometry ), contactNormal );

                contact->SetContactPoint ( intersectionShapeAProjection );
            }
            break;

            case eGXLineRelationship::Overlap:
            {
                GXUInt todo = 0u;
                GXLogA ( "%u\n", todo );
            }
            break;

            default:
                // NOTHING
            break;
        }

        contact->SetShapes ( *normalizedShapeA, *normalizedShapeB );

        normalizedShapeA->GetRigidBody ().SetAwake ();
        normalizedShapeB->GetRigidBody ().SetAwake ();

        contact->SetLinkedContacts ( 1u );
        contact->SetNormal ( contactNormal );
        contact->SetPenetration ( contactPenetration );
        contact->SetGJKIterations ( gjkIterations );
        contact->SetEPAIterations ( epaIterations );
        contact->SetSupportPoints ( totalSupportPoints );
        contact->SetEdges ( maximumEdgesUsed );
        contact->SetFaces ( totalFaces );

        collisionData.AddContacts ( 1u );

        if ( shapeAPlanarContactGeometryDebug )
            UpdateDebugData ( nullptr );

        return;
    }

    // Clipping planar shapes

    GXVec2* clipGeometry;
    GXUShort totalClipGeometryPoints;
    GXVec2* subjectGeometry;
    GXUInt totalSubjectGeometryPoints;

    if ( totalShapeAContactGeometryPoints == 2u )
    {
        clipGeometry = shapeBPlanarContactGeometry;
        totalClipGeometryPoints = static_cast<GXUShort> ( totalShapeBContactGeometryPoints );
        subjectGeometry = shapeAPlanarContactGeometry;
        totalSubjectGeometryPoints = totalShapeAContactGeometryPoints;
    }
    else
    {
        clipGeometry = shapeAPlanarContactGeometry;
        totalClipGeometryPoints = static_cast<GXUShort> ( totalShapeAContactGeometryPoints );
        subjectGeometry = shapeBPlanarContactGeometry;
        totalSubjectGeometryPoints = totalShapeBContactGeometryPoints;
    }

    GXVec2* clippedGeometry = nullptr;
    GetClippedPlanarContactGeometry ( &clippedGeometry, totalIntersectionPoints, clipGeometry, totalClipGeometryPoints, subjectGeometry, totalSubjectGeometryPoints );

    if ( shapeAPlanarContactGeometryDebug )
        UpdateDebugData ( clippedGeometry );

    // True contact points

    GXVec3* intersection = static_cast<GXVec3*> ( intersectionGeometry.GetData () );
    GetIntersectionGeometryCoordinates ( intersection, clippedGeometry, totalIntersectionPoints, xAxis, yAxis );

    GXVec3 shapeAContactGeometryCentroid;
    GXVec3 shapeBContactGeometryCentroid;

    GetCentroid ( shapeAContactGeometryCentroid, shapeAContactGeometry, totalShapeAContactGeometryPoints );
    GetCentroid ( shapeBContactGeometryCentroid, shapeBContactGeometry, totalShapeBContactGeometryPoints );

    GXContact* contact = collisionData.GetContactsBegin ();
    GXUInt contactIndex = 0u;

    GXVec3 reverseContactNormal ( contactNormal );
    reverseContactNormal.Reverse ();

    normalizedShapeA->GetRigidBody ().SetAwake ();
    normalizedShapeB->GetRigidBody ().SetAwake ();

    for ( GXUInt i = 0u; i < totalIntersectionPoints; ++i )
    {
        const GXVec3& intersectionPoint = intersection[ i ];

        GXVec3 toShapeAContactGeometry;
        toShapeAContactGeometry.Substract ( shapeAContactGeometryCentroid, intersectionPoint );

        GXVec3 toShapeBContactGeometry;
        toShapeBContactGeometry.Substract ( shapeBContactGeometryCentroid, intersectionPoint );

        GXVec3 intersectionShapeAProjection;
        intersectionShapeAProjection.Sum ( intersectionPoint, reverseContactNormal.DotProduct ( toShapeAContactGeometry ), reverseContactNormal );

        GXVec3 intersectionShapeBProjection;
        intersectionShapeBProjection.Sum ( intersectionPoint, reverseContactNormal.DotProduct ( toShapeBContactGeometry ), reverseContactNormal );

        GXVec3 checkDirection;
        checkDirection.Substract ( intersectionShapeAProjection, intersectionShapeBProjection );

        if ( checkDirection.DotProduct ( reverseContactNormal ) < 0.0f ) continue;

        contact[ contactIndex ].SetShapes ( *normalizedShapeA, *normalizedShapeB );
        contact[ contactIndex ].SetNormal ( contactNormal );
        contact[ contactIndex ].SetContactPoint ( intersectionShapeAProjection );
        contact[ contactIndex ].SetPenetration ( contactPenetration );
        contact[ contactIndex ].SetGJKIterations ( gjkIterations );
        contact[ contactIndex ].SetEPAIterations ( epaIterations );
        contact[ contactIndex ].SetSupportPoints ( totalSupportPoints );
        contact[ contactIndex ].SetEdges ( maximumEdgesUsed );
        contact[ contactIndex ].SetFaces ( totalFaces );

        ++contactIndex;
    }

    contact->SetLinkedContacts ( contactIndex );
    collisionData.AddContacts ( contactIndex );
}

GXBool GXCollisionDetector::GXCollisionDetector::DoesCollide ( const GXShape &shapeA, const GXShape &shapeB )
{
    GXSimplex simplex;
    GXUInt gjkIterations;

    return RunGJK ( simplex, gjkIterations, shapeA, shapeB );
}

GXVoid GXCollisionDetector::EnableDebugData ()
{
    if ( shapeAPlanarContactGeometryDebug ) return;

    GXUPointer size = totalDeviationAxes * sizeof ( GXVec3 );

    shapeAPlanarContactGeometryDebug = static_cast<GXVec3*> ( malloc ( size ) );
    shapeBPlanarContactGeometryDebug = static_cast<GXVec3*> ( malloc ( size ) );

    if ( planarIntersectionGeometry.GetLength () >= alphaPlanarIntersectionGeometry.GetLength () ) return;

    planarIntersectionGeometry.Resize ( alphaPlanarIntersectionGeometry.GetLength () );
}

GXVoid GXCollisionDetector::DisableDebugData ()
{
    if ( !shapeAPlanarContactGeometryDebug ) return;

    GXSafeFree ( shapeAPlanarContactGeometryDebug );
    GXSafeFree ( shapeBPlanarContactGeometryDebug );
}

GXVoid GXCollisionDetector::SetDeviationAxes ( GXUShort axes )
{
    if ( axes == totalDeviationAxes ) return;

    GXSafeFree ( deviationAxesLocal );

    GXUPointer sizeAlpha = axes * sizeof ( GXVec3 );

    deviationAxesLocal = static_cast<GXVec3*> ( malloc ( sizeAlpha ) );
    totalDeviationAxes = axes;

    GXSafeFree ( shapeAContactGeometry );
    GXSafeFree ( shapeBContactGeometry );
    GXSafeFree ( shapeAProjectedContactGeometry );
    GXSafeFree ( shapeBProjectedContactGeometry );

    shapeAContactGeometry = static_cast<GXVec3*> ( malloc ( sizeAlpha ) );
    shapeBContactGeometry = static_cast<GXVec3*> ( malloc ( sizeAlpha ) );
    shapeAProjectedContactGeometry = static_cast<GXVec3*> ( malloc ( sizeAlpha ) );
    shapeBProjectedContactGeometry = static_cast<GXVec3*> ( malloc ( sizeAlpha ) );

    GXUPointer sizeBetta = axes * sizeof ( GXVec2 );

    GXSafeFree ( shapeAPlanarContactGeometry );
    GXSafeFree ( shapeBPlanarContactGeometry );

    shapeAPlanarContactGeometry = static_cast<GXVec2*> ( malloc ( sizeBetta ) );
    shapeBPlanarContactGeometry = static_cast<GXVec2*> ( malloc ( sizeBetta ) );

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
    return shapeAPlanarContactGeometryDebug ? shapeAContactGeometry : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeAContactGeometryPoints () const
{
    return shapeAPlanarContactGeometryDebug ? totalShapeAContactGeometryPoints : 0u;
}

const GXVec3* GXCollisionDetector::GetShapeBContactGeometry () const
{
    return shapeAPlanarContactGeometryDebug ? shapeBContactGeometry : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeBContactGeometryPoints () const
{
    return shapeAPlanarContactGeometryDebug ? totalShapeBContactGeometryPoints : 0u;
}

const GXVec3* GXCollisionDetector::GetShapeAPlanarContactGeometry () const
{
    return shapeAPlanarContactGeometryDebug ? shapeAPlanarContactGeometryDebug : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeAPlanarContactGeometryPoints () const
{
    return shapeAPlanarContactGeometryDebug ? totalShapeAContactGeometryPoints : 0u;
}

const GXVec3* GXCollisionDetector::GetShapeBPlanarContactGeometry () const
{
    return shapeAPlanarContactGeometryDebug ? shapeBPlanarContactGeometryDebug : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeBPlanarContactGeometryPoints () const
{
    return shapeAPlanarContactGeometryDebug ? totalShapeBContactGeometryPoints : 0u;
}

const GXVec3* GXCollisionDetector::GetPlanarIntersectionGeometry () const
{
    return shapeAPlanarContactGeometryDebug ? static_cast<const GXVec3*> ( planarIntersectionGeometry.GetData () ) : nullptr;
}

GXUInt GXCollisionDetector::GetTotalPlanarIntersectionPoints () const
{
    return shapeAPlanarContactGeometryDebug ? totalIntersectionPoints : 0u;
}

GXUInt GXCollisionDetector::GetAllocatedSupportPoints () const
{
    return static_cast<GXUInt> ( supportPoints.GetLength () );
}

GXUInt GXCollisionDetector::GetAllocatedEdges () const
{
    return static_cast<GXUInt> ( edges.GetLength () );
}

GXUInt GXCollisionDetector::GetAllocatedFaces () const
{
    return static_cast<GXUInt> ( faces.GetLength () );
}

GXCollisionDetector::GXCollisionDetector ():
    supportPoints ( sizeof ( GXSupportPoint ) ),
    totalSupportPoints ( 0u ),
    edges ( sizeof ( GXEdge ) ),
    totalEdges ( 0u ),
    faces ( sizeof ( GXFace ) ),
    totalFaces ( 0u ),
    deviationAxesLocal ( nullptr ),
    totalDeviationAxes ( 0u ),
    deviationAngle ( DEFAULT_DEVIATION_ANGLE ),
    shapeAContactGeometry ( nullptr ),
    totalShapeAContactGeometryPoints ( 0u ),
    shapeBContactGeometry ( nullptr ),
    totalShapeBContactGeometryPoints ( 0u ),
    shapeAProjectedContactGeometry ( nullptr ),
    shapeBProjectedContactGeometry ( nullptr ),
    shapeAPlanarContactGeometry ( nullptr ),
    shapeAPlanarContactGeometryDebug ( nullptr ),
    shapeBPlanarContactGeometry ( nullptr ),
    shapeBPlanarContactGeometryDebug ( nullptr ),
    alphaPlanarIntersectionGeometry ( sizeof ( GXVec2 ) ),
    bettaPlanarIntersectionGeometry ( sizeof ( GXVec2 ) ),
    planarIntersectionGeometry ( sizeof ( GXVec3 ) ),
    intersectionGeometry ( sizeof ( GXVec3 ) ),
    totalIntersectionPoints ( 0u )
{
    supportPoints.Resize ( EPA_INITIAL_SUPPORT_POINT_ARRAY_CAPACITY );
    edges.Resize ( EPA_INITIAL_EDGE_ARRAY_CAPACITY );
    faces.Resize ( EPA_INITIAL_FACE_ARRAY_CAPACITY );
    alphaPlanarIntersectionGeometry.Resize ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY );
    bettaPlanarIntersectionGeometry.Resize ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY );
    planarIntersectionGeometry.Resize ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY );
    intersectionGeometry.Resize ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY );

    DisableDebugData ();

    SetDeviationAxes ( DEFAULT_DEVIATION_AXES );
}

GXBool GXCollisionDetector::RunGJK ( GXSimplex &simplex, GXUInt &usedIterations, const GXShape &shapeA, const GXShape &shapeB )
{
    static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );
    static const GXVec3 initialDirection ( 0.0f, 0.0f, 1.0f );

    GXSupportPoint supportPoint;
    GXVec3 direction;

    GXBool isLoop = GX_TRUE;
    GXBool isIntersected = GX_FALSE;
    usedIterations = 0u;

    CalculateSupportPoint ( supportPoint, shapeA, shapeB, initialDirection );

    simplex.PushPoint ( supportPoint );
    direction.Substract ( origin, supportPoint.difference );

    while ( isLoop )
    {
        ++usedIterations;

        CalculateSupportPoint ( supportPoint, shapeA, shapeB, direction );

        if ( supportPoint.difference.DotProduct ( direction ) < 0.0f )
        {
            isLoop = GX_FALSE;
            isIntersected = GX_FALSE;

            continue;
        }

        simplex.PushPoint ( supportPoint );

        switch ( simplex.numPoints )
        {
            case 2u:
            {
                GXVec3 simplexLineDirection;
                simplexLineDirection.Substract ( simplex.supportPoints[ 0 ].difference, simplex.supportPoints[ 1 ].difference );
                simplexLineDirection.Normalize ();

                GXVec3 toOrigin;
                toOrigin.Substract ( origin, simplex.supportPoints[ 1 ].difference );

                GXVec3 alpha;
                alpha.Project ( toOrigin, simplexLineDirection );

                GXVec3 betta;
                betta.Sum ( simplex.supportPoints[ 1 ].difference, alpha );

                direction.Substract ( origin, betta );
            }
            break;

            case 3u:
            {
                GXPlane simplexPlane;
                simplexPlane.From ( simplex.supportPoints[ 0 ].difference, simplex.supportPoints[ 1 ].difference, simplex.supportPoints[ 2 ].difference );

                switch ( simplexPlane.ClassifyVertex ( origin ) )
                {
                    case eGXPlaneClassifyVertex::On:
                    case eGXPlaneClassifyVertex::InFront:
                        direction.Init ( simplexPlane._a, simplexPlane._b, simplexPlane._c );
                    break;

                    case eGXPlaneClassifyVertex::Behind:
                        direction.Init ( -simplexPlane._a, -simplexPlane._b, -simplexPlane._c );
                    break;

                    default:
                        // NOTHING
                    break;
                }
            }
            break;

            case 4u:
            {
                isIntersected = GX_TRUE;

                for ( GXUByte i = 0u; i < 4u; ++i )
                {
                    GXPlane simplexPlane;
                    simplexPlane.From ( simplex.supportPoints[ simplex.faces[ i ].pointA ].difference, simplex.supportPoints[ simplex.faces[ i ].pointB ].difference, simplex.supportPoints[ simplex.faces[ i ].pointC ].difference );

                    if ( simplexPlane.ClassifyVertex ( origin ) != eGXPlaneClassifyVertex::Behind ) continue;

                    direction.Init ( -simplexPlane._a, -simplexPlane._b, -simplexPlane._c );

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
                // NOTHING
            break;
        }
    }

    if ( isIntersected ) return GX_TRUE;

    totalShapeAContactGeometryPoints = 0u;
    totalShapeBContactGeometryPoints = 0u;

    return GX_FALSE;
}

GXBool GXCollisionDetector::RunEPA ( GXVec3 &contactNormal, GXFloat &contactPenetration, GXUInt &usedEdges, GXUInt &usedIterations, const GXShape &shapeA, const GXShape &shapeB, GXSimplex &simplex )
{
    static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );

    GXSupportPoint* supportPointArray = static_cast<GXSupportPoint*> ( supportPoints.GetData () );
    GXEdge* edgeArray = static_cast<GXEdge*> ( edges.GetData () );
    GXFace* faceArray = static_cast<GXFace*> ( faces.GetData () );

    memcpy ( supportPointArray, simplex.supportPoints, 4 * sizeof ( GXSupportPoint ) );
    totalSupportPoints = 4u;

    totalFaces = 0u;

    GXFace& alphaFace = simplex.faces[ 0 ];
    GXPlane plane;
    plane.From ( supportPointArray[ alphaFace.pointA ].difference, supportPointArray[ alphaFace.pointB ].difference, supportPointArray[ alphaFace.pointC ].difference );
    alphaFace.normal.Init ( plane._a, plane._b, plane._c );
    alphaFace.next = INVALID_INDEX;
    alphaFace.prev = INVALID_INDEX;
    GXUInt lastFaceIndex = 0u;
    faceArray[ lastFaceIndex ] = alphaFace;
    ++totalFaces;

    for ( GXUInt i = 1u; i < 4u; ++i )
    {
        GXFace& bettaFace = simplex.faces[ i ];
        plane.From ( supportPointArray[ bettaFace.pointA ].difference, supportPointArray[ bettaFace.pointB ].difference, supportPointArray[ bettaFace.pointC ].difference );
        bettaFace.normal.Init ( plane._a, plane._b, plane._c );
        bettaFace.next = INVALID_INDEX;
        bettaFace.prev = lastFaceIndex;
        lastFaceIndex = totalFaces;
        faceArray[ lastFaceIndex ] = bettaFace;
        faceArray[ bettaFace.prev ].next = lastFaceIndex;
        ++totalFaces;
    }

    // contactNormal will contain direction to move FIRST body

    contactPenetration = FLT_MAX;
    usedIterations = 0u;
    usedEdges = 0u;

    while ( GX_TRUE )
    {
        ++usedIterations;

        GXFloat smallestDistance = FLT_MAX;
        GXUInt smallestDistanceFaceIndex = 0u;
        GXVec3 smallestDistanceFaceNormal;

        for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faceArray[ i ].prev )
        {
            const GXFace& yottaFace = faceArray[ i ];

            GXVec3 gamma;
            gamma.Substract ( origin, supportPointArray[ yottaFace.pointA ].difference );

            GXFloat newDistance = gamma.DotProduct ( yottaFace.normal );

            if ( newDistance >= smallestDistance ) continue;

            smallestDistance = newDistance;
            smallestDistanceFaceIndex = i;
            smallestDistanceFaceNormal = yottaFace.normal;
        }

        if ( fabsf ( smallestDistance - contactPenetration ) <= EPA_DISTANCE_EPSILON )
        {
            // Disable contacts with penetration depth equal 0.0f or bigger than half FLT_MAX

            if ( contactPenetration == 0.0f || contactPenetration > FLT_MAX * 0.5f )
            {
                totalShapeAContactGeometryPoints = 0u;
                totalShapeBContactGeometryPoints = 0u;

                return GX_FALSE;
            }

            contactPenetration = smallestDistance;
            contactNormal = smallestDistanceFaceNormal;

            break;
        }

        contactPenetration = smallestDistance;

        if ( supportPoints.GetLength () < totalSupportPoints + 1u )
        {
            supportPoints.Resize ( supportPoints.GetLength () + EPA_SUPPORT_POINTS_ALLOCATING_STEP );
            supportPointArray = static_cast<GXSupportPoint*> ( supportPoints.GetData () );
        }

        GXSupportPoint supportPoint;
        GXVec3 direction ( smallestDistanceFaceNormal );
        direction.Reverse ();

        CalculateSupportPoint ( supportPoint, shapeA, shapeB, direction );

        GXUInt newSupportPointIndex = totalSupportPoints;
        supportPointArray[ newSupportPointIndex ] = supportPoint;
        ++totalSupportPoints;

        GXUInt lastEdgeIndex = INVALID_INDEX;
        totalEdges = 0u;

        for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faceArray[ i ].prev )
        {
            GXFace& gammaFace = faceArray[ i ];

            GXVec3 omega;
            omega.Substract ( supportPointArray[ gammaFace.pointA ].difference, supportPoint.difference );

            if ( omega.DotProduct ( gammaFace.normal ) <= 0.0f ) continue;

            GXEdge edge1 = GXEdge ( gammaFace.pointA, gammaFace.pointB );
            ProceedEdge ( edge1, lastEdgeIndex, &edgeArray, totalEdges );

            GXEdge edge2 = GXEdge ( gammaFace.pointB, gammaFace.pointC );
            ProceedEdge ( edge2, lastEdgeIndex, &edgeArray, totalEdges );

            GXEdge edge3 = GXEdge ( gammaFace.pointC, gammaFace.pointA );
            ProceedEdge ( edge3, lastEdgeIndex, &edgeArray, totalEdges );

            if ( i == lastFaceIndex )
                lastFaceIndex = gammaFace.prev;

            if ( lastFaceIndex > 500u )
            {
                GXVec3 wtf;
            }

            if ( gammaFace.prev != INVALID_INDEX )
                faceArray[ gammaFace.prev ].next = gammaFace.next;

            if ( gammaFace.next != INVALID_INDEX )
                faceArray[ gammaFace.next ].prev = gammaFace.prev;
        }

        if ( totalEdges > usedEdges )
            usedEdges = totalEdges;

        for ( GXUInt i = lastEdgeIndex; i != INVALID_INDEX; i = edgeArray[ i ].prev )
        {
            const GXEdge& edge = edgeArray[ i ];

            plane.From ( supportPointArray[ newSupportPointIndex ].difference, supportPointArray[ edge.startPoint ].difference, supportPointArray[ edge.endPoint ].difference );
            GXFace omegaFace ( newSupportPointIndex, edge.startPoint, edge.endPoint, GXVec3 ( plane._a, plane._b, plane._c ) );

            if ( plane.ClassifyVertex ( origin ) == eGXPlaneClassifyVertex::Behind )
                omegaFace.ChangeOrder ();

            if ( faces.GetLength () < totalFaces + 1 )
            {
                faces.Resize ( faces.GetLength () + EPA_FACE_ALLOCATING_STEP );
                faceArray = static_cast<GXFace*> ( faces.GetData () );
            }

            omegaFace.next = INVALID_INDEX;
            omegaFace.prev = lastFaceIndex;
            lastFaceIndex = totalFaces;

            faceArray[ lastFaceIndex ] = omegaFace;
            faceArray[ omegaFace.prev ].next = lastFaceIndex;
            ++totalFaces;
        }
    }

    return GX_TRUE;
}

GXVoid GXCollisionDetector::CalculateSupportPoint ( GXSupportPoint &supportPoint, const GXShape &shapeA, const GXShape &shapeB, const GXVec3 &direction )
{
    GXVec3 extremeA;
    shapeA.GetExtremePoint ( extremeA, direction );

    GXVec3 reverseDirection ( direction );
    reverseDirection.Reverse ();
    GXVec3 extremeB;
    shapeB.GetExtremePoint ( extremeB, reverseDirection );

    supportPoint.difference.Substract ( extremeA, extremeB );
}

GXVoid GXCollisionDetector::ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edgeArrayPointer, GXUInt &totalEdgeNumber )
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

    if ( edges.GetLength () < totalEdgeNumber + 1 )
    {
        edges.Resize ( edges.GetLength () + EPA_EDGE_ALLOCATING_STEP );
        *edgeArrayPointer = static_cast<GXEdge*> ( edges.GetData () );
    }

    edge.next = INVALID_INDEX;
    edge.prev = lastEdgeIndex;
    lastEdgeIndex = totalEdgeNumber;
    edgeArray[ lastEdgeIndex ] = edge;

    if ( lastEdgeIndex > 0 )
        edgeArray[ edge.prev ].next = lastEdgeIndex;

    ++totalEdgeNumber;
}

GXVoid GXCollisionDetector::ProjectContactGeometry ( GXVec3* contactGeometryProjection, const GXVec3* contactGeometryWorld, GXUShort totalContactGeometryPoints, const GXVec3 &contactNormal )
{
    static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );

    for ( GXUShort i = 0u; i < totalContactGeometryPoints; ++i )
    {
        GXVec3 v;
        v.Substract ( origin, contactGeometryWorld[ i ] );

        GXVec3 offset;
        offset.Multiply ( contactNormal, contactNormal.DotProduct ( v ) );

        contactGeometryProjection[ i ].Sum ( contactGeometryWorld[ i ], offset );
    }
}

GXVoid GXCollisionDetector::CalculatePlanarContactGeometryCoordinates ( GXVec2* planarContactGeometry, const GXVec3* projectedContactGeometry, GXUShort totalContactGeometryPoints, const GXVec3 &xAxis, const GXVec3 &yAxis )
{
    static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );

    for ( GXUShort i = 0u; i < totalContactGeometryPoints; ++i )
    {
        GXVec3 v;
        v.Substract ( projectedContactGeometry[ i ], origin );

        planarContactGeometry[ i ].Init ( xAxis.DotProduct ( v ), yAxis.DotProduct ( v ) );
    }
}

GXVoid GXCollisionDetector::GetClippedPlanarContactGeometry ( GXVec2** clippedGeometryArrayPointer, GXUInt &totalPoints, const GXVec2* clipGeometry, GXUInt totalClipGeometryPoints, const GXVec2* subjectGeometry, GXUInt totalSubjectGeometryPoints )
{
    GXVec2* outputGeometry = static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () );
    GXUInt totalOutputPoints = totalSubjectGeometryPoints;

    memcpy ( outputGeometry, subjectGeometry, totalSubjectGeometryPoints * sizeof ( GXVec2 ) );

    GXUInt previousClipPointIndex = totalClipGeometryPoints - 1u;

    for ( GXUInt currentClipPointIndex = 0u; currentClipPointIndex < totalClipGeometryPoints; ++currentClipPointIndex )
    {
        GXVec2* inputGeometry = outputGeometry;
        GXUInt totalInputPoints = totalOutputPoints;

        outputGeometry = ( outputGeometry == (GXVec2*)alphaPlanarIntersectionGeometry.GetData () ) ? (GXVec2*)bettaPlanarIntersectionGeometry.GetData () : (GXVec2*)alphaPlanarIntersectionGeometry.GetData ();
        totalOutputPoints = 0u;

        GXVec2 clipEdgeNormal;
        clipEdgeNormal.CalculateNormalFast ( clipGeometry[ previousClipPointIndex ], clipGeometry[ currentClipPointIndex ] );

        GXUInt previousPointIndex = totalInputPoints - 1u;

        for ( GXUInt currentPointIndex = 0u; currentPointIndex < totalInputPoints; ++currentPointIndex )
        {
            GXVec2 toCurrentPoint;
            toCurrentPoint.Substract ( inputGeometry[ currentPointIndex ], clipGeometry[ previousClipPointIndex ] );

            GXVec2 toPreviousPoint;
            toPreviousPoint.Substract ( inputGeometry[ previousPointIndex ], clipGeometry[ previousClipPointIndex ] );

            if ( clipEdgeNormal.DotProduct ( toPreviousPoint ) >= 0.0f )
            {
                if ( clipEdgeNormal.DotProduct ( toCurrentPoint ) < 0.0f )
                {
                    GXVec2 intersectionPoint;
                    eGXLineRelationship lineRelationship = GXLineIntersection2D ( intersectionPoint, inputGeometry[ previousPointIndex ], inputGeometry[ currentPointIndex ], clipGeometry[ previousClipPointIndex ], clipGeometry[ currentClipPointIndex ] );

                    switch ( lineRelationship )
                    {
                        case eGXLineRelationship::NoIntersection:
                        {
                            GXUInt wtf = 0;
                            GXLogA ( "%u\n", wtf );
                        }
                        break;

                        case eGXLineRelationship::Intersection:
                        {
                            if ( alphaPlanarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
                            {
                                GXUInt newSize = totalOutputPoints + PLANAR_INTERSECTION_ALLOCATING_STEP;

                                if ( shapeAPlanarContactGeometryDebug && planarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
                                    planarIntersectionGeometry.Resize ( newSize );

                                intersectionGeometry.Resize ( newSize );
                                alphaPlanarIntersectionGeometry.Resize ( newSize );
                                bettaPlanarIntersectionGeometry.Resize ( newSize );

                                if ( outputGeometry == static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () ) )
                                {
                                    outputGeometry = static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () );
                                    inputGeometry = static_cast<GXVec2*> ( bettaPlanarIntersectionGeometry.GetData () );
                                }
                                else
                                {
                                    outputGeometry = static_cast<GXVec2*> ( bettaPlanarIntersectionGeometry.GetData () );
                                    inputGeometry = static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () );
                                }
                            }

                            outputGeometry[ totalOutputPoints ] = intersectionPoint;
                            ++totalOutputPoints;
                        }
                        break;

                        case eGXLineRelationship::Overlap:
                        {
                            GXUInt wtf = 0;
                            GXLogA ( "%u\n", wtf );
                        }
                        break;

                        default:
                            // NOTHING
                        break;
                    }
                }
                else
                {
                    if ( alphaPlanarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
                    {
                        GXUInt newSize = totalOutputPoints + PLANAR_INTERSECTION_ALLOCATING_STEP;

                        if ( shapeAPlanarContactGeometryDebug && planarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
                            planarIntersectionGeometry.Resize ( newSize );

                        intersectionGeometry.Resize ( newSize );
                        alphaPlanarIntersectionGeometry.Resize ( newSize );
                        bettaPlanarIntersectionGeometry.Resize ( newSize );

                        if ( outputGeometry == static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () ) )
                        {
                            outputGeometry = static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () );
                            inputGeometry = static_cast<GXVec2*> ( bettaPlanarIntersectionGeometry.GetData () );
                        }
                        else
                        {
                            outputGeometry = static_cast<GXVec2*> ( bettaPlanarIntersectionGeometry.GetData () );
                            inputGeometry = static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () );
                        }
                    }

                    outputGeometry[ totalOutputPoints ] = inputGeometry[ currentPointIndex ];
                    ++totalOutputPoints;
                }
            }
            else if ( clipEdgeNormal.DotProduct ( toCurrentPoint ) >= 0.0f )
            {
                GXVec2 intersectionPoint;
                eGXLineRelationship lineRelationship = GXLineIntersection2D ( intersectionPoint, inputGeometry[ previousPointIndex ], inputGeometry[ currentPointIndex ], clipGeometry[ previousClipPointIndex ], clipGeometry[ currentClipPointIndex ] );

                switch ( lineRelationship )
                {
                    case eGXLineRelationship::NoIntersection:
                    {
                        GXUInt wtf = 0u;
                        GXLogA ( "%u\n", wtf );
                    }
                    break;

                    case eGXLineRelationship::Intersection:
                    {
                        if ( alphaPlanarIntersectionGeometry.GetLength () < totalOutputPoints + 2 )
                        {
                            GXUInt newSize = totalOutputPoints + PLANAR_INTERSECTION_ALLOCATING_STEP;

                            if ( shapeAPlanarContactGeometryDebug && planarIntersectionGeometry.GetLength () < totalOutputPoints + 2 )
                                planarIntersectionGeometry.Resize ( newSize );

                            intersectionGeometry.Resize ( newSize );
                            alphaPlanarIntersectionGeometry.Resize ( newSize );
                            bettaPlanarIntersectionGeometry.Resize ( newSize );

                            if ( outputGeometry == static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () ) )
                            {
                                outputGeometry = static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () );
                                inputGeometry = static_cast<GXVec2*> ( bettaPlanarIntersectionGeometry.GetData () );
                            }
                            else
                            {
                                outputGeometry = static_cast<GXVec2*> ( bettaPlanarIntersectionGeometry.GetData () );
                                inputGeometry = static_cast<GXVec2*> ( alphaPlanarIntersectionGeometry.GetData () );
                            }
                        }

                        outputGeometry[ totalOutputPoints ] = intersectionPoint;
                        ++totalOutputPoints;

                        outputGeometry[ totalOutputPoints ] = inputGeometry[ currentPointIndex ];
                        ++totalOutputPoints;
                    }
                    break;

                    case eGXLineRelationship::Overlap:
                    {
                        GXUInt wtf = 0u;
                        GXLogA ( "%u\n", wtf );
                    }
                    break;

                    default:
                        // NOTHING
                    break;
                }
            }

            previousPointIndex = currentPointIndex;
        }

        previousClipPointIndex = currentClipPointIndex;
    }

    *clippedGeometryArrayPointer = outputGeometry;
    totalPoints = totalOutputPoints;
}

GXVoid GXCollisionDetector::GetIntersectionGeometryCoordinates ( GXVec3* out, const GXVec2* planarIntersectionGeometryArray, GXUInt totalIntersectionPointNumber, const GXVec3 &xAxis, const GXVec3 &yAxis )
{
    for ( GXUInt i = 0u; i < totalIntersectionPointNumber; ++i )
    {
        out[ i ].Multiply ( xAxis, planarIntersectionGeometryArray[ i ]._data[ 0 ] );

        GXVec3 alpha;
        alpha.Multiply ( yAxis, planarIntersectionGeometryArray[ i ]._data[ 1 ] );

        out[ i ].Sum ( out[ i ], alpha );
    }
}

GXVoid GXCollisionDetector::GetCentroid ( GXVec3 &centroid, const GXVec3* points, GXUInt totalPoints )
{
    GXVec3 sum ( 0.0f, 0.0f, 0.0f );

    for ( GXUInt i = 0u; i < totalPoints; ++i )
        sum.Sum ( sum, points[ i ] );

    centroid.Multiply ( sum, 1.0f / static_cast<GXFloat> ( totalPoints ) );
}

GXVoid GXCollisionDetector::UpdateDeviationAxes ()
{
    GXFloat step = GX_MATH_DOUBLE_PI / (GXFloat)totalDeviationAxes;
    GXFloat limit = GX_MATH_DOUBLE_PI - 0.5f * step;
    GXUShort axisIndex = 0u;
    GXFloat deviation = sinf ( deviationAngle );
    GXVec3 deviationVector ( deviation, 0.0f, 1.0f );
    GXFloat normalizeScale = 1.0f / deviationVector.Length ();

    for ( GXFloat angle = 0.0f; angle < limit; angle += step )
    {
        deviationAxesLocal[ axisIndex ].Init ( cosf ( angle ) * deviation, sinf ( angle ) * deviation, 1.0f );
        deviationAxesLocal[ axisIndex ].Multiply ( deviationAxesLocal[ axisIndex ], normalizeScale );
        ++axisIndex;
    }
}

GXVoid GXCollisionDetector::UpdateDebugData ( const GXVec2* planarIntersectionGeometryArray )
{
    for ( GXUInt i = 0u; i < totalShapeAContactGeometryPoints; ++i )
        shapeAPlanarContactGeometryDebug[ i ].Init ( shapeAPlanarContactGeometry[ i ]._data[ 0 ], shapeAPlanarContactGeometry[ i ]._data[ 1 ], 0.0f );

    for ( GXUInt i = 0u; i < totalShapeBContactGeometryPoints; ++i )
        shapeBPlanarContactGeometryDebug[ i ].Init ( shapeBPlanarContactGeometry[ i ]._data[ 0 ], shapeBPlanarContactGeometry[ i ]._data[ 1 ], 0.0f );

    if ( !planarIntersectionGeometryArray ) return;

    for ( GXUInt i = 0u; i < totalIntersectionPoints; ++i )
    {
        GXVec3 v ( planarIntersectionGeometryArray[ i ]._data[ 0 ], planarIntersectionGeometryArray[ i ]._data[ 1 ], 0.0f );
        planarIntersectionGeometry.SetValue ( i, &v );
    }
}

GXVoid GXCollisionDetector::NormalizeShapes ( const GXShape** normalizedShapeA, const GXShape** normalizedShapeB, const GXShape &shapeA, const GXShape &shapeB ) const
{
    if ( shapeB.GetRigidBody ().IsKinematic () )
    {
        *normalizedShapeA = &shapeA;
        *normalizedShapeB = &shapeB;
        return;
    }

    if ( shapeA.GetRigidBody ().IsKinematic () || shapeA.GetRigidBody ().GetMass () > shapeB.GetRigidBody ().GetMass () )
    {
        *normalizedShapeA = &shapeB;
        *normalizedShapeB = &shapeA;
        return;
    }

    *normalizedShapeA = &shapeA;
    *normalizedShapeB = &shapeB;
}
