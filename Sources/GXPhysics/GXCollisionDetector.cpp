// version 1.7

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
    GXVec3      _difference;

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
    GXUInt      _previous;
    GXUInt      _next;

    GXUInt      _startPoint;
    GXUInt      _endPoint;

    GXEdge ();
    explicit GXEdge ( GXUInt startPoint, GXUInt endPoint );

    static GXBool IsOpposite ( const GXEdge &alpha, const GXEdge &betta );

    GXEdge& operator = ( const GXEdge &other );
};

GXEdge::GXEdge ():
    _previous ( INVALID_INDEX ),
    _next ( INVALID_INDEX ),
    _startPoint ( INVALID_INDEX ),
    _endPoint ( INVALID_INDEX )
{
    // NOTHING
}

GXEdge::GXEdge ( GXUInt startPoint, GXUInt endPoint ):
    _previous ( INVALID_INDEX ),
    _next ( INVALID_INDEX ),
    _startPoint ( startPoint ),
    _endPoint ( endPoint )
{
    // NOTHING
}

GXBool GXEdge::IsOpposite ( const GXEdge &alpha, const GXEdge &betta )
{
    return ( alpha._startPoint == betta._endPoint && alpha._endPoint == betta._startPoint );
}

GXEdge& GXEdge::operator = ( const GXEdge &other )
{
    memcpy ( this, &other, sizeof ( GXEdge ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

struct GXFace final
{
    GXUInt      _next;
    GXUInt      _previous;

    GXUInt      _pointA;
    GXUInt      _pointB;
    GXUInt      _pointC;

    GXVec3      _normal;

    GXFace ();
    explicit GXFace ( GXUInt pointA, GXUInt pointB, GXUInt pointC, const GXVec3 &normal );

    GXVoid ChangeOrder ();

    GXFace& operator = ( const GXFace &other );
};

GXFace::GXFace ():
    _next ( INVALID_INDEX ),
    _previous ( INVALID_INDEX ),
    _pointA ( INVALID_INDEX ), 
    _pointB ( INVALID_INDEX ),
    _pointC ( INVALID_INDEX ),
    _normal ( 0.0f, 0.0f, 0.0f )
{
    // NOTHING
}

GXFace::GXFace ( GXUInt pointA, GXUInt pointB, GXUInt pointC, const GXVec3 &normal ):
    _next ( INVALID_INDEX ),
    _previous ( INVALID_INDEX ),
    _pointA ( pointA ),
    _pointB ( pointB ),
    _pointC ( pointC ),
    _normal ( normal )
{
    // NOTHING
}

GXVoid GXFace::ChangeOrder ()
{
    const GXUInt tmp = _pointA;
    _pointA = _pointB;
    _pointB = tmp;

    _normal.Reverse ();
}

GXFace& GXFace::operator = ( const GXFace &other )
{
    memcpy ( this, &other, sizeof ( GXFace ) );
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------

struct GXSimplex final
{
    GXUByte             _numPoints;
    GXSupportPoint      _supportPoints[ 4u ];
    GXFace              _faces[ 4u ];

    GXSimplex ();

    GXVoid PushPoint ( const GXSupportPoint &point );
    GXVoid Reset ();
};

GXSimplex::GXSimplex ():
    _numPoints ( 0u )
{
    // NOTHING
}

GXVoid GXSimplex::PushPoint ( const GXSupportPoint &point )
{
    memmove ( _supportPoints + 1u, _supportPoints, 3u * sizeof ( GXSupportPoint ) );
    _supportPoints[ 0u ] = point;
    ++_numPoints;

    if ( _numPoints < 4u ) return;

    _faces[ 0u ]._pointA = 0u;
    _faces[ 0u ]._pointB = 1u;
    _faces[ 0u ]._pointC = 2u;

    GXPlane plane;
    plane.From ( _supportPoints[ _faces[ 0u ]._pointA ]._difference, _supportPoints[ _faces[ 0u ]._pointB ]._difference, _supportPoints[ _faces[ 0u ]._pointC ]._difference );

    if ( plane.ClassifyVertex ( _supportPoints[ 3 ]._difference ) == eGXPlaneClassifyVertex::Behind )
        _faces[ 0u ].ChangeOrder ();

    _faces[ 1u ]._pointA = 0u;
    _faces[ 1u ]._pointB = 2u;
    _faces[ 1u ]._pointC = 3u;
    plane.From ( _supportPoints[ _faces[ 1u ]._pointA ]._difference, _supportPoints[ _faces[ 1u ]._pointB ]._difference, _supportPoints[ _faces[ 1u ]._pointC ]._difference );

    if ( plane.ClassifyVertex ( _supportPoints[ 1 ]._difference ) == eGXPlaneClassifyVertex::Behind )
        _faces[ 1u ].ChangeOrder ();

    _faces[ 2u ]._pointA = 0u;
    _faces[ 2u ]._pointB = 3u;
    _faces[ 2u ]._pointC = 1u;
    plane.From ( _supportPoints[ _faces[ 2u ]._pointA ]._difference, _supportPoints[ _faces[ 2u ]._pointB ]._difference, _supportPoints[ _faces[ 2u ]._pointC ]._difference );

    if ( plane.ClassifyVertex ( _supportPoints[ 2u ]._difference ) == eGXPlaneClassifyVertex::Behind )
        _faces[ 2u ].ChangeOrder ();

    _faces[ 3u ]._pointA = 1u;
    _faces[ 3u ]._pointB = 3u;
    _faces[ 3u ]._pointC = 2u;
    plane.From ( _supportPoints[ _faces[ 3u ]._pointA ]._difference, _supportPoints[ _faces[ 3u ]._pointB ]._difference, _supportPoints[ _faces[ 3u ]._pointC ]._difference );

    if ( plane.ClassifyVertex ( _supportPoints[ 0u ]._difference ) == eGXPlaneClassifyVertex::Behind )
        _faces[ 3u ].ChangeOrder ();
}

GXVoid GXSimplex::Reset ()
{
    _numPoints = 0u;
}

//---------------------------------------------------------------------------------------------------------------------

GXCollisionDetector* GXCollisionDetector::_instance = nullptr;

GXCollisionDetector& GXCALL GXCollisionDetector::GetInstance ()
{
    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXCollisionDetector" );
        _instance = new GXCollisionDetector ();
    }

    return *_instance;
}

GXCollisionDetector::~GXCollisionDetector ()
{
    Free ( _deviationAxesLocal );
    Free ( _shapeAContactGeometry );
    Free ( _shapeBContactGeometry );
    Free ( _shapeAProjectedContactGeometry );
    Free ( _shapeBProjectedContactGeometry );
    Free ( _shapeAPlanarContactGeometry );
    Free ( _shapeBPlanarContactGeometry );

    _instance = nullptr;

    if ( !_shapeAPlanarContactGeometryDebug ) return;

    Free ( _shapeAPlanarContactGeometryDebug );
    Free ( _shapeBPlanarContactGeometryDebug );
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
        contact->SetSupportPoints ( _totalSupportPoints );
        contact->SetEdges ( maximumEdgesUsed );
        contact->SetFaces ( _totalFaces );

        collisionData.AddContacts ( 1u );

        _totalIntersectionPoints = 0u;
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

    _totalShapeAContactGeometryPoints = _totalShapeBContactGeometryPoints = 0u;

    for ( GXUShort i = 0u; i < _totalDeviationAxes; ++i )
    {
        GXVec3 direction;
        tbn.MultiplyVectorMatrix ( direction, _deviationAxesLocal[ i ] );

        GXVec3 shapeBPoint;
        normalizedShapeB->GetExtremePoint ( shapeBPoint, direction );

        GXVec3 shapeAPoint;
        direction.Reverse ();
        normalizedShapeA->GetExtremePoint ( shapeAPoint, direction );

        if ( !lastShapeAPoint.IsEqual ( shapeAPoint ) )
        {
            _shapeAContactGeometry[ _totalShapeAContactGeometryPoints ] = shapeAPoint;
            lastShapeAPoint = shapeAPoint;
            ++_totalShapeAContactGeometryPoints;
        }

        if ( !lastShapeBPoint.IsEqual ( shapeBPoint ) )
        {
            _shapeBContactGeometry[ _totalShapeBContactGeometryPoints ] = shapeBPoint;
            lastShapeBPoint = shapeBPoint;
            ++_totalShapeBContactGeometryPoints;
        }
    }

    if ( _totalShapeAContactGeometryPoints == 1u || _totalShapeBContactGeometryPoints == 1u )
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
        contact->SetSupportPoints ( _totalSupportPoints );
        contact->SetEdges ( maximumEdgesUsed );
        contact->SetFaces ( _totalFaces );

        if ( _totalShapeAContactGeometryPoints == 1u )
            contact->SetContactPoint ( _shapeAContactGeometry[ 0u ] );
        else
            contact->SetContactPoint ( _shapeBContactGeometry[ 0u ] );

        collisionData.AddContacts ( 1u );

        _totalIntersectionPoints = 0u;

        return;
    }

    if ( lastShapeAPoint.IsEqual ( _shapeAContactGeometry[ 0u ] ) )
        --_totalShapeAContactGeometryPoints;

    if ( lastShapeBPoint.IsEqual ( _shapeBContactGeometry[ 0u ] ) )
        --_totalShapeBContactGeometryPoints;

    // Projecting contact geometry onto a plane with normal "contactNormal" and origin at ( 0.0f, 0.0f, 0.0f )

    ProjectContactGeometry ( _shapeAProjectedContactGeometry, _shapeAContactGeometry, static_cast<GXUShort> ( _totalShapeAContactGeometryPoints ), contactNormal );
    ProjectContactGeometry ( _shapeBProjectedContactGeometry, _shapeBContactGeometry, static_cast<GXUShort> ( _totalShapeBContactGeometryPoints ), contactNormal );

    GXVec3 xAxis;
    xAxis.Substract ( _shapeAProjectedContactGeometry[ 1u ], _shapeAProjectedContactGeometry[ 0u ] );
    xAxis.Normalize ();

    GXVec3 yAxis;
    yAxis.CrossProduct ( contactNormal, xAxis );

    CalculatePlanarContactGeometryCoordinates ( _shapeAPlanarContactGeometry, _shapeAProjectedContactGeometry, static_cast<GXUShort> ( _totalShapeAContactGeometryPoints ), xAxis, yAxis );
    CalculatePlanarContactGeometryCoordinates ( _shapeBPlanarContactGeometry, _shapeBProjectedContactGeometry, static_cast<GXUShort> ( _totalShapeBContactGeometryPoints ), xAxis, yAxis );

    if ( _totalShapeAContactGeometryPoints == 2u && _totalShapeBContactGeometryPoints == 2u )
    {
        GXVec2 intersection2D;
        eGXLineRelationship lineRelationship = GXLineIntersection2D ( intersection2D, _shapeAPlanarContactGeometry[ 0 ], _shapeAPlanarContactGeometry[ 1 ], _shapeBPlanarContactGeometry[ 0 ], _shapeBPlanarContactGeometry[ 1 ] );

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
                GetCentroid ( shapeAContactGeometryCentroid, _shapeAContactGeometry, _totalShapeAContactGeometryPoints );

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
        contact->SetSupportPoints ( _totalSupportPoints );
        contact->SetEdges ( maximumEdgesUsed );
        contact->SetFaces ( _totalFaces );

        collisionData.AddContacts ( 1u );

        if ( _shapeAPlanarContactGeometryDebug )
            UpdateDebugData ( nullptr );

        return;
    }

    // Clipping planar shapes

    GXVec2* clipGeometry;
    GXUShort totalClipGeometryPoints;
    GXVec2* subjectGeometry;
    GXUInt totalSubjectGeometryPoints;

    if ( _totalShapeAContactGeometryPoints == 2u )
    {
        clipGeometry = _shapeBPlanarContactGeometry;
        totalClipGeometryPoints = static_cast<GXUShort> ( _totalShapeBContactGeometryPoints );
        subjectGeometry = _shapeAPlanarContactGeometry;
        totalSubjectGeometryPoints = _totalShapeAContactGeometryPoints;
    }
    else
    {
        clipGeometry = _shapeAPlanarContactGeometry;
        totalClipGeometryPoints = static_cast<GXUShort> ( _totalShapeAContactGeometryPoints );
        subjectGeometry = _shapeBPlanarContactGeometry;
        totalSubjectGeometryPoints = _totalShapeBContactGeometryPoints;
    }

    GXVec2* clippedGeometry = nullptr;
    GetClippedPlanarContactGeometry ( &clippedGeometry, _totalIntersectionPoints, clipGeometry, totalClipGeometryPoints, subjectGeometry, totalSubjectGeometryPoints );

    if ( _shapeAPlanarContactGeometryDebug )
        UpdateDebugData ( clippedGeometry );

    // True contact points

    GXVec3* intersection = static_cast<GXVec3*> ( _intersectionGeometry.GetData () );
    GetIntersectionGeometryCoordinates ( intersection, clippedGeometry, _totalIntersectionPoints, xAxis, yAxis );

    GXVec3 shapeAContactGeometryCentroid;
    GXVec3 shapeBContactGeometryCentroid;

    GetCentroid ( shapeAContactGeometryCentroid, _shapeAContactGeometry, _totalShapeAContactGeometryPoints );
    GetCentroid ( shapeBContactGeometryCentroid, _shapeBContactGeometry, _totalShapeBContactGeometryPoints );

    GXContact* contact = collisionData.GetContactsBegin ();
    GXUInt contactIndex = 0u;

    GXVec3 reverseContactNormal ( contactNormal );
    reverseContactNormal.Reverse ();

    normalizedShapeA->GetRigidBody ().SetAwake ();
    normalizedShapeB->GetRigidBody ().SetAwake ();

    for ( GXUInt i = 0u; i < _totalIntersectionPoints; ++i )
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
        contact[ contactIndex ].SetSupportPoints ( _totalSupportPoints );
        contact[ contactIndex ].SetEdges ( maximumEdgesUsed );
        contact[ contactIndex ].SetFaces ( _totalFaces );

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
    if ( _shapeAPlanarContactGeometryDebug ) return;

    const GXUPointer size = _totalDeviationAxes * sizeof ( GXVec3 );

    _shapeAPlanarContactGeometryDebug = static_cast<GXVec3*> ( Malloc ( size ) );
    _shapeBPlanarContactGeometryDebug = static_cast<GXVec3*> ( Malloc ( size ) );

    if ( _planarIntersectionGeometry.GetLength () >= _alphaPlanarIntersectionGeometry.GetLength () ) return;

    _planarIntersectionGeometry.Resize ( _alphaPlanarIntersectionGeometry.GetLength () );
}

GXVoid GXCollisionDetector::DisableDebugData ()
{
    if ( !_shapeAPlanarContactGeometryDebug ) return;

    GXSafeFree ( _shapeAPlanarContactGeometryDebug );
    GXSafeFree ( _shapeBPlanarContactGeometryDebug );
}

GXVoid GXCollisionDetector::SetDeviationAxes ( GXUShort axes )
{
    if ( axes == _totalDeviationAxes ) return;

    GXSafeFree ( _deviationAxesLocal );

    const GXUPointer sizeAlpha = axes * sizeof ( GXVec3 );

    _deviationAxesLocal = static_cast<GXVec3*> ( Malloc ( sizeAlpha ) );
    _totalDeviationAxes = axes;

    GXSafeFree ( _shapeAContactGeometry );
    GXSafeFree ( _shapeBContactGeometry );
    GXSafeFree ( _shapeAProjectedContactGeometry );
    GXSafeFree ( _shapeBProjectedContactGeometry );

    _shapeAContactGeometry = static_cast<GXVec3*> ( Malloc ( sizeAlpha ) );
    _shapeBContactGeometry = static_cast<GXVec3*> ( Malloc ( sizeAlpha ) );
    _shapeAProjectedContactGeometry = static_cast<GXVec3*> ( Malloc ( sizeAlpha ) );
    _shapeBProjectedContactGeometry = static_cast<GXVec3*> ( Malloc ( sizeAlpha ) );

    const GXUPointer sizeBetta = axes * sizeof ( GXVec2 );

    GXSafeFree ( _shapeAPlanarContactGeometry );
    GXSafeFree ( _shapeBPlanarContactGeometry );

    _shapeAPlanarContactGeometry = static_cast<GXVec2*> ( Malloc ( sizeBetta ) );
    _shapeBPlanarContactGeometry = static_cast<GXVec2*> ( Malloc ( sizeBetta ) );

    UpdateDeviationAxes ();
}

GXUShort GXCollisionDetector::GetDeviationAxes () const
{
    return _totalDeviationAxes;
}

GXVoid GXCollisionDetector::SetDeviationAngle ( GXFloat radians )
{
    if ( _deviationAngle == radians ) return;

    _deviationAngle = radians;
    UpdateDeviationAxes ();
}

GXFloat GXCollisionDetector::GetDeviationAngle () const
{
    return _deviationAngle;
}

const GXVec3* GXCollisionDetector::GetShapeAContactGeometry () const
{
    return _shapeAPlanarContactGeometryDebug ? _shapeAContactGeometry : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeAContactGeometryPoints () const
{
    return _shapeAPlanarContactGeometryDebug ? _totalShapeAContactGeometryPoints : 0u;
}

const GXVec3* GXCollisionDetector::GetShapeBContactGeometry () const
{
    return _shapeAPlanarContactGeometryDebug ? _shapeBContactGeometry : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeBContactGeometryPoints () const
{
    return _shapeAPlanarContactGeometryDebug ? _totalShapeBContactGeometryPoints : 0u;
}

const GXVec3* GXCollisionDetector::GetShapeAPlanarContactGeometry () const
{
    return _shapeAPlanarContactGeometryDebug ? _shapeAPlanarContactGeometryDebug : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeAPlanarContactGeometryPoints () const
{
    return _shapeAPlanarContactGeometryDebug ? _totalShapeAContactGeometryPoints : 0u;
}

const GXVec3* GXCollisionDetector::GetShapeBPlanarContactGeometry () const
{
    return _shapeAPlanarContactGeometryDebug ? _shapeBPlanarContactGeometryDebug : nullptr;
}

GXUInt GXCollisionDetector::GetTotalShapeBPlanarContactGeometryPoints () const
{
    return _shapeAPlanarContactGeometryDebug ? _totalShapeBContactGeometryPoints : 0u;
}

const GXVec3* GXCollisionDetector::GetPlanarIntersectionGeometry () const
{
    return _shapeAPlanarContactGeometryDebug ? static_cast<const GXVec3*> ( _planarIntersectionGeometry.GetData () ) : nullptr;
}

GXUInt GXCollisionDetector::GetTotalPlanarIntersectionPoints () const
{
    return _shapeAPlanarContactGeometryDebug ? _totalIntersectionPoints : 0u;
}

GXUInt GXCollisionDetector::GetAllocatedSupportPoints () const
{
    return static_cast<GXUInt> ( _supportPoints.GetLength () );
}

GXUInt GXCollisionDetector::GetAllocatedEdges () const
{
    return static_cast<GXUInt> ( _edges.GetLength () );
}

GXUInt GXCollisionDetector::GetAllocatedFaces () const
{
    return static_cast<GXUInt> ( _faces.GetLength () );
}

GXCollisionDetector::GXCollisionDetector ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXCollisionDetector" )
    _supportPoints ( sizeof ( GXSupportPoint ) ),
    _totalSupportPoints ( 0u ),
    _edges ( sizeof ( GXEdge ) ),
    _totalEdges ( 0u ),
    _faces ( sizeof ( GXFace ) ),
    _totalFaces ( 0u ),
    _deviationAxesLocal ( nullptr ),
    _totalDeviationAxes ( 0u ),
    _deviationAngle ( DEFAULT_DEVIATION_ANGLE ),
    _shapeAContactGeometry ( nullptr ),
    _totalShapeAContactGeometryPoints ( 0u ),
    _shapeBContactGeometry ( nullptr ),
    _totalShapeBContactGeometryPoints ( 0u ),
    _shapeAProjectedContactGeometry ( nullptr ),
    _shapeBProjectedContactGeometry ( nullptr ),
    _shapeAPlanarContactGeometry ( nullptr ),
    _shapeAPlanarContactGeometryDebug ( nullptr ),
    _shapeBPlanarContactGeometry ( nullptr ),
    _shapeBPlanarContactGeometryDebug ( nullptr ),
    _alphaPlanarIntersectionGeometry ( sizeof ( GXVec2 ) ),
    _bettaPlanarIntersectionGeometry ( sizeof ( GXVec2 ) ),
    _planarIntersectionGeometry ( sizeof ( GXVec3 ) ),
    _intersectionGeometry ( sizeof ( GXVec3 ) ),
    _totalIntersectionPoints ( 0u )
{
    _supportPoints.Resize ( EPA_INITIAL_SUPPORT_POINT_ARRAY_CAPACITY );
    _edges.Resize ( EPA_INITIAL_EDGE_ARRAY_CAPACITY );
    _faces.Resize ( EPA_INITIAL_FACE_ARRAY_CAPACITY );
    _alphaPlanarIntersectionGeometry.Resize ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY );
    _bettaPlanarIntersectionGeometry.Resize ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY );
    _planarIntersectionGeometry.Resize ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY );
    _intersectionGeometry.Resize ( INITIAL_PLANAR_INTERSECTION_ARRAY_CAPACITY );

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
    direction.Substract ( origin, supportPoint._difference );

    while ( isLoop )
    {
        ++usedIterations;

        CalculateSupportPoint ( supportPoint, shapeA, shapeB, direction );

        if ( supportPoint._difference.DotProduct ( direction ) < 0.0f )
        {
            isLoop = GX_FALSE;
            isIntersected = GX_FALSE;

            continue;
        }

        simplex.PushPoint ( supportPoint );

        switch ( simplex._numPoints )
        {
            case 2u:
            {
                GXVec3 simplexLineDirection;
                simplexLineDirection.Substract ( simplex._supportPoints[ 0 ]._difference, simplex._supportPoints[ 1 ]._difference );
                simplexLineDirection.Normalize ();

                GXVec3 toOrigin;
                toOrigin.Substract ( origin, simplex._supportPoints[ 1 ]._difference );

                GXVec3 alpha;
                alpha.Project ( toOrigin, simplexLineDirection );

                GXVec3 betta;
                betta.Sum ( simplex._supportPoints[ 1 ]._difference, alpha );

                direction.Substract ( origin, betta );
            }
            break;

            case 3u:
            {
                GXPlane simplexPlane;
                simplexPlane.From ( simplex._supportPoints[ 0 ]._difference, simplex._supportPoints[ 1 ]._difference, simplex._supportPoints[ 2 ]._difference );

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
                    simplexPlane.From ( simplex._supportPoints[ simplex._faces[ i ]._pointA ]._difference, simplex._supportPoints[ simplex._faces[ i ]._pointB ]._difference, simplex._supportPoints[ simplex._faces[ i ]._pointC ]._difference );

                    if ( simplexPlane.ClassifyVertex ( origin ) != eGXPlaneClassifyVertex::Behind ) continue;

                    direction.Init ( -simplexPlane._a, -simplexPlane._b, -simplexPlane._c );

                    const GXSupportPoint a = simplex._supportPoints[ simplex._faces[ i ]._pointA ];
                    const GXSupportPoint b = simplex._supportPoints[ simplex._faces[ i ]._pointB ];
                    const GXSupportPoint c = simplex._supportPoints[ simplex._faces[ i ]._pointC ];

                    simplex.Reset ();
                    simplex.PushPoint ( a );
                    simplex.PushPoint ( b );
                    simplex.PushPoint ( c );

                    isIntersected = GX_FALSE;

                    break;
                }

                if ( !isIntersected ) break;

                isLoop = GX_FALSE;
            }
            break;

            default:
                // NOTHING
            break;
        }
    }

    if ( isIntersected )
        return GX_TRUE;

    _totalShapeAContactGeometryPoints = 0u;
    _totalShapeBContactGeometryPoints = 0u;

    return GX_FALSE;
}

GXBool GXCollisionDetector::RunEPA ( GXVec3 &contactNormal, GXFloat &contactPenetration, GXUInt &usedEdges, GXUInt &usedIterations, const GXShape &shapeA, const GXShape &shapeB, GXSimplex &simplex )
{
    static const GXVec3 origin ( 0.0f, 0.0f, 0.0f );

    GXSupportPoint* supportPointArray = static_cast<GXSupportPoint*> ( _supportPoints.GetData () );
    GXEdge* edgeArray = static_cast<GXEdge*> ( _edges.GetData () );
    GXFace* faceArray = static_cast<GXFace*> ( _faces.GetData () );

    memcpy ( supportPointArray, simplex._supportPoints, 4 * sizeof ( GXSupportPoint ) );
    _totalSupportPoints = 4u;

    _totalFaces = 0u;

    GXFace& alphaFace = simplex._faces[ 0 ];
    GXPlane plane;
    plane.From ( supportPointArray[ alphaFace._pointA ]._difference, supportPointArray[ alphaFace._pointB ]._difference, supportPointArray[ alphaFace._pointC ]._difference );
    alphaFace._normal.Init ( plane._a, plane._b, plane._c );
    alphaFace._next = INVALID_INDEX;
    alphaFace._previous = INVALID_INDEX;
    GXUInt lastFaceIndex = 0u;
    faceArray[ lastFaceIndex ] = alphaFace;
    ++_totalFaces;

    for ( GXUInt i = 1u; i < 4u; ++i )
    {
        GXFace& bettaFace = simplex._faces[ i ];
        plane.From ( supportPointArray[ bettaFace._pointA ]._difference, supportPointArray[ bettaFace._pointB ]._difference, supportPointArray[ bettaFace._pointC ]._difference );
        bettaFace._normal.Init ( plane._a, plane._b, plane._c );
        bettaFace._next = INVALID_INDEX;
        bettaFace._previous = lastFaceIndex;
        lastFaceIndex = _totalFaces;
        faceArray[ lastFaceIndex ] = bettaFace;
        faceArray[ bettaFace._previous ]._next = lastFaceIndex;
        ++_totalFaces;
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

        for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faceArray[ i ]._previous )
        {
            const GXFace& yottaFace = faceArray[ i ];

            GXVec3 gamma;
            gamma.Substract ( origin, supportPointArray[ yottaFace._pointA ]._difference );

            GXFloat newDistance = gamma.DotProduct ( yottaFace._normal );

            if ( newDistance >= smallestDistance ) continue;

            smallestDistance = newDistance;
            smallestDistanceFaceIndex = i;
            smallestDistanceFaceNormal = yottaFace._normal;
        }

        if ( fabsf ( smallestDistance - contactPenetration ) <= EPA_DISTANCE_EPSILON )
        {
            // Disable contacts with penetration depth equal 0.0f or bigger than half FLT_MAX

            if ( contactPenetration == 0.0f || contactPenetration > FLT_MAX * 0.5f )
            {
                _totalShapeAContactGeometryPoints = 0u;
                _totalShapeBContactGeometryPoints = 0u;

                return GX_FALSE;
            }

            contactPenetration = smallestDistance;
            contactNormal = smallestDistanceFaceNormal;

            break;
        }

        contactPenetration = smallestDistance;

        if ( _supportPoints.GetLength () < _totalSupportPoints + 1u )
        {
            _supportPoints.Resize ( _supportPoints.GetLength () + EPA_SUPPORT_POINTS_ALLOCATING_STEP );
            supportPointArray = static_cast<GXSupportPoint*> ( _supportPoints.GetData () );
        }

        GXSupportPoint supportPoint;
        GXVec3 direction ( smallestDistanceFaceNormal );
        direction.Reverse ();

        CalculateSupportPoint ( supportPoint, shapeA, shapeB, direction );

        GXUInt newSupportPointIndex = _totalSupportPoints;
        supportPointArray[ newSupportPointIndex ] = supportPoint;
        ++_totalSupportPoints;

        GXUInt lastEdgeIndex = INVALID_INDEX;
        _totalEdges = 0u;

        for ( GXUInt i = lastFaceIndex; i != INVALID_INDEX; i = faceArray[ i ]._previous )
        {
            GXFace& gammaFace = faceArray[ i ];

            GXVec3 omega;
            omega.Substract ( supportPointArray[ gammaFace._pointA ]._difference, supportPoint._difference );

            if ( omega.DotProduct ( gammaFace._normal ) <= 0.0f ) continue;

            GXEdge edge1 = GXEdge ( gammaFace._pointA, gammaFace._pointB );
            ProceedEdge ( edge1, lastEdgeIndex, &edgeArray, _totalEdges );

            GXEdge edge2 = GXEdge ( gammaFace._pointB, gammaFace._pointC );
            ProceedEdge ( edge2, lastEdgeIndex, &edgeArray, _totalEdges );

            GXEdge edge3 = GXEdge ( gammaFace._pointC, gammaFace._pointA );
            ProceedEdge ( edge3, lastEdgeIndex, &edgeArray, _totalEdges );

            if ( i == lastFaceIndex )
                lastFaceIndex = gammaFace._previous;

            if ( lastFaceIndex > 500u )
            {
                GXVec3 wtf;
            }

            if ( gammaFace._previous != INVALID_INDEX )
                faceArray[ gammaFace._previous ]._next = gammaFace._next;

            if ( gammaFace._next != INVALID_INDEX )
                faceArray[ gammaFace._next ]._previous = gammaFace._previous;
        }

        if ( _totalEdges > usedEdges )
            usedEdges = _totalEdges;

        for ( GXUInt i = lastEdgeIndex; i != INVALID_INDEX; i = edgeArray[ i ]._previous )
        {
            const GXEdge& edge = edgeArray[ i ];

            plane.From ( supportPointArray[ newSupportPointIndex ]._difference, supportPointArray[ edge._startPoint ]._difference, supportPointArray[ edge._endPoint ]._difference );
            GXFace omegaFace ( newSupportPointIndex, edge._startPoint, edge._endPoint, GXVec3 ( plane._a, plane._b, plane._c ) );

            if ( plane.ClassifyVertex ( origin ) == eGXPlaneClassifyVertex::Behind )
                omegaFace.ChangeOrder ();

            if ( _faces.GetLength () < _totalFaces + 1 )
            {
                _faces.Resize ( _faces.GetLength () + EPA_FACE_ALLOCATING_STEP );
                faceArray = static_cast<GXFace*> ( _faces.GetData () );
            }

            omegaFace._next = INVALID_INDEX;
            omegaFace._previous = lastFaceIndex;
            lastFaceIndex = _totalFaces;

            faceArray[ lastFaceIndex ] = omegaFace;
            faceArray[ omegaFace._previous ]._next = lastFaceIndex;
            ++_totalFaces;
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

    supportPoint._difference.Substract ( extremeA, extremeB );
}

GXVoid GXCollisionDetector::ProceedEdge ( GXEdge &edge, GXUInt &lastEdgeIndex, GXEdge** edgeArrayPointer, GXUInt &totalEdgeNumber )
{
    GXBool allowInsert = GX_TRUE;
    GXEdge* edgeArray = *edgeArrayPointer;

    for ( GXUInt i = lastEdgeIndex; i != INVALID_INDEX; i = edgeArray[ i ]._previous )
    {
        if ( !GXEdge::IsOpposite ( edge, edgeArray[ i ] ) ) continue;

        if ( i == lastEdgeIndex )
            lastEdgeIndex = edgeArray[ lastEdgeIndex ]._previous;

        if ( edgeArray[ i ]._previous != INVALID_INDEX )
            edgeArray[ edgeArray[ i ]._previous ]._next = edgeArray[ i ]._next;

        if ( edgeArray[ i ]._next != INVALID_INDEX )
            edgeArray[ edgeArray[ i ]._next ]._previous = edgeArray[ i ]._previous;

        allowInsert = GX_FALSE;
        break;
    }

    if ( !allowInsert ) return;

    if ( _edges.GetLength () < totalEdgeNumber + 1 )
    {
        _edges.Resize ( _edges.GetLength () + EPA_EDGE_ALLOCATING_STEP );
        *edgeArrayPointer = static_cast<GXEdge*> ( _edges.GetData () );
    }

    edge._next = INVALID_INDEX;
    edge._previous = lastEdgeIndex;
    lastEdgeIndex = totalEdgeNumber;
    edgeArray[ lastEdgeIndex ] = edge;

    if ( lastEdgeIndex > 0u )
        edgeArray[ edge._previous ]._next = lastEdgeIndex;

    ++totalEdgeNumber;
}

GXVoid GXCollisionDetector::ProjectContactGeometry ( GXVec3* contactGeometryProjection, const GXVec3* contactGeometryWorld, GXUShort totalContactGeometryPoints, const GXVec3 &contactNormal )
{
    constexpr GXVec3 origin ( 0.0f, 0.0f, 0.0f );

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
    constexpr GXVec3 origin ( 0.0f, 0.0f, 0.0f );

    for ( GXUShort i = 0u; i < totalContactGeometryPoints; ++i )
    {
        GXVec3 v;
        v.Substract ( projectedContactGeometry[ i ], origin );

        planarContactGeometry[ i ].Init ( xAxis.DotProduct ( v ), yAxis.DotProduct ( v ) );
    }
}

GXVoid GXCollisionDetector::GetClippedPlanarContactGeometry ( GXVec2** clippedGeometryArrayPointer, GXUInt &totalPoints, const GXVec2* clipGeometry, GXUInt totalClipGeometryPoints, const GXVec2* subjectGeometry, GXUInt totalSubjectGeometryPoints )
{
    GXVec2* outputGeometry = static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () );
    GXUInt totalOutputPoints = totalSubjectGeometryPoints;

    memcpy ( outputGeometry, subjectGeometry, totalSubjectGeometryPoints * sizeof ( GXVec2 ) );

    GXUInt previousClipPointIndex = totalClipGeometryPoints - 1u;

    for ( GXUInt currentClipPointIndex = 0u; currentClipPointIndex < totalClipGeometryPoints; ++currentClipPointIndex )
    {
        GXVec2* inputGeometry = outputGeometry;
        GXUInt totalInputPoints = totalOutputPoints;

        outputGeometry = ( outputGeometry == (GXVec2*)_alphaPlanarIntersectionGeometry.GetData () ) ? (GXVec2*)_bettaPlanarIntersectionGeometry.GetData () : (GXVec2*)_alphaPlanarIntersectionGeometry.GetData ();
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
                            if ( _alphaPlanarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
                            {
                                GXUInt newSize = totalOutputPoints + PLANAR_INTERSECTION_ALLOCATING_STEP;

                                if ( _shapeAPlanarContactGeometryDebug && _planarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
                                    _planarIntersectionGeometry.Resize ( newSize );

                                _intersectionGeometry.Resize ( newSize );
                                _alphaPlanarIntersectionGeometry.Resize ( newSize );
                                _bettaPlanarIntersectionGeometry.Resize ( newSize );

                                if ( outputGeometry == static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () ) )
                                {
                                    outputGeometry = static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () );
                                    inputGeometry = static_cast<GXVec2*> ( _bettaPlanarIntersectionGeometry.GetData () );
                                }
                                else
                                {
                                    outputGeometry = static_cast<GXVec2*> ( _bettaPlanarIntersectionGeometry.GetData () );
                                    inputGeometry = static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () );
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
                    if ( _alphaPlanarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
                    {
                        GXUInt newSize = totalOutputPoints + PLANAR_INTERSECTION_ALLOCATING_STEP;

                        if ( _shapeAPlanarContactGeometryDebug && _planarIntersectionGeometry.GetLength () < totalOutputPoints + 1 )
                            _planarIntersectionGeometry.Resize ( newSize );

                        _intersectionGeometry.Resize ( newSize );
                        _alphaPlanarIntersectionGeometry.Resize ( newSize );
                        _bettaPlanarIntersectionGeometry.Resize ( newSize );

                        if ( outputGeometry == static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () ) )
                        {
                            outputGeometry = static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () );
                            inputGeometry = static_cast<GXVec2*> ( _bettaPlanarIntersectionGeometry.GetData () );
                        }
                        else
                        {
                            outputGeometry = static_cast<GXVec2*> ( _bettaPlanarIntersectionGeometry.GetData () );
                            inputGeometry = static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () );
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
                        if ( _alphaPlanarIntersectionGeometry.GetLength () < totalOutputPoints + 2 )
                        {
                            GXUInt newSize = totalOutputPoints + PLANAR_INTERSECTION_ALLOCATING_STEP;

                            if ( _shapeAPlanarContactGeometryDebug && _planarIntersectionGeometry.GetLength () < totalOutputPoints + 2 )
                                _planarIntersectionGeometry.Resize ( newSize );

                            _intersectionGeometry.Resize ( newSize );
                            _alphaPlanarIntersectionGeometry.Resize ( newSize );
                            _bettaPlanarIntersectionGeometry.Resize ( newSize );

                            if ( outputGeometry == static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () ) )
                            {
                                outputGeometry = static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () );
                                inputGeometry = static_cast<GXVec2*> ( _bettaPlanarIntersectionGeometry.GetData () );
                            }
                            else
                            {
                                outputGeometry = static_cast<GXVec2*> ( _bettaPlanarIntersectionGeometry.GetData () );
                                inputGeometry = static_cast<GXVec2*> ( _alphaPlanarIntersectionGeometry.GetData () );
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
    const GXFloat step = GX_MATH_DOUBLE_PI / (GXFloat)_totalDeviationAxes;
    const GXFloat limit = GX_MATH_DOUBLE_PI - 0.5f * step;
    const GXFloat deviation = sinf ( _deviationAngle );
    const GXVec3 deviationVector ( deviation, 0.0f, 1.0f );
    const GXFloat normalizeScale = 1.0f / deviationVector.Length ();

    GXUShort axisIndex = 0u;

    for ( GXFloat angle = 0.0f; angle < limit; angle += step )
    {
        _deviationAxesLocal[ axisIndex ].Init ( cosf ( angle ) * deviation, sinf ( angle ) * deviation, 1.0f );
        _deviationAxesLocal[ axisIndex ].Multiply ( _deviationAxesLocal[ axisIndex ], normalizeScale );
        ++axisIndex;
    }
}

GXVoid GXCollisionDetector::UpdateDebugData ( const GXVec2* planarIntersectionGeometryArray )
{
    for ( GXUInt i = 0u; i < _totalShapeAContactGeometryPoints; ++i )
        _shapeAPlanarContactGeometryDebug[ i ].Init ( _shapeAPlanarContactGeometry[ i ]._data[ 0 ], _shapeAPlanarContactGeometry[ i ]._data[ 1 ], 0.0f );

    for ( GXUInt i = 0u; i < _totalShapeBContactGeometryPoints; ++i )
        _shapeBPlanarContactGeometryDebug[ i ].Init ( _shapeBPlanarContactGeometry[ i ]._data[ 0 ], _shapeBPlanarContactGeometry[ i ]._data[ 1 ], 0.0f );

    if ( !planarIntersectionGeometryArray ) return;

    for ( GXUInt i = 0u; i < _totalIntersectionPoints; ++i )
    {
        GXVec3 v ( planarIntersectionGeometryArray[ i ]._data[ 0 ], planarIntersectionGeometryArray[ i ]._data[ 1 ], 0.0f );
        _planarIntersectionGeometry.SetValue ( i, &v );
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
