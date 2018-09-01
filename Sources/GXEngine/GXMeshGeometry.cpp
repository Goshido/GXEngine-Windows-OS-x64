// version 1.4

#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXNativeStaticMeshSaver.h>
#include <GXCommon/GXNativeMesh.h>
#include <GXCommon/GXNativeSkin.h>
#include <GXCommon/GXUIntAtomic.h>
#include <GXCommon/GXLogger.h>


#define CACHE_DIRECTORY_NAME	L"Cache"
#define CACHE_FILE_EXTENSION	L"cache"

//---------------------------------------------------------------------------------------------------------------------

class GXMesh final
{
	friend class GXMeshGeometry;

	private:
		GXUIntAtomic			referenceCount;
		GXMesh*					previous;
		GLsizeiptr				vboSize;

		static GXMesh*			top;
		GXMesh*					next;

		GXWChar*				meshFile;
		GLenum					vboUsage;

	public:
		GLsizei					totalVertices;
		GLuint					meshVBO;

	public:
		GXVoid AddReference ();
		GXVoid Release ();

		// Method return nullptr if mesh is procedure mesh.
		// Method returns valid zero terminated string if mesh is not procedure mesh.
		const GXWChar* GetMeshFileName () const;

		// Method automatically converts static mesh to procedure mesh if needed.
		GXVoid FillVBO ( const GXVoid* data, GLsizeiptr size, GLenum usage );

		// Method returns valid pointer if mesh was found.
		// Method returns nullptr if mesh was not found.
		static GXMesh* GXCALL Find ( const GXWChar* fileName );

		static GXUInt GXCALL GetTotalLoadedMeshes ( const GXWChar** lastMesh );

	private:
		// Creates procedure mesh.
		GXMesh ();

		// Creates static mesh.
		explicit GXMesh ( const GXWChar* fileName );

		~GXMesh ();

		GXBool LoadFromOBJ ( const GXWChar* fileName );
		GXBool LoadFromSTM ( const GXWChar* fileName );
		GXBool LoadFromSKM ( const GXWChar* fileName );
		GXBool LoadFromMESH ( const GXWChar* fileName );

		GXMesh ( const GXMesh &other ) = delete;
		GXMesh& operator = ( const GXMesh &other ) = delete;
};

GXMesh* GXMesh::top = nullptr;

GXVoid GXMesh::AddReference ()
{
	++referenceCount;
}

GXVoid GXMesh::Release ()
{
	--referenceCount;

	if ( referenceCount > 0u ) return;

	delete this;
}

const GXWChar* GXMesh::GetMeshFileName () const
{
	return meshFile;
}

GXVoid GXMesh::FillVBO ( const GXVoid* data, GLsizeiptr size, GLenum usage )
{
	GXSafeFree ( meshFile );

	if ( meshVBO == 0u )
		glGenBuffers ( 1, &meshVBO );

	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	// {
		if ( size <= vboSize && usage == vboUsage )
		{
			glBufferSubData ( GL_ARRAY_BUFFER, 0, size, data );
		}
		else
		{
			vboUsage = usage;
			vboSize = size;
			glBufferData ( GL_ARRAY_BUFFER, vboSize, data, vboUsage );
		}
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0u );
}

GXMesh* GXCALL GXMesh::Find ( const GXWChar* fileName )
{
	for ( GXMesh* mesh = top; mesh; mesh = mesh->next )
		if ( GXWcscmp ( mesh->meshFile, fileName ) == 0 ) return mesh;

	return nullptr;
}

GXUInt GXCALL GXMesh::GetTotalLoadedMeshes ( const GXWChar** lastMesh )
{
	GXUInt total = 0u;

	for ( GXMesh* mesh = top; mesh; mesh = mesh->next )
		++total;

	if ( total > 0u )
		*lastMesh = top->GetMeshFileName ();
	else
		*lastMesh = nullptr;

	return total;
}

GXMesh::GXMesh ():
	referenceCount ( 1u ),
	previous ( nullptr ), 
	vboSize ( 0 ),
	next ( nullptr ),
	meshFile ( nullptr ),
	vboUsage ( GL_INVALID_ENUM ),
	totalVertices ( 0 ),
	meshVBO ( 0u )
{
	// NOTHING
}

GXMesh::GXMesh ( const GXWChar* fileName ):
	referenceCount ( 1u ),
	previous ( nullptr ),
	vboSize ( 0 )
{
	if ( top )
		top->previous = this;

	next = top;
	top = this;

	GXWcsclone ( &meshFile, fileName );

	GXWChar* extension = nullptr;
	GXGetFileExtension ( &extension, fileName );

	GXBool result = GX_FALSE;

	if ( GXWcscmp ( extension, L"stm" ) == 0 || GXWcscmp ( extension, L"STM" ) == 0 )
		result = LoadFromSTM ( fileName );
	else if ( GXWcscmp ( extension, L"skm" ) == 0 || GXWcscmp ( extension, L"SKM" ) == 0 )
		result = LoadFromSKM ( fileName );
	else if ( GXWcscmp ( extension, L"obj" ) == 0 || GXWcscmp ( extension, L"OBJ" ) == 0 )
		result = LoadFromOBJ ( fileName );
	else if ( GXWcscmp ( extension, L"mesh" ) == 0 || GXWcscmp ( extension, L"MESH" ) == 0 )
		result = LoadFromMESH ( fileName );

	GXSafeFree ( extension );

	if ( result ) return;

	GXLogW ( L"GXMesh::GXMesh::Error - Не могу загрузить меш %s.", fileName );
}

GXMesh::~GXMesh ()
{
	if ( meshVBO == 0u ) return;

	glBindBuffer ( GL_ARRAY_BUFFER, 0u );
	glDeleteBuffers ( 1, &meshVBO );

	if ( !meshFile ) return;

	free ( meshFile );

	if ( top == this )
		top = top->next;

	if ( previous )
		previous->next = next;

	if ( next )
		next->previous = previous;
}

GXBool GXMesh::LoadFromOBJ ( const GXWChar* fileName )
{
	GXWChar* path = nullptr;
	GXGetFileDirectoryPath ( &path, fileName );
	GXUInt size = GXWcslen ( path ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar );		// L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'/' symbol

	GXWChar* baseFileName = nullptr;
	GXGetBaseFileName ( &baseFileName, fileName );
	size += GXWcslen ( baseFileName ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar );		// L'.' symbol
	size += GXWcslen ( CACHE_FILE_EXTENSION ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'\0' symbol

	GXWChar* cacheFileName = static_cast<GXWChar*> ( malloc ( size ) );
	wsprintfW ( cacheFileName, L"%s/%s/%s.%s", path, CACHE_DIRECTORY_NAME, baseFileName, CACHE_FILE_EXTENSION );

	if ( GXDoesFileExist ( cacheFileName ) )
	{
		GXBool result = LoadFromSTM ( cacheFileName );

		free ( path );
		free ( baseFileName );
		free ( cacheFileName );

		return result;
	}

	GXOBJPoint* points = nullptr;
	totalVertices = static_cast<GLsizei> ( GXLoadOBJ ( fileName, &points ) );

	GXAABB bounds;

	GXNativeStaticMeshDesc descriptor;
	descriptor.numVertices = static_cast<GXUInt> ( totalVertices );
	descriptor.numNormals = static_cast<GXUInt> ( totalVertices );
	descriptor.numTBPairs = static_cast<GXUInt> ( totalVertices );
	descriptor.numUVs = static_cast<GXUInt> ( totalVertices );
	descriptor.numElements = 0u;

	GXUInt alpha = totalVertices * sizeof ( GXVec3 );
	GXUInt betta = totalVertices * sizeof ( GXVec2 );
	descriptor.vertices = (GXVec3*)malloc ( alpha );
	descriptor.uvs = static_cast<GXVec2*> ( malloc ( betta ) );
	descriptor.normals = static_cast<GXVec3*> ( malloc ( alpha ) );
	descriptor.tangents = static_cast<GXVec3*> ( malloc ( alpha ) );
	descriptor.bitangents = static_cast<GXVec3*> ( malloc ( alpha ) );
	descriptor.elements = nullptr;

	vboSize = static_cast<GLsizeiptr> ( alpha + betta + alpha + alpha + alpha );
	GXUByte* cache = static_cast<GXUByte*> ( malloc ( (size_t)vboSize ) );
	GXUPointer offset = 0u;

	for ( GXUInt i = 0u; i < descriptor.numVertices; ++i )
	{
		bounds.AddVertex ( points[ i ].vertex );

		descriptor.vertices[ i ] = points[ i ].vertex;
		descriptor.uvs[ i ] = points[ i ].uv;
		descriptor.normals[ i ] = points[ i ].normal;

		const GXUByte* vertices = reinterpret_cast<const GXUByte*> ( &points[ 3 * ( i / 3 ) ].vertex );
		const GXUByte* uvs = reinterpret_cast<const GXUByte*> ( &points[ 3 * ( i / 3 ) ].uv );

		GXGetTangentBitangent ( descriptor.tangents[ i ], descriptor.bitangents[ i ], i % 3, vertices, sizeof ( GXOBJPoint ), uvs, sizeof ( GXOBJPoint ) );

		GXVec3* gamma = reinterpret_cast<GXVec3*> ( cache + offset );
		*gamma = descriptor.vertices[ i ];
		offset += sizeof ( GXVec3 );

		GXVec2* zeta = reinterpret_cast<GXVec2*> ( cache + offset );
		*zeta = descriptor.uvs[ i ];
		offset += sizeof ( GXVec2 );

		gamma = reinterpret_cast<GXVec3*> ( cache + offset );
		*gamma = descriptor.normals[ i ];
		offset += sizeof ( GXVec3 );

		gamma = reinterpret_cast<GXVec3*> ( cache + offset );
		*gamma = descriptor.tangents[ i ];
		offset += sizeof ( GXVec3 );

		gamma = reinterpret_cast<GXVec3*> ( cache + offset );
		*gamma = descriptor.bitangents[ i ];
		offset += sizeof ( GXVec3 );
	}

	free ( points );

	size = GXWcslen ( path ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'\0' symbol

	GXWChar* cacheDirectory = static_cast<GXWChar*> ( malloc ( size ) );
	wsprintfW ( cacheDirectory, L"%s/%s", path, CACHE_DIRECTORY_NAME );

	if ( !GXDoesDirectoryExist ( cacheDirectory ) )
		GXCreateDirectory ( cacheDirectory );

	free ( cacheDirectory );

	GXExportNativeStaticMesh ( cacheFileName, descriptor );

	free ( descriptor.vertices );
	free ( descriptor.uvs );
	free ( descriptor.normals );
	free ( descriptor.tangents );
	free ( descriptor.bitangents );

	free ( path );
	free ( baseFileName );
	free ( cacheFileName );

	vboUsage = GL_STATIC_DRAW;

	glGenBuffers ( 1, &meshVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	// {
		glBufferData ( GL_ARRAY_BUFFER, vboSize, cache, vboUsage );
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	free ( cache );

	return GX_TRUE;
}

GXBool GXMesh::LoadFromSTM ( const GXWChar* fileName )
{
	GXNativeStaticMeshInfo info;
	GXLoadNativeStaticMesh ( fileName, info );

	totalVertices = static_cast<GLsizei> ( info.numVertices );
	vboSize = static_cast<GLsizeiptr> ( totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) ) );
	vboUsage = GL_STATIC_DRAW;

	glGenBuffers ( 1, &meshVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	// {
		glBufferData ( GL_ARRAY_BUFFER, vboSize, info.vboData, vboUsage );
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	info.Cleanup ();

	return GX_TRUE;
}

GXBool GXMesh::LoadFromSKM ( const GXWChar* fileName )
{
	GXSkeletalMeshData skeletalMeshData;
	GXLoadNativeSkeletalMesh ( skeletalMeshData, fileName );

	totalVertices = static_cast<GLsizei> ( skeletalMeshData.totalVertices );
	const GXUByte* source = reinterpret_cast<const GXUByte*> ( skeletalMeshData.vboData );
	static const GXUPointer meshVBOStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	static const GXUPointer skeletalMeshVBOStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );

	vboSize = static_cast<GLsizeiptr> ( totalVertices * meshVBOStride );
	GXUByte* destination = static_cast<GXUByte*> ( malloc ( (size_t)vboSize ) );
	GXUPointer offset = 0u;

	for ( GLsizei i = 0u; i < totalVertices; ++i )
	{
		// Vertex, UV, normal, tangent, bitangent.
		memcpy ( destination + offset, source, meshVBOStride );

		offset += meshVBOStride;
		source += skeletalMeshVBOStride;
	}

	skeletalMeshData.Cleanup ();

	vboUsage = GL_STATIC_DRAW;

	glGenBuffers ( 1, &meshVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	// {
		glBufferData ( GL_ARRAY_BUFFER, vboSize, destination, vboUsage );
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0u );

	free ( destination );

	return GX_TRUE;
}

GXBool GXMesh::LoadFromMESH ( const GXWChar* fileName )
{
	GXMeshInfo meshInfo;
	GXLoadNativeMesh ( meshInfo, fileName );

	totalVertices = static_cast<GLsizei> ( meshInfo.totalVertices );
	vboSize = static_cast<GLsizeiptr> ( totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) ) );
	vboUsage = GL_STATIC_DRAW;

	glGenBuffers ( 1, &meshVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	// {
		glBufferData ( GL_ARRAY_BUFFER, vboSize, meshInfo.vboData, vboUsage );
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0u );

	meshInfo.Cleanup ();

	return GX_TRUE;
}

//---------------------------------------------------------------------------------------------------------------------

class GXSkin final
{
	friend class GXMeshGeometry;

	private:
		GXUInt			referenceCount;
		GXSkin*			previous;

		static GXSkin*	top;
		GXSkin*			next;

		GXWChar*		skinFile;

		public:
		GLsizei			totalVertices;
		GLuint			skinVBO;

		public:
		GXVoid AddReference ();
		GXVoid Release ();

		// Method returns valid pointer or nullptr.
		static GXSkin* GXCALL Find ( const GXWChar* fileName );
		static GXUInt GXCALL GetTotalLoadedSkins ( const GXWChar** lastSkin );

	private:
		explicit GXSkin ( const GXWChar* fileName );
		~GXSkin ();

		GXBool LoadFromSKM ( const GXWChar* fileName );
		GXBool LoadFromSKIN ( const GXWChar* fileName );

		GXSkin ( const GXSkin &other ) = delete;
		GXSkin& operator = ( const GXSkin &other ) = delete;
};

GXSkin* GXSkin::top = nullptr;

GXVoid GXSkin::AddReference ()
{
	++referenceCount;
}

GXVoid GXSkin::Release ()
{
	--referenceCount;

	if ( referenceCount > 0u ) return;

	delete this;
}

GXSkin* GXCALL GXSkin::Find ( const GXWChar* fileName )
{
	for ( GXSkin* skin = top; skin; skin = skin->next )
		if ( GXWcscmp ( skin->skinFile, fileName ) == 0 ) return skin;

	return nullptr;
}

GXUInt GXCALL GXSkin::GetTotalLoadedSkins ( const GXWChar** lastSkin )
{
	GXUInt total = 0u;

	for ( GXSkin* skin = top; skin; skin = skin->next )
		++total;

	if ( total > 0u )
		*lastSkin = top->skinFile;
	else
		*lastSkin = nullptr;

	return total;
}

GXSkin::GXSkin ( const GXWChar* fileName ):
	referenceCount ( 1u ),
	previous ( nullptr )
{
	if ( top )
		top->previous = this;

	next = top;
	top = this;

	GXWcsclone ( &skinFile, fileName );

	GXWChar* extension = nullptr;
	GXGetFileExtension ( &extension, fileName );

	GXBool result = GX_FALSE;

	if ( GXWcscmp ( extension, L"skm" ) == 0 || GXWcscmp ( extension, L"SKM" ) == 0 )
		result = LoadFromSKM ( fileName );
	else if ( GXWcscmp ( extension, L"skin" ) == 0 || GXWcscmp ( extension, L"SKIN" ) == 0 )
		result = LoadFromSKIN ( fileName );

	GXSafeFree ( extension );

	if ( result ) return;

	GXLogW ( L"GXMesh::GXMesh::Error - Не могу загрузить скин %s.", fileName );
}

GXSkin::~GXSkin ()
{
	free ( skinFile );

	glBindBuffer ( GL_ARRAY_BUFFER, 0u );
	glDeleteBuffers ( 1, &skinVBO );

	if ( top == this )
		top = top->next;
	else
		previous->next = next;

	if ( next )
		next->previous = previous;
}

GXBool GXSkin::LoadFromSKM ( const GXWChar* fileName )
{
	GXSkeletalMeshData skeletalMeshData;
	GXLoadNativeSkeletalMesh ( skeletalMeshData, fileName );

	totalVertices = static_cast<GLsizei> ( skeletalMeshData.totalVertices );
	const GXUByte* source = reinterpret_cast<const GXUByte*> ( skeletalMeshData.vboData ) + sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	static const GXUPointer skinVBOStride = sizeof ( GXVec4 ) + sizeof ( GXVec4 );
	static const GXUPointer skeletalMeshVBOStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );

	GXUPointer skinVBOSize = totalVertices * skinVBOStride;
	GXUByte* destination = static_cast<GXUByte*> ( malloc ( skinVBOSize ) );
	GXUPointer offset = 0u;

	for ( GLsizei i = 0; i < totalVertices; ++i )
	{
		// Bone index, bone weight
		memcpy ( destination + offset, source, skinVBOStride );

		offset += skinVBOStride;
		source += skeletalMeshVBOStride;
	}

	skeletalMeshData.Cleanup ();

	glGenBuffers ( 1, &skinVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, skinVBO );
	// {
		glBufferData ( GL_ARRAY_BUFFER, static_cast<GLsizeiptr> ( skinVBOSize ), destination, GL_STATIC_DRAW );
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0u );

	free ( destination );

	return GX_TRUE;
}

GXBool GXSkin::LoadFromSKIN ( const GXWChar* fileName )
{
	GXSkinInfo skinInfo;
	GXLoadNativeSkin ( skinInfo, fileName );

	totalVertices = static_cast<GLsizei> ( skinInfo.totalVertices );

	glGenBuffers ( 1, &skinVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, skinVBO );
	// {
		glBufferData ( GL_ARRAY_BUFFER, static_cast<GLsizeiptr> ( skinInfo.totalVertices * ( sizeof ( GXVec4 ) + sizeof ( GXVec4 ) ) ), skinInfo.vboData, GL_STATIC_DRAW );
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0u );

	skinInfo.Cleanup ();

	return GX_TRUE;
}

//---------------------------------------------------------------------------------------------------------------------

GXMeshGeometry::GXMeshGeometry ():
	mesh ( nullptr ),
	meshVAO ( 0u ),
	topology ( GL_TRIANGLES ),
	skin ( nullptr ),
	skinningSwitchIndex ( 0u ),
	skinningMaterial ( nullptr )
{
	pose[ 0u ] = pose[ 1u ] = nullptr;
	poseVAO[ 0u ] = poseVAO[ 1u ] = 0u;
}

GXMeshGeometry::~GXMeshGeometry ()
{
	if ( mesh )
	{
		glDeleteVertexArrays ( 1, &meshVAO );
		mesh->Release ();
	}

	if ( skin )
		skin->Release ();

	if ( !skinningMaterial ) return;

	delete skinningMaterial;

	glDeleteVertexArrays ( 2, poseVAO );

	pose[ 0u ]->Release ();
	pose[ 1u ]->Release ();
}

GXVoid GXMeshGeometry::Render ()
{
	if ( !mesh || mesh->totalVertices == 0 ) return;

	if ( skinningMaterial )
	{
		glBindVertexArray ( poseVAO[ skinningSwitchIndex ] );
		glDrawArrays ( topology, 0, skin->totalVertices );
		glBindVertexArray ( 0u );

		if ( skinningSwitchIndex == 0u )
			skinningSwitchIndex = 1u;
		else
			skinningSwitchIndex = 0u;

		return;
	}

	glBindVertexArray ( meshVAO );
	glDrawArrays ( topology, 0, mesh->totalVertices );
	glBindVertexArray ( 0u );
}

GXVoid GXMeshGeometry::SetBoundsLocal ( const GXAABB& bounds )
{
	this->boundsLocal = bounds;
}

const GXAABB& GXMeshGeometry::GetBoundsLocal () const
{
	return boundsLocal;
}

GXVoid GXMeshGeometry::SetTotalVertices ( GLsizei totalVertices )
{
	if ( !mesh || !mesh->GetMeshFileName () )
	{
		// Procedure generated mesh.

		if ( !mesh )
			mesh = new GXMesh ();

		mesh->totalVertices = totalVertices;
		return;
	}

	if ( mesh )
		mesh->Release ();

	mesh = new GXMesh ();
	mesh->totalVertices = totalVertices;
}

GXVoid GXMeshGeometry::FillVertexBuffer ( const GXVoid* data, GLsizeiptr size, GLenum usage )
{
	if ( !mesh || !mesh->GetMeshFileName () )
	{
		// Procedure generated mesh.

		if ( !mesh )
			mesh = new GXMesh ();

		mesh->FillVBO ( data, size, usage );
		return;
	}

	if ( mesh )
		mesh->Release ();

	mesh = new GXMesh ();
	mesh->FillVBO ( data, size, usage );
}

GXVoid GXMeshGeometry::SetBufferStream ( eGXMeshStreamIndex streamIndex, GLint numElements, GLenum elementType, GLsizei stride, const GLvoid* offset )
{
	if ( meshVAO == 0u )
		glGenVertexArrays ( 1, &meshVAO );

	if ( !mesh || !mesh->GetMeshFileName () )
	{
		// Procedure generated mesh.

		if ( !mesh )
			mesh = new GXMesh ();

		glBindVertexArray ( meshVAO );
		// {
			if ( mesh->meshVBO == 0u )
				glGenBuffers ( 1, &mesh->meshVBO );

			glBindBuffer ( GL_ARRAY_BUFFER, mesh->meshVBO );
			glEnableVertexAttribArray ( static_cast<GLuint> ( streamIndex ) );
			glVertexAttribPointer ( static_cast<GLuint> ( streamIndex ), numElements, elementType, GL_FALSE, stride, offset );
		// }
		glBindVertexArray ( 0u );

		return;
	}

	if ( mesh )
		mesh->Release ();

	mesh = new GXMesh ();

	glBindVertexArray ( meshVAO );
	// {
		glBindBuffer ( GL_ARRAY_BUFFER, mesh->meshVBO );
		glEnableVertexAttribArray ( static_cast<GLuint> ( streamIndex ) );
		glVertexAttribPointer ( static_cast<GLuint> ( streamIndex ), numElements, elementType, GL_FALSE, stride, offset );
	// }
	glBindVertexArray ( 0u );
}

GXVoid GXMeshGeometry::SetTopology ( GLenum newTopology )
{
	topology = newTopology;
}

GXVoid GXMeshGeometry::UpdatePose ( const GXSkeleton &skeleton )
{
	if ( !skinningMaterial ) return;

	skinningMaterial->SetSkeleton ( skeleton );
	skinningMaterial->Bind ( GXTransform::GetNullTransform () );

	glDisable ( GL_DEPTH_TEST );
	glEnable ( GL_RASTERIZER_DISCARD );
	glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0u, pose[ skinningSwitchIndex ]->meshVBO );
	glBeginTransformFeedback ( GL_POINTS );

	glBindVertexArray ( meshVAO );

	glDrawArrays ( GL_POINTS, 0, skin->totalVertices );

	glBindVertexArray ( 0u );

	glEndTransformFeedback ();
	glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0u, 0u );
	glDisable ( GL_RASTERIZER_DISCARD );
	glEnable ( GL_DEPTH_TEST );

	skinningMaterial->Unbind ();
}

GXBool GXMeshGeometry::LoadMesh ( const GXWChar* fileName )
{
	if ( mesh )
		mesh->Release ();

	mesh = GXMesh::Find ( fileName );

	if ( mesh )
		mesh->AddReference ();
	else
		mesh = new GXMesh ( fileName );

	UpdateGraphicResources ();

	return GX_TRUE;
}

GXBool GXMeshGeometry::LoadSkin ( const GXWChar* fileName )
{
	if ( skin )
		skin->Release ();

	skin = GXSkin::Find ( fileName );

	if ( skin )
		skin->AddReference ();
	else
		skin = new GXSkin ( fileName );

	UpdateGraphicResources ();

	return GX_TRUE;
}

GXUInt GXCALL GXMeshGeometry::GetTotalLoadedMeshes ( const GXWChar** lastMesh )
{
	return GXMesh::GetTotalLoadedMeshes ( lastMesh );
}

GXUInt GXCALL GXMeshGeometry::GetTotalLoadedSkins ( const GXWChar** lastSkin )
{
	return GXSkin::GetTotalLoadedSkins ( lastSkin );
}

GXVoid GXMeshGeometry::UpdateGraphicResources ()
{
	if ( !mesh || !mesh->GetMeshFileName () || !skin || mesh->totalVertices != skin->totalVertices )
	{
		if ( mesh && meshVAO == 0u )
		{
			glGenVertexArrays ( 1, &meshVAO );
			glBindVertexArray ( meshVAO );
			// {
				GLsizei meshStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
				GXUPointer offset = 0u;

				glBindBuffer ( GL_ARRAY_BUFFER, mesh->meshVBO );

				glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::CurrenVertex ) );
				glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::CurrenVertex ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
				offset += sizeof ( GXVec3 );

				glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::UV ) );
				glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::UV ), 2, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
				offset += sizeof ( GXVec2 );

				glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Normal ) );
				glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Normal ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
				offset += sizeof ( GXVec3 );

				glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Tangent ) );
				glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Tangent ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
				offset += sizeof ( GXVec3 );

				glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Bitangent ) );
				glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Bitangent ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );

				glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ) );
				glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ), 3, GL_FLOAT, GL_FALSE, meshStride, static_cast<const GLvoid*> ( 0u ) );
			// }
			glBindVertexArray ( 0u );
		}

		if ( !skinningMaterial ) return;

		delete skinningMaterial;

		glBindVertexArray ( 0u );
		glDeleteVertexArrays ( 2, poseVAO );

		pose[ 0u ]->Release ();
		pose[ 1u ]->Release ();

		pose[ 0u ] = pose[ 1u ] = nullptr;

		return;
	}

	skinningMaterial = new GXSkinningMaterial ();
	GLsizei meshStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	GLsizeiptr poseVBOSize = static_cast<GLsizeiptr> ( meshStride * skin->totalVertices );

	glGenVertexArrays ( 2, poseVAO );
	
	pose[ 0u ] = new GXMesh ();
	pose[ 0u ]->FillVBO ( nullptr, poseVBOSize, GL_DYNAMIC_DRAW );

	pose[ 1u ] = new GXMesh ();
	pose[ 1u ]->FillVBO ( nullptr, poseVBOSize, GL_DYNAMIC_DRAW );

	GXUPointer offset = 0u;

	glBindVertexArray ( poseVAO[ 0 ] );
	// {
		glBindBuffer ( GL_ARRAY_BUFFER, pose[ 0 ]->meshVBO );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::CurrenVertex ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::CurrenVertex ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::UV ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::UV ), 2, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec2 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Normal ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Normal ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Tangent ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Tangent ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Bitangent ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Bitangent ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );

		glBindBuffer ( GL_ARRAY_BUFFER, pose[ 1u ]->meshVBO );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ), 3, GL_FLOAT, GL_FALSE, meshStride, static_cast<const GLvoid*> ( 0u ) );
	// }
	glBindVertexArray ( 0u );

	offset = 0u;

	glBindVertexArray ( poseVAO[ 1u ] );
	// {
		glBindBuffer ( GL_ARRAY_BUFFER, pose[ 1u ]->meshVBO );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::CurrenVertex ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::CurrenVertex ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::UV ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::UV ), 2, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec2 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Normal ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Normal ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Tangent ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Tangent ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Bitangent ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Bitangent ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );

		glBindBuffer ( GL_ARRAY_BUFFER, pose[ 0u ]->meshVBO );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ), 3, GL_FLOAT, GL_FALSE, meshStride, static_cast<const GLvoid*> ( 0u ) );
	// }
	glBindVertexArray ( 0u );

	offset = 0u;
	GLsizei skinStride = sizeof ( GXVec4 ) + sizeof ( GXVec4 );

	glBindVertexArray ( meshVAO );
	// {
		glBindBuffer ( GL_ARRAY_BUFFER, mesh->meshVBO );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::CurrenVertex ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::CurrenVertex ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::UV ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::UV ), 2, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec2 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Normal ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Normal ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Tangent ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Tangent ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::Bitangent ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::Bitangent ), 3, GL_FLOAT, GL_FALSE, meshStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec3 );

		glBindBuffer ( GL_ARRAY_BUFFER, skin->skinVBO );

		offset = 0u;

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::SkinningIndices ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::SkinningIndices ), 4, GL_FLOAT, GL_FALSE, skinStride, reinterpret_cast<const GLvoid*> ( offset ) );
		offset += sizeof ( GXVec4 );

		glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::SkinningWeights ) );
		glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::SkinningWeights ), 4, GL_FLOAT, GL_FALSE, skinStride, reinterpret_cast<const GLvoid*> ( offset ) );
	// }
	glBindVertexArray ( 0u );
}
