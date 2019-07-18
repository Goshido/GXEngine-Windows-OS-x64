//version 1.13

#include <GXEngineDLL/GXEngineDLL.h>
#include <GXEngineDLL/GXPhysXAdapter.h>
#include <GXCommon/GXBKELoader.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXPHGStructs.h>
#include <GXCommon/GXTime.h>


#define PHYSICS_NB_THREADS          1u
#define PHYSICS_SIMULATE_DELAY      0.007
#define PHYSICS_TOLERANCE_LENGTH    1.0f
#define PHYSICS_TOLERANCE_MASS      1000.0f
#define PHYSICS_TOLERANCE_SPEED     9.81f

//---------------------------------------------------------------------------------------------------------------------

class GXPhysicsErrorCallback : public physx::PxErrorCallback
{
    public:
        GXVoid reportError ( PxErrorCode::Enum code, const GXChar* message, const GXChar* file, GXInt line ) override;
};

GXVoid GXPhysicsErrorCallback::reportError ( PxErrorCode::Enum code, const GXChar* message, const GXChar* /*file*/, GXInt /*line*/ )
{
    GXChar kind [ 30u ] = { 0 };

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

//---------------------------------------------------------------------------------------------------------------------

class MemoryOutputStream : public PxOutputStream
{
    private:
        PxU8*       _data;
        PxU32       _size;
        PxU32       _capacity;

    public:
        MemoryOutputStream ();
        ~MemoryOutputStream () override;

        PxU32 write ( const GXVoid* src, PxU32 count ) override;

        PxU32 getSize () const;
        PxU8* getData () const;
        GXVoid CacheToDisk ( GXWChar* filepath );
};


MemoryOutputStream::MemoryOutputStream ():
    _data ( nullptr ),
    _size ( 0u ),
    _capacity ( 0 )
{
    // NOTHING
}

MemoryOutputStream::~MemoryOutputStream()
{
    if ( !_data ) return;

    delete [] _data;
}

PxU32 MemoryOutputStream::write ( const GXVoid* src, PxU32 size )
{
    const PxU32 expectedSize = _size + size;

    if ( expectedSize > _capacity )
    {
        _capacity = expectedSize + 4096u;

        PxU8* newData = new PxU8[ _capacity ];
        PX_ASSERT ( newData != 0 );

        if ( newData )
        {
            memcpy ( newData, _data, _size );
            delete [] _data;
        }

        _data = newData;
    }

    memcpy ( _data + _size, src, size );
    _size += size;
    return size;
}

PxU32 MemoryOutputStream::getSize () const
{
    return _size; 
}

PxU8* MemoryOutputStream::getData () const
{
    return _data;
}

GXVoid MemoryOutputStream::CacheToDisk ( GXWChar* filepath )
{
    GXWriteToFile ( filepath, reinterpret_cast<const GXChar*> ( _data ), _size );
}

//---------------------------------------------------------------------------------------------------------------------

class MemoryInputData: public PxInputData
{
    private:
        PxU32           _size;
        const PxU8*     _data;
        PxU32           _pos;

    public:
        explicit MemoryInputData ( PxU8* data, PxU32 length );

        PxU32 read ( GXVoid* dest, PxU32 count );
        PxU32 getLength () const;
        GXVoid seek ( PxU32 pos );
        PxU32 tell () const;
};

MemoryInputData::MemoryInputData ( PxU8* data, PxU32 length ):
    _size ( length ),
    _data ( data ),
    _pos ( 0u )
{
    // NOTHING
}

PxU32 MemoryInputData::read ( GXVoid* dest, PxU32 count )
{
    PxU32 length = PxMin< PxU32 >( count, _size-_pos );
    memcpy ( dest, _data + _pos, length );
    _pos += length;
    return length;
}

PxU32 MemoryInputData::getLength () const
{
    return _size;
}

GXVoid MemoryInputData::seek ( PxU32 offset )
{
    _pos = PxMin< PxU32 >( _size, offset );
}

PxU32 MemoryInputData::tell () const
{
    return _pos;
}

//---------------------------------------------------------------------------------------------------------------------

PxFilterFlags DefaultFilterShader ( PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const GXVoid* constantBlock, PxU32 constantBlockSize )
{
    PxFilterFlags f = PxDefaultSimulationFilterShader ( attributes0, filterData0, attributes1, filterData1, pairFlags, constantBlock, constantBlockSize );
    return f;
}

//---------------------------------------------------------------------------------------------------------------------

static GXPhysicsErrorCallback       gGXPhysicsErrorCallback;
static PxDefaultAllocator           gGXPhysicsAllocatorCallback;

//---------------------------------------------------------------------------------------------------------------------


class GXPhysXBackend : public GXPhysXAdapter
{
    private:
        static GXBool                                   _isSimulating;
        static PxFoundation*                            _foundation;
        static PxPhysics*                               _physics;
        static PxScene*                                 _scene;
        static PxCooking*                               _cooking;
        static PxDefaultCpuDispatcher*                  _cpuDispatcher;
        static PxControllerManager*                     _controllerManager;
        static PxVec3                                   _gravity;
        static PxTolerancesScale                        _toleranceScale;
        static GXPhysXOnPhysics                         _onPhysics;
        static HANDLE                                   _simulateMutex;
        static GXDouble                                 _lastTime;
        static GXDouble                                 _accumulator;

        PxVisualDebuggerConnection*                     _pvdConnection;

        GXUInt                                          _maxVehicles;
        GXUInt                                          _numVehicles;
        PxVehicleWheels**                               _vehicles;

        PxVehicleDrivableSurfaceToTireFrictionPairs*    _tireSurfacePairs;

        GXUInt                                          _maxQueries;
        GXUInt                                          _numQueries;
        PxRaycastQueryResult*                           _queryResults;
        PxRaycastHit*                                   _queryHits;
        PxBatchQueryPreFilterShader                     _queryFilterShader;
        PxBatchQuery*                                   _batchQuery;

    public:
        GXPhysXBackend ();
        ~GXPhysXBackend () override;

        GXBool IsSumulating () override;

        GXBool SetLinearVelocity ( PxRigidDynamic* actor, PxVec3 &vel ) override;
        GXVoid SetOnPhysicsFunc ( GXPhysXOnPhysics callback ) override;

        PxRigidDynamic* CreateRigidDynamic ( GXVoid* address, PxTransform& location ) override;
        PxRigidStatic* CreateRigidStatic ( GXVoid* address, PxTransform &location ) override;
        PxVehicleDrive4W* CreateVehicle ( const GXVehicleInfo &info ) override;
        GXVoid DeleteVehicle ( PxVehicleDrive4W* vehicle ) override;
        PxController* CreateCharacterController ( PxCapsuleControllerDesc desc ) override;

        GXVoid SetRigidActorOrigin ( PxRigidActor* actor, const GXVec3 &location, const GXQuat &rotation ) override;

        GXVoid AddActor ( PxActor &actor ) override;
        GXVoid RemoveActor ( PxActor &actor ) override;

        PxMaterial* CreateMaterial ( GXFloat staticFriction, GXFloat dynamicFriction, GXFloat restitution ) override;
        
        PxTriangleMesh* CreateTriangleMesh ( const GXWChar* bke_file_name ) override;
        PxConvexMesh* CreateConvexMesh ( const GXWChar* bke_file_name ) override;

        PxParticleSystem* CreateParticleSystem ( GXUInt maxCountParticles, GXBool particleRestOffset = GX_FALSE ) override;
        GXVoid AddParticleSystem ( PxParticleSystem &ps ) override;

        GXVec3 GetGravityAcceleration () override;
        GXFloat GetSimulationDelay () override;
        GXFloat GetToleranceLength () override;

        GXBool RaycastSingle ( const GXVec3 &start, const GXVec3 &direction, const GXFloat maxDistance, PxRaycastHit &hit ) override;
        GXVoid DoSimulate () override;

        GXVoid ControlVehicle ( PxVehicleDrive4W* vehicle, GXUInt controlType, GXFloat value ) override;

    private:
        PxConvexMesh* CreateCylinderConvex ( GXFloat width, GXFloat radius, GXUInt numCirclePoints );

        static PxQueryHitType::Enum BatchQueryPreFilterShader ( PxFilterData filterData0, PxFilterData filterData1, const void* constantBlock, PxU32 constantBlockSize, PxHitFlags &filterFlags );
        static PxFilterFlags FilterShader ( PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize );
};

GXBool                      GXPhysXBackend::_isSimulating;
PxFoundation*               GXPhysXBackend::_foundation;
PxPhysics*                  GXPhysXBackend::_physics;
PxControllerManager*        GXPhysXBackend::_controllerManager;
PxCooking*                  GXPhysXBackend::_cooking;
PxScene*                    GXPhysXBackend::_scene;
PxDefaultCpuDispatcher*     GXPhysXBackend::_cpuDispatcher;
PxTolerancesScale           GXPhysXBackend::_toleranceScale;
PxVec3                      GXPhysXBackend::_gravity;
GXPhysXOnPhysics            GXPhysXBackend::_onPhysics;
HANDLE                      GXPhysXBackend::_simulateMutex;
GXDouble                    GXPhysXBackend::_lastTime;
GXDouble                    GXPhysXBackend::_accumulator = 0.0;

GXPhysXBackend::GXPhysXBackend ()
{
    GXLogInit ();

    _lastTime = GXGetProcessorTicks ();

    _scene = nullptr;
    _onPhysics = nullptr;

    _foundation = PxCreateFoundation ( PX_PHYSICS_VERSION, gGXPhysicsAllocatorCallback, gGXPhysicsErrorCallback );

    if ( !_foundation )
        GXLogW ( L"GXPhysXBackend::GXPhysXBackend::Error - pFoundation не создано\n" );

    _toleranceScale.length = PHYSICS_TOLERANCE_LENGTH;
    _toleranceScale.mass = PHYSICS_TOLERANCE_MASS;
    _toleranceScale.speed = PHYSICS_TOLERANCE_SPEED;
    _gravity.x = 0.0f;
    _gravity.y = GX_PHYSICS_GRAVITY_FACTOR;
    _gravity.z = 0.0f;

    _physics = PxCreatePhysics ( PX_PHYSICS_VERSION, *_foundation, _toleranceScale );

    if ( !_physics )
        GXLogW ( L"GXPhysXBackend::GXPhysXBackend::Error - pPhysics не создано\n" );

    if ( !PxInitExtensions ( *_physics ) )
        GXLogW ( L"GXPhysXBackend::GXPhysXBackend::Error - PxInitExtensions провалено\n" );

    _cooking = PxCreateCooking ( PX_PHYSICS_VERSION, *_foundation, PxCookingParams ( _toleranceScale ) );

    if ( !_cooking )
    {
        GXLogW ( L"GXPhysXBackend::GXPhysXBackend::Error - pCooking не создано\n" );
    }

    PxSceneDesc sceneDesc ( _physics->getTolerancesScale () );
    sceneDesc.gravity = _gravity;

    if ( !sceneDesc.cpuDispatcher )
    {
        _cpuDispatcher = PxDefaultCpuDispatcherCreate ( PHYSICS_NB_THREADS );

        if ( !_cpuDispatcher )
        {
            GXLogW ( L"GXPhysXBackend::GXPhysXBackend::Error - pCpuDispatcher не создано\n" );
        }
        else
        {
            sceneDesc.cpuDispatcher = _cpuDispatcher;
        }
    }

    if ( !sceneDesc.filterShader )
    {
        sceneDesc.filterShader = &FilterShader;
        sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
    }

    _scene = _physics->createScene ( sceneDesc );

    if ( !_scene )
        GXLogW ( L"GXPhysXBackend::GXPhysXBackend::Error - pScene не создано\n" );

    _controllerManager = PxCreateControllerManager ( *_scene );

    if ( !_controllerManager )
        GXLogW ( L"GXPhysXBackend::GXPhysXBackend::Error - pControllerManager не создано\n" );

    if ( !PxInitVehicleSDK ( *_physics ) )
        GXLogW ( L"GXPhysXBackend::GXPhysXBackend::Error - PxInitVehicleSDK провалено\n" );

    PxVehicleSetBasisVectors ( PxVec3 ( 0.0f, 1.0f, 0.0f ), PxVec3 ( 0.0f, 0.0f, 1.0f ) );
    PxVehicleSetUpdateMode ( PxVehicleUpdateMode::eVELOCITY_CHANGE );

    _maxVehicles = 8u;
    _numVehicles = 0u;
    _vehicles = static_cast<PxVehicleWheels**> ( malloc ( _maxVehicles * sizeof ( PxVehicleWheels* ) ) );

    _maxQueries = 8u * 6u;	//8 машин с 6 колёсами
    _numQueries = 0u;

    _queryResults = static_cast<PxRaycastQueryResult*> ( malloc ( _maxQueries * sizeof ( PxRaycastQueryResult ) ) );
    _queryHits = static_cast<PxRaycastHit*> ( malloc ( _maxQueries * sizeof ( PxRaycastHit ) ) );

    constexpr GXFloat frictionPairs[ 4u ][ 4u ] =
    {
        //  Сырые   Скользкие   Зимние  Внедорожные
        {   1.10f,  0.95f,      0.95f,  0.95f   },         // Грязь
        {   1.10f,  1.15f,      1.10f,  1.10f   },         // Асфальт
        {   0.70f,  0.70f,      0.70f,  0.70f   },         // Лёд
        {   0.80f,  0.80f,      0.80f,  0.80f   }          // Трава
    };


    PxMaterial* surfaceMaterials[ 4u ];
    surfaceMaterials[ 0u ] = _physics->createMaterial ( 0.2f, 0.5f, 0.5f );
    surfaceMaterials[ 1u ] = _physics->createMaterial ( 0.2f, 0.5f, 0.5f );
    surfaceMaterials[ 2u ] = _physics->createMaterial ( 0.2f, 0.5f, 0.5f );
    surfaceMaterials[ 3u ] = _physics->createMaterial ( 0.2f, 0.5f, 0.5f );

    PxVehicleDrivableSurfaceType surfaceTypes[ 4 ];

    for ( PxU32 i = 0u; i < 4u; ++i )
        surfaceTypes[ i ].mType = i;

    _tireSurfacePairs = PxVehicleDrivableSurfaceToTireFrictionPairs::allocate ( 4, 4 ); 
    _tireSurfacePairs->setup ( 4u, 4u, reinterpret_cast<const PxMaterial**> ( static_cast<GXVoid*> ( surfaceMaterials ) ), surfaceTypes );

    for ( PxU32 i = 0u; i < 4u; ++i )
    {
        for ( PxU32 j = 0u; j < 4u; ++j )
        {
            _tireSurfacePairs->setTypePairFriction ( i, j, frictionPairs[ i ][ j ] );
        }
    }

    PxBatchQueryDesc batchQueryDesc ( _maxQueries, 0u, 0u );
    batchQueryDesc.queryMemory.userRaycastResultBuffer = _queryResults;
    batchQueryDesc.queryMemory.userRaycastTouchBuffer = _queryHits;
    batchQueryDesc.queryMemory.raycastTouchBufferSize = _maxQueries;
    batchQueryDesc.preFilterShader = BatchQueryPreFilterShader;
    batchQueryDesc.spuPreFilterShader = nullptr;
    batchQueryDesc.spuPreFilterShaderSize = 0u;
    _batchQuery = _scene->createBatchQuery ( batchQueryDesc );

    _pvdConnection = nullptr;

    if ( _physics->getPvdConnectionManager () )
    {
        const GXChar pvd_host_ip[] = "localhost";
        const GXInt port = 5425;
        const GXUInt timeout = 100u;

        PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags ();
        _pvdConnection = PxVisualDebuggerExt::createConnection ( _physics->getPvdConnectionManager (), pvd_host_ip, port, timeout, connectionFlags );
    }

    _simulateMutex = CreateMutexW ( nullptr, GX_FALSE, L"GX_PHYSICS_SIMULATE_MUTEX" );
}

GXPhysXBackend::~GXPhysXBackend ()
{
    if ( _pvdConnection )
        _pvdConnection->release ();

    GXSafeFree ( _vehicles );
    GXSafeFree ( _queryResults );
    GXSafeFree ( _queryHits );
    _tireSurfacePairs->release ();

    _scene->release ();
    _controllerManager->release ();
    PxCloseVehicleSDK ();
    _physics->release ();
    PxCloseExtensions ();
    _cooking->release ();
    _foundation->release ();
    CloseHandle ( _simulateMutex );
}

GXBool GXPhysXBackend::IsSumulating ()
{
    return _isSimulating;
}

GXBool GXPhysXBackend::SetLinearVelocity ( PxRigidDynamic* actor, PxVec3 &vel )
{
    if ( _isSimulating ) return GX_FALSE;
    actor->setLinearVelocity ( vel );
    return GX_TRUE;
}

GXVoid GXPhysXBackend::SetOnPhysicsFunc ( GXPhysXOnPhysics callback )
{
    _onPhysics = callback;
}

PxRigidDynamic* GXPhysXBackend::CreateRigidDynamic ( GXVoid* address, PxTransform& location )
{
    PxRigidDynamic* rigidDynamic = _physics->createRigidDynamic ( location );
    rigidDynamic->setLinearDamping ( 0.001f );
    rigidDynamic->userData = address;
    return rigidDynamic;
}

PxRigidStatic* GXPhysXBackend::CreateRigidStatic ( GXVoid* address, PxTransform &location )
{
    PxRigidStatic* rigidStatic = _physics->createRigidStatic ( location );
    rigidStatic->userData = address;
    return rigidStatic;
}

PxVehicleDrive4W* GXPhysXBackend::CreateVehicle ( const GXVehicleInfo &info )
{
    if ( ( _numVehicles + 1 ) > _maxVehicles )
        return nullptr;

    PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate ( info.numWheels );
    PxVehicleDriveSimData4W driveSimData;
    PxVehicleChassisData chassisData;

    PxVec3 chassisCMOffset = PxVec3 ( info.chassisCoMOffset._data[ 1u ], info.chassisCoMOffset._data[ 1u ], info.chassisCoMOffset._data[ 2u ] );
    PxVec3 chassisMOI ( info.chassisMoI._data[ 0u ], info.chassisMoI._data[ 1u ], info.chassisMoI._data[ 2u ] );

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

    for ( GXUChar i = 0; i < info.numWheels; ++i )
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

        suspensionTravelDirections[ i ] = PxVec3 ( info.suspensionTraverses[ i ]._data[ 0u ], info.suspensionTraverses[ i ]._data[ 1u ], info.suspensionTraverses[ i ]._data[ 2u ] );
        wheelCentreCoMOffsets[ i ] = PxVec3 ( info.wheelLocalCentres[ i ]._data[ 0u ] - info.chassisCoMOffset._data[ 0u ], info.wheelLocalCentres[ i ]._data[ 1u ] - info.chassisCoMOffset._data[ 1u ],  info.wheelLocalCentres[ i ]._data[ 2u ] - info.chassisCoMOffset._data[ 2u ] );
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
            // NOTHING
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

    for ( GXUChar i = 0u; i < info.numGears; ++i )
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

    const GXQuat& spawnRotation = info.spawnRotation;
    PxRigidDynamic* rigidVehicle = _physics->createRigidDynamic ( PxTransform ( PxVec3 ( info.spawnLocation._data[ 0u ], info.spawnLocation._data[ 1u ], info.spawnLocation._data[ 2u ] ), PxQuat ( spawnRotation._data[ 1u ], spawnRotation._data[ 2u ], spawnRotation._data[ 3u ], spawnRotation._data[ 0u ] ) ) );
    rigidVehicle->userData = info.renderCarAddress;

    const PxMaterial* mtr = _physics->createMaterial ( 0.8f, 0.5f, 0.4f );

    for ( GXUChar i = 0u; i < info.numWheels; i++ )
    {
        PxConvexMesh* wheelConvex = CreateCylinderConvex ( info.wheelWidths[ i ], info.wheelRadiuses[ i ], 16 );
        PxConvexMeshGeometry wheelGeometry ( wheelConvex );
        PxShape* wheelShape = rigidVehicle->createShape ( wheelGeometry, *mtr );
            
        PxFilterData filterData;
        filterData.word0 = GX_PHYSICS_COLLUSION_GROUP_WHEEL;
        filterData.word1 = GX_PHYSICS_COLLUSION_GROUP_WHEEL | GX_PHYSICS_COLLUSION_GROUP_OBSTACLE;
        filterData.word3 = GX_PHYSICS_RAYCAST_UNDRIVABLE_SURFACE;

        wheelShape->setQueryFilterData ( filterData );
        wheelShape->setSimulationFilterData ( filterData );

        wheelShape->setLocalPose ( PxTransform::createIdentity () );
    }

    PxConvexMesh* convex = CreateConvexMesh ( info.bodyBakeFile );
    PxConvexMeshGeometry convexGeometry ( convex );
    PxShape* chassisShape = rigidVehicle->createShape ( convexGeometry, *mtr );
    chassisShape->setLocalPose ( PxTransform ( PxVec3 ( info.bodyConvexOffset._data[ 0u ], info.bodyConvexOffset._data[ 1u ], info.bodyConvexOffset._data[ 2u ] ) ) );

    PxFilterData filterData;
    filterData.word0 = GX_PHYSICS_COLLUSION_GROUP_CHASSIS;
    filterData.word1 = GX_PHYSICS_COLLUSION_GROUP_CHASSIS | GX_PHYSICS_COLLUSION_GROUP_DRIVABLE | GX_PHYSICS_COLLUSION_GROUP_OBSTACLE | GX_PHYSICS_COLLUSION_GROUP_WHEEL;
    filterData.word3 = GX_PHYSICS_RAYCAST_UNDRIVABLE_SURFACE;

    chassisShape->setSimulationFilterData ( filterData );
    chassisShape->setQueryFilterData ( filterData );

    rigidVehicle->setLinearDamping ( 0.001f );
    rigidVehicle->setMass ( info.chassisMass );
    rigidVehicle->setMassSpaceInertiaTensor ( PxVec3 ( info.chassisMoI._data[ 0u ], info.chassisMoI._data[ 1u ], info.chassisMoI._data[ 2u ] ) );
    rigidVehicle->setCMassLocalPose ( PxTransform ( PxVec3 ( info.chassisCoMOffset._data[ 0u ], info.chassisCoMOffset._data[ 1u ], info.chassisCoMOffset._data[ 2u ] ) ) );
    PxVec3 offset ( info.chassisCoMOffset._data[ 0u ], info.chassisCoMOffset._data[ 1u ], info.chassisCoMOffset._data[ 2u ] );

    PxVehicleDrive4W* vehicle = PxVehicleDrive4W::allocate ( info.numWheels );
    vehicle->setup ( _physics, rigidVehicle, *wheelsSimData, driveSimData, 0 );
    
    for ( GXUChar i = 0u; i < info.numWheels; ++i )
    {
        wheelsSimData->setWheelShapeMapping ( i, i );
        wheelsSimData->setSceneQueryFilterData ( i, filterData );
    }
    
    _scene->addActor ( *rigidVehicle );

    vehicle->mDriveDynData.setToRestState ();
    vehicle->mDriveDynData.setUseAutoGears ( GX_TRUE );

    _vehicles[ _numVehicles ] = vehicle;
    ++_numVehicles;

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

GXVoid GXPhysXBackend::DeleteVehicle ( PxVehicleDrive4W* vehicle )
{
    GXUInt id = 0u;

    for ( ; id < _maxVehicles; ++id )
    {
        if ( _vehicles[ id ] != vehicle ) continue;

        break;
    }

    vehicle->getRigidDynamicActor ()->release ();
    --_numVehicles;

    if ( id >= _numVehicles ) return;

    memcpy ( _vehicles + id, _vehicles + id + 1, _numVehicles * sizeof ( PxVehicleWheels* ) );
}

PxController* GXPhysXBackend::CreateCharacterController ( PxCapsuleControllerDesc desc )
{
    return _controllerManager->createController ( *_physics, _scene, desc );
}

GXVoid GXPhysXBackend::SetRigidActorOrigin ( PxRigidActor* actor, const GXVec3 &location, const GXQuat &rotation )
{
    actor->setGlobalPose ( PxTransform ( PxVec3 ( location._data[ 0u ], location._data[ 1u ], location._data[ 2u ] ), PxQuat ( rotation._data[ 1u ], rotation._data[ 2u ], rotation._data[ 3u ], rotation._data[ 0u ] ) ) );
}

GXVoid GXPhysXBackend::AddActor ( PxActor &actor )
{
    _scene->addActor ( actor );
}

GXVoid GXPhysXBackend::RemoveActor ( PxActor &actor )
{
    _scene->removeActor ( actor );
}

PxMaterial* GXPhysXBackend::CreateMaterial ( GXFloat staticFriction, GXFloat dynamicFriction, GXFloat restitution )
{
    return _physics->createMaterial ( staticFriction, dynamicFriction, restitution );
}

PxTriangleMesh* GXPhysXBackend::CreateTriangleMesh ( const GXWChar* bke_file_name )
{
    GXBakeInfo info;
    GXLoadBKE ( bke_file_name, info );

    GXUPointer size;
    GXUByte* cacheSource;

    GXFile file ( bke_file_name );

    if ( file.LoadContent ( cacheSource, size, eGXFileContentOwner::GXFile, GX_FALSE ) )
    {
        MemoryOutputStream	writebuffer;
        writebuffer.write ( cacheSource, static_cast<PxU32> ( size ) );
        MemoryInputData readBuffer ( writebuffer.getData (), writebuffer.getSize () );
        return _physics->createTriangleMesh ( readBuffer );
    }

    // Запечённой геометрии не оказалось. Запекаем геометрию и сбрасываем на диск.
    GXNativeStaticMeshInfo meshInfo;
    GXLoadNativeStaticMesh ( info._fileName, meshInfo );

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = meshInfo._numVertices;
    meshDesc.points.stride = sizeof ( PxVec3 );
    meshDesc.points.data = meshInfo._vboData;

    meshDesc.triangles.count = meshInfo._numElements / 3u;
    meshDesc.triangles.stride = 3u * sizeof ( PxU32 );
    meshDesc.triangles.data = meshInfo._eboData;

    MemoryOutputStream	writebuffer;

    if ( !_cooking->cookTriangleMesh ( meshDesc, writebuffer ) )
    {
        GXLogW ( L"GXPhysXBackend::CreateTriangleMesh::Error - Запекание файла %s провалено\n", info._fileName );
        return nullptr;
    }

    MemoryInputData readBuffer ( writebuffer.getData (), writebuffer.getSize () );

    writebuffer.CacheToDisk ( info._cacheFileName );
    info.Cleanup ();
    meshInfo.Cleanup ();

    return _physics->createTriangleMesh ( readBuffer );
}

PxConvexMesh* GXPhysXBackend::CreateConvexMesh ( const GXWChar* bke_file_name )
{
    GXBakeInfo info;
    GXLoadBKE ( bke_file_name, info );

    GXUPointer size;
    GXUByte* data;

    GXFile cacheFile ( bke_file_name );

    if ( cacheFile.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_FALSE ) )
    {
        // Запечённая геометрия есть в кеше
        MemoryOutputStream	writebuffer;
        writebuffer.write ( data, static_cast<PxU32> ( size ) );
        MemoryInputData readBuffer ( writebuffer.getData (), writebuffer.getSize () );
        return _physics->createConvexMesh ( readBuffer );
    }

    //Запечённой геометрии в кеше не оказалось. Запекаем и сбрасывает кеш на диск.

    GXFile rawFile ( info._fileName );
    rawFile.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_TRUE );

    const GXPhysicsGeometryHeader* h = reinterpret_cast<const GXPhysicsGeometryHeader*> ( data );

    PxConvexMeshDesc convexDesc;
    convexDesc.points.count	= h->_numVertices;
    convexDesc.points.stride = sizeof ( PxVec3 );
    convexDesc.points.data = data + sizeof ( GXPhysicsGeometryHeader );
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

    PxConvexMesh* convexMesh = nullptr;
    MemoryOutputStream buf;

    if ( _cooking->cookConvexMesh ( convexDesc, buf ) )
    {
        MemoryInputData id ( buf.getData(), buf.getSize () );
        convexMesh = _physics->createConvexMesh ( id );
    }
    else
    {
        GXLogW ( L"GXPhysXBackend::CreateConvexMesh::Error - Запекание файла %s провалено\n", info._fileName );
        return nullptr;
    }

    buf.CacheToDisk ( info._cacheFileName );
    info.Cleanup ();

    return convexMesh;
}

PxParticleSystem* GXPhysXBackend::CreateParticleSystem ( GXUInt maxCountParticles, GXBool particleRestOffset )
{
    return _physics ? _physics->createParticleSystem ( maxCountParticles, particleRestOffset ) : nullptr;
}

GXVoid GXPhysXBackend::AddParticleSystem ( PxParticleSystem &ps )
{
    if ( !_scene ) return;
    _scene->addActor ( ps );
}

GXVec3 GXPhysXBackend::GetGravityAcceleration ()
{
    return GXVec3 ( _gravity.x, _gravity.y, _gravity.z );
}

GXFloat GXPhysXBackend::GetSimulationDelay ()
{
    constexpr GXFloat delay = static_cast<GXFloat> ( PHYSICS_SIMULATE_DELAY );
    return delay;
}

GXFloat GXPhysXBackend::GetToleranceLength ()
{
    return _toleranceScale.length;
}

GXBool GXPhysXBackend::RaycastSingle ( const GXVec3 &start, const GXVec3 &direction, const GXFloat maxDistance, PxRaycastHit &hit )
{
    WaitForSingleObject ( _simulateMutex, INFINITE );
    static const PxSceneQueryFlags outputFlags = PxSceneQueryFlag::eDISTANCE | PxSceneQueryFlag::eIMPACT | PxSceneQueryFlag::eNORMAL;

    if ( !_scene )
    {
        ReleaseMutex ( _simulateMutex );
        return GX_FALSE;
    }

    const GXBool status =  _scene->raycastSingle ( PxVec3 ( start._data[ 0u ], start._data[ 1u ], start._data[ 2u ] ), PxVec3 ( direction._data[ 0u ], direction._data[ 1u ], direction._data[ 2u ] ), maxDistance, outputFlags, hit );
    ReleaseMutex ( _simulateMutex );

    return status;
}

GXBool gx_physiscs_Flag = GX_TRUE;

GXVoid GXPhysXBackend::DoSimulate ()
{
    if ( gx_physiscs_Flag )
    {
        _lastTime = GXGetProcessorTicks ();
        _accumulator = 0.0;
        gx_physiscs_Flag = GX_FALSE;
        return;
    }

    GXDouble newtime = GXGetProcessorTicks ();
    _accumulator += ( newtime - _lastTime ) / GXGetProcessorTicksPerSecond ();
    _lastTime = newtime;

    WaitForSingleObject ( _simulateMutex, INFINITE );
    _isSimulating = GX_TRUE;
    constexpr PxReal delay = static_cast<GXFloat> ( PHYSICS_SIMULATE_DELAY );

    while ( _accumulator >= PHYSICS_SIMULATE_DELAY )
    {
        if ( _scene )
        {
            _scene->simulate ( delay );
            _scene->checkResults ( GX_TRUE );
            _scene->fetchResults ();

            if ( _numVehicles )
            {
                PxVehicleSuspensionRaycasts ( _batchQuery, _numVehicles, _vehicles, _maxQueries, _queryResults );
                PxVehicleUpdates ( delay, _gravity, *_tireSurfacePairs, _numVehicles, _vehicles, 0 );
            }

            PxU32 nbActiveTransforms;
            const PxActiveTransform* activeTransforms = _scene->getActiveTransforms ( nbActiveTransforms );

            for ( PxU32 i = 0u; i < nbActiveTransforms; ++i )
            {
                const PxActiveTransform& activeTransform = activeTransforms[ i ];

                if ( activeTransform.userData && activeTransform.actor->isRigidBody () )
                {
                    const PxVec3& location = activeTransform.actor2World.p;
                    const PxQuat& rotation = activeTransform.actor2World.q;

                    GXPhysXActorState* renderObject = static_cast<GXPhysXActorState*> ( activeTransform.userData );
                    renderObject->SetPivotOrigin ( GXVec3 ( location.x, location.y, location.z ), GXQuat ( rotation.w, rotation.x, rotation.y, rotation.z ) );
                }
            }

            PxShape* carShapes[ 7u ];   // 6 колёс и корпус. Уйти от констант!!!!!

            for ( GXUInt i = 0u; i < _numVehicles; ++i )
            {
                const PxRigidDynamic* rigidVihicle = _vehicles[ i ]->getRigidDynamicActor ();
                const GXUInt numShapes = rigidVihicle->getNbShapes ();
                rigidVihicle->getShapes ( carShapes, numShapes );
                GXPhysXActorState* vehicle = static_cast<GXPhysXActorState*> ( rigidVihicle->userData );

                for ( GXUInt shapeID = 0u; shapeID < numShapes; ++shapeID )
                {
                    const PxTransform transform = PxShapeExt::getGlobalPose ( *carShapes[ shapeID ], *rigidVihicle );
                    const PxVec3& location = transform.p;
                    const PxQuat& rotation = transform.q;

                    vehicle->SetShapeOrigin ( static_cast<GXUShort> ( shapeID ), GXVec3 ( location.x, location.y, location.z ), GXQuat ( rotation.w, rotation.x, rotation.y, rotation.z ) );
                }
            }
        }

        if ( _onPhysics )
            _onPhysics ( delay );
        
        _accumulator -= PHYSICS_SIMULATE_DELAY;
    }

    ReleaseMutex ( _simulateMutex );
    _isSimulating = GX_FALSE;
}

GXVoid GXPhysXBackend::ControlVehicle ( PxVehicleDrive4W* vehicle, GXUInt controlType, GXFloat value )
{
    vehicle->mDriveDynData.setAnalogInput ( controlType, value );
}

PxConvexMesh* GXPhysXBackend::CreateCylinderConvex ( GXFloat width, GXFloat radius, GXUInt numCirclePoints )
{

#define  MAX_NUM_VERTS_IN_CIRCLE 8

    if ( numCirclePoints < MAX_NUM_VERTS_IN_CIRCLE )
        numCirclePoints = MAX_NUM_VERTS_IN_CIRCLE;

    const GXUInt numVerts = 2u * numCirclePoints;
    PxVec3* verts = static_cast<PxVec3*> ( malloc ( numVerts * sizeof ( PxVec3 ) ) );
    
    const GXFloat dtheta = 2.0f * GX_MATH_PI / numCirclePoints;

    for( GXUInt i = 0u; i < numCirclePoints; ++i )
    {
        const GXFloat theta = dtheta * i;
        const GXFloat cosTheta = radius * cosf ( theta );
        const GXFloat sinTheta = radius * sinf ( theta );

        verts[ i << 1 ] = PxVec3 ( -0.5f * width, cosTheta, sinTheta );
        verts[ ( i << 1 ) + 1 ] = PxVec3 ( 0.5f * width, cosTheta, sinTheta );
    }

    PxConvexMeshDesc convexDesc;
    convexDesc.points.count	= numVerts;
    convexDesc.points.stride = sizeof ( PxVec3 );
    convexDesc.points.data = verts;
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

    PxConvexMesh* convexMesh = nullptr;
    MemoryOutputStream buf;

    if ( _cooking->cookConvexMesh ( convexDesc, buf ) )
    {
        MemoryInputData id ( buf.getData(), buf.getSize () );
        convexMesh = _physics->createConvexMesh ( id );
    }

    free ( verts );
    return convexMesh;

#undef MAX_NUM_VERTS_IN_CIRCLE

}

PxQueryHitType::Enum GXPhysXBackend::BatchQueryPreFilterShader ( PxFilterData /*filterData0*/, PxFilterData filterData1, const void* /*constantBlock*/, PxU32 /*constantBlockSize*/, PxHitFlags& /*filterFlags*/ )
{
    if ( filterData1.word3 == GX_PHYSICS_RAYCAST_DRIVABLE_SURFACE )
        return PxQueryHitType::eBLOCK;

    return PxQueryHitType::eNONE;
}

PxFilterFlags GXPhysXBackend::FilterShader ( PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize )
{
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

GXDLLEXPORT GXPhysXAdapter* GXCALL GXPhysXCreate ()
{
    return new GXPhysXBackend ();
}

GXDLLEXPORT GXVoid GXCALL GXPhysXDestroy ( GXPhysXAdapter* physics )
{
    GXSafeDelete ( physics );
}
