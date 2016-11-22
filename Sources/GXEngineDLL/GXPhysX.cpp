//version 1.9

#include <GXEngineDLL/GXEngineDLL.h>
#include <GXEngineDLL/GXPhysX.h>
#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXBKELoader.h>
#include <GXCommon/GXPHGStructs.h>
#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXTime.h>
#include <GXCommon/GXFileSystem.h>


#define GX_PHYSICS_NB_THREADS				1
#define GX_PHYSICS_SIMULATE_DELAY			0.007
#define GX_PHYSICS_TOLERANCE_LENGTH			1.0f
#define GX_PHYSICS_TOLERANCE_MASS			1000.0f
#define GX_PHYSICS_TOLERANCE_SPEED			9.81f


class GXPhysicsErrorCallback : public physx::PxErrorCallback
{
	public:
		virtual GXVoid reportError ( PxErrorCode::Enum code, const GXChar* message, const GXChar* file, GXInt line );
};

GXVoid GXPhysicsErrorCallback::reportError ( PxErrorCode::Enum code, const GXChar* message, const GXChar* file, GXInt line )
{
	GXChar kind [ 30 ] = { 0 };
	switch ( code )
	{
		case PxErrorCode::eNO_ERROR:
			sprintf_s ( kind, "NO_ERROR" );
		break;

		//! \brief An informational message.
		case PxErrorCode::eDEBUG_INFO:
			sprintf_s ( kind, "DEBUG_INFO" );
		break;

		//! \brief a warning message for the user to help with debugging
		case PxErrorCode::eDEBUG_WARNING:
			sprintf_s ( kind, "DEBUG_WARNING" );
		break;

		//! \brief method called with invalid parameter(s)
		case PxErrorCode::eINVALID_PARAMETER:
			sprintf_s ( kind, "INVALID_PARAMETER" );
		break;

		//! \brief method was called at a time when an operation is not possible
		case PxErrorCode::eINVALID_OPERATION:
			sprintf_s ( kind, "INVALID_OPERATION" );
		break;

		//! \brief method failed to allocate some memory
		case PxErrorCode::eOUT_OF_MEMORY:
			sprintf_s ( kind, "OUT_OF_MEMORY" );
		break;

		/** \brief The library failed for some reason.
		Possibly you have passed invalid values like NaNs, which are not checked for.
		*/
		case PxErrorCode::eINTERNAL_ERROR:
			sprintf_s ( kind, "INTERNAL_ERROR" );
		break;

		//! \brief An unrecoverable error, execution should be halted and log output flushed 
		case PxErrorCode::eABORT:
			sprintf_s ( kind, "ABORT" );
		break;

		//! \brief The SDK has determined that an operation may result in poor performance. 
		case PxErrorCode::ePERF_WARNING:
			sprintf_s ( kind, "PERF_WARNING" );
		break;

		//! \brief A bit mask for including all errors
		case PxErrorCode::eMASK_ALL:
			sprintf_s ( kind, "MASK_ALL" );
		break;
	}

	GXLogA ( "PhysX::%s - %s\n", kind, message );
}

//----------------------------------------------------------------------

class MemoryOutputStream: public PxOutputStream
{
	private:
		PxU8*		mData;
		PxU32		mSize;
		PxU32		mCapacity;

	public:
		MemoryOutputStream ();
		virtual	~MemoryOutputStream ();
		PxU32 write ( const GXVoid* src, PxU32 count );
		PxU32 getSize () const;
		PxU8* getData () const;
		GXVoid CacheToDisk ( GXWChar* filepath );
};


MemoryOutputStream::MemoryOutputStream ()
{
	mData = 0;
	mSize = 0;
	mCapacity = 0;
}

MemoryOutputStream::~MemoryOutputStream()
{
	if ( mData )
		delete [] mData;
}

PxU32 MemoryOutputStream::write ( const GXVoid* src, PxU32 size )
{
	PxU32 expectedSize = mSize + size;
	if ( expectedSize > mCapacity )
	{
		mCapacity = expectedSize + 4096;

		PxU8* newData = new PxU8 [ mCapacity ];
		PX_ASSERT ( newData != 0 );

		if ( newData )
		{
			memcpy ( newData, mData, mSize );
			delete [] mData;
		}
		mData = newData;
	}
	memcpy ( mData + mSize, src, size );
	mSize += size;
	return size;
}

PxU32 MemoryOutputStream::getSize () const	
{	
	return mSize; 
}

PxU8* MemoryOutputStream::getData () const	
{
	return mData;
}

GXVoid MemoryOutputStream::CacheToDisk ( GXWChar* filepath )
{
	GXWriteToFile ( filepath, ( GXChar* )mData, mSize );
}

//----------------------------------------------------------------------

class MemoryInputData: public PxInputData
{
	private:
		PxU32		mSize;
		const PxU8*	mData;
		PxU32		mPos;

	public:
		MemoryInputData ( PxU8* data, PxU32 length );
		PxU32 read ( GXVoid* dest, PxU32 count );
		PxU32 getLength () const;
		GXVoid seek ( PxU32 pos );
		PxU32 tell () const;
};

MemoryInputData::MemoryInputData ( PxU8* data, PxU32 length)
{
	mSize = length;
	mData = data;
	mPos = 0;
}

PxU32 MemoryInputData::read ( GXVoid* dest, PxU32 count )
{
	PxU32 length = PxMin< PxU32 >( count, mSize-mPos );
	memcpy ( dest, mData + mPos, length );
	mPos += length;
	return length;
}

PxU32 MemoryInputData::getLength () const
{
	return mSize;
}

GXVoid MemoryInputData::seek ( PxU32 offset )
{
	mPos = PxMin< PxU32 >( mSize, offset );
}

PxU32 MemoryInputData::tell () const
{
	return mPos;
}

//----------------------------------------------------------------------

PxFilterFlags DefaultFilterShader ( PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const GXVoid* constantBlock, PxU32 constantBlockSize )
{
	PxFilterFlags f = PxDefaultSimulationFilterShader ( attributes0, filterData0, attributes1, filterData1, pairFlags, constantBlock, constantBlockSize );
	return f;
}

//----------------------------------------------------------------------

static GXPhysicsErrorCallback			gGXPhysicsErrorCallback;
static PxDefaultAllocator				gGXPhysicsAllocatorCallback;

//----------------------------------------------------------------------


class GXPhysicsBase : public GXPhysics
{
	private:
		static GXBool									bIsSimulating;
		static PxFoundation*							pFoundation;
		static PxPhysics*								pPhysics;
		static PxScene*									pScene;
		static PxCooking*								pCooking;
		static PxDefaultCpuDispatcher*					pCpuDispatcher;
		static PxControllerManager*						pControllerManager;
		static PxVec3									gravity;
		static PxTolerancesScale						toleranceScale;
		static PLONPHYSICSPROC							OnPhysics;
		static HANDLE									hSimulateMutex;
		static GXDouble									lastTime;
		static GXDouble									accumulator;

		PxVisualDebuggerConnection*						pvdConnection;

		GXUInt											maxVehicles;
		GXUInt											numVehicles;
		PxVehicleWheels**								vehicles;

		PxVehicleDrivableSurfaceToTireFrictionPairs*	tireSurfacePairs;

		GXUInt											maxQueries;
		GXUInt											numQueries;
		PxRaycastQueryResult*							queryResults;
		PxRaycastHit*									queryHits;
		PxBatchQueryPreFilterShader						queryFilterShader;
		PxBatchQuery*									batchQuery;

	public:
		GXPhysicsBase ();
		virtual ~GXPhysicsBase ();

		virtual GXBool IsSumulating ();

		virtual GXBool SetLinearVelocity ( PxRigidDynamic* actor, PxVec3 &vel );
		virtual GXVoid SetOnPhysicsFunc ( PLONPHYSICSPROC func );

		virtual PxRigidDynamic* CreateRigidDynamic ( GXVoid* address, PxTransform& location );
		virtual PxRigidStatic* CreateRigidStatic ( GXVoid* address, PxTransform &location );
		virtual PxVehicleDrive4W* CreateVehicle ( const GXVehicleInfo &info );
		virtual GXVoid DeleteVehicle ( PxVehicleDrive4W* vehicle );
		virtual PxController* CreateCharacterController ( PxCapsuleControllerDesc desc );

		virtual GXVoid SetRigidActorOrigin ( PxRigidActor* actor, const GXVec3 &location, const GXQuat &rotation );

		virtual GXVoid AddActor ( PxActor &actor );
		virtual GXVoid RemoveActor ( PxActor &actor );

		virtual PxMaterial* CreateMaterial ( GXFloat staticFriction, GXFloat dynamicFriction, GXFloat restitution );
		
		virtual PxTriangleMesh* CreateTriangleMesh ( const GXWChar* bke_file_name );
		virtual PxConvexMesh* CreateConvexMesh ( const GXWChar* bke_file_name );

		virtual PxParticleSystem* CreateParticleSystem ( GXUInt maxCountParticles, GXBool particleRestOffset = GX_FALSE );
		virtual GXVoid AddParticleSystem ( PxParticleSystem &ps );

		virtual GXVec3 GetGravityAcceleration ();
		virtual GXFloat GetSimulationDelay ();
		virtual GXFloat GetToleranceLength ();

		virtual GXBool RaycastSingle ( const GXVec3 &start, const GXVec3 &direction, const GXFloat maxDistance, PxRaycastHit &hit );
		virtual GXVoid DoSimulate ();

		virtual GXVoid ControlVehicle ( PxVehicleDrive4W* vehicle, GXUInt controlType, GXFloat value );

	private:
		PxConvexMesh* CreateCylinderConvex ( GXFloat width, GXFloat radius, GXUInt numCirclePoints );
		
		static PxQueryHitType::Enum BatchQueryPreFilterShader ( PxFilterData filterData0, PxFilterData filterData1, const void* constantBlock, PxU32 constantBlockSize, PxHitFlags &filterFlags );
		static PxFilterFlags FilterShader ( PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize );
};

GXBool							GXPhysicsBase::bIsSimulating;
PxFoundation*					GXPhysicsBase::pFoundation;
PxPhysics*						GXPhysicsBase::pPhysics;
PxControllerManager*			GXPhysicsBase::pControllerManager;
PxCooking*						GXPhysicsBase::pCooking;
PxScene*						GXPhysicsBase::pScene;
PxDefaultCpuDispatcher*			GXPhysicsBase::pCpuDispatcher;
PxTolerancesScale				GXPhysicsBase::toleranceScale;
PxVec3							GXPhysicsBase::gravity;
PLONPHYSICSPROC					GXPhysicsBase::OnPhysics;
HANDLE							GXPhysicsBase::hSimulateMutex;
GXDouble						GXPhysicsBase::lastTime;
GXDouble						GXPhysicsBase::accumulator = 0.0;

GXPhysicsBase::GXPhysicsBase ()
{
	GXLogInit ();

	lastTime = GXGetProcessorTicks ();

	pScene = 0;
	OnPhysics = 0;

	if ( !( pFoundation = PxCreateFoundation ( PX_PHYSICS_VERSION, gGXPhysicsAllocatorCallback, gGXPhysicsErrorCallback ) ) )
	{
		GXDebugBox ( L"pFoundation не создано" );
		GXLogW ( L"GXPhysicsBase::GXPhysicsBase::Error - pFoundation не создано\n" );
	}

	toleranceScale.length = GX_PHYSICS_TOLERANCE_LENGTH;
	toleranceScale.mass = GX_PHYSICS_TOLERANCE_MASS;
	toleranceScale.speed = GX_PHYSICS_TOLERANCE_SPEED;
	gravity.x = 0.0f;
	gravity.y = GX_PHYSICS_GRAVITY_FACTOR;
	gravity.z = 0.0f;
	if ( !( pPhysics = PxCreatePhysics ( PX_PHYSICS_VERSION, *pFoundation, toleranceScale ) ) )
	{
		GXDebugBox ( L"pPhysics не создано" );
		GXLogW ( L"GXPhysicsBase::GXPhysicsBase::Error - pPhysics не создано\n" );
	}

	if ( !PxInitExtensions ( *pPhysics ) )
	{
		GXDebugBox ( L"PxInitExtensions провалено" );
		GXLogW ( L"GXPhysicsBase::GXPhysicsBase::Error - PxInitExtensions провалено\n" );
	}

	if ( !( pCooking = PxCreateCooking ( PX_PHYSICS_VERSION, *pFoundation, PxCookingParams ( toleranceScale ) ) ) )
	{
		GXDebugBox ( L"pCooking не создано" );
		GXLogW ( L"GXPhysicsBase::GXPhysicsBase::Error - pCooking не создано\n" );
	}

	PxSceneDesc sceneDesc ( pPhysics->getTolerancesScale () );
	sceneDesc.gravity = gravity;
	if ( !sceneDesc.cpuDispatcher )
	{
		if ( !( pCpuDispatcher = PxDefaultCpuDispatcherCreate ( GX_PHYSICS_NB_THREADS ) ) )
		{
			GXDebugBox ( L"pCpuDispatcher не создано" );
			GXLogW ( L"GXPhysicsBase::GXPhysicsBase::Error - pCpuDispatcher не создано\n" );
		}
		else
			sceneDesc.cpuDispatcher = pCpuDispatcher;
	}
	if ( !sceneDesc.filterShader )
	{
		sceneDesc.filterShader = &FilterShader;
		sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
	}
	if ( !( pScene = pPhysics->createScene ( sceneDesc ) ) )
	{
		GXDebugBox ( L"pScene не создано" );
		GXLogW ( L"GXPhysicsBase::GXPhysicsBase::Error - pScene не создано\n" );
	}

	if ( !( pControllerManager = PxCreateControllerManager ( *pScene ) ) )
	{
		GXDebugBox ( L"pControllerManager не создано" );
		GXLogW ( L"GXPhysicsBase::GXPhysicsBase::Error - pControllerManager не создано\n" );
	}

	if ( !PxInitVehicleSDK ( *pPhysics ) )
	{
		GXDebugBox ( L"PxInitVehicleSDK провалено" );
		GXLogW ( L"GXPhysicsBase::GXPhysicsBase::Error - PxInitVehicleSDK провалено\n" );
	}

	PxVehicleSetBasisVectors ( PxVec3 ( 0.0f, 1.0f, 0.0f ), PxVec3 ( 0.0f, 0.0f, 1.0f ) );
	PxVehicleSetUpdateMode ( PxVehicleUpdateMode::eVELOCITY_CHANGE );

	maxVehicles = 8;
	numVehicles = 0;
	vehicles = (PxVehicleWheels**)malloc ( maxVehicles * sizeof ( PxVehicleWheels* ) );

	maxQueries = 8 * 6;	//8 машин с 6 колёсами
	numQueries = 0;

	queryResults = (PxRaycastQueryResult*)malloc ( maxQueries * sizeof ( PxRaycastQueryResult ) );
	queryHits = (PxRaycastHit*)malloc ( maxQueries * sizeof ( PxRaycastHit ) );

	GXFloat frictionPairs[ 4 ][ 4 ] =
	{
		//	Сырые	Скользкие	Зимние	Внедорожные
		{	1.10f,	0.95f,		0.95f,	0.95f	},         //Грязь
		{	1.10f,	1.15f,		1.10f,	1.10f	},         //Асфальт
		{	0.70f,	0.70f,		0.70f,	0.70f	},         //Лёд
		{	0.80f,	0.80f,		0.80f,	0.80f	}          //Трава

	};


	PxMaterial* surfaceMaterials[ 4 ];
	surfaceMaterials[ 0 ] = pPhysics->createMaterial ( 0.2f, 0.5f, 0.5f );
	surfaceMaterials[ 1 ] = pPhysics->createMaterial ( 0.2f, 0.5f, 0.5f );
	surfaceMaterials[ 2 ] = pPhysics->createMaterial ( 0.2f, 0.5f, 0.5f );
	surfaceMaterials[ 3 ] = pPhysics->createMaterial ( 0.2f, 0.5f, 0.5f );

	PxVehicleDrivableSurfaceType surfaceTypes[ 4 ];
	for ( GXUChar i = 0; i < 4; i++ )
		surfaceTypes[ i ].mType = i;

	tireSurfacePairs = PxVehicleDrivableSurfaceToTireFrictionPairs::allocate ( 4, 4 ); 
	tireSurfacePairs->setup ( 4, 4, (const PxMaterial**)surfaceMaterials, surfaceTypes );

	for ( GXUChar i = 0; i < 4; i++ )
		for ( GXUChar j = 0; j < 4; j++ )
			tireSurfacePairs->setTypePairFriction ( i, j, frictionPairs[ i ][ j ] );

	PxBatchQueryDesc batchQueryDesc ( maxQueries, 0, 0 );
	batchQueryDesc.queryMemory.userRaycastResultBuffer = queryResults;
	batchQueryDesc.queryMemory.userRaycastTouchBuffer = queryHits;
	batchQueryDesc.queryMemory.raycastTouchBufferSize = maxQueries;
	batchQueryDesc.preFilterShader = BatchQueryPreFilterShader;
	batchQueryDesc.spuPreFilterShader = 0;
	batchQueryDesc.spuPreFilterShaderSize = 0;
	batchQuery = pScene->createBatchQuery ( batchQueryDesc );

	pvdConnection = 0;
	if ( pPhysics->getPvdConnectionManager () )
	{
		const GXChar pvd_host_ip[] = "localhost";
		GXInt port = 5425;
		GXUInt timeout = 100;

		PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags ();

		pvdConnection = PxVisualDebuggerExt::createConnection ( pPhysics->getPvdConnectionManager (), pvd_host_ip, port, timeout, connectionFlags );
	}

	hSimulateMutex = CreateMutexW ( 0, GX_FALSE, L"GX_PHYSICS_SIMULATE_MUTEX" );
}

GXPhysicsBase::~GXPhysicsBase ()
{
	if ( pvdConnection )
		pvdConnection->release ();

	GXSafeFree ( vehicles );
	GXSafeFree ( queryResults );
	GXSafeFree ( queryHits );
	tireSurfacePairs->release ();

	pScene->release ();
	pControllerManager->release ();
	PxCloseVehicleSDK ();
	pPhysics->release ();
	PxCloseExtensions ();
	pCooking->release ();
	pFoundation->release ();
	CloseHandle ( hSimulateMutex );
}

GXBool GXPhysicsBase::IsSumulating ()
{
	return bIsSimulating;
}

GXBool GXPhysicsBase::SetLinearVelocity ( PxRigidDynamic* actor, PxVec3 &vel )
{
	if ( bIsSimulating ) return GX_FALSE;
	actor->setLinearVelocity ( vel );
	return GX_TRUE;
}

GXVoid GXPhysicsBase::SetOnPhysicsFunc ( PLONPHYSICSPROC func )
{
	OnPhysics = func;
}

PxRigidDynamic* GXPhysicsBase::CreateRigidDynamic ( GXVoid* address, PxTransform& location )
{
	PxRigidDynamic* rigidDynamic = pPhysics->createRigidDynamic ( location );
	rigidDynamic->setLinearDamping ( 0.001f );
	rigidDynamic->userData = address;
	return rigidDynamic;
}

PxRigidStatic* GXPhysicsBase::CreateRigidStatic ( GXVoid* address, PxTransform &location )
{
	PxRigidStatic* rigidStatic = pPhysics->createRigidStatic ( location );
	rigidStatic->userData = address;
	return rigidStatic;
}

PxVehicleDrive4W* GXPhysicsBase::CreateVehicle ( const GXVehicleInfo &info )
{
	if ( ( numVehicles + 1 ) > maxVehicles )
		return 0;

	PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate ( info.numWheels );
	PxVehicleDriveSimData4W driveSimData;
	PxVehicleChassisData chassisData;

	PxVec3 chassisCMOffset = PxVec3 ( info.chassisCoMOffset.x, info.chassisCoMOffset.y, info.chassisCoMOffset.z );
	PxVec3 chassisMOI ( info.chassisMoI.x, info.chassisMoI.y, info.chassisMoI.z );

	chassisData.mMass = info.chassisMass;
	chassisData.mMOI = chassisMOI;
	chassisData.mCMOffset = chassisCMOffset;

	PxVehicleWheelData* wheelData = (PxVehicleWheelData*)malloc ( info.numWheels * sizeof ( PxVehicleWheelData ) );
	PxVehicleTireData* tireData = (PxVehicleTireData*)malloc ( info.numWheels * sizeof ( PxVehicleTireData ) );
	PxVehicleSuspensionData* suspensionData = (PxVehicleSuspensionData*)malloc ( info.numWheels * sizeof ( PxVehicleSuspensionData ) );
	PxVec3* suspensionTravelDirections = (PxVec3*)malloc ( info.numWheels * sizeof ( PxVec3 ) );

	PxVec3* wheelCentreCoMOffsets = (PxVec3*)malloc ( info.numWheels * sizeof ( PxVec3 ) );
	PxVec3* suspensionForceAppCoMOffsets = (PxVec3*)malloc ( info.numWheels * sizeof ( PxVec3 ) );
	PxVec3* tireForceAppCoMOffsets = (PxVec3*)malloc ( info.numWheels * sizeof ( PxVec3 ) );

	for ( GXUChar i = 0; i < info.numWheels; i++ )
	{
		wheelData[ i ].PxVehicleWheelData::PxVehicleWheelData ();
		wheelData[ i ].mRadius = info.wheelRadiuses[ i ];
		wheelData[ i ].mMass = info.wheelMasses[ i ];
		wheelData[ i ].mMOI = info.wheelMoIs[ i ];
		wheelData[ i ].mWidth = info.wheelWidths[ i ];
		wheelData[ i ].mMaxBrakeTorque = info.wheelBreaks[ i ];
		wheelData[ i ].mMaxHandBrakeTorque = info.wheelHandBreaks[ i ]; 
		wheelData[ i ].mMaxSteer = info.wheelSteers[ i ];

		tireData[ i ].PxVehicleTireData::PxVehicleTireData ();

		suspensionData[ i ].PxVehicleSuspensionData::PxVehicleSuspensionData ();
		suspensionData[ i ].mSprungMass = info.suspensionSprungMasses[ i ];
		suspensionData[ i ].mSpringStrength = info.suspensionSpringForces[ i ];
		suspensionData[ i ].mSpringDamperRate = info.suspensionSpringDampers[ i ];
		suspensionData[ i ].mMaxCompression = info.suspensionCompressions[ i ];
		suspensionData[ i ].mMaxDroop = info.suspensionDroops[ i ];

		suspensionTravelDirections[ i ] = PxVec3 ( info.suspensionTraverses[ i ].x, info.suspensionTraverses[ i ].y, info.suspensionTraverses[ i ].z );
		wheelCentreCoMOffsets[ i ] = PxVec3 ( info.wheelLocalCentres[ i ].x - info.chassisCoMOffset.x, info.wheelLocalCentres[ i ].y - info.chassisCoMOffset.y,  info.wheelLocalCentres[ i ].z - info.chassisCoMOffset.z );
		suspensionForceAppCoMOffsets[ i ] = wheelCentreCoMOffsets[ i ];
		tireForceAppCoMOffsets[ i ] = wheelCentreCoMOffsets[ i ];

		wheelsSimData->setWheelData ( i, wheelData[ i ] );
		wheelsSimData->setTireData ( i, tireData[ i ] );
		wheelsSimData->setSuspensionData ( i, suspensionData[ i ] );
		wheelsSimData->setSuspTravelDirection ( i, suspensionTravelDirections[ i ] );
		wheelsSimData->setWheelCentreOffset ( i, wheelCentreCoMOffsets[ i ] );
		wheelsSimData->setSuspForceAppPointOffset ( i, suspensionForceAppCoMOffsets[ i ] );
		wheelsSimData->setTireForceAppPointOffset ( i, tireForceAppCoMOffsets[ i ] );
	}

	PxVehicleDifferential4WData differentialData;
	switch ( info.differentialType )
	{
		case GX_PHYSICS_DIFFERENTIAL_LS_4WD:
			differentialData.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
		break;

		case GX_PHYSICS_DIFFERENTIAL_LS_FRONTWD:
			differentialData.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_FRONTWD;
		break;

		case GX_PHYSICS_DIFFERENTIAL_LS_REARWD:
			differentialData.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_REARWD;
		break;

		case GX_PHYSICS_DIFFERENTIAL_OPEN_4WD:
			differentialData.mType = PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_4WD;
		break;

		case GX_PHYSICS_DIFFERENTIAL_OPEN_FRONTWD:
			differentialData.mType = PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_FRONTWD;
		break;

		case GX_PHYSICS_DIFFERENTIAL_OPEN_REARWD:
			differentialData.mType = PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_REARWD;
		break;

		default:
		break;
	}

	driveSimData.setDiffData ( differentialData );

	PxVehicleEngineData engineData;
	engineData.mPeakTorque = info.enginePeakTorque;
	engineData.mMaxOmega = info.engineRotation;
	driveSimData.setEngineData ( engineData );

	PxVehicleGearsData gearData;
	gearData.mSwitchTime = info.gearboxSwitchTime;
	gearData.mNbRatios = info.numGears;
	for ( GXUChar i = 0; i < info.numGears; i++ )
		gearData.mRatios[ i ] = info.gearboxRatios[ i ];

	driveSimData.setGearsData ( gearData );

	PxVehicleClutchData clutchData;
	clutchData.mStrength = info.clutchStrength;
	driveSimData.setClutchData ( clutchData );

	PxVehicleAckermannGeometryData ackermannData;
	ackermannData.mAccuracy = info.ackermannAccuracy;
	ackermannData.mFrontWidth = info.ackermannFrontWidth;
	ackermannData.mRearWidth = info.ackermannRearWidth;
	ackermannData.mAxleSeparation = info.ackermannAxleSeparation;
	driveSimData.setAckermannGeometryData ( ackermannData );

	GXQuat physXRotation;
	GXQuatRehandCoordinateSystem ( physXRotation, info.spawnRotation );
	PxRigidDynamic* rigidVehicle = pPhysics->createRigidDynamic ( PxTransform ( PxVec3 ( info.spawnLocation.x, info.spawnLocation.y, info.spawnLocation.z ), PxQuat ( physXRotation.x, physXRotation.y, physXRotation.z, physXRotation.w ) ) );
	rigidVehicle->userData = info.renderCarAddress;
	
	PxMaterial* mtr = pPhysics->createMaterial ( 0.8f, 0.5f, 0.4f );
	for ( GXUChar i = 0; i < info.numWheels; i++ )
	{
		PxConvexMesh* wheelConvex = CreateCylinderConvex ( info.wheelWidths[ i ], info.wheelRadiuses[ i ], 16 );
		PxConvexMeshGeometry wheelGeometry ( wheelConvex );
		PxShape* wheelShape = rigidVehicle->createShape ( wheelGeometry, *mtr );
			
		PxFilterData filterData;
		filterData.word0 = GX_PHYSICS_COLLUSION_GROUP_WHEEL;
		filterData.word1 = GX_PHYSICS_COLLUSION_GROUP_WHEEL | GX_PHYSICS_COLLUSION_GROUP_OBSTACLE /*| GX_PHYSICS_COLLUSION_GROUP_CHASSIS*/;
		filterData.word3 = GX_PHYSICS_RAYCAST_UNDRIVABLE_SURFACE;

		wheelShape->setQueryFilterData ( filterData );
		wheelShape->setSimulationFilterData ( filterData );

		wheelShape->setLocalPose ( PxTransform::createIdentity () );
	}

	PxConvexMesh* convex = CreateConvexMesh ( info.bodyBakeFile );
	PxConvexMeshGeometry convexGeometry ( convex );
	PxShape* chassisShape = rigidVehicle->createShape ( convexGeometry, *mtr );
	chassisShape->setLocalPose ( PxTransform ( PxVec3 ( info.bodyConvexOffset.x, info.bodyConvexOffset.y, info.bodyConvexOffset.z ) ) );

	PxFilterData filterData;
	filterData.word0 = GX_PHYSICS_COLLUSION_GROUP_CHASSIS;
	filterData.word1 = GX_PHYSICS_COLLUSION_GROUP_CHASSIS | GX_PHYSICS_COLLUSION_GROUP_DRIVABLE | GX_PHYSICS_COLLUSION_GROUP_OBSTACLE | GX_PHYSICS_COLLUSION_GROUP_WHEEL;
	filterData.word3 = GX_PHYSICS_RAYCAST_UNDRIVABLE_SURFACE;

	chassisShape->setSimulationFilterData ( filterData );
	chassisShape->setQueryFilterData ( filterData );

	rigidVehicle->setLinearDamping ( 0.001f );
	rigidVehicle->setMass ( info.chassisMass );
	rigidVehicle->setMassSpaceInertiaTensor ( PxVec3 ( info.chassisMoI.x, info.chassisMoI.y, info.chassisMoI.z ) );
	rigidVehicle->setCMassLocalPose ( PxTransform ( PxVec3 ( info.chassisCoMOffset.x, info.chassisCoMOffset.y, info.chassisCoMOffset.z ) ) );
	PxVec3 offset ( info.chassisCoMOffset.x, info.chassisCoMOffset.y, info.chassisCoMOffset.z );

	PxVehicleDrive4W* vehicle = PxVehicleDrive4W::allocate ( info.numWheels );
	vehicle->setup ( pPhysics, rigidVehicle, *wheelsSimData, driveSimData, 0 );
	
	for ( GXUChar i = 0; i < info.numWheels; i++ )
	{
		wheelsSimData->setWheelShapeMapping ( i, i );
		wheelsSimData->setSceneQueryFilterData ( i, filterData );
	}
	
	pScene->addActor ( *rigidVehicle );

	vehicle->mDriveDynData.setToRestState ();
	vehicle->mDriveDynData.setUseAutoGears ( GX_TRUE );

	vehicles[ numVehicles ] = vehicle;
	numVehicles++;

	free ( wheelData );
	free ( tireData );
	free ( suspensionData );
	free ( suspensionTravelDirections );

	free ( wheelCentreCoMOffsets );
	free ( suspensionForceAppCoMOffsets );
	free ( tireForceAppCoMOffsets );
	
	wheelsSimData->free ();

	return vehicle;
}

GXVoid GXPhysicsBase::DeleteVehicle ( PxVehicleDrive4W* vehicle )
{
	GXUInt id = 0;
	for ( ; id < maxVehicles; id++ )
	{
		if ( vehicles[ id ] == vehicle )
			break;
	}

	vehicle->getRigidDynamicActor ()->release ();

	numVehicles--;

	if ( id < numVehicles )
	{
		memcpy ( vehicles + id, vehicles + id + 1, numVehicles * sizeof ( PxVehicleWheels* ) );
	}
}

PxController* GXPhysicsBase::CreateCharacterController ( PxCapsuleControllerDesc desc )
{
	return pControllerManager->createController ( *pPhysics, pScene, desc );
}

GXVoid GXPhysicsBase::SetRigidActorOrigin ( PxRigidActor* actor, const GXVec3 &location, const GXQuat &rotation )
{
	GXQuat physXRotation;
	GXQuatRehandCoordinateSystem ( physXRotation, rotation );
	actor->setGlobalPose ( PxTransform ( PxVec3 ( location.x, location.y, location.z ), PxQuat ( physXRotation.x, physXRotation.y, physXRotation.z, physXRotation.w ) ) );
}

GXVoid GXPhysicsBase::AddActor ( PxActor &actor )
{
	pScene->addActor ( actor );
}

GXVoid GXPhysicsBase::RemoveActor ( PxActor &actor )
{
	pScene->removeActor ( actor );
}

PxMaterial* GXPhysicsBase::CreateMaterial ( GXFloat staticFriction, GXFloat dynamicFriction, GXFloat restitution )
{
	return pPhysics->createMaterial ( staticFriction, dynamicFriction, restitution );
}

PxTriangleMesh* GXPhysicsBase::CreateTriangleMesh ( const GXWChar* bke_file_name )
{
	GXBakeInfo info;
	GXLoadBKE ( bke_file_name, info );

	GXUInt size;
	GXUByte* cache_source;

	//Есть ли запечёная геометрия?
	if ( GXLoadFile ( info.cacheFileName, (GXVoid**)&cache_source, size, GX_FALSE ) )
	{
		MemoryOutputStream	writebuffer;
		writebuffer.write ( cache_source, size );
		MemoryInputData readBuffer ( writebuffer.getData (), writebuffer.getSize () );
		GXSafeFree ( cache_source );
		return pPhysics->createTriangleMesh ( readBuffer );
	}

	//Запечённой геометрии не оказалось. Запекаем геометрию и сбрасываем на диск.
	GXNativeStaticMeshInfo meshInfo;
	GXLoadNativeStaticMesh ( info.fileName, meshInfo );

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = meshInfo.numVertices;
	meshDesc.points.stride = sizeof ( PxVec3 );
	meshDesc.points.data = meshInfo.vboData;

	meshDesc.triangles.count = meshInfo.numElements / 3;
	meshDesc.triangles.stride = 3 * sizeof ( PxU32 );
	meshDesc.triangles.data = meshInfo.eboData;

	MemoryOutputStream	writebuffer;
	GXBool status = pCooking->cookTriangleMesh ( meshDesc, writebuffer );
	if( !status )
	{
		GXLogW ( L"GXPhysicsBase::CreateTriangleMesh::Error - Запекание файла %s провалено\n", info.fileName );
		GXDebugBox ( L"Что-то не так с triangle mesh" );
		return 0;
	}

	MemoryInputData readBuffer ( writebuffer.getData (), writebuffer.getSize () );

	writebuffer.CacheToDisk ( info.cacheFileName );
	info.Cleanup ();
	meshInfo.Cleanup ();

	return pPhysics->createTriangleMesh ( readBuffer );

	/*
	GXOBJPoint* rawGeometry;	
	GXInt totalVerts = GXLoadOBJ ( info.fileName, &rawGeometry );
	PxVec3* vertexBuffer = (PxVec3*)malloc ( totalVerts * sizeof ( PxVec3 ) );
	PxU32* indexBuffer = (PxU32*)malloc ( totalVerts * sizeof ( PxU32 ) );

	PxU32 i = 0;
	while ( i < (PxU32)totalVerts )
	{
		PxVec3 v1 ( rawGeometry[ i ].vertex.x, rawGeometry[ i ].vertex.y, rawGeometry[ i ].vertex.z );
		memcpy ( vertexBuffer + i, &v1, sizeof ( PxVec3 ) );
		indexBuffer[ i ] = i++;

		PxVec3 v2 ( rawGeometry[ i ].vertex.x, rawGeometry[ i ].vertex.y, rawGeometry[ i ].vertex.z );
		memcpy ( vertexBuffer + i, &v2, sizeof ( PxVec3 ) );
		indexBuffer[ i ] = i++;

		PxVec3 v3 ( rawGeometry[ i ].vertex.x, rawGeometry[ i ].vertex.y, rawGeometry[ i ].vertex.z );
		memcpy ( vertexBuffer + i, &v3, sizeof ( PxVec3 ) );
		indexBuffer[ i ] = i++;
	}

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count           = totalVerts;
	meshDesc.points.stride          = sizeof ( PxVec3 );
	meshDesc.points.data			= vertexBuffer;
	meshDesc.triangles.count        = totalVerts / 3;
	meshDesc.triangles.stride       = 3 * sizeof ( PxU32 );
	meshDesc.triangles.data			= indexBuffer;

	MemoryOutputStream	writebuffer;
	GXBool status = pCooking->cookTriangleMesh ( meshDesc, writebuffer );
	if( !status )
	{
		GXLogW ( L"GXPhysicsBase::CreateTriangleMesh::Error - Запекание файла %s провалено\n", info.fileName );
		GXDebugBox ( L"Что-то не так с triangle mesh" );
		return 0;
	}

	MemoryInputData readBuffer ( writebuffer.getData (), writebuffer.getSize () );

	free ( vertexBuffer );
	free ( indexBuffer );

	free ( rawGeometry );

	writebuffer.CacheToDisk ( info.cacheFileName );
	info.Cleanup ();

	return pPhysics->createTriangleMesh ( readBuffer );

	*/
}

PxConvexMesh* GXPhysicsBase::CreateConvexMesh ( const GXWChar* bke_file_name )
{
	GXBakeInfo info;
	GXLoadBKE ( bke_file_name, info );

	GXUInt size;
	GXUChar* data;

	//Запечённая геометрия есть в кеше

	if ( GXLoadFile ( info.cacheFileName, (GXVoid**)&data, size, GX_FALSE ) )
	{
		MemoryOutputStream	writebuffer;
		writebuffer.write ( data, size );
		MemoryInputData readBuffer ( writebuffer.getData (), writebuffer.getSize () );
		GXSafeFree ( data );
		return pPhysics->createConvexMesh ( readBuffer );
	}

	//Запечённой геометрии в кеше не оказалось. Запекаем и сбрасывает кеш на диск.

	GXLoadFile ( info.fileName, (GXVoid**)&data, size, GX_TRUE );
	GXPhysicsGeometryHeader* h = (GXPhysicsGeometryHeader*)data;

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count	= h->numVertices;
	convexDesc.points.stride = sizeof ( PxVec3 );
	convexDesc.points.data = data + sizeof ( GXPhysicsGeometryHeader );
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

	PxConvexMesh* convexMesh = 0;
	MemoryOutputStream buf;
	if ( pCooking->cookConvexMesh ( convexDesc, buf ) )
	{
		MemoryInputData id ( buf.getData(), buf.getSize () );
		convexMesh = pPhysics->createConvexMesh ( id );
	}
	else
	{
		GXLogW ( L"GXPhysicsBase::CreateConvexMesh::Error - Запекание файла %s провалено\n", info.fileName );
		GXDebugBox ( L"Что-то не так с конвексом" );
		return 0;
	}

	free ( data );

	buf.CacheToDisk ( info.cacheFileName );
	info.Cleanup ();

	return convexMesh;
}

PxParticleSystem* GXPhysicsBase::CreateParticleSystem ( GXUInt maxCountParticles, GXBool particleRestOffset )
{
	if ( !pPhysics ) return 0;

	return pPhysics->createParticleSystem ( maxCountParticles, particleRestOffset );
}

GXVoid GXPhysicsBase::AddParticleSystem ( PxParticleSystem &ps )
{
	if ( !pScene ) return;
	pScene->addActor ( ps );
}

GXVec3 GXPhysicsBase::GetGravityAcceleration ()
{
	GXVec3 g;
	g.x = gravity.x;
	g.y = gravity.y;
	g.z = gravity.z;
	return g;
}

GXFloat GXPhysicsBase::GetSimulationDelay ()
{
	return (GXFloat)GX_PHYSICS_SIMULATE_DELAY;
}

GXFloat GXPhysicsBase::GetToleranceLength ()
{
	return toleranceScale.length;
}

GXBool GXPhysicsBase::RaycastSingle ( const GXVec3 &start, const GXVec3 &direction, const GXFloat maxDistance, PxRaycastHit &hit )
{
	WaitForSingleObject ( hSimulateMutex, INFINITE );		
	const PxSceneQueryFlags outputFlags = PxSceneQueryFlag::eDISTANCE | PxSceneQueryFlag::eIMPACT | PxSceneQueryFlag::eNORMAL;
	if ( !pScene )
	{
		ReleaseMutex ( hSimulateMutex );
		return GX_FALSE;
	}

	PxVec3 begin ( start.x, start.y, start.z );
	PxVec3 dir ( direction.x, direction.y, direction.z );
	GXBool status =  pScene->raycastSingle ( begin, dir, maxDistance, outputFlags, hit );
	ReleaseMutex ( hSimulateMutex );

	return status;
}

GXBool gx_physiscs_Flag = GX_TRUE;

GXVoid GXPhysicsBase::DoSimulate ()
{
	if ( gx_physiscs_Flag )
	{
		lastTime = GXGetProcessorTicks ();
		accumulator = 0.0;
		gx_physiscs_Flag = GX_FALSE;
		return;
	}

	GXDouble newtime = GXGetProcessorTicks ();
	accumulator += ( newtime - lastTime ) / GXGetProcessorTicksPerSecond ();
	lastTime = newtime;

	WaitForSingleObject ( hSimulateMutex, INFINITE );
	bIsSimulating = GX_TRUE;

	while ( accumulator >= GX_PHYSICS_SIMULATE_DELAY )
	{
		if ( pScene )
		{
			pScene->simulate ( (GXFloat)GX_PHYSICS_SIMULATE_DELAY );
			pScene->checkResults ( GX_TRUE );
			pScene->fetchResults ();

			if ( numVehicles )
			{
				PxVehicleSuspensionRaycasts ( batchQuery, numVehicles, vehicles, maxQueries, queryResults );
				PxVehicleUpdates ( (GXFloat)GX_PHYSICS_SIMULATE_DELAY, gravity, *tireSurfacePairs, numVehicles, vehicles, 0 );
			}

			PxU32 nbActiveTransforms;
			const PxActiveTransform* activeTransforms = pScene->getActiveTransforms ( nbActiveTransforms );
			for ( PxU32 i = 0; i < nbActiveTransforms; i++ )
			{
				if ( activeTransforms [ i ].userData && activeTransforms [ i ].actor->isRigidBody () )
				{
					GXPhysicsActorState* renderObject = (GXPhysicsActorState*)activeTransforms [ i ].userData;
					GXVec3 location = GXCreateVec3 ( activeTransforms [ i ].actor2World.p.x, activeTransforms [ i ].actor2World.p.y, activeTransforms [ i ].actor2World.p.z );

					//PhysX - левосторонняя система координат. 
					//Графические вычисления в правосторонней системе координат, поэтому
					//кватернион надо преобразовать.
					GXQuat rotationPhysX, rotation;
					rotationPhysX.x = activeTransforms [ i ].actor2World.q.x;
					rotationPhysX.y = activeTransforms [ i ].actor2World.q.y;
					rotationPhysX.z = activeTransforms [ i ].actor2World.q.z;
					rotationPhysX.w = activeTransforms [ i ].actor2World.q.w;
					GXQuatRehandCoordinateSystem ( rotation, rotationPhysX );
					renderObject->SetPivotOrigin ( location, rotation );
				}
			}

			PxShape* carShapes[ 7 ];		//6 колёс и корпус. Уйти от констант!!!!!
			for ( GXUInt i = 0; i < numVehicles; i++ )
			{
				PxRigidDynamic* rigidVihicle = vehicles[ i ]->getRigidDynamicActor ();
				GXUInt numShapes = rigidVihicle->getNbShapes ();
				rigidVihicle->getShapes ( carShapes, numShapes );
				GXPhysicsActorState* vehicle = (GXPhysicsActorState*)rigidVihicle->userData;

				for ( GXUInt shapeID = 0; shapeID < numShapes; shapeID++ )
				{
					PxTransform trans = PxShapeExt::getGlobalPose ( *carShapes[ shapeID ], *rigidVihicle );
					GXVec3 location = GXCreateVec3 ( trans.p.x, trans.p.y, trans.p.z );

					GXQuat physXRotation;
					physXRotation.x = trans.q.x;
					physXRotation.y = trans.q.y;
					physXRotation.z = trans.q.z;
					physXRotation.w = trans.q.w;

					GXQuat rotation;
					GXQuatRehandCoordinateSystem ( rotation, physXRotation );

					vehicle->SetShapeOrigin ( shapeID, location, rotation );
				}
			}
		}

		if ( OnPhysics )
			OnPhysics ( (GXFloat)GX_PHYSICS_SIMULATE_DELAY );
		
		accumulator -= GX_PHYSICS_SIMULATE_DELAY;
	}

	ReleaseMutex ( hSimulateMutex );
	bIsSimulating = GX_FALSE;
}

GXVoid GXPhysicsBase::ControlVehicle ( PxVehicleDrive4W* vehicle, GXUInt controlType, GXFloat value )
{
	vehicle->mDriveDynData.setAnalogInput ( controlType, value );
}

PxConvexMesh* GXPhysicsBase::CreateCylinderConvex ( GXFloat width, GXFloat radius, GXUInt numCirclePoints )
{
	#define  MAX_NUM_VERTS_IN_CIRCLE 8

	if ( numCirclePoints < MAX_NUM_VERTS_IN_CIRCLE )
		numCirclePoints = MAX_NUM_VERTS_IN_CIRCLE;

	GXUInt numVerts = 2 * numCirclePoints;
	PxVec3* verts = (PxVec3*)malloc ( numVerts * sizeof ( PxVec3 ) );
	
	GXFloat dtheta = 2.0f * GX_MATH_PI / numCirclePoints;
	for( GXUInt i = 0; i < numCirclePoints; i++ )
	{
		GXFloat theta = dtheta * i;
		GXFloat cosTheta = radius * cosf ( theta );
		GXFloat sinTheta = radius * sinf ( theta );

		verts[ i << 1 ] = PxVec3 ( -0.5f * width, cosTheta, sinTheta );
		verts[ ( i << 1 ) + 1 ] = PxVec3 ( 0.5f * width, cosTheta, sinTheta );
	}

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count	= numVerts;
	convexDesc.points.stride = sizeof ( PxVec3 );
	convexDesc.points.data = verts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

	PxConvexMesh* convexMesh = 0;
	MemoryOutputStream buf;
	if ( pCooking->cookConvexMesh ( convexDesc, buf ) )
	{
		MemoryInputData id ( buf.getData(), buf.getSize () );
		convexMesh = pPhysics->createConvexMesh ( id );
	}

	free ( verts );
	return convexMesh;

	#undef MAX_NUM_VERTS_IN_CIRCLE
}

PxQueryHitType::Enum GXPhysicsBase::BatchQueryPreFilterShader ( PxFilterData filterData0, PxFilterData filterData1, const void* constantBlock, PxU32 constantBlockSize, PxHitFlags &filterFlags )
{
	PX_UNUSED ( filterFlags );
	PX_UNUSED ( constantBlockSize );
	PX_UNUSED ( constantBlock );
	PX_UNUSED ( filterData0 );

	if ( filterData1.word3 == GX_PHYSICS_RAYCAST_DRIVABLE_SURFACE )
		return PxQueryHitType::eBLOCK;

	return PxQueryHitType::eNONE;
}

PxFilterFlags GXPhysicsBase::FilterShader ( PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize )
{
	PX_UNUSED ( constantBlock );
	PX_UNUSED ( constantBlockSize );

	if ( ( filterData0.word0 == 0 ) && ( filterData1.word0 == 0 ) )
		return PxDefaultSimulationFilterShader ( attributes0, filterData0, attributes1, filterData1, pairFlags, constantBlock, constantBlockSize ); 

	if ( !( ( filterData0.word0 & filterData1.word1 ) || ( filterData1.word0 & filterData0.word1 ) ) )
		return PxFilterFlag::eSUPPRESS;

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	GXDword sweptIntegration = 0x00000001;
	if ( ( filterData0.word3 | filterData1.word3 ) & sweptIntegration )
		pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT | PxPairFlag::eSOLVE_CONTACT;

	pairFlags |= PxPairFlags ( PxU16 ( filterData0.word2 | filterData1.word2 ) );
	return PxFilterFlags ();
}

//----------------------------------------------------------------------

GXDLLEXPORT GXPhysics* GXCALL GXPhysicsCreate ()
{
	return new GXPhysicsBase ();
}

GXDLLEXPORT GXVoid GXCALL GXPhysicsDestroy ( GXPhysics* physics )
{
	GXSafeDelete ( physics );
}