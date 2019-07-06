// version 1.3

#include <GXPhysics/GXPairBodyConstraintSolver.h>
#include <GXPhysics/GXPhysicsEngine.h>


#define DEFAULT_INITIAL_LAMBDA              0.0f
#define CONSTRAINT_STORAGE_GROW_FACTOR      32u
#define ELEMENTS_PER_CONSTRAINT             2u

//---------------------------------------------------------------------------------------------------------------------

GXPairBodyConstraintSolver::GXPairBodyConstraintSolver ( GXUShort maximumIterations ):
    _jacobian ( sizeof ( GXVec6 ) ),
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

GXPairBodyConstraintSolver::~GXPairBodyConstraintSolver ()
{
    // NOTHING
}

GXVoid GXPairBodyConstraintSolver::Begin ( GXRigidBody &first, GXRigidBody &second )
{
    _firstBody = &first;
    _secondBody = &second;

    _constraints = 0u;
}

GXVoid GXPairBodyConstraintSolver::AddConstraint ( const GXConstraint& constraint )
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

        const GXUInt alpha = newCapacity * ELEMENTS_PER_CONSTRAINT;
        _jacobian.Resize ( alpha );
        _b.Resize ( alpha );
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

GXVoid GXPairBodyConstraintSolver::End ()
{
    UpdateB ();
    UpdateEta ();

    const GXFloat* initialLambdaData = static_cast<const GXFloat*> ( _initialLambda.GetData () );
    GXFloat* lambdaData = static_cast<GXFloat*> ( _lambda.GetData () );
    memcpy ( lambdaData, initialLambdaData, _constraints * sizeof ( GXFloat ) );

    UpdateA ();

    GXFloat* dData = static_cast<GXFloat*> ( _d.GetData () );

    for ( GXUInt i = 0u; i < _constraints; ++i )
    {
        const GXVec6& j0 = GetJacobianElement ( i, 0u );
        const GXVec6& j1 = GetJacobianElement ( i, 1u );
        dData[ i ] = j0.DotProduct ( GetBElement ( 0u, i ) ) + j1.DotProduct ( GetBElement ( 1u, i ) );
    }

    const GXFloat* etaData = static_cast<const GXFloat*> ( _eta.GetData () );
    const GXVec2* lambdaRangeData = static_cast<const GXVec2*> ( _lambdaRange.GetData () );

    for ( GXUInt iteration = 0u; iteration < _maximumIterations; ++iteration )
    {
        for ( GXUInt i = 0u; i < _constraints; ++i )
        {
            GXFloat deltaLambda = ( etaData[ i ] - _a[ 0 ].DotProduct ( GetJacobianElement ( i, 0u ) ) - _a[ 1 ].DotProduct ( GetJacobianElement ( i, 1u ) ) ) / dData[ i ];
            const GXFloat oldLambda = lambdaData[ i ];

            lambdaData[ i ] = GXMaxf ( lambdaRangeData[ i ]._data[ 0 ], GXMinf ( lambdaData[ i ] + deltaLambda, lambdaRangeData[ i ]._data[ 1 ] ) );
            deltaLambda = lambdaData[ i ] - oldLambda;

            GXVec6 alpha;
            alpha.Multiply ( GetBElement ( 0u, i ), deltaLambda );
            _a[ 0u ].Sum ( _a[ 0u ], alpha );

            alpha.Multiply ( GetBElement ( 1u, i ), deltaLambda );
            _a[ 1u ].Sum ( _a[ 1u ], alpha );
        }
    }

    UpdateBodyVelocities ();

    _firstBody = _secondBody = nullptr;
    _constraints = 0u;
}

GXVoid GXPairBodyConstraintSolver::UpdateB ()
{
    GXVec6* bData = static_cast<GXVec6*> ( _b.GetData () );

    const GXFloat firstInverseMass = _firstBody->GetInverseMass ();
    const GXMat3& firstInverseInertia = _firstBody->GetInverseInertiaTensorWorld ();

    const GXFloat secondInverseMass = _secondBody->GetInverseMass ();
    const GXMat3& secondInverseInertia = _secondBody->GetInverseInertiaTensorWorld ();

    for ( GXUInt i = 0u; i < _constraints; ++i )
    {
        const GXVec6& j0 = GetJacobianElement ( i, 0u );

        bData->_data[ 0u ] = firstInverseMass * j0._data[ 0u ];
        bData->_data[ 1u ] = firstInverseMass * j0._data[ 1u ];
        bData->_data[ 2u ] = firstInverseMass * j0._data[ 2u ];
        bData->_data[ 3u ] = firstInverseInertia._m[ 0u ][ 0u ] * j0._data[ 3u ] + firstInverseInertia._m[ 0u ][ 1u ] * j0._data[ 4u ] + firstInverseInertia._m[ 0u ][ 2u ] * j0._data[ 5u ];
        bData->_data[ 4u ] = firstInverseInertia._m[ 1u ][ 0u ] * j0._data[ 3u ] + firstInverseInertia._m[ 1u ][ 1u ] * j0._data[ 4u ] + firstInverseInertia._m[ 1u ][ 2u ] * j0._data[ 5u ];
        bData->_data[ 5u ] = firstInverseInertia._m[ 2u ][ 0u ] * j0._data[ 3u ] + firstInverseInertia._m[ 2u ][ 1u ] * j0._data[ 4u ] + firstInverseInertia._m[ 2u ][ 2u ] * j0._data[ 5u ];

        const GXVec6& j1 = GetJacobianElement ( i, 1u );
        ++bData;

        bData->_data[ 0u ] = secondInverseMass * j1._data[ 0u ];
        bData->_data[ 1u ] = secondInverseMass * j1._data[ 1u ];
        bData->_data[ 2u ] = secondInverseMass * j1._data[ 2u ];
        bData->_data[ 3u ] = secondInverseInertia._m[ 0u ][ 0u ] * j1._data[ 3u ] + secondInverseInertia._m[ 0u ][ 1u ] * j1._data[ 4u ] + secondInverseInertia._m[ 0u ][ 2u ] * j1._data[ 5u ];
        bData->_data[ 4u ] = secondInverseInertia._m[ 1u ][ 0u ] * j1._data[ 3u ] + secondInverseInertia._m[ 1u ][ 1u ] * j1._data[ 4u ] + secondInverseInertia._m[ 1u ][ 2u ] * j1._data[ 5u ];
        bData->_data[ 5u ] = secondInverseInertia._m[ 2u ][ 0u ] * j1._data[ 3u ] + secondInverseInertia._m[ 2u ][ 1u ] * j1._data[ 4u ] + secondInverseInertia._m[ 2u ][ 2u ] * j1._data[ 5u ];

        ++bData;
    }
}

GXVoid GXPairBodyConstraintSolver::UpdateEta ()
{
    GXVec6 alpha[ 2u ];
    GXVec6& alpha0 = alpha[ 0u ];
    GXVec6& alpha1 = alpha[ 1u ];

    alpha0.From ( _firstBody->GetLinearVelocity (), _firstBody->GetAngularVelocity () );
    alpha1.From ( _secondBody->GetLinearVelocity (), _secondBody->GetAngularVelocity () );

    const GXFloat inverseDeltaTime = 1.0f / GXPhysicsEngine::GetInstance ().GetTimeStep ();

    alpha0.Multiply ( alpha0, inverseDeltaTime );
    alpha1.Multiply ( alpha1, inverseDeltaTime );

    GXVec6 betta[ 2u ];
    GXVec6& betta0 = betta[ 0u ];
    GXVec6& betta1 = betta[ 1u ];

    const GXFloat firstInverseMass = _firstBody->GetInverseMass ();
    const GXMat3& firstInverseInertia = _firstBody->GetInverseInertiaTensorWorld ();
    const GXVec3& firstForce = _firstBody->GetTotalForce ();
    const GXVec3& firstTorque = _firstBody->GetTotalTorque ();

    const GXFloat secondInverseMass = _secondBody->GetInverseMass ();
    const GXMat3& secondInverseInertia = _secondBody->GetInverseInertiaTensorWorld ();
    const GXVec3& secondForce = _secondBody->GetTotalForce ();
    const GXVec3& secondTorque = _secondBody->GetTotalTorque ();

    betta0._data[ 0u ] = firstInverseMass * firstForce._data[ 0u ];
    betta0._data[ 1u ] = firstInverseMass * firstForce._data[ 1u ];
    betta0._data[ 2u ] = firstInverseMass * firstForce._data[ 2u ];
    betta0._data[ 3u ] = firstInverseInertia._m[ 0u ][ 0u ] * firstTorque._data[ 0u ] + firstInverseInertia._m[ 0u ][ 1u ] * firstTorque._data[ 1u ] + firstInverseInertia._m[ 0u ][ 2u ] * firstTorque._data[ 2u ];
    betta0._data[ 4u ] = firstInverseInertia._m[ 1u ][ 0u ] * firstTorque._data[ 0u ] + firstInverseInertia._m[ 1u ][ 1u ] * firstTorque._data[ 1u ] + firstInverseInertia._m[ 1u ][ 2u ] * firstTorque._data[ 2u ];
    betta0._data[ 5u ] = firstInverseInertia._m[ 2u ][ 0u ] * firstTorque._data[ 0u ] + firstInverseInertia._m[ 2u ][ 1u ] * firstTorque._data[ 1u ] + firstInverseInertia._m[ 2u ][ 2u ] * firstTorque._data[ 2u ];

    betta1._data[ 0u ] = secondInverseMass * secondForce._data[ 0u ];
    betta1._data[ 1u ] = secondInverseMass * secondForce._data[ 1u ];
    betta1._data[ 2u ] = secondInverseMass * secondForce._data[ 2u ];
    betta1._data[ 3u ] = secondInverseInertia._m[ 0u ][ 0u ] * secondTorque._data[ 0u ] + secondInverseInertia._m[ 0u ][ 1u ] * secondTorque._data[ 1u ] + secondInverseInertia._m[ 0u ][ 2u ] * secondTorque._data[ 2u ];
    betta1._data[ 4u ] = secondInverseInertia._m[ 1u ][ 0u ] * secondTorque._data[ 0u ] + secondInverseInertia._m[ 1u ][ 1u ] * secondTorque._data[ 1u ] + secondInverseInertia._m[ 1u ][ 2u ] * secondTorque._data[ 2u ];
    betta1._data[ 5u ] = secondInverseInertia._m[ 2u ][ 0u ] * secondTorque._data[ 0u ] + secondInverseInertia._m[ 2u ][ 1u ] * secondTorque._data[ 1u ] + secondInverseInertia._m[ 2u ][ 2u ] * secondTorque._data[ 2u ];

    alpha0.Sum ( alpha0, betta0 );
    alpha1.Sum ( alpha1, betta1 );

    GXFloat* etaData = static_cast<GXFloat*> ( _eta.GetData () );
    const GXFloat* biasData = static_cast<GXFloat*> ( _bias.GetData () );

    for ( GXUInt i = 0u; i < _constraints; ++i )
    {
        etaData[ i ] = inverseDeltaTime * biasData[ i ] - ( alpha0.DotProduct ( GetJacobianElement ( i, 0u ) ) + alpha1.DotProduct ( GetJacobianElement ( i, 1u ) ) );
    }
}

GXVoid GXPairBodyConstraintSolver::UpdateA ()
{
    const GXFloat* lambdaData = static_cast<const GXFloat*> ( _lambda.GetData () );
    memset ( _a, 0, 2u * sizeof ( GXVec6 ) );

    GXVec6& a0 = _a[ 0u ];
    GXVec6& a1 = _a[ 1u ];

    for ( GXUInt i = 0u; i < _constraints; ++i )
    {
        GXFloat alpha = lambdaData[ i ];
        a0.Sum ( a0, alpha, GetBElement ( 0u, i ) );
        a1.Sum ( a1, alpha, GetBElement ( 1u, i ) );
    }
}

GXVoid GXPairBodyConstraintSolver::UpdateBodyVelocities ()
{
    const GXFloat* lambdaData = static_cast<const GXFloat*> ( _lambda.GetData () );

    const GXFloat firstInverseMass = _firstBody->GetInverseMass ();
    const GXMat3& firstInverseInertia = _firstBody->GetInverseInertiaTensorWorld ();

    const GXFloat secondInverseMass = _secondBody->GetInverseMass ();
    const GXMat3& secondInverseInertia = _secondBody->GetInverseInertiaTensorWorld ();

    GXVec6 alpha[ 2u ];
    GXVec6& alpha0 = alpha[ 0u ];
    GXVec6& alpha1 = alpha[ 1u ];
    memset ( alpha, 0, 2u * sizeof ( GXVec6 ) );

    for ( GXUInt i = 0u; i < _constraints; ++i )
    {
        const GXFloat yotta = lambdaData[ i ];

        alpha0.Sum ( alpha0, yotta, GetJacobianElement ( i, 0u ) );
        alpha1.Sum ( alpha1, yotta, GetJacobianElement ( i, 1u ) );
    }

    GXVec6 betta[ 2u ];
    GXVec6& betta0 = betta[ 0u ];
    GXVec6& betta1 = betta[ 1u ];

    betta[ 0u ].From ( _firstBody->GetTotalForce (), _firstBody->GetTotalTorque () );
    betta[ 1u ].From ( _secondBody->GetTotalForce (), _secondBody->GetTotalTorque () );

    alpha0.Sum ( alpha0, betta0 );
    alpha1.Sum ( alpha1, betta1 );

    betta0._data[ 0u ] = firstInverseMass * alpha0._data[ 0u ];
    betta0._data[ 1u ] = firstInverseMass * alpha0._data[ 1u ];
    betta0._data[ 2u ] = firstInverseMass * alpha0._data[ 2u ];
    betta0._data[ 3u ] = firstInverseInertia._m[ 0u ][ 0u ] * alpha0._data[ 3u ] + firstInverseInertia._m[ 0u ][ 1u ] * alpha0._data[ 4u ] + firstInverseInertia._m[ 0u ][ 2u ] * alpha0._data[ 5u ];
    betta0._data[ 4u ] = firstInverseInertia._m[ 1u ][ 0u ] * alpha0._data[ 3u ] + firstInverseInertia._m[ 1u ][ 1u ] * alpha0._data[ 4u ] + firstInverseInertia._m[ 1u ][ 2u ] * alpha0._data[ 5u ];
    betta0._data[ 5u ] = firstInverseInertia._m[ 2u ][ 0u ] * alpha0._data[ 3u ] + firstInverseInertia._m[ 2u ][ 1u ] * alpha0._data[ 4u ] + firstInverseInertia._m[ 2u ][ 2u ] * alpha0._data[ 5u ];

    betta1._data[ 0u ] = secondInverseMass * alpha1._data[ 0u ];
    betta1._data[ 1u ] = secondInverseMass * alpha1._data[ 1u ];
    betta1._data[ 2u ] = secondInverseMass * alpha1._data[ 2u ];
    betta1._data[ 3u ] = secondInverseInertia._m[ 0u ][ 0u ] * alpha1._data[ 3u ] + secondInverseInertia._m[ 0u ][ 1u ] * alpha1._data[ 4u ] + secondInverseInertia._m[ 0u ][ 2u ] * alpha1._data[ 5u ];
    betta1._data[ 4u ] = secondInverseInertia._m[ 1u ][ 0u ] * alpha1._data[ 3u ] + secondInverseInertia._m[ 1u ][ 1u ] * alpha1._data[ 4u ] + secondInverseInertia._m[ 1u ][ 2u ] * alpha1._data[ 5u ];
    betta1._data[ 5u ] = secondInverseInertia._m[ 2u ][ 0u ] * alpha1._data[ 3u ] + secondInverseInertia._m[ 2u ][ 1u ] * alpha1._data[ 4u ] + secondInverseInertia._m[ 2u ][ 2u ] * alpha1._data[ 5u ];

    const GXFloat deltaTime = GXPhysicsEngine::GetInstance ().GetTimeStep ();

    betta0.Multiply ( betta0, deltaTime );
    betta1.Multiply ( betta1, deltaTime );

    alpha0.From ( _firstBody->GetLinearVelocity (), _firstBody->GetAngularVelocity () );
    alpha1.From ( _secondBody->GetLinearVelocity (), _secondBody->GetAngularVelocity () );

    alpha0.Sum ( alpha0, betta0 );
    alpha1.Sum ( alpha1, betta1 );

    _firstBody->SetLinearVelocity ( GXVec3 ( alpha0._data[ 0u ], alpha0._data[ 1u ], alpha0._data[ 2u ] ) );
    _firstBody->SetAngularVelocity ( GXVec3 ( alpha0._data[ 3u ], alpha0._data[ 4u ], alpha0._data[ 5u ] ) );

    _secondBody->SetLinearVelocity ( GXVec3 ( alpha1._data[ 0u ], alpha1._data[ 1u ], alpha1._data[ 2u ] ) );
    _secondBody->SetAngularVelocity ( GXVec3 ( alpha1._data[ 3u ], alpha1._data[ 4u ], alpha1._data[ 5u ] ) );
}

const GXVec6& GXPairBodyConstraintSolver::GetJacobianElement ( GXUInt constraint, GXUByte bodyIndex ) const
{
    const GXVec6* elements = static_cast<const GXVec6*> ( _jacobian.GetData () );
    return elements[ constraint * ELEMENTS_PER_CONSTRAINT + static_cast<GXUInt> ( bodyIndex ) ];
}

const GXVec6& GXPairBodyConstraintSolver::GetBElement ( GXUByte bodyIndex, GXUInt constraint ) const
{
    const GXVec6* elements = static_cast<const GXVec6*> ( _b.GetData () );
    return elements[ constraint * ELEMENTS_PER_CONSTRAINT + static_cast<GXUInt> ( bodyIndex ) ];
}
