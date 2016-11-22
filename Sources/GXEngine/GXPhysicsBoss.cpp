//version 1.1

#include <GXEngine/GXPhysicsBoss.h>


GXPhysicsBoss::GXPhysicsBoss ()
{
	mesh = 0;
	actor = 0;
}

GXVoid GXPhysicsBoss::SetPivotOrigin ( const GXVec3 &location, const GXQuat &rotation )
{
	SetLocation ( location.x, location.y, location.z );
	SetRotation ( rotation );
}

GXVoid GXPhysicsBoss::SetShapeOrigin ( GXUShort shapeID, const GXVec3 &location, const GXQuat &rotation ) 
{
	//NOTHING
}

GXVoid GXPhysicsBoss::SetRotation ( GXQuat qt )
{
	if ( mesh )
		mesh->SetRotation ( qt );
}

GXVoid GXPhysicsBoss::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	if ( mesh )
		mesh->SetRotation ( pitch_rad, yaw_rad, roll_rad );
}

GXVoid GXPhysicsBoss::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	if ( mesh )
		mesh->SetLocation ( x, y, z );
}

GXVoid GXPhysicsBoss::SetRigidDynamicActor ( PxRigidDynamic* actor )
{
	this->actor = actor;
}

PxRigidDynamic* GXPhysicsBoss::GetRigidDynamicActor ()
{
	return actor;
}

GXVoid GXPhysicsBoss::SetMesh ( GXMesh* mesh )
{
	this->mesh = mesh;
}

GXMesh* GXPhysicsBoss::GetMesh ()
{
	return mesh;
}