//version 1.10

#include <GXEngine/GXMesh.h>
#include <GXEngine/GXCamera.h>
#include <GXCommon/GXCommon.h>


GXMesh::GXMesh ()
{
	GXSetMat4Identity ( trans_mat );
	GXSetMat4Identity ( rot_mat );
	GXSetMat4Identity ( scale_mat );
	GXSetMat4Identity ( scale_rot_mat );
	GXSetMat4Identity ( mod_mat );

	memset ( &vaoInfo, 0, sizeof ( GXVAOInfo ) );
	GXSetAABBEmpty ( boundsWorld );

	numPrograms = 0;
	programs = 0;

	numTextures = 0;
	textures = 0;
}

GXMesh::~GXMesh ()
{
	for ( GXUByte i = 0; i < numPrograms; i++ )
		GXRemoveShaderProgram ( programs[ i ] );

	GXSafeFree ( programs );

	for ( GXUByte i = 0; i < numTextures; i++ )
		GXRemoveTexture ( textures[ i ] );

	GXSafeFree ( textures );
}

GXVoid GXMesh::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	GXSetMat4Translation ( trans_mat, x, y, z );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::SetLocation ( const GXVec3 &loc )
{
	trans_mat.wv = loc;
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::SetRotation ( GXFloat* Rot_rad )
{
	GXSetMat4RotationXYZ ( rot_mat, Rot_rad[ 0 ], Rot_rad[ 1 ], Rot_rad[ 2 ] );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::SetRotation ( const GXVec3 &rot_rad  )
{
	GXSetMat4RotationXYZ ( rot_mat, rot_rad.pitch_rad, rot_rad.yaw_rad, rot_rad.roll_rad );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::SetRotation ( const GXMat4 &rot )
{
	rot_mat = rot;
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( rot_mat, pitch_rad, yaw_rad, roll_rad );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::SetRotation ( const GXQuat &q )
{
	rot_mat.SetRotation ( q );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::SetScale ( GXFloat x, GXFloat y, GXFloat z )
{
	GXSetMat4Scale ( scale_mat, x, y, z );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::SetScale ( GXFloat* Scl )
{
	GXSetMat4Scale ( scale_mat, Scl[ 0 ], Scl[ 1 ], Scl[ 2 ] );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::SetScale ( const GXVec3 &scale )
{
	GXSetMat4Scale ( scale_mat, scale.x, scale.y, scale.z );
	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot_mat );
	GXMulMat4Mat4 ( mod_mat, scale_rot_mat, trans_mat );

	UpdateBounds ();
}

GXVoid GXMesh::GetLocation ( GXFloat* x, GXFloat* y, GXFloat* z )
{
	*x = trans_mat.wv.x;
	*y = trans_mat.wv.y;
	*z = trans_mat.wv.z;
}

GXVoid GXMesh::GetLocation ( GXVec3 &loc )
{
	loc = trans_mat.wv;
} 

GXVoid GXMesh::GetRotation ( GXMat4 &rot )
{
	rot = rot_mat;
}

GXVoid GXMesh::GetRotation ( GXQuat &rot )
{
	rot = GXCreateQuat ( rot_mat );
}

GXVoid GXMesh::GetScale ( GXFloat* x, GXFloat* y, GXFloat* z )
{
	*x = scale_mat.m11;
	*y = scale_mat.m22;
	*z = scale_mat.m33;
}

GXVoid GXMesh::GetScale ( GXVec3 &scale )
{
	scale.x = scale_mat.m11;
	scale.y = scale_mat.m22;
	scale.z = scale_mat.m33;
}

const GXVAOInfo& GXMesh::GetMeshVAOInfo ()
{
	return vaoInfo;
}

const GXAABB& GXMesh::GetBoundsWorld ()
{
	return boundsWorld;
}

const GXMat4& GXMesh::GetModelMatrix ()
{
	return mod_mat;
}

GXBool GXMesh::IsVisible ()
{
	return GXCamera::GetActiveCamera ()->IsObjectVisible ( boundsWorld );
}

GXVoid GXMesh::UpdateBounds ()
{
	GXSetAABBEmpty ( boundsWorld );

	GXVec3 v;
	GXMulVec3Mat4AsPoint ( v, vaoInfo.bounds.min, mod_mat );
	GXAddVertexToAABB ( boundsWorld, v );

	GXVec3 vertex;
	vertex.x = vaoInfo.bounds.min.x;
	vertex.y = vaoInfo.bounds.max.y;
	vertex.z = vaoInfo.bounds.min.z;

	GXMulVec3Mat4AsPoint ( v, vertex, mod_mat );
	GXAddVertexToAABB ( boundsWorld, v );

	vertex.x = vaoInfo.bounds.max.x;
	vertex.y = vaoInfo.bounds.max.y;
	vertex.z = vaoInfo.bounds.min.z;

	GXMulVec3Mat4AsPoint ( v, vertex, mod_mat );
	GXAddVertexToAABB ( boundsWorld, v );

	vertex.x = vaoInfo.bounds.max.x;
	vertex.y = vaoInfo.bounds.min.y;
	vertex.z = vaoInfo.bounds.min.z;

	GXMulVec3Mat4AsPoint ( v, vertex, mod_mat );
	GXAddVertexToAABB ( boundsWorld, v );

	vertex.x = vaoInfo.bounds.min.x;
	vertex.y = vaoInfo.bounds.min.y;
	vertex.z = vaoInfo.bounds.max.z;

	GXMulVec3Mat4AsPoint ( v, vertex, mod_mat );
	GXAddVertexToAABB ( boundsWorld, v );

	vertex.x = vaoInfo.bounds.min.x;
	vertex.y = vaoInfo.bounds.max.y;
	vertex.z = vaoInfo.bounds.max.z;

	GXMulVec3Mat4AsPoint ( v, vertex, mod_mat );
	GXAddVertexToAABB ( boundsWorld, v );

	GXMulVec3Mat4AsPoint ( v, vaoInfo.bounds.max, mod_mat );
	GXAddVertexToAABB ( boundsWorld, v );

	vertex.x = vaoInfo.bounds.max.x;
	vertex.y = vaoInfo.bounds.min.y;
	vertex.z = vaoInfo.bounds.max.z;

	GXMulVec3Mat4AsPoint ( v, vertex, mod_mat );
	GXAddVertexToAABB ( boundsWorld, v );
}

GXVoid GXMesh::AllocateShaderPrograms ( GXUByte numPrograms )
{
	this->numPrograms = numPrograms;
	programs = (GXShaderProgram*)malloc ( numPrograms * sizeof ( GXShaderProgram ) );
}

GXVoid GXMesh::AllocateTextures ( GXUByte numTextures )
{
	this->numTextures = numTextures;
	textures = (GXTexture*)malloc ( numTextures * sizeof ( GXTexture ) );
}
