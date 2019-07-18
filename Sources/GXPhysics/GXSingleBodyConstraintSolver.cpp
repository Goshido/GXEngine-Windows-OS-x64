// version 1.3

#include <GXPhysics/GXSingleBodyConstraintSolver.h>
#include <GXPhysics/GXPhysicsEngine.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_INITIAL_LAMBDA              0.0f
#define CONSTRAINT_STORAGE_GROW_FACTOR      32u
#define ELEMENTS_PER_CONSTRAINT             2u

//---------------------------------------------------------------------------------------------------------------------

GXSingleBodyConstraintSolver::GXSingleBodyConstraintSolver ( GXUShort maximumIterations ):
    _jacobian (sizeof ( GXVec6 ) ),
    _bias ( sizeof ( GXFloat ) ),
    _lambdaRange ( sizeof ( GXVec2 ) ),
    _initialLambda ( sizeof ( GXFloat ) ),
    _lambda ( sizeof ( GXFloat ) ),
    _eta ( sizeof ( GXFloat ) ),
    _d ( sizeof ( GXVec6 ) ),
    _b ( sizeof ( GXVec6 ) ),
    _maximumIterations ( maximumIterations ),
    _constraints ( 0u ),
    _firstBody ( nullptr ),
    _secondBody ( nullptr )
{
    // NOTHING
}

GXSingleBodyConstraintSolver::~GXSingleBodyConstraintSolver ()
{
    // NOTHING
}

GXVoid GXSingleBodyConstraintSolver::Begin ( GXRigidBody &first, GXRigidBody &second )
{
    _firstBody = &first;
    _secondBody = &second;

    _constraints = 0u;
}

GXVoid GXSingleBodyConstraintSolver::AddConstraint ( const GXConstraint& constraint )
{
    if ( _constraints >= _lambda.GetLength () )
    {
        const GXUInt newCapacity = _constraints + CONSTRAINT_STORAGE_GROW_FACTOR;

        _bias.Resize ( newCapacity );
        _lambdaRange.Resize ( newCapacity );
        _initialLambda.Resize ( newCapacity );
        _lambda.Resize ( newCapacity );
        _eta.Resize ( newCapacity );
        _d.Resize ( newCapacity );
        _b.Resize ( newCapacity );

        _jacobian.Resize ( newCapacity * ELEMENTS_PER_CONSTRAINT );
    }

    GXUInt offset = _constraints * ELEMENTS_PER_CONSTRAINT;

    GXVec6* jacobianData = static_cast<GXVec6*> ( _jacobian.GetData () );
    memcpy ( jacobianData + offset, constraint.GetJacobian (), ELEMENTS_PER_CONSTRAINT * sizeof ( GXVec6 ) );

    GXFloat* biasData = static_cast<GXFloat*> ( _bias.GetData () );
    biasData[ _constraints ] = constraint.GetBias ();

    GXVec2* lambdaRangeData = static_cast<GXVec2*> ( _lambdaRange.GetData () );
    lambdaRangeData[ _constraints ] = constraint.GetLambdaRange ();

    GXFloat* initialLambdaData = static_cast<GXFloat*> ( _initialLambda.GetData () );
    initialLambdaData[ _constraints ] = DEFAULT_INITIAL_LAMBDA;

    ++_constraints;
}

GXVoid GXSingleBodyConstraintSolver::End ()
{
    UpdateB ();
    UpdateEta ();

    const GXFloat* initialLambdaData = static_cast<const GXFloat*> ( _initialLambda.GetData () );
    GXFloat* lambdaData = (GXFloat*)_lambda.GetData ();
    memcpy ( lambdaData, initialLambdaData, _constraints * sizeof ( GXFloat ) );

    UpdateA ();

    GXFloat* dData = static_cast<GXFloat*> ( _d.GetData () );
    const GXVec6* bData = static_cast<const GXVec6*> ( _b.GetData () );

    for ( GXUInt i = 0u; i < _constraints; ++i )
        dData[ i ] = bData[ i ].DotProduct ( GetJacobianElement ( i, 0 ) );

    const GXFloat* etaData = static_cast<const GXFloat*> ( _eta.GetData () );
    const GXVec2* lambdaRangeData = static_cast<const GXVec2*> ( _lambdaRange.GetData () );

    for ( GXUInt iteration = 0u; iteration < _maximumIterations; ++iteration )
    {
        for ( GXUInt i = 0u; i < _constraints; ++i )
        {
            GXFloat deltaLambda = ( etaData[ i ] - _a.DotProduct ( GetJacobianElement ( i, 0 ) ) ) / dData[ i ];
            const GXFloat oldLambda = lambdaData[ i ];

            lambdaData[ i ] = GXMaxf ( lambdaRangeData[ i ]._data[ 0 ], GXMinf ( lambdaData[ i ] + deltaLambda, lambdaRangeData[ i ]._data[ 1 ] ) );
            deltaLambda = lambdaData[ i ] - oldLambda;

            GXVec6 alpha;
            alpha.Multiply ( bData[ i ], deltaLambda );
            _a.Sum ( _a, alpha );
        }
    }

    UpdateBodyVelocity ();

    _firstBody = _secondBody = nullptr;
    _constraints = 0;
}

GXVoid GXSingleBodyConstraintSolver::UpdateB ()
{
    GXVec6* bData = static_cast<GXVec6*> ( _b.GetData () );

    const GXFloat firstInverseMass = _firstBody->GetInverseMass ();
    const GXMat3& firstInverseInertia = _firstBody->GetInverseInertiaTensorWorld ();

    for ( GXUInt i = 0u; i < _constraints; ++i )
    {
        const GXVec6& j0 = GetJacobianElement ( i, 0u );

        bData->_data[ 0u ] = firstInverseMass * j0._data[ 0u ];
        bData->_data[ 1u ] = firstInverseMass * j0._data[ 1u ];
        bData->_data[ 2u ] = firstInverseMass * j0._data[ 2u ];
        bData->_data[ 3u ] = firstInverseInertia._m[ 0u ][ 0u ] * j0._data[ 3u ] + firstInverseInertia._m[ 0u ][ 1u ] * j0._data[ 4u ] + firstInverseInertia._m[ 0u ][ 2u ] * j0._data[ 5u ];
        bData->_data[ 4u ] = firstInverseInertia._m[ 1u ][ 0u ] * j0._data[ 3u ] + firstInverseInertia._m[ 1u ][ 1u ] * j0._data[ 4u ] + firstInverseInertia._m[ 1u ][ 2u ] * j0._data[ 5u ];
        bData->_data[ 5u ] = firstInverseInertia._m[ 2u ][ 0u ] * j0._data[ 3u ] + firstInverseInertia._m[ 2u ][ 1u ] * j0._data[ 4u ] + firstInverseInertia._m[ 2u ][ 2u ] * j0._data[ 5u ];

        ++bData;
    }
}

GXVoid GXSingleBodyConstraintSolver::UpdateEta ()
{
    const GXFloat inverseDeltaTime = 1.0f / GXPhysicsEngine::GetInstance ().GetTimeStep ();

    GXVec6 alpha[ 2u ];
    GXVec6& alpha0 = alpha[ 0u ];
    GXVec6& alpha1 = alpha[ 1u ];

    alpha0.From ( _firstBody->GetLinearVelocity (), _firstBody->GetAngularVelocity () );
    alpha1.From ( _secondBody->GetLinearVelocity (), _secondBody->GetAngularVelocity () );

    alpha0.Multiply ( alpha0, inverseDeltaTime );
    alpha1.Multiply ( alpha1, inverseDeltaTime );

    const GXFloat firstInverseMass = _firstBody->GetInverseMass ();
    const GXMat3& firstInverseInertia = _firstBody->GetInverseInertiaTensorWorld ();
    const GXVec3& firstForce = _firstBody->GetTotalForce ();
    const GXVec3& firstTorque = _firstBody->GetTotalTorque ();

    GXVec6 betta;

    betta._data[ 0u ] = firstInverseMass * firstForce._data[ 0u ];
    betta._data[ 1u ] = firstInverseMass * firstForce._data[ 1u ];
    betta._data[ 2u ] = firstInverseMass * firstForce._data[ 2u ];
    betta._data[ 3u ] = firstInverseInertia._m[ 0u ][ 0u ] * firstTorque._data[ 0u ] + firstInverseInertia._m[ 0u ][ 1u ] * firstTorque._data[ 1u ] + firstInverseInertia._m[ 0u ][ 2u ] * firstTorque._data[ 2u ];
    betta._data[ 4u ] = firstInverseInertia._m[ 1u ][ 0u ] * firstTorque._data[ 0u ] + firstInverseInertia._m[ 1u ][ 1u ] * firstTorque._data[ 1u ] + firstInverseInertia._m[ 1u ][ 2u ] * firstTorque._data[ 2u ];
    betta._data[ 5u ] = firstInverseInertia._m[ 2u ][ 0u ] * firstTorque._data[ 0u ] + firstInverseInertia._m[ 2u ][ 1u ] * firstTorque._data[ 1u ] + firstInverseInertia._m[ 2u ][ 2u ] * firstTorque._data[ 2u ];

    alpha0.Sum ( alpha0, betta );

    GXFloat* etaData = static_cast<GXFloat*> ( _eta.GetData () );
    const GXFloat* biasData = static_cast<GXFloat*> ( _bias.GetData () );

    for ( GXUInt i = 0u; i < _constraints; ++i )
    {
        etaData[ i ] = inverseDeltaTime * biasData[ i ] - ( alpha0.DotProduct ( GetJacobianElement ( i, 0u ) ) + alpha1.DotProduct ( GetJacobianElement ( i, 1u ) ) );
    }
}

GXVoid GXSingleBodyConstraintSolver::UpdateA ()
{
    const GXFloat* lambdaData = static_cast<const GXFloat*> ( _lambda.GetData () );
    const GXVec6* bData = (const GXVec6*)_b.GetData ();

    memset ( &_a, 0, sizeof ( GXVec6 ) );

    for ( GXUInt i = 0u; i < _constraints; ++i )
    {
        _a.Sum ( _a, lambdaData[ i ], bData[ i ] );
    }
}

GXVoid GXSingleBodyConstraintSolver::UpdateBodyVelocity ()
{
    const GXFloat* lambdaData = static_cast<const GXFloat*> ( _lambda.GetData () );
    const GXFloat firstInverseMass = _firstBody->GetInverseMass ();
    const GXMat3& firstInverseInertia = _firstBody->GetInverseInertiaTensorWorld ();
    const GXFloat deltaTime = GXPhysicsEngine::GetInstance ().GetTimeStep ();

    GXVec6 alpha;
    memset ( &alpha, 0, sizeof ( GXVec6 ) );

    for ( GXUInt i = 0u; i < _constraints; ++i )
        alpha.Sum ( alpha, lambdaData[ i ], GetJacobianElement ( i, 0 ) );

    GXVec6 betta;
    betta.From ( _firstBody->GetTotalForce (), _firstBody->GetTotalTorque () );
    alpha.Sum ( alpha, betta );

    betta._data[ 0u ] = firstInverseMass * alpha._data[ 0u ];
    betta._data[ 1u ] = firstInverseMass * alpha._data[ 1u ];
    betta._data[ 2u ] = firstInverseMass * alpha._data[ 2u ];
    betta._data[ 3u ] = firstInverseInertia._m[ 0u ][ 0u ] * alpha._data[ 3u ] + firstInverseInertia._m[ 0u ][ 1u ] * alpha._data[ 4u ] + firstInverseInertia._m[ 0u ][ 2u ] * alpha._data[ 5u ];
    betta._data[ 4u ] = firstInverseInertia._m[ 1u ][ 0u ] * alpha._data[ 3u ] + firstInverseInertia._m[ 1u ][ 1u ] * alpha._data[ 4u ] + firstInverseInertia._m[ 1u ][ 2u ] * alpha._data[ 5u ];
    betta._data[ 5u ] = firstInverseInertia._m[ 2u ][ 0u ] * alpha._data[ 3u ] + firstInverseInertia._m[ 2u ][ 1u ] * alpha._data[ 4u ] + firstInverseInertia._m[ 2u ][ 2u ] * alpha._data[ 5u ];

    betta.Multiply ( betta, deltaTime );
    alpha.From ( _firstBody->GetLinearVelocity (), _firstBody->GetAngularVelocity () );
    alpha.Sum ( alpha, betta );

    _firstBody->SetLinearVelocity ( GXVec3 ( alpha._data[ 0u ], alpha._data[ 1u ], alpha._data[ 2u ] ) );
    _firstBody->SetAngularVelocity ( GXVec3 ( alpha._data[ 3u ], alpha._data[ 4u ], alpha._data[ 5u ] ) );
}

const GXVec6& GXSingleBodyConstraintSolver::GetJacobianElement ( GXUInt constraint, GXUByte bodyIndex ) const
{
    const GXVec6* elements = static_cast<const GXVec6*> ( _jacobian.GetData () );
    return elements[ constraint * ELEMENTS_PER_CONSTRAINT + (GXUInt)bodyIndex ];
}
