//version 1.12

#ifndef GX_PHYSX
#define GX_PHYSX


#include <GXCommon/GXMath.h>

GX_DISABLE_COMMON_WARNINGS

#include <PhysX/PxPhysicsAPI.h>

GX_RESTORE_WARNING_STATE


#define GX_PHYSICS_GRAVITY_FACTOR		-9.81f


typedef GXVoid ( GXCALL* PLONPHYSXPROC ) ( GXFloat deltatime );


using namespace physx;

//----------------------------------------------------------------------

class GXPhysXActorState
{
	public:
		virtual ~GXPhysXActorState (){ /*PURE VIRTUAL*/ };

		virtual GXVoid SetPivotOrigin ( const GXVec3 &location, const GXQuat &rotation ) = 0;
		virtual GXVoid SetShapeOrigin ( GXUShort shapeID, const GXVec3 &location, const GXQuat &rotation ) = 0;
};

//----------------------------------------------------------------------

#define GX_PHYSICS_COLLUSION_GROUP_DEFAULT		0x00000000
#define GX_PHYSICS_COLLUSION_GROUP_DRIVABLE		0x00000001
#define GX_PHYSICS_COLLUSION_GROUP_WHEEL		0x00000002
#define GX_PHYSICS_COLLUSION_GROUP_CHASSIS		0x00000004
#define GX_PHYSICS_COLLUSION_GROUP_OBSTACLE		0x00000008

#define GX_PHYSICS_RAYCAST_DRIVABLE_SURFACE		0xFFFF0000
#define GX_PHYSICS_RAYCAST_UNDRIVABLE_SURFACE	0x0000FFFF

#define GX_PHYSICS_DIFFERENTIAL_LS_4WD			0	//limited slip differential for car with 4 driven wheels
#define GX_PHYSICS_DIFFERENTIAL_LS_FRONTWD		1	//limited slip differential for car with front-wheel drive
#define GX_PHYSICS_DIFFERENTIAL_LS_REARWD		2	//limited slip differential for car with rear-wheel drive
#define GX_PHYSICS_DIFFERENTIAL_OPEN_4WD		3	//open differential for car with 4 driven wheels 
#define GX_PHYSICS_DIFFERENTIAL_OPEN_FRONTWD	4	//open differential for car with front-wheel drive
#define GX_PHYSICS_DIFFERENTIAL_OPEN_REARWD		5	//open differentila for car with rear-wheel drive

//----------------------------------------------------------------------

#define GX_PHYSICS_VEHICLE_ACCELERATE	0
#define GX_PHYSICS_VEHICLE_BRAKE		1
#define GX_PHYSICS_VEHICLE_HANDBRAKE	2
#define GX_PHYSICS_VEHICLE_TURN_LEFT	3
#define GX_PHYSICS_VEHICLE_TURN_RIGHT	4


struct GXVehicleInfo
{
	GXPhysXActorState*			renderCarAddress;

	GXVec3						spawnLocation;
	GXQuat						spawnRotation;

	GXFloat						chassisMass;
	GXVec3						chassisCoMOffset;
	GXVec3						chassisMoI;

	const GXWChar*				bodyBakeFile;
	GXVec3						bodyConvexOffset;

	GXUChar						numWheels;

	GXFloat*					wheelRadiuses;
	GXFloat*					wheelWidths;
	GXVec3*						wheelLocalCentres;
	GXFloat*					wheelMasses;
	GXFloat*					wheelMoIs;
	GXFloat*					wheelBreaks;
	GXFloat*					wheelHandBreaks;
	GXFloat*					wheelSteers;
	GXFloat*					wheelDampings;

	GXFloat*					suspensionSprungMasses;
	GXFloat*					suspensionSpringForces;
	GXFloat*					suspensionSpringDampers;
	GXFloat*					suspensionCompressions;
	GXFloat*					suspensionDroops;
	GXVec3*						suspensionTraverses;

	GXUChar						differentialType;

	GXFloat						clutchStrength;

	GXFloat						enginePeakTorque;
	GXFloat						engineRotation;

	GXUChar						numGears;

	GXFloat*					gearboxRatios;
	GXFloat						gearboxSwitchTime;

	GXFloat						ackermannAccuracy;
	GXFloat						ackermannFrontWidth;
	GXFloat						ackermannRearWidth;
	GXFloat						ackermannAxleSeparation;
};

//----------------------------------------------------------------------

class GXPhysXAdapter
{
	public:
		virtual ~GXPhysXAdapter (){ /*Pure Virtual*/ };

		virtual GXBool IsSumulating () = 0;

		virtual GXBool SetLinearVelocity ( PxRigidDynamic* actor, PxVec3 &vel ) = 0;
		virtual GXVoid SetOnPhysicsFunc ( PLONPHYSXPROC func ) = 0;

		virtual PxRigidDynamic* CreateRigidDynamic ( GXVoid* address, PxTransform &location ) = 0;
		virtual PxRigidStatic* CreateRigidStatic ( GXVoid* address, PxTransform &location ) = 0;
		virtual PxVehicleDrive4W* CreateVehicle ( const GXVehicleInfo &info ) = 0;
		virtual GXVoid DeleteVehicle ( PxVehicleDrive4W* vehicle ) = 0;
		virtual PxController* CreateCharacterController ( PxCapsuleControllerDesc desc ) = 0;

		virtual GXVoid SetRigidActorOrigin ( PxRigidActor* actor, const GXVec3 &location, const GXQuat &rotation ) = 0;

		virtual GXVoid AddActor ( PxActor &actor ) = 0;
		virtual GXVoid RemoveActor ( PxActor &actor ) = 0;

		virtual PxMaterial* CreateMaterial ( GXFloat staticFriction, GXFloat dynamicFriction, GXFloat restitution ) = 0;

		virtual PxTriangleMesh* CreateTriangleMesh ( const GXWChar* bke_file_name ) = 0;
		virtual PxConvexMesh* CreateConvexMesh ( const GXWChar* bke_file_name ) = 0;

		virtual PxParticleSystem* CreateParticleSystem ( GXUInt maxCountParticles, GXBool particleRestOffset = GX_FALSE ) = 0;
		virtual GXVoid AddParticleSystem ( PxParticleSystem &ps ) = 0;

		virtual GXVec3 GetGravityAcceleration () = 0;
		virtual GXFloat GetSimulationDelay () = 0;
		virtual GXFloat GetToleranceLength () = 0;

		virtual GXBool RaycastSingle ( const GXVec3 &start, const GXVec3 &direction, const GXFloat maxDistance, PxRaycastHit &hit ) = 0;
		virtual GXVoid DoSimulate () = 0;

		virtual GXVoid ControlVehicle ( PxVehicleDrive4W* vehicle, GXUInt controlType, GXFloat value ) = 0;
};


#endif //GX_PHYSX
