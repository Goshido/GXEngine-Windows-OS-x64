// version 1.0

#include <GXPhysics/GXPairBodyConstraintSolver.h>
#include <GXPhysics/GXPhysicsEngine.h>


#define CONSTRAINT_STORAGE_GROW_FACTOR		32
#define ELEMENTS_PER_CONSTRAINT				2
#define DEFAULT_INITIAL_LAMBDA				0.0f


GXPairBodyConstraintSolver::GXPairBodyConstraintSolver ( GXUShort maximumIterations ) :
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
	constraints = 0;
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

	constraints = 0;
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

	GXVec6* jacobianData = (GXVec6*)jacobian.GetData ();
	memcpy ( jacobianData + offset, constraint.GetJacobian (), ELEMENTS_PER_CONSTRAINT * sizeof ( GXVec6 ) );

	GXFloat* biasData = (GXFloat*)bias.GetData ();
	biasData[ constraints ] = constraint.GetBias ();

	GXVec2* lambdaRangeData = (GXVec2*)lambdaRange.GetData ();
	lambdaRangeData[ constraints ] = constraint.GetLambdaRange ();

	GXFloat* initialLambdaData = (GXFloat*)initialLambda.GetData ();
	initialLambdaData[ constraints ] = DEFAULT_INITIAL_LAMBDA;

	constraints++;
}

GXVoid GXPairBodyConstraintSolver::End ()
{
	UpdateB ();
	UpdateEta ();

	const GXFloat* initialLambdaData = (const GXFloat*)initialLambda.GetData ();
	GXFloat* lambdaData = (GXFloat*)lambda.GetData ();
	memcpy ( lambdaData, initialLambdaData, constraints * sizeof ( GXFloat ) );

	UpdateA ();

	GXFloat* dData = (GXFloat*)d.GetData ();

	for ( GXUInt i = 0u; i < constraints; i++ )
	{
		const GXVec6& j0 = GetJacobianElement ( i, 0 );
		const GXVec6& j1 = GetJacobianElement ( i, 1 );
		dData[ i ] = j0.DotProduct ( GetBElement ( 0, i ) ) + j1.DotProduct ( GetBElement ( 1, i ) );
	}

	const GXFloat* etaData = (const GXFloat*)eta.GetData ();
	const GXVec2* lambdaRangeData = (const GXVec2*)lambdaRange.GetData ();

	for ( GXUInt iteration = 0u; iteration < maximumIterations; iteration++ )
	{
		for ( GXUInt i = 0u; i < constraints; i++ )
		{
			GXFloat deltaLambda = ( etaData[ i ] - a[ 0 ].DotProduct ( GetJacobianElement ( i, 0 ) ) - a[ 1 ].DotProduct ( GetJacobianElement ( i, 1 ) ) ) / dData[ i ];
			GXFloat oldLambda = lambdaData[ i ];

			lambdaData[ i ] = GXMaxf ( lambdaRangeData[ i ].data[ 0 ], GXMinf ( lambdaData[ i ] + deltaLambda, lambdaRangeData[ i ].data[ 1 ] ) );
			deltaLambda = lambdaData[ i ] - oldLambda;

			GXVec6 alpha;
			alpha.Multiply ( GetBElement ( 0, i ), deltaLambda );
			a[ 0 ].Sum ( a[ 0 ], alpha );

			alpha.Multiply ( GetBElement ( 1, i ), deltaLambda );
			a[ 1 ].Sum ( a[ 1 ], alpha );
		}
	}

	UpdateBodyVelocities ();

	firstBody = secondBody = nullptr;
	constraints = 0;
}

GXVoid GXPairBodyConstraintSolver::UpdateB ()
{
	GXVec6* bData = (GXVec6*)b.GetData ();

	GXFloat firstInverseMass = firstBody->GetInverseMass ();
	const GXMat3& firstInverseInertia = firstBody->GetInverseInertiaTensorWorld ();

	GXFloat secondInverseMass = secondBody->GetInverseMass ();
	const GXMat3& secondInverseInertia = secondBody->GetInverseInertiaTensorWorld ();

	for ( GXUInt i = 0u; i < constraints; i++ )
	{
		const GXVec6& j0 = GetJacobianElement ( i, 0 );

		bData->data[ 0 ] = firstInverseMass * j0.data[ 0 ];
		bData->data[ 1 ] = firstInverseMass * j0.data[ 1 ];
		bData->data[ 2 ] = firstInverseMass * j0.data[ 2 ];
		bData->data[ 3 ] = firstInverseInertia.m[ 0 ][ 0 ] * j0.data[ 3 ] + firstInverseInertia.m[ 0 ][ 1 ] * j0.data[ 4 ] + firstInverseInertia.m[ 0 ][ 2 ] * j0.data[ 5 ];
		bData->data[ 4 ] = firstInverseInertia.m[ 1 ][ 0 ] * j0.data[ 3 ] + firstInverseInertia.m[ 1 ][ 1 ] * j0.data[ 4 ] + firstInverseInertia.m[ 1 ][ 2 ] * j0.data[ 5 ];
		bData->data[ 5 ] = firstInverseInertia.m[ 2 ][ 0 ] * j0.data[ 3 ] + firstInverseInertia.m[ 2 ][ 1 ] * j0.data[ 4 ] + firstInverseInertia.m[ 2 ][ 2 ] * j0.data[ 5 ];

		const GXVec6& j1 = GetJacobianElement ( i, 1 );
		bData++;

		bData->data[ 0 ] = secondInverseMass * j1.data[ 0 ];
		bData->data[ 1 ] = secondInverseMass * j1.data[ 1 ];
		bData->data[ 2 ] = secondInverseMass * j1.data[ 2 ];
		bData->data[ 3 ] = secondInverseInertia.m[ 0 ][ 0 ] * j1.data[ 3 ] + secondInverseInertia.m[ 0 ][ 1 ] * j1.data[ 4 ] + secondInverseInertia.m[ 0 ][ 2 ] * j1.data[ 5 ];
		bData->data[ 4 ] = secondInverseInertia.m[ 1 ][ 0 ] * j1.data[ 3 ] + secondInverseInertia.m[ 1 ][ 1 ] * j1.data[ 4 ] + secondInverseInertia.m[ 1 ][ 2 ] * j1.data[ 5 ];
		bData->data[ 5 ] = secondInverseInertia.m[ 2 ][ 0 ] * j1.data[ 3 ] + secondInverseInertia.m[ 2 ][ 1 ] * j1.data[ 4 ] + secondInverseInertia.m[ 2 ][ 2 ] * j1.data[ 5 ];

		bData++;
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

	betta0.data[ 0 ] = firstInverseMass * firstForce.data[ 0 ];
	betta0.data[ 1 ] = firstInverseMass * firstForce.data[ 1 ];
	betta0.data[ 2 ] = firstInverseMass * firstForce.data[ 2 ];
	betta0.data[ 3 ] = firstInverseInertia.m[ 0 ][ 0 ] * firstTorque.data[ 0 ] + firstInverseInertia.m[ 0 ][ 1 ] * firstTorque.data[ 1 ] + firstInverseInertia.m[ 0 ][ 2 ] * firstTorque.data[ 2 ];
	betta0.data[ 4 ] = firstInverseInertia.m[ 1 ][ 0 ] * firstTorque.data[ 0 ] + firstInverseInertia.m[ 1 ][ 1 ] * firstTorque.data[ 1 ] + firstInverseInertia.m[ 1 ][ 2 ] * firstTorque.data[ 2 ];
	betta0.data[ 5 ] = firstInverseInertia.m[ 2 ][ 0 ] * firstTorque.data[ 0 ] + firstInverseInertia.m[ 2 ][ 1 ] * firstTorque.data[ 1 ] + firstInverseInertia.m[ 2 ][ 2 ] * firstTorque.data[ 2 ];

	betta1.data[ 0 ] = secondInverseMass * secondForce.data[ 0 ];
	betta1.data[ 1 ] = secondInverseMass * secondForce.data[ 1 ];
	betta1.data[ 2 ] = secondInverseMass * secondForce.data[ 2 ];
	betta1.data[ 3 ] = secondInverseInertia.m[ 0 ][ 0 ] * secondTorque.data[ 0 ] + secondInverseInertia.m[ 0 ][ 1 ] * secondTorque.data[ 1 ] + secondInverseInertia.m[ 0 ][ 2 ] * secondTorque.data[ 2 ];
	betta1.data[ 4 ] = secondInverseInertia.m[ 1 ][ 0 ] * secondTorque.data[ 0 ] + secondInverseInertia.m[ 1 ][ 1 ] * secondTorque.data[ 1 ] + secondInverseInertia.m[ 1 ][ 2 ] * secondTorque.data[ 2 ];
	betta1.data[ 5 ] = secondInverseInertia.m[ 2 ][ 0 ] * secondTorque.data[ 0 ] + secondInverseInertia.m[ 2 ][ 1 ] * secondTorque.data[ 1 ] + secondInverseInertia.m[ 2 ][ 2 ] * secondTorque.data[ 2 ];

	alpha0.Sum ( alpha0, betta0 );
	alpha1.Sum ( alpha1, betta1 );

	GXFloat* etaData = (GXFloat*)eta.GetData ();
	const GXFloat* biasData = (GXFloat*)bias.GetData ();

	for ( GXUInt i = 0u; i < constraints; i++ )
		etaData[ i ] = inverseDeltaTime * biasData[ i ] - ( alpha0.DotProduct ( GetJacobianElement ( i, 0 ) ) + alpha1.DotProduct ( GetJacobianElement ( i, 1 ) ) );
}

GXVoid GXPairBodyConstraintSolver::UpdateA ()
{
	const GXFloat* lambdaData = (const GXFloat*)lambda.GetData ();
	memset ( a, 0, 2 * sizeof ( GXVec6 ) );

	GXVec6& a0 = a[ 0 ];
	GXVec6& a1 = a[ 1 ];

	for ( GXUInt i = 0u; i < constraints; i++ )
	{
		const GXVec6& b0 = GetBElement ( 0, i );

		a0.data[ 0 ] += b0.data[ 0 ] * lambdaData[ i ];
		a0.data[ 1 ] += b0.data[ 1 ] * lambdaData[ i ];
		a0.data[ 2 ] += b0.data[ 2 ] * lambdaData[ i ];
		a0.data[ 3 ] += b0.data[ 3 ] * lambdaData[ i ];
		a0.data[ 4 ] += b0.data[ 4 ] * lambdaData[ i ];
		a0.data[ 5 ] += b0.data[ 5 ] * lambdaData[ i ];

		const GXVec6& b1 = GetBElement ( 1, i );

		a1.data[ 0 ] += b1.data[ 0 ] * lambdaData[ i ];
		a1.data[ 1 ] += b1.data[ 1 ] * lambdaData[ i ];
		a1.data[ 2 ] += b1.data[ 2 ] * lambdaData[ i ];
		a1.data[ 3 ] += b1.data[ 3 ] * lambdaData[ i ];
		a1.data[ 4 ] += b1.data[ 4 ] * lambdaData[ i ];
		a1.data[ 5 ] += b1.data[ 5 ] * lambdaData[ i ];
	}
}

GXVoid GXPairBodyConstraintSolver::UpdateBodyVelocities ()
{
	const GXFloat* lambdaData = (const GXFloat*)lambda.GetData ();

	GXFloat firstInverseMass = firstBody->GetInverseMass ();
	const GXMat3& firstInverseInertia = firstBody->GetInverseInertiaTensorWorld ();

	GXFloat secondInverseMass = secondBody->GetInverseMass ();
	const GXMat3& secondInverseInertia = secondBody->GetInverseInertiaTensorWorld ();

	GXVec6 alpha[ 2 ];
	GXVec6& alpha0 = alpha[ 0 ];
	GXVec6& alpha1 = alpha[ 1 ];
	memset ( alpha, 0, 2 * sizeof ( GXVec6 ) );

	for ( GXUInt i = 0u; i < constraints; i++ )
	{
		const GXVec6& j0 = GetJacobianElement ( i, 0 );

		alpha0.data[ 0 ] += j0.data[ 0 ] * lambdaData[ i ];
		alpha0.data[ 1 ] += j0.data[ 1 ] * lambdaData[ i ];
		alpha0.data[ 2 ] += j0.data[ 2 ] * lambdaData[ i ];
		alpha0.data[ 3 ] += j0.data[ 3 ] * lambdaData[ i ];
		alpha0.data[ 4 ] += j0.data[ 4 ] * lambdaData[ i ];
		alpha0.data[ 5 ] += j0.data[ 5 ] * lambdaData[ i ];

		const GXVec6& j1 = GetJacobianElement ( i, 1 );

		alpha1.data[ 0 ] += j1.data[ 0 ] * lambdaData[ i ];
		alpha1.data[ 1 ] += j1.data[ 1 ] * lambdaData[ i ];
		alpha1.data[ 2 ] += j1.data[ 2 ] * lambdaData[ i ];
		alpha1.data[ 3 ] += j1.data[ 3 ] * lambdaData[ i ];
		alpha1.data[ 4 ] += j1.data[ 4 ] * lambdaData[ i ];
		alpha1.data[ 5 ] += j1.data[ 5 ] * lambdaData[ i ];
	}

	GXVec6 betta[ 2 ];
	GXVec6& betta0 = betta[ 0 ];
	GXVec6& betta1 = betta[ 1 ];

	betta[ 0 ].From ( firstBody->GetTotalForce (), firstBody->GetTotalTorque () );
	betta[ 1 ].From ( secondBody->GetTotalForce (), secondBody->GetTotalTorque () );

	alpha0.Sum ( alpha0, betta[ 0 ] );
	alpha1.Sum ( alpha1, betta[ 1 ] );

	betta0.data[ 0 ] = firstInverseMass * alpha0.data[ 0 ];
	betta0.data[ 1 ] = firstInverseMass * alpha0.data[ 1 ];
	betta0.data[ 2 ] = firstInverseMass * alpha0.data[ 2 ];
	betta0.data[ 3 ] = firstInverseInertia.m[ 0 ][ 0 ] * alpha0.data[ 3 ] + firstInverseInertia.m[ 0 ][ 1 ] * alpha0.data[ 4 ] + firstInverseInertia.m[ 0 ][ 2 ] * alpha0.data[ 5 ];
	betta0.data[ 4 ] = firstInverseInertia.m[ 1 ][ 0 ] * alpha0.data[ 3 ] + firstInverseInertia.m[ 1 ][ 1 ] * alpha0.data[ 4 ] + firstInverseInertia.m[ 1 ][ 2 ] * alpha0.data[ 5 ];
	betta0.data[ 5 ] = firstInverseInertia.m[ 2 ][ 0 ] * alpha0.data[ 3 ] + firstInverseInertia.m[ 2 ][ 1 ] * alpha0.data[ 4 ] + firstInverseInertia.m[ 2 ][ 2 ] * alpha0.data[ 5 ];

	betta1.data[ 0 ] = secondInverseMass * alpha1.data[ 0 ];
	betta1.data[ 1 ] = secondInverseMass * alpha1.data[ 1 ];
	betta1.data[ 2 ] = secondInverseMass * alpha1.data[ 2 ];
	betta1.data[ 3 ] = secondInverseInertia.m[ 0 ][ 0 ] * alpha1.data[ 3 ] + secondInverseInertia.m[ 0 ][ 1 ] * alpha1.data[ 4 ] + secondInverseInertia.m[ 0 ][ 2 ] * alpha1.data[ 5 ];
	betta1.data[ 4 ] = secondInverseInertia.m[ 1 ][ 0 ] * alpha1.data[ 3 ] + secondInverseInertia.m[ 1 ][ 1 ] * alpha1.data[ 4 ] + secondInverseInertia.m[ 1 ][ 2 ] * alpha1.data[ 5 ];
	betta1.data[ 5 ] = secondInverseInertia.m[ 2 ][ 0 ] * alpha1.data[ 3 ] + secondInverseInertia.m[ 2 ][ 1 ] * alpha1.data[ 4 ] + secondInverseInertia.m[ 2 ][ 2 ] * alpha1.data[ 5 ];

	GXFloat deltaTime = GXPhysicsEngine::GetInstance ().GetTimeStep ();

	betta0.Multiply ( betta0, deltaTime );
	betta1.Multiply ( betta1, deltaTime );

	alpha0.From ( firstBody->GetLinearVelocity (), firstBody->GetAngularVelocity () );
	alpha1.From ( secondBody->GetLinearVelocity (), secondBody->GetAngularVelocity () );

	alpha0.Sum ( alpha0, betta0 );
	alpha1.Sum ( alpha1, betta1 );

	firstBody->SetLinearVelocity ( GXVec3 ( alpha0.data[ 0 ], alpha0.data[ 1 ], alpha0.data[ 2 ] ) );
	firstBody->SetAngularVelocity ( GXVec3 ( alpha0.data[ 3 ], alpha0.data[ 4 ], alpha0.data[ 5 ] ) );

	secondBody->SetLinearVelocity ( GXVec3 ( alpha1.data[ 0 ], alpha1.data[ 1 ], alpha1.data[ 2 ] ) );
	secondBody->SetAngularVelocity ( GXVec3 ( alpha1.data[ 3 ], alpha1.data[ 4 ], alpha1.data[ 5 ] ) );
}

const GXVec6& GXPairBodyConstraintSolver::GetJacobianElement ( GXUInt constraint, GXUByte bodyIndex ) const
{
	const GXVec6* elements = (const GXVec6*)jacobian.GetData ();
	return elements[ constraint * ELEMENTS_PER_CONSTRAINT + (GXUInt)bodyIndex ];
}

const GXVec6& GXPairBodyConstraintSolver::GetBElement ( GXUByte bodyIndex, GXUInt constraint ) const
{
	const GXVec6* elements = (const GXVec6*)b.GetData ();
	return elements[ constraint * ELEMENTS_PER_CONSTRAINT + (GXUInt)bodyIndex ];
}
