//version 1.0

#include <GXEngine/GXRenderable.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXMemory.h>


GXRenderable::GXRenderable ()
{
	GXSetMat4Identity ( trans_mat );
	GXSetMat4Identity ( rot_mat );
	GXSetMat4Identity ( scale_mat );
	GXSetMat4Identity ( scale_rot_mat );
	GXSetMat4Identity ( mod_mat );
}

GXRenderable::~GXRenderable ()
{
	// NOTHING
}

GXVoid GXRenderable::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	GXSetMat4Translation ( trans_mat, x, y, z );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXRenderable::SetLocation ( const GXVec3 &loc )
{
	trans_mat.wv = loc;
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXRenderable::SetRotation ( const GXVec3 &rot_rad  )
{
	GXSetMat4RotationXYZ ( rot_mat, rot_rad.pitch_rad, rot_rad.yaw_rad, rot_rad.roll_rad );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXRenderable::SetRotation ( const GXMat4 &rot )
{
	rot_mat = rot;
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXRenderable::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( rot_mat, pitch_rad, yaw_rad, roll_rad );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXRenderable::SetRotation ( const GXQuat &quaternion )
{
	rot_mat.SetRotation ( quaternion );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXRenderable::SetScale ( GXFloat x, GXFloat y, GXFloat z ) 
{
	GXSetMat4Scale ( scale_mat, x, y, z );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXRenderable::SetScale ( const GXVec3 &scale )
{
	GXSetMat4Scale ( scale_mat, scale.x, scale.y, scale.z );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXRenderable::GetLocation ( GXVec3 &loc ) const
{
	loc = trans_mat.wv;
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
	scale.x = scale_mat.m11;
	scale.y = scale_mat.m22;
	scale.z = scale_mat.m33;
}

const GXMat4& GXRenderable::GetModelMatrix () const
{
	return mod_mat;
}
