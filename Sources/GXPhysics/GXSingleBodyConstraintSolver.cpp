// version 1.2

#include <GXPhysics/GXSingleBodyConstraintSolver.h>
#include <GXPhysics/GXPhysicsEngine.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_INITIAL_LAMBDA				0.0f
#define CONSTRAINT_STORAGE_GROW_FACTOR		32u
#define ELEMENTS_PER_CONSTRAINT				2u

//---------------------------------------------------------------------------------------------------------------------

GXSingleBodyConstraintSolver::GXSingleBodyConstraintSolver ( GXUShort maximumIterations ):
	jacobian (sizeof ( GXVec6 ) ),
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

GXSingleBodyConstraintSolver::~GXSingleBodyConstraintSolver ()
{
	// NOTHING
}

GXVoid GXSingleBodyConstraintSolver::Begin ( GXRigidBody &first, GXRigidBody &second )
{
	firstBody = &first;
	secondBody = &second;

	constraints = 0;
}

GXVoid GXSingleBodyConstraintSolver::AddConstraint ( const GXConstraint& constraint )
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
		b.Resize ( newCapacity );

		jacobian.Resize ( newCapacity * ELEMENTS_PER_CONSTRAINT );
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

GXVoid GXSingleBodyConstraintSolver::End ()
{
	UpdateB ();
	UpdateEta ();

	const GXFloat* initialLambdaData = static_cast<const GXFloat*> ( initialLambda.GetData () );
	GXFloat* lambdaData = (GXFloat*)lambda.GetData ();
	memcpy ( lambdaData, initialLambdaData, constraints * sizeof ( GXFloat ) );

	UpdateA ();

	GXFloat* dData = (GXFloat*)d.GetData ();
	const GXVec6* bData = static_cast<const GXVec6*> ( b.GetData () );

	for ( GXUInt i = 0u; i < constraints; ++i )
		dData[ i ] = bData[ i ].DotProduct ( GetJacobianElement ( i, 0 ) );

	const GXFloat* etaData = static_cast<const GXFloat*> ( eta.GetData () );
	const GXVec2* lambdaRangeData = static_cast<const GXVec2*> ( lambdaRange.GetData () );

	for ( GXUInt iteration = 0u; iteration < maximumIterations; ++iteration )
	{
		for ( GXUInt i = 0u; i < constraints; ++i )
		{
			GXFloat deltaLambda = ( etaData[ i ] - a.DotProduct ( GetJacobianElement ( i, 0 ) ) ) / dData[ i ];
			GXFloat oldLambda = lambdaData[ i ];

			lambdaData[ i ] = GXMaxf ( lambdaRangeData[ i ]._data[ 0 ], GXMinf ( lambdaData[ i ] + deltaLambda, lambdaRangeData[ i ]._data[ 1 ] ) );
			deltaLambda = lambdaData[ i ] - oldLambda;

			GXVec6 alpha;
			alpha.Multiply ( bData[ i ], deltaLambda );
			a.Sum ( a, alpha );
		}
	}

	UpdateBodyVelocity ();

	firstBody = secondBody = nullptr;
	constraints = 0;
}

GXVoid GXSingleBodyConstraintSolver::UpdateB ()
{
	GXVec6* bData = static_cast<GXVec6*> ( b.GetData () );

	GXFloat firstInverseMass = firstBody->GetInverseMass ();
	const GXMat3& firstInverseInertia = firstBody->GetInverseInertiaTensorWorld ();

	for ( GXUInt i = 0u; i < constraints; ++i )
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
	GXFloat inverseDeltaTime = 1.0f / GXPhysicsEngine::GetInstance ().GetTimeStep ();

	GXVec6 alpha[ 2 ];
	GXVec6& alpha0 = alpha[ 0 ];
	GXVec6& alpha1 = alpha[ 1 ];

	alpha0.From ( firstBody->GetLinearVelocity (), firstBody->GetAngularVelocity () );
	alpha1.From ( secondBody->GetLinearVelocity (), secondBody->GetAngularVelocity () );

	alpha0.Multiply ( alpha0, inverseDeltaTime );
	alpha1.Multiply ( alpha1, inverseDeltaTime );

	GXFloat firstInverseMass = firstBody->GetInverseMass ();
	const GXMat3& firstInverseInertia = firstBody->GetInverseInertiaTensorWorld ();
	const GXVec3& firstForce = firstBody->GetTotalForce ();
	const GXVec3& firstTorque = firstBody->GetTotalTorque ();

	GXVec6 betta;

	betta._data[ 0 ] = firstInverseMass * firstForce._data[ 0 ];
	betta._data[ 1 ] = firstInverseMass * firstForce._data[ 1 ];
	betta._data[ 2 ] = firstInverseMass * firstForce._data[ 2 ];
	betta._data[ 3 ] = firstInverseInertia._m[ 0 ][ 0 ] * firstTorque._data[ 0 ] + firstInverseInertia._m[ 0 ][ 1 ] * firstTorque._data[ 1 ] + firstInverseInertia._m[ 0 ][ 2 ] * firstTorque._data[ 2 ];
	betta._data[ 4 ] = firstInverseInertia._m[ 1 ][ 0 ] * firstTorque._data[ 0 ] + firstInverseInertia._m[ 1 ][ 1 ] * firstTorque._data[ 1 ] + firstInverseInertia._m[ 1 ][ 2 ] * firstTorque._data[ 2 ];
	betta._data[ 5 ] = firstInverseInertia._m[ 2 ][ 0 ] * firstTorque._data[ 0 ] + firstInverseInertia._m[ 2 ][ 1 ] * firstTorque._data[ 1 ] + firstInverseInertia._m[ 2 ][ 2 ] * firstTorque._data[ 2 ];

	alpha0.Sum ( alpha0, betta );

	GXFloat* etaData = static_cast<GXFloat*> ( eta.GetData () );
	const GXFloat* biasData = static_cast<GXFloat*> ( bias.GetData () );

	for ( GXUInt i = 0u; i < constraints; ++i )
		etaData[ i ] = inverseDeltaTime * biasData[ i ] - ( alpha0.DotProduct ( GetJacobianElement ( i, 0 ) ) + alpha1.DotProduct ( GetJacobianElement ( i, 1 ) ) );
}

GXVoid GXSingleBodyConstraintSolver::UpdateA ()
{
	const GXFloat* lambdaData = static_cast<const GXFloat*> ( lambda.GetData () );
	const GXVec6* bData = (const GXVec6*)b.GetData ();

	memset ( &a, 0, sizeof ( GXVec6 ) );

	for ( GXUInt i = 0u; i < constraints; ++i )
		a.Sum ( a, lambdaData[ i ], bData[ i ] );
}

GXVoid GXSingleBodyConstraintSolver::UpdateBodyVelocity ()
{
	const GXFloat* lambdaData = static_cast<const GXFloat*> ( lambda.GetData () );
	GXFloat firstInverseMass = firstBody->GetInverseMass ();
	const GXMat3& firstInverseInertia = firstBody->GetInverseInertiaTensorWorld ();
	GXFloat deltaTime = GXPhysicsEngine::GetInstance ().GetTimeStep ();

	GXVec6 alpha;
	memset ( &alpha, 0, sizeof ( GXVec6 ) );

	for ( GXUInt i = 0u; i < constraints; ++i )
		alpha.Sum ( alpha, lambdaData[ i ], GetJacobianElement ( i, 0 ) );

	GXVec6 betta;
	betta.From ( firstBody->GetTotalForce (), firstBody->GetTotalTorque () );
	alpha.Sum ( alpha, betta );

	betta._data[ 0 ] = firstInverseMass * alpha._data[ 0 ];
	betta._data[ 1 ] = firstInverseMass * alpha._data[ 1 ];
	betta._data[ 2 ] = firstInverseMass * alpha._data[ 2 ];
	betta._data[ 3 ] = firstInverseInertia._m[ 0 ][ 0 ] * alpha._data[ 3 ] + firstInverseInertia._m[ 0 ][ 1 ] * alpha._data[ 4 ] + firstInverseInertia._m[ 0 ][ 2 ] * alpha._data[ 5 ];
	betta._data[ 4 ] = firstInverseInertia._m[ 1 ][ 0 ] * alpha._data[ 3 ] + firstInverseInertia._m[ 1 ][ 1 ] * alpha._data[ 4 ] + firstInverseInertia._m[ 1 ][ 2 ] * alpha._data[ 5 ];
	betta._data[ 5 ] = firstInverseInertia._m[ 2 ][ 0 ] * alpha._data[ 3 ] + firstInverseInertia._m[ 2 ][ 1 ] * alpha._data[ 4 ] + firstInverseInertia._m[ 2 ][ 2 ] * alpha._data[ 5 ];

	betta.Multiply ( betta, deltaTime );
	alpha.From ( firstBody->GetLinearVelocity (), firstBody->GetAngularVelocity () );
	alpha.Sum ( alpha, betta );

	firstBody->SetLinearVelocity ( GXVec3 ( alpha._data[ 0 ], alpha._data[ 1 ], alpha._data[ 2 ] ) );
	firstBody->SetAngularVelocity ( GXVec3 ( alpha._data[ 3 ], alpha._data[ 4 ], alpha._data[ 5 ] ) );

	return;
}

const GXVec6& GXSingleBodyConstraintSolver::GetJacobianElement ( GXUInt constraint, GXUByte bodyIndex ) const
{
	const GXVec6* elements = static_cast<const GXVec6*> ( jacobian.GetData () );
	return elements[ constraint * ELEMENTS_PER_CONSTRAINT + (GXUInt)bodyIndex ];
}
