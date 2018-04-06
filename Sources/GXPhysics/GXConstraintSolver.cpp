//version 1.0

#include <GXPhysics/GXConstraintSolver.h>
#include <GXPhysics/GXPhysicsEngine.h>


#define CONSTRAINT_STORAGE_GROW_FACTOR		32
#define ELEMENTS_PER_CONSTRAINT				2
#define KINEMATIC_BODY						0xFFu
#define DEFAULT_INITIAL_LAMBDA				0.0f


GXConstraintSolver::GXConstraintSolver ( GXUShort maximumIterations ):
	jacobian (sizeof ( GXSparseMatrixElement ) ),
	mapper ( sizeof ( GXUByte ) ),
	bias ( sizeof ( GXFloat ) ),
	lambdaRange ( sizeof ( GXVec2 ) ),
	initialLambda ( sizeof ( GXFloat ) ),
	lambda ( sizeof ( GXFloat ) ),
	eta ( sizeof ( GXFloat ) ),
	d ( sizeof ( GXSparseMatrixElement ) ),
	b ( sizeof ( GXSparseMatrixElement ) )
{
	this->maximumIterations = maximumIterations;
	constraints = 0;
	firstBody = nullptr;
	secondBody = nullptr;
}

GXConstraintSolver::~GXConstraintSolver ()
{
	// NOTHING
}

GXVoid GXConstraintSolver::Reset ()
{
	constraints = 0;
	firstBody = secondBody = nullptr;
}

GXVoid GXConstraintSolver::SetBodies ( const GXRigidBody &first, const GXRigidBody &second )
{
	firstBody = &first;
	secondBody = &second;
}

GXVoid GXConstraintSolver::AddConstraint ( const GXConstraint& constraint )
{
	if ( constraints >= eta.GetLength () )
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
		mapper.Resize ( alpha );
		b.Resize ( alpha );
	}

	GXUInt offset = constraints * ELEMENTS_PER_CONSTRAINT;

	GXSparseMatrixElement* jacobianData = (GXSparseMatrixElement*)jacobian.GetData ();
	memcpy ( jacobianData + offset, constraint.GetJacobian (), ELEMENTS_PER_CONSTRAINT * sizeof ( GXSparseMatrixElement ) );

	GXFloat* biasData = (GXFloat*)bias.GetData ();
	biasData[ constraints ] = constraint.GetBias ();

	GXVec2* lambdaRangeData = (GXVec2*)lambdaRange.GetData ();
	lambdaRangeData[ constraints ] = constraint.GetLambdaRange ();

	GXFloat* initialLambdaData = (GXFloat*)initialLambda.GetData ();
	initialLambdaData[ constraints ] = DEFAULT_INITIAL_LAMBDA;

	constraints++;
}

GXVoid GXConstraintSolver::Solve ()
{
	UpdateMapper ();
	UpdateInverseM ();
	UpdateB ();
	UpdateEta ();

	const GXFloat* initialLambdaData = (const GXFloat*)initialLambda.GetData ();
	GXFloat* lambdaData = (GXFloat*)lambda.GetData ();

	memcpy ( lambdaData, initialLambdaData, constraints * sizeof ( GXFloat ) );

	// TODO
}

GXVoid GXConstraintSolver::UpdateMapper ()
{
	GXUByte alpha = firstBody->IsKinematic () ? KINEMATIC_BODY : 0u;
	const GXUByte betta = 1u;

	GXUByte* mapperData = (GXUByte*)mapper.GetData ();

	for ( GXUInt i = 0; i < constraints; i++ )
	{
		mapperData[ 0 ] = alpha;
		mapperData[ 1 ] = betta;

		mapperData += 2;
	}
}

GXVoid GXConstraintSolver::UpdateInverseM ()
{
	const GXSparseMatrixElement zero ( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );

	GXFloat firstInverseMass = firstBody->GetInverseMass ();
	GXFloat secondInverseMass = secondBody->GetInverseMass ();

	const GXMat3& firstInverseInertia = firstBody->GetInverseInertiaTensorWorld ();
	const GXMat3& secondInverseInertia = secondBody->GetInverseInertiaTensorWorld ();
	
	inverseM[ 0 ].Init ( firstInverseMass, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	inverseM[ 1 ] = zero;
	inverseM[ 2 ].Init ( 0.0f, firstInverseMass, 0.0f, 0.0f, 0.0f, 0.0f );
	inverseM[ 3 ] = zero;
	inverseM[ 4 ].Init ( 0.0f, 0.0f, firstInverseMass, 0.0f, 0.0f, 0.0f );
	inverseM[ 5 ] = zero;

	inverseM[ 6 ].Init ( 0.0f, 0.0f, 0.0f, firstInverseInertia.m[ 0 ][ 0 ], firstInverseInertia.m[ 0 ][ 1 ], firstInverseInertia.m[ 0 ][ 2 ] );
	inverseM[ 7 ] = zero;
	inverseM[ 8 ].Init ( 0.0f, 0.0f, 0.0f, firstInverseInertia.m[ 1 ][ 0 ], firstInverseInertia.m[ 1 ][ 1 ], firstInverseInertia.m[ 1 ][ 2 ] );
	inverseM[ 9 ] = zero;
	inverseM[ 10 ].Init ( 0.0f, 0.0f, 0.0f, firstInverseInertia.m[ 2 ][ 0 ], firstInverseInertia.m[ 2 ][ 1 ], firstInverseInertia.m[ 2 ][ 2 ] );
	inverseM[ 11 ] = zero;

	inverseM[ 12 ] = zero;
	inverseM[ 13 ].Init ( secondInverseMass, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	inverseM[ 14 ] = zero;
	inverseM[ 15 ].Init ( 0.0f, secondInverseMass, 0.0f, 0.0f, 0.0f, 0.0f );
	inverseM[ 16 ] = zero;
	inverseM[ 17 ].Init ( 0.0f, 0.0f, secondInverseMass, 0.0f, 0.0f, 0.0f );

	inverseM[ 18 ] = zero;
	inverseM[ 19 ].Init ( 0.0f, 0.0f, 0.0f, secondInverseInertia.m[ 0 ][ 0 ], secondInverseInertia.m[ 0 ][ 1 ], secondInverseInertia.m[ 0 ][ 2 ] );
	inverseM[ 20 ] = zero;
	inverseM[ 21 ].Init ( 0.0f, 0.0f, 0.0f, secondInverseInertia.m[ 1 ][ 0 ], secondInverseInertia.m[ 1 ][ 1 ], secondInverseInertia.m[ 1 ][ 2 ] );
	inverseM[ 22 ] = zero;
	inverseM[ 23 ].Init ( 0.0f, 0.0f, 0.0f, secondInverseInertia.m[ 2 ][ 0 ], secondInverseInertia.m[ 2 ][ 1 ], secondInverseInertia.m[ 2 ][ 2 ] );
}

GXVoid GXConstraintSolver::UpdateB ()
{
	GXSparseMatrixElement* bData = (GXSparseMatrixElement*)b.GetData ();
	const GXSparseMatrixElement* jacobianData = (const GXSparseMatrixElement*)jacobian.GetData ();

	for ( GXUInt i = 0u; i < constraints; i++ )
	{
		bData->data[ 0 ] = inverseM[ 0 ].data[ 0 ] * jacobianData->data[ 0 ];
		bData->data[ 1 ] = inverseM[ 2 ].data[ 1 ] * jacobianData->data[ 1 ];
		bData->data[ 2 ] = inverseM[ 4 ].data[ 2 ] * jacobianData->data[ 2 ];
		bData->data[ 3 ] = inverseM[ 6 ].data[ 3 ] * jacobianData->data[ 3 ] + inverseM[ 6 ].data[ 4 ] * jacobianData->data[ 4 ] + inverseM[ 6 ].data[ 5 ] * jacobianData->data[ 5 ];
		bData->data[ 4 ] = inverseM[ 8 ].data[ 3 ] * jacobianData->data[ 3 ] + inverseM[ 8 ].data[ 4 ] * jacobianData->data[ 4 ] + inverseM[ 8 ].data[ 5 ] * jacobianData->data[ 5 ];
		bData->data[ 5 ] = inverseM[ 10 ].data[ 3 ] * jacobianData->data[ 3 ] + inverseM[ 10 ].data[ 4 ] * jacobianData->data[ 4 ] + inverseM[ 10 ].data[ 5 ] * jacobianData->data[ 5 ];

		bData++;
		jacobianData++;

		bData->data[ 0 ] = inverseM[ 13 ].data[ 0 ] * jacobianData->data[ 0 ];
		bData->data[ 1 ] = inverseM[ 15 ].data[ 1 ] * jacobianData->data[ 1 ];
		bData->data[ 2 ] = inverseM[ 17 ].data[ 2 ] * jacobianData->data[ 2 ];
		bData->data[ 3 ] = inverseM[ 19 ].data[ 3 ] * jacobianData->data[ 3 ] + inverseM[ 19 ].data[ 4 ] * jacobianData->data[ 4 ] + inverseM[ 19 ].data[ 5 ] * jacobianData->data[ 5 ];
		bData->data[ 4 ] = inverseM[ 21 ].data[ 3 ] * jacobianData->data[ 3 ] + inverseM[ 21 ].data[ 4 ] * jacobianData->data[ 4 ] + inverseM[ 21 ].data[ 5 ] * jacobianData->data[ 5 ];
		bData->data[ 5 ] = inverseM[ 23 ].data[ 3 ] * jacobianData->data[ 3 ] + inverseM[ 23 ].data[ 4 ] * jacobianData->data[ 4 ] + inverseM[ 23 ].data[ 5 ] * jacobianData->data[ 5 ];

		bData++;
		jacobianData++;
	}
}

GXVoid GXConstraintSolver::UpdateEta ()
{
	const GXVec3& firstLinear = firstBody->GetLinearVelocity ();
	const GXVec3& firstAngular = firstBody->GetAngularVelocity ();
	const GXVec3& firstForce = firstBody->GetTotalForce ();
	const GXVec3& firstTorque = firstBody->GetTotalTorque ();

	const GXVec3& secondLinear = secondBody->GetLinearVelocity ();
	const GXVec3& secondAngular = secondBody->GetAngularVelocity ();
	const GXVec3& secondForce = secondBody->GetTotalForce ();
	const GXVec3& secondTorque = secondBody->GetTotalTorque ();

	GXFloat inverseDeltaTime = 1.0f / GXPhysicsEngine::GetInstance ().GetTimeStep ();

	GXSparseMatrixElement alpha[ 2 ];
	alpha[ 0 ].Init ( firstLinear.data[ 0 ], firstLinear.data[ 1 ], firstLinear.data[ 1 ], firstAngular.data[ 0 ], firstAngular.data[ 1 ], firstAngular.data[ 2 ] );
	alpha[ 1 ].Init ( secondLinear.data[ 0 ], secondLinear.data[ 1 ], secondLinear.data[ 1 ], secondAngular.data[ 0 ], secondAngular.data[ 1 ], secondAngular.data[ 2 ] );

	alpha[ 0 ].Multiply ( alpha[ 0 ], inverseDeltaTime );
	alpha[ 1 ].Multiply ( alpha[ 1 ], inverseDeltaTime );

	GXSparseMatrixElement betta[ 2 ];
	betta[ 0 ].data[ 0 ] = inverseM[ 0 ].data[ 0 ] * firstForce.data[ 0 ];
	betta[ 0 ].data[ 1 ] = inverseM[ 2 ].data[ 1 ] * firstForce.data[ 1 ];
	betta[ 0 ].data[ 2 ] = inverseM[ 4 ].data[ 2 ] * firstForce.data[ 2 ];
	betta[ 0 ].data[ 3 ] = inverseM[ 6 ].data[ 3 ] * firstTorque.data[ 0 ] + inverseM[ 6 ].data[ 4 ] * firstTorque.data[ 1 ] + inverseM[ 6 ].data[ 5 ] * firstTorque.data[ 2 ];
	betta[ 0 ].data[ 4 ] = inverseM[ 8 ].data[ 3 ] * firstTorque.data[ 0 ] + inverseM[ 8 ].data[ 4 ] * firstTorque.data[ 1 ] + inverseM[ 8 ].data[ 5 ] * firstTorque.data[ 2 ];
	betta[ 0 ].data[ 5 ] = inverseM[ 10 ].data[ 3 ] * firstTorque.data[ 0 ] + inverseM[ 10 ].data[ 4 ] * firstTorque.data[ 1 ] + inverseM[ 10 ].data[ 5 ] * firstTorque.data[ 2 ];

	betta[ 1 ].data[ 0 ] = inverseM[ 13 ].data[ 0 ] * secondForce.data[ 0 ];
	betta[ 1 ].data[ 1 ] = inverseM[ 15 ].data[ 1 ] * secondForce.data[ 1 ];
	betta[ 1 ].data[ 2 ] = inverseM[ 17 ].data[ 2 ] * secondForce.data[ 2 ];
	betta[ 1 ].data[ 3 ] = inverseM[ 19 ].data[ 3 ] * secondTorque.data[ 0 ] + inverseM[ 19 ].data[ 4 ] * secondTorque.data[ 1 ] + inverseM[ 19 ].data[ 5 ] * secondTorque.data[ 2 ];
	betta[ 1 ].data[ 4 ] = inverseM[ 21 ].data[ 3 ] * secondTorque.data[ 0 ] + inverseM[ 21 ].data[ 4 ] * secondTorque.data[ 1 ] + inverseM[ 21 ].data[ 5 ] * secondTorque.data[ 2 ];
	betta[ 1 ].data[ 5 ] = inverseM[ 23 ].data[ 3 ] * secondTorque.data[ 0 ] + inverseM[ 23 ].data[ 4 ] * secondTorque.data[ 1 ] + inverseM[ 23 ].data[ 5 ] * secondTorque.data[ 2 ];

	alpha[ 0 ].Sum ( alpha[ 0 ], betta[ 0 ] );
	alpha[ 1 ].Sum ( alpha[ 1 ], betta[ 1 ] );

	GXFloat* etaData = (GXFloat*)eta.GetData ();
	const GXFloat* biasData = (GXFloat*)bias.GetData ();
	const GXSparseMatrixElement* jacobianData = (const GXSparseMatrixElement*)jacobian.GetData ();

	for ( GXUInt i = 0u; i < constraints; i++ )
	{
		etaData[ i ] = inverseDeltaTime * biasData[ i ] - ( jacobianData[ 0 ].DotProduct ( alpha[ 0 ] ) + jacobianData[ 1 ].DotProduct ( alpha[ 1 ] ) );
		jacobianData += 2u;
	}
}
