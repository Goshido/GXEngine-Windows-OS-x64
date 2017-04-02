//version 1.0

#include <GXEngine/GXSkeletalMeshStorage.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


class GXSkeletalMeshUnit
{
	private:
		GXSkeletalMeshUnit*		next;
		GXSkeletalMeshUnit*		prev;

		GXInt					refs;
		GXWChar*				name;

		GXSkeletalMeshInfoExt	info;

	public:
		GXSkeletalMeshUnit ( const GXWChar* name );

		GXVoid AddRef ();
		GXVoid Release ();

		GXSkeletalMeshUnit* Find ( const GXWChar* name );
		GXSkeletalMeshUnit* Find ( GLuint vbo );

		GXVoid SetSkeletalMeshInfo ( const GXSkeletalMeshInfoExt &info );
		const GXSkeletalMeshInfoExt& GetSkeletalMeshInfo ();

		GXUInt CheckMemoryLeak ( const GXWChar** lastMesh );

	private:
		~GXSkeletalMeshUnit ();
};

GXSkeletalMeshUnit* gx_strg_SkeletalMeshes = 0;

GXSkeletalMeshUnit::GXSkeletalMeshUnit ( const GXWChar* name )
{
	if ( !gx_strg_SkeletalMeshes )
		next = prev = 0;
	else
	{
		next = gx_strg_SkeletalMeshes;
		prev = 0;
		gx_strg_SkeletalMeshes->prev = this;
	}

	gx_strg_SkeletalMeshes = this;

	refs = 1;
	GXWcsclone ( &this->name, name );

	info.numVertices = 0;
	info.vao = 0;
	info.vbo = 0;
	GXSetAABBEmpty ( info.bounds );
	info.numBones = 0;
	info.boneNames = 0;
	info.parentIndex = 0;
	info.refPose = 0;
	info.bindTransform = 0;
}

GXVoid GXSkeletalMeshUnit::AddRef ()
{
	refs++;
}

GXVoid GXSkeletalMeshUnit::Release ()
{
	refs--;

	if ( refs < 1 )
		delete this;
}

GXSkeletalMeshUnit* GXSkeletalMeshUnit::Find ( const GXWChar* name )
{
	GXSkeletalMeshUnit* p = this;

	while ( p )
	{
		if ( GXWcscmp ( p->name, name ) == 0 )
			return p;

		p = p->next;
	}

	return 0;
}

GXSkeletalMeshUnit* GXSkeletalMeshUnit::Find ( GLuint vbo )
{
	GXSkeletalMeshUnit* p = this;

	while ( p )
	{
		if ( p->info.vbo == vbo )
			return p;

		p = p->next;
	}

	return 0;
}

GXVoid GXSkeletalMeshUnit::SetSkeletalMeshInfo ( const GXSkeletalMeshInfoExt &info )
{
	memcpy ( &this->info, &info, sizeof ( GXSkeletalMeshInfoExt ) );
}

const GXSkeletalMeshInfoExt& GXSkeletalMeshUnit::GetSkeletalMeshInfo ()
{
	return info;
}

GXUInt GXSkeletalMeshUnit::CheckMemoryLeak ( const GXWChar** lastMesh )
{
	GXUInt objects = 0;
	*lastMesh = 0;

	for ( GXSkeletalMeshUnit* p = gx_strg_SkeletalMeshes; p; p = p->next )
	{
		*lastMesh = p->name;
		objects++;
	}

	return objects;
}

GXSkeletalMeshUnit::~GXSkeletalMeshUnit ()
{
	if ( next ) next->prev = prev;

	if ( prev )
		prev->next = next;
	else
		gx_strg_SkeletalMeshes = next;

	GXSafeFree ( name );

	if ( info.numVertices > 0 )
	{
		glBindVertexArray ( 0 );
		glBindBuffer ( GL_ARRAY_BUFFER, 0 );
		glDeleteVertexArrays ( 1, &info.vao );
		glDeleteBuffers ( 1, &info.vbo );
	}

	if ( info.numBones > 0 )
	{
		free ( info.boneNames );
		free ( info.parentIndex );
		free ( info.refPose );
		free ( info.bindTransform );
	}
}

//------------------------------------------------------------------------------------------------

class GXAnimationUnit
{
	private:
		GXAnimationUnit*		next;
		GXAnimationUnit*		prev;

		GXInt					refs;
		GXWChar*				name;

		GXAnimationInfoExt		info;

	public:
		GXAnimationUnit ( const GXWChar* name );

		GXVoid AddRef ();
		GXVoid Release ();

		GXAnimationUnit* Find ( const GXWChar* name );
		GXAnimationUnit* Find ( GXQuatLocJoint* keys );

		GXVoid SetAnimationInfo ( const GXAnimationInfoExt &info );
		const GXAnimationInfoExt& GetAnimationInfo ();

		GXUInt CheckMemoryLeak ( const GXWChar** lastAnimation );

	private:
		~GXAnimationUnit ();
};

GXAnimationUnit* gx_strg_Animations = 0;

GXAnimationUnit::GXAnimationUnit ( const GXWChar* name )
{
	if ( !gx_strg_Animations )
		next = prev = 0;
	else
	{
		next = gx_strg_Animations;
		prev = 0;
		gx_strg_Animations->prev = this;
	}

	gx_strg_Animations = this;

	refs = 1;
	GXWcsclone ( &this->name, name );

	info.numBones = 0;
	info.boneNames = 0;
	info.fps = 30.0f;
	info.numFrames = 0;
	info.keys = 0;
}

GXVoid GXAnimationUnit::AddRef ()
{
	refs++;
}

GXVoid GXAnimationUnit::Release ()
{
	refs--;

	if ( refs < 1 )
		delete this;
}

GXAnimationUnit* GXAnimationUnit::Find ( const GXWChar* name )
{
	GXAnimationUnit* p = this;

	while ( p )
	{
		if ( GXWcscmp ( p->name, name ) == 0 )
			return p;

		p = p->next;
	}

	return 0;
}

GXAnimationUnit* GXAnimationUnit::Find ( GXQuatLocJoint* keys )
{
	GXAnimationUnit* p = this;

	while ( p )
	{
		if ( p->info.keys == keys )
			return p;

		p = p->next;
	}

	return 0;
}

GXVoid GXAnimationUnit::SetAnimationInfo ( const GXAnimationInfoExt &info )
{
	memcpy ( &this->info, &info, sizeof ( GXAnimationInfoExt ) );
}

const GXAnimationInfoExt& GXAnimationUnit::GetAnimationInfo ()
{
	return info;
}

GXUInt GXAnimationUnit::CheckMemoryLeak ( const GXWChar** lastAnimation )
{
	GXUInt objects = 0;
	*lastAnimation = 0;

	for ( GXAnimationUnit* p = gx_strg_Animations; p; p = p->next )
	{
		*lastAnimation = p->name;
		objects++;
	}

	return objects;
}

GXAnimationUnit::~GXAnimationUnit ()
{
	if ( next ) next->prev = prev;

	if ( prev )
		prev->next = next;
	else
		gx_strg_Animations = next;

	GXSafeFree ( name );

	if ( info.numBones > 0 )
		free ( info.boneNames );

	if ( info.numFrames > 0 )
		free ( info.keys );
}

//------------------------------------------------------------------------------------------------

#define GX_VERTEX_INDEX		0
#define GX_UV_COORD_INDEX	1
#define GX_NORMAL_INDEX		2
#define GX_TANGENT_INDEX	3
#define GX_BITANGENT_INDEX	4
#define GX_BONE_INDEX		5
#define GX_WEIGHT_INDEX		6

GXVoid GXCALL GXCreateSkeletalMesh ( GXSkeletalMeshUnit &mesh, const GXSkeletalMeshData &data )
{
	GXSkeletalMeshInfoExt info;
	info.numVertices = data.numVertices;

	glGenVertexArrays ( 1, &info.vao );
	glBindVertexArray ( info.vao );
	//{
		GXUInt stride = (GXUInt)( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );

		GXPointer vertexOffset = 0;
		GXPointer uvOffset = sizeof ( GXVec3 );
		GXPointer normalOffset = uvOffset + sizeof ( GXVec2 );
		GXPointer tangentOffset = normalOffset + sizeof ( GXVec3 );
		GXPointer bitangentOffset = tangentOffset + sizeof ( GXVec3 );
		GXPointer boneOffset = bitangentOffset + sizeof ( GXVec3 );
		GXPointer weightOffset = boneOffset + sizeof ( GXVec4 );

		GXPointer size = info.numVertices * stride;

		glGenBuffers ( 1, &info.vbo );
		glBindBuffer ( GL_ARRAY_BUFFER, info.vbo );
		//{
			glBufferData ( GL_ARRAY_BUFFER, size, data.vboData, GL_STATIC_DRAW );

			glVertexAttribPointer ( GX_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)vertexOffset );
			glEnableVertexAttribArray ( GX_VERTEX_INDEX );

			glVertexAttribPointer ( GX_UV_COORD_INDEX, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)uvOffset );
			glEnableVertexAttribArray ( GX_UV_COORD_INDEX );

			glVertexAttribPointer ( GX_NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)normalOffset );
			glEnableVertexAttribArray ( GX_NORMAL_INDEX );

			glVertexAttribPointer ( GX_TANGENT_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)tangentOffset );
			glEnableVertexAttribArray ( GX_TANGENT_INDEX );

			glVertexAttribPointer ( GX_BITANGENT_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)bitangentOffset );
			glEnableVertexAttribArray ( GX_BITANGENT_INDEX );

			glVertexAttribPointer ( GX_BONE_INDEX, 4, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)boneOffset );
			glEnableVertexAttribArray ( GX_BONE_INDEX );

			glVertexAttribPointer ( GX_WEIGHT_INDEX, 4, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)weightOffset );
			glEnableVertexAttribArray ( GX_WEIGHT_INDEX );
		//}
	//}

	glBindVertexArray ( 0 );

	//TODO
	GXSetAABBEmpty ( info.bounds );
	GXAddVertexToAABB ( info.bounds, FLT_MAX, FLT_MAX, FLT_MAX );
	GXAddVertexToAABB ( info.bounds, FLT_MIN, FLT_MIN, FLT_MIN );

	info.numBones = data.numBones;

	size = info.numBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
	info.boneNames = (GXUTF8*)malloc ( size );
	memcpy ( info.boneNames, data.boneNames, size );

	size = info.numBones * sizeof ( GXShort );
	info.parentIndex = (GXShort*)malloc ( size );
	memcpy ( info.parentIndex, data.parentIndex, size );

	size = info.numBones * sizeof ( GXQuatLocJoint );
	info.refPose = (GXQuatLocJoint*)malloc ( size );
	memcpy ( info.refPose, data.refPose, size );

	info.bindTransform = (GXQuatLocJoint*)malloc ( size );
	memcpy ( info.bindTransform, data.bindTransform, size );

	mesh.SetSkeletalMeshInfo ( info );
}

GXVoid GXCALL GXGetSkeletalMesh ( const GXWChar* fileName, GXSkeletalMeshInfoExt &info )
{
	GXSkeletalMeshUnit* mesh;

	if ( !gx_strg_SkeletalMeshes )
		mesh = new GXSkeletalMeshUnit ( fileName );
	else
	{
		mesh = gx_strg_SkeletalMeshes->Find ( fileName );

		if ( mesh )
		{
			mesh->AddRef ();
			const GXSkeletalMeshInfoExt& meshInfo = mesh->GetSkeletalMeshInfo ();
			memcpy ( &info, &meshInfo, sizeof ( GXSkeletalMeshInfoExt ) );
			return;
		}
		else
			mesh = new GXSkeletalMeshUnit ( fileName );
	}
	
	GXSkeletalMeshData data;
	GXLoadNativeSkeletalMesh ( fileName, data );
	GXCreateSkeletalMesh ( *mesh, data );

	data.Cleanup ();

	const GXSkeletalMeshInfoExt& meshInfo = mesh->GetSkeletalMeshInfo ();
	memcpy ( &info, &meshInfo, sizeof ( GXSkeletalMeshInfoExt ) );
}

GXVoid GXCALL GXRemoveSkeletalMesh ( const GXSkeletalMeshInfoExt &info )
{
	if ( !gx_strg_SkeletalMeshes ) return;

	GXSkeletalMeshUnit* mesh = gx_strg_SkeletalMeshes->Find ( info.vbo );

	if ( !mesh ) return;

	mesh->Release ();
}

GXUInt GXCALL GXGetTotalSkeletalMeshStorageObjects ( const GXWChar** lastSkeletalMesh )
{
	if ( !gx_strg_SkeletalMeshes )
	{
		*lastSkeletalMesh = 0;
		return 0;
	}

	return gx_strg_SkeletalMeshes->CheckMemoryLeak ( lastSkeletalMesh );
}

//------------------------------------------------------------------------------------------------

GXVoid GXCALL GXGetAnimation ( const GXWChar* fileName, GXAnimationInfoExt &info )
{
	GXAnimationUnit* animation;

	if ( !gx_strg_Animations )
		animation = new GXAnimationUnit ( fileName );
	else
	{
		animation = gx_strg_Animations->Find ( fileName );

		if ( animation )
		{
			animation->AddRef ();
			const GXAnimationInfoExt& animInfo = animation->GetAnimationInfo ();
			memcpy ( &info, &animInfo, sizeof ( GXAnimationInfoExt ) );
			return;
		}
		else
			animation = new GXAnimationUnit ( fileName );
	}
	
	GXLoadNativeAnimation ( fileName, info );
	animation->SetAnimationInfo ( info );
}

GXVoid GXCALL GXRemoveAnimation ( const GXAnimationInfoExt &info )
{
	if ( !gx_strg_Animations) return;

	GXAnimationUnit* animation = gx_strg_Animations->Find ( info.keys);

	if ( !animation ) return;

	animation->Release ();
}

GXUInt GXCALL GXGetTotalAnimationStorageObjects ( const GXWChar** lastAnimation )
{
	if ( !gx_strg_Animations )
	{
		*lastAnimation = 0;
		return 0;
	}

	return gx_strg_Animations->CheckMemoryLeak ( lastAnimation );
}
