// version 1.2

#include <GXPhysics/GXPairBodyConstraintSolver.h>
#include <GXPhysics/GXPhysicsEngine.h>


#define DEFAULT_INITIAL_LAMBDA				0.0f
#define CONSTRAINT_STORAGE_GROW_FACTOR		32u
#define ELEMENTS_PER_CONSTRAINT				2u

//---------------------------------------------------------------------------------------------------------------------

GXPairBodyConstraintSolver::GXPairBodyConstraintSolver ( GXUShort maximumIterations ):
	jacobian ( sizeof ( GXVec6 ) ),
	bias ( sizeof ( GXFloat ) ),
	lambdaRange ( sizeof ( GXVec2 ) ),
	initialLambda ( sizeof ( GXFloat ) ),
	lambda ( sizeof ( GXFloat ) ),
	eta ( sizeof ( GXFloat ) ),
	d ( sizeof ( GXVec6 ) ),
	b ( sizeof ( GXVec6 ) )
{
	this->maximumIterations = maximumIterations;
	constraints = 0u;
	firstBody = nullptr;
	secondBody = nullptr;
}

GXPairBodyConstraintSolver::~GXPairBodyConstraintSolver ()
{
	// NOTHING
}

GXVoid GXPairBodyConstraintSolver::Begin ( GXRigidBody &first, GXRigidBody &second )
{
	firstBody = &first;
	secondBody = &second;

	constraints = 0u;
}

GXVoid GXPairBodyConstraintSolver::AddConstraint ( const GXConstraint& constraint )
{
	if ( constraints >= lambda.GetLength () )
	{
		GXUInt newCapacity = constraints + CONSTRAINT_STORAGE_GROW_FACTOR;

		bias.Resize ( newCapacity );
		lambdaRange.Resize ( newCapacity );
		initialLambda.Resize ( newCapacity );
		lambda.Resize ( newCapacity );
		eta.Resize ( newCapacity );
		d.Resize ( newCapacity );

		GXUInt alpha = newCapacity * ELEMENTS_PER_CONSTRAINT;
		jacobian.Resize ( alpha );
		b.Resize ( alpha );
	}

	GXUInt offset = constraints * ELEMENTS_PER_CONSTRAINT;

	GXVec6* jacobianData = static_cast<GXVec6*> ( jacobian.GetData () );
	memcpy ( jacobianData + offset, constraint.GetJacobian (), ELEMENTS_PER_CONSTRAINT * sizeof ( GXVec6 ) );

	GXFloat* biasData = static_cast<GXFloat*> ( bias.GetData () );
	biasData[ constraints ] = constraint.GetBias ();

	GXVec2* lambdaRangeData = static_cast<GXVec2*> ( lambdaRange.GetData () );
	lambdaRangeData[ constraints ] = constraint.GetLambdaRange ();

	GXFloat* initialLambdaData = static_cast<GXFloat*> ( initialLambda.GetData () );
	initialLambdaData[ constraints ] = DEFAULT_INITIAL_LAMBDA;

	++constraints;
}

GXVoid GXPairBodyConstraintSolver::End ()
{
	UpdateB ();
	UpdateEta ();

	const GXFloat* initialLambdaData = static_cast<const GXFloat*> ( initialLambda.GetData () );
	GXFloat* lambdaData = static_cast<GXFloat*> ( lambda.GetData () );
	memcpy ( lambdaData, initialLambdaData, constraints * sizeof ( GXFloat ) );

	UpdateA ();

	GXFloat* dData = static_cast<GXFloat*> ( d.GetData () );

	for ( GXUInt i = 0u; i < constraints; ++i )
	{
		const GXVec6& j0 = GetJacobianElement ( i, 0u );
		const GXVec6& j1 = GetJacobianElement ( i, 1u );
		dData[ i ] = j0.DotProduct ( GetBElement ( 0u, i ) ) + j1.DotProduct ( GetBElement ( 1u, i ) );
	}

	const GXFloat* etaData = static_cast<const GXFloat*> ( eta.GetData () );
	const GXVec2* lambdaRangeData = static_cast<const GXVec2*> ( lambdaRange.GetData () );

	for ( GXUInt iteration = 0u; iteration < maximumIterations; ++iteration )
	{
		for ( GXUInt i = 0u; i < constraints; ++i )
		{
			GXFloat deltaLambda = ( etaData[ i ] - a[ 0 ].DotProduct ( GetJacobianElement ( i, 0u ) ) - a[ 1 ].DotProduct ( GetJacobianElement ( i, 1u ) ) ) / dData[ i ];
			GXFloat oldLambda = lambdaData[ i ];

			lambdaData[ i ] = GXMaxf ( lambdaRangeData[ i ]._data[ 0 ], GXMinf ( lambdaData[ i ] + deltaLambda, lambdaRangeData[ i ]._data[ 1 ] ) );
			deltaLambda = lambdaData[ i ] - oldLambda;

			GXVec6 alpha;
			alpha.Multiply ( GetBElement ( 0u, i ), deltaLambda );
			a[ 0 ].Sum ( a[ 0 ], alpha );

			alpha.Multiply ( GetBElement ( 1u, i ), deltaLambda );
			a[ 1 ].Sum ( a[ 1 ], alpha );
		}
	}

	UpdateBodyVelocities ();

	firstBody = secondBody = nullptr;
	constraints = 0u;
}

GXVoid GXPairBodyConstraintSolver::UpdateB ()
{
	GXVec6* bData = static_cast<GXVec6*> ( b.GetData () );

	GXFloat firstInverseMass = firstBody->GetInverseMass ();
	const GXMat3& firstInverseInertia = firstBody->GetInverseInertiaTensorWorld ();

	GXFloat secondInverseMass = secondBody->GetInverseMass ();
	const GXMat3& secondInverseInertia = secondBody->GetInverseInertiaTensorWorld ();

	for ( GXUInt i = 0u; i < constraints; ++i )
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
	GXVec6 alpha[ 2 ];
	GXVec6& alpha0 = alpha[ 0 ];
	GXVec6& alpha1 = alpha[ 1 ];

	alpha0.From ( firstBody->GetLinearVelocity (), firstBody->GetAngularVelocity () );
	alpha1.From ( secondBody->GetLinearVelocity (), secondBody->GetAngularVelocity () );

	GXFloat inverseDeltaTime = 1.0f / GXPhysicsEngine::GetInstance ().GetTimeStep ();

	alpha0.Multiply ( alpha0, inverseDeltaTime );
	alpha1.Multiply ( alpha1, inverseDeltaTime );

	GXVec6 betta[ 2 ];
	GXVec6& betta0 = betta[ 0 ];
	GXVec6& betta1 = betta[ 1 ];

	GXFloat firstInverseMass = firstBody->GetInverseMass ();
	const GXMat3& firstInverseInertia = firstBody->GetInverseInertiaTensorWorld ();
	const GXVec3& firstForce = firstBody->GetTotalForce ();
	const GXVec3& firstTorque = firstBody->GetTotalTorque ();

	GXFloat secondInverseMass = secondBody->GetInverseMass ();
	const GXMat3& secondInverseInertia = secondBody->GetInverseInertiaTensorWorld ();
	const GXVec3& secondForce = secondBody->GetTotalForce ();
	const GXVec3& secondTorque = secondBody->GetTotalTorque ();

	betta0._data[ 0 ] = firstInverseMass * firstForce._data[ 0 ];
	betta0._data[ 1 ] = firstInverseMass * firstForce._data[ 1 ];
	betta0._data[ 2 ] = firstInverseMass * firstForce._data[ 2 ];
	betta0._data[ 3 ] = firstInverseInertia._m[ 0 ][ 0 ] * firstTorque._data[ 0 ] + firstInverseInertia._m[ 0 ][ 1 ] * firstTorque._data[ 1 ] + firstInverseInertia._m[ 0 ][ 2 ] * firstTorque._data[ 2 ];
	betta0._data[ 4 ] = firstInverseInertia._m[ 1 ][ 0 ] * firstTorque._data[ 0 ] + firstInverseInertia._m[ 1 ][ 1 ] * firstTorque._data[ 1 ] + firstInverseInertia._m[ 1 ][ 2 ] * firstTorque._data[ 2 ];
	betta0._data[ 5 ] = firstInverseInertia._m[ 2 ][ 0 ] * firstTorque._data[ 0 ] + firstInverseInertia._m[ 2 ][ 1 ] * firstTorque._data[ 1 ] + firstInverseInertia._m[ 2 ][ 2 ] * firstTorque._data[ 2 ];

	betta1._data[ 0 ] = secondInverseMass * secondForce._data[ 0 ];
	betta1._data[ 1 ] = secondInverseMass * secondForce._data[ 1 ];
	betta1._data[ 2 ] = secondInverseMass * secondForce._data[ 2 ];
	betta1._data[ 3 ] = secondInverseInertia._m[ 0 ][ 0 ] * secondTorque._data[ 0 ] + secondInverseInertia._m[ 0 ][ 1 ] * secondTorque._data[ 1 ] + secondInverseInertia._m[ 0 ][ 2 ] * secondTorque._data[ 2 ];
	betta1._data[ 4 ] = secondInverseInertia._m[ 1 ][ 0 ] * secondTorque._data[ 0 ] + secondInverseInertia._m[ 1 ][ 1 ] * secondTorque._data[ 1 ] + secondInverseInertia._m[ 1 ][ 2 ] * secondTorque._data[ 2 ];
	betta1._data[ 5 ] = secondInverseInertia._m[ 2 ][ 0 ] * secondTorque._data[ 0 ] + secondInverseInertia._m[ 2 ][ 1 ] * secondTorque._data[ 1 ] + secondInverseInertia._m[ 2 ][ 2 ] * secondTorque._data[ 2 ];

	alpha0.Sum ( alpha0, betta0 );
	alpha1.Sum ( alpha1, betta1 );

	GXFloat* etaData = static_cast<GXFloat*> ( eta.GetData () );
	const GXFloat* biasData = static_cast<GXFloat*> ( bias.GetData () );

	for ( GXUInt i = 0u; i < constraints; ++i )
		etaData[ i ] = inverseDeltaTime * biasData[ i ] - ( alpha0.DotProduct ( GetJacobianElement ( i, 0u ) ) + alpha1.DotProduct ( GetJacobianElement ( i, 1u ) ) );
}

GXVoid GXPairBodyConstraintSolver::UpdateA ()
{
	const GXFloat* lambdaData = static_cast<const GXFloat*> ( lambda.GetData () );
	memset ( a, 0, 2 * sizeof ( GXVec6 ) );

	GXVec6& a0 = a[ 0 ];
	GXVec6& a1 = a[ 1 ];

	for ( GXUInt i = 0u; i < constraints; ++i )
	{
		GXFloat alpha = lambdaData[ i ];
		a0.Sum ( a0, alpha, GetBElement ( 0u, i ) );
		a1.Sum ( a1, alpha, GetBElement ( 1u, i ) );
	}
}

GXVoid GXPairBodyConstraintSolver::UpdateBodyVelocities ()
{
	const GXFloat* lambdaData = static_cast<const GXFloat*> ( lambda.GetData () );

	GXFloat firstInverseMass = firstBody->GetInverseMass ();
	const GXMat3& firstInverseInertia = firstBody->GetInverseInertiaTensorWorld ();

	GXFloat secondInverseMass = secondBody->GetInverseMass ();
	const GXMat3& secondInverseInertia = secondBody->GetInverseInertiaTensorWorld ();

	GXVec6 alpha[ 2u ];
	GXVec6& alpha0 = alpha[ 0u ];
	GXVec6& alpha1 = alpha[ 1u ];
	memset ( alpha, 0, 2u * sizeof ( GXVec6 ) );

	for ( GXUInt i = 0u; i < constraints; ++i )
	{
		GXFloat yotta = lambdaData[ i ];

		alpha0.Sum ( alpha0, yotta, GetJacobianElement ( i, 0u ) );
		alpha1.Sum ( alpha1, yotta, GetJacobianElement ( i, 1u ) );
	}

	GXVec6 betta[ 2u ];
	GXVec6& betta0 = betta[ 0u ];
	GXVec6& betta1 = betta[ 1u ];

	betta[ 0u ].From ( firstBody->GetTotalForce (), firstBody->GetTotalTorque () );
	betta[ 1u ].From ( secondBody->GetTotalForce (), secondBody->GetTotalTorque () );

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

	GXFloat deltaTime = GXPhysicsEngine::GetInstance ().GetTimeStep ();

	betta0.Multiply ( betta0, deltaTime );
	betta1.Multiply ( betta1, deltaTime );

	alpha0.From ( firstBody->GetLinearVelocity (), firstBody->GetAngularVelocity () );
	alpha1.From ( secondBody->GetLinearVelocity (), secondBody->GetAngularVelocity () );

	alpha0.Sum ( alpha0, betta0 );
	alpha1.Sum ( alpha1, betta1 );

	firstBody->SetLinearVelocity ( GXVec3 ( alpha0._data[ 0u ], alpha0._data[ 1u ], alpha0._data[ 2u ] ) );
	firstBody->SetAngularVelocity ( GXVec3 ( alpha0._data[ 3u ], alpha0._data[ 4u ], alpha0._data[ 5u ] ) );

	secondBody->SetLinearVelocity ( GXVec3 ( alpha1._data[ 0u ], alpha1._data[ 1u ], alpha1._data[ 2u ] ) );
	secondBody->SetAngularVelocity ( GXVec3 ( alpha1._data[ 3u ], alpha1._data[ 4u ], alpha1._data[ 5u ] ) );
}

const GXVec6& GXPairBodyConstraintSolver::GetJacobianElement ( GXUInt constraint, GXUByte bodyIndex ) const
{
	const GXVec6* elements = static_cast<const GXVec6*> ( jacobian.GetData () );
	return elements[ constraint * ELEMENTS_PER_CONSTRAINT + static_cast<GXUInt> ( bodyIndex ) ];
}

const GXVec6& GXPairBodyConstraintSolver::GetBElement ( GXUByte bodyIndex, GXUInt constraint ) const
{
	const GXVec6* elements = static_cast<const GXVec6*> ( b.GetData () );
	return elements[ constraint * ELEMENTS_PER_CONSTRAINT + static_cast<GXUInt> ( bodyIndex ) ];
}
