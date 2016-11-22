//version 1.1

#include <GXEngine/GXPlayerBoss.h>


GXPlayerBoss::GXPlayerBoss ()
{
	GXPhysics* physics = gx_Core->GetPhysics ();
	fToleranceLength = physics->GetToleranceLength ();
	gravity = physics->GetGravityAcceleration ();
	gravity.y = GX_PHYSICS_GRAVITY_FACTOR * 0.5f;
	fSimulationDelay = physics->GetSimulationDelay ();

	bIsWalk = bIsRun = bIsStrafe = bIsRotate = bIsJumping = GX_FALSE;
	fStrafeSpeed = 20.0f;
	fWalkSpeed = 20.0f;
	fRunSpeed = 60.0f;
	fRotateSpeed = 3.0f;
	fJumpSpeed = 2.0f;
	fHitImpulse = 5.0f;
	fCurrentVerticalSpeed = 0.0f;
	lasttime = 0;
	mesh = 0;
	actor = 0;
	SetRotation ( 0.0f, 0.0f, 0.0f );

	animSolver = 0;
}

GXPlayerBoss::~GXPlayerBoss ()
{
	delete hitReport;
}

GXVoid GXPlayerBoss::SetAnimSolver ( GXAnimSolverMovement* animSolver )
{
	this->animSolver = animSolver;
}

GXFloat GXPlayerBoss::GetHitImpulse ()
{
	return fHitImpulse;
}

GXVoid GXPlayerBoss::GetLocation ( GXVec3* out )
{
	memcpy ( out->v, location.v, sizeof ( GXVec3 ) );
}

GXVoid GXPlayerBoss::GetRotation ( GXVec3* out_rad )
{
	memcpy ( out_rad->v, rotation.v, sizeof ( GXVec3 ) );
}

GXVoid GXPlayerBoss::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	location.x = x;
	location.y = y;
	location.z = z;
	if ( mesh )
		mesh->SetLocation ( location.v );
}

GXVoid GXPlayerBoss::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	rotation.pitch_rad = pitch_rad;
	rotation.yaw_rad = yaw_rad;
	rotation.roll_rad = roll_rad;
	if ( mesh )
		mesh->SetRotation ( rotation );
}

GXVoid GXPlayerBoss::SetRotation ( GXQuat qt )
{
	//NOTHING
}

PxControllerFilters gx_PlayerControllerFilters;

GXVoid GXPlayerBoss::Perform ( GXFloat deltatime )
{
	if ( !animSolver ) return;
	
	if ( bIsRun ) animSolver->SetState ( GX_ANIM_SOLVER_MOVEMENT_RUN );
	else if ( bIsWalk )	animSolver->SetState ( GX_ANIM_SOLVER_MOVEMENT_WALK );
	else animSolver->SetState ( GX_ANIM_SOLVER_MOVEMENT_IDLE );

	if ( !actor ) return;

	PxExtendedVec3 pos = actor->getFootPosition ();
	SetLocation ( (GXFloat)pos.x, (GXFloat)pos.y, (GXFloat)pos.z );

	fJumpTime += deltatime;
	GXFloat speed;
	if ( bIsRotate )
	{
		speed = ( ( eRotate == BOSS_LEFT ) ? -fRotateSpeed : fRotateSpeed ) * deltatime;
		rotation.yaw_rad += speed;
		SetRotation ( rotation.pitch_rad, rotation.yaw_rad, rotation.roll_rad );
	}

	PxVec3 resultDir;
	resultDir.x = resultDir.y = resultDir.z = 0.0f;

	if ( bIsWalk )
	{
		speed = ( bIsRun ? fRunSpeed : fWalkSpeed ) * deltatime;
		speed = ( eWalk == BOSS_FORWARD ) ? -speed : speed;
		resultDir.z = cos ( rotation.yaw_rad ) * speed;
		resultDir.x = sin ( rotation.yaw_rad ) * speed;
	}

	if ( bIsStrafe )
	{
		speed = ( ( eStrafe == BOSS_LEFT ) ? fStrafeSpeed : -fStrafeSpeed ) * deltatime;
		resultDir.z += cos ( rotation.yaw_rad + GX_MATH_HALFPI ) * speed;
		resultDir.x += sin ( rotation.yaw_rad + GX_MATH_HALFPI ) * speed;
	}

	fCurrentVerticalSpeed += deltatime * gravity.y;
	resultDir.y = fCurrentVerticalSpeed;

	PxU32 doEvent = actor->move ( resultDir, 0.03f, deltatime, gx_PlayerControllerFilters );
	switch ( doEvent )
	{
		case PxControllerFlag::eCOLLISION_DOWN:
			bIsJumping = GX_FALSE;
			fCurrentVerticalSpeed = 0.0f;
			fJumpTime = 0;
		break;

		case PxControllerFlag::eCOLLISION_UP:
			fCurrentVerticalSpeed = 0.0f;
			fJumpTime = 0;
		break;
	}
}

GXVoid GXPlayerBoss::WalkForward ()
{
	if ( bIsWalk && ( eWalk == BOSS_FORWARD ) ) return;
	bIsWalk = GX_TRUE;
	eWalk = BOSS_FORWARD;
}

GXVoid GXPlayerBoss::WalkBackward ()
{
	if ( bIsWalk && ( eWalk == BOSS_BACKWARD ) ) return;
	bIsWalk = GX_TRUE;
	bIsRun = GX_FALSE;
	eWalk = BOSS_BACKWARD;
}

GXVoid GXPlayerBoss::StopWalk ()
{
	if ( !bIsWalk ) return;
	bIsWalk = GX_FALSE;
}

GXVoid GXPlayerBoss::Freeze ()
{
	if ( !bIsWalk && !bIsRun && !bIsStrafe && !bIsRotate ) return;
	bIsWalk = bIsRun = bIsStrafe = bIsRotate = GX_FALSE;
}

GXVoid GXPlayerBoss::Run ()
{
	if ( !bIsWalk || ( eWalk != BOSS_FORWARD ) || bIsRun ) return;
	bIsRun = GX_TRUE;
}

GXVoid GXPlayerBoss::StopRun ()
{
	if ( !bIsRun ) return;
	bIsRun = GX_FALSE;
}

GXVoid GXPlayerBoss::StrafeLeft ()
{
	if ( bIsStrafe && ( eStrafe == BOSS_LEFT )  ) return;
	eStrafe = BOSS_LEFT;
	bIsStrafe = GX_TRUE;
}

GXVoid GXPlayerBoss::StrafeRight ()
{
	if ( bIsStrafe && ( eStrafe == BOSS_RIGHT ) ) return;
	eStrafe = BOSS_RIGHT;
	bIsStrafe = GX_TRUE;
}

GXVoid GXPlayerBoss::StopStrafe ()
{
	bIsStrafe = GX_FALSE;
}

GXVoid GXPlayerBoss::RotateLeft ()
{
	if ( bIsRotate && ( eRotate == BOSS_LEFT ) ) return;
	eRotate = BOSS_LEFT;
	bIsRotate = GX_TRUE;
}

GXVoid GXPlayerBoss::RotateRight ()
{
	if ( bIsRotate && ( eRotate == BOSS_RIGHT ) ) return;
	eRotate = BOSS_RIGHT;
	bIsRotate = GX_TRUE;
}

GXVoid GXPlayerBoss::StopRotate ()
{
	if ( !bIsRotate ) return;
	bIsRotate = GX_FALSE;
}

GXVoid GXPlayerBoss::Jump ()
{
	if ( bIsJumping ) return;
	bIsJumping = GX_TRUE;
	fCurrentVerticalSpeed = fJumpSpeed;
	fJumpTime = 0;
}


//--------------------------------------------------

GXHitReportBase::~GXHitReportBase ()
{
	//NOTHING
}

GXVoid GXHitReportBase::onShapeHit ( const PxControllerShapeHit& hit )
{
	//NOTHING
}

GXVoid GXHitReportBase::onControllerHit ( const PxControllersHit& hit )
{
	//NOTHING
}

GXVoid GXHitReportBase::onObstacleHit ( const PxControllerObstacleHit& hit )
{
	//NOTHING
}