//version 1.0

#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXMemory.h>


GXRenderable::GXRenderable ()
{
	location = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	scale = GXCreateVec3 ( 1.0f, 1.0f, 1.0f );
	GXSetMat4Identity ( rot_mat );
	GXSetMat4Identity ( mod_mat );
}

GXRenderable::~GXRenderable ()
{
	// NOTHING
}

GXVoid GXRenderable::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	location = GXCreateVec3 ( x, y, z );
	mod_mat.wv = location;

	UpdateBounds ();
}

GXVoid GXRenderable::SetLocation ( const GXVec3 &loc )
{
	location = loc;
	mod_mat.wv = location;

	UpdateBounds ();
}

GXVoid GXRenderable::SetRotation ( const GXVec3 &rot_rad  )
{
	GXSetMat4RotationXYZ ( rot_mat, rot_rad.pitch_rad, rot_rad.yaw_rad, rot_rad.roll_rad );
	
	GXMulVec3Scalar ( mod_mat.xv, rot_mat.xv, scale.x );
	GXMulVec3Scalar ( mod_mat.yv, rot_mat.yv, scale.y );
	GXMulVec3Scalar ( mod_mat.zv, rot_mat.zv, scale.z );

	UpdateBounds ();
}

GXVoid GXRenderable::SetRotation ( const GXMat4 &rot )
{
	rot_mat = rot;

	GXMulVec3Scalar ( mod_mat.xv, rot_mat.xv, scale.x );
	GXMulVec3Scalar ( mod_mat.yv, rot_mat.yv, scale.y );
	GXMulVec3Scalar ( mod_mat.zv, rot_mat.zv, scale.z );

	UpdateBounds ();
}

GXVoid GXRenderable::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( rot_mat, pitch_rad, yaw_rad, roll_rad );

	GXMulVec3Scalar ( mod_mat.xv, rot_mat.xv, scale.x );
	GXMulVec3Scalar ( mod_mat.yv, rot_mat.yv, scale.y );
	GXMulVec3Scalar ( mod_mat.zv, rot_mat.zv, scale.z );

	UpdateBounds ();
}

GXVoid GXRenderable::SetRotation ( const GXQuat &quaternion )
{
	rot_mat.SetRotation ( quaternion );

	GXMulVec3Scalar ( mod_mat.xv, rot_mat.xv, scale.x );
	GXMulVec3Scalar ( mod_mat.yv, rot_mat.yv, scale.y );
	GXMulVec3Scalar ( mod_mat.zv, rot_mat.zv, scale.z );

	UpdateBounds ();
}

GXVoid GXRenderable::SetScale ( GXFloat x, GXFloat y, GXFloat z ) 
{
	scale = GXCreateVec3 ( x, y, z );

	GXMulVec3Scalar ( mod_mat.xv, rot_mat.xv, scale.x );
	GXMulVec3Scalar ( mod_mat.yv, rot_mat.yv, scale.y );
	GXMulVec3Scalar ( mod_mat.zv, rot_mat.zv, scale.z );

	UpdateBounds ();
}

GXVoid GXRenderable::SetScale ( const GXVec3 &scale )
{
	this->scale = scale;

	GXMulVec3Scalar ( mod_mat.xv, rot_mat.xv, scale.x );
	GXMulVec3Scalar ( mod_mat.yv, rot_mat.yv, scale.y );
	GXMulVec3Scalar ( mod_mat.zv, rot_mat.zv, scale.z );

	UpdateBounds ();
}

GXVoid GXRenderable::GetLocation ( GXVec3 &loc ) const
{
	loc = location;
} 

GXVoid GXRenderable::GetRotation ( GXMat4 &rot ) const
{
	rot = rot_mat;
}

GXVoid GXRenderable::GetRotation ( GXQuat &rot ) const
{
	rot = GXCreateQuat ( rot_mat );
}

GXVoid GXRenderable::GetScale ( GXVec3 &scale ) const
{
	scale = this->scale;
}

const GXMat4& GXRenderable::GetModelMatrix () const
{
	return mod_mat;
}
