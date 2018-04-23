// version 1.1

#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXNativeStaticMeshSaver.h>
#include <GXCommon/GXNativeMesh.h>
#include <GXCommon/GXNativeSkin.h>
#include <GXCommon/GXLogger.h>


#define CACHE_DIRECTORY_NAME	L"Cache"
#define CACHE_FILE_EXTENSION	L"cache"


GXMesh* GXMesh::top = nullptr;

GXMesh::GXMesh ()
{
	next = previous = nullptr;
	referenceCount = 1;
	meshFile = nullptr;
	vboSize = 0;
	vboUsage = GL_INVALID_ENUM;
	meshVBO = 0;
	totalVertices = 0;
}

GXMesh::GXMesh ( const GXWChar* fileName )
{
	previous = nullptr;

	if ( top )
		top->previous = this;

	next = top;
	top = this;

	referenceCount = 1;
	GXWcsclone ( &meshFile, fileName );
	vboSize = 0;

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

GXVoid GXMesh::AddReference ()
{
	referenceCount++;
}

GXVoid GXMesh::Release ()
{
	referenceCount--;

	if ( referenceCount > 0 ) return;

	delete this;
}

const GXWChar* GXMesh::GetMeshFileName () const
{
	return meshFile;
}

GXVoid GXMesh::FillVBO ( const GXVoid* data, GLsizeiptr size, GLenum usage )
{
	GXSafeFree ( meshFile );

	if ( meshVBO == 0 )
		glGenBuffers ( 1, &meshVBO );

	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	// {
		if ( size <= vboSize && usage == vboUsage )
		{
			glBufferSubData ( GL_ARRAY_BUFFER, (GLintptr)0, size, data );
		}
		else
		{
			vboUsage = usage;
			vboSize = size;
			glBufferData ( GL_ARRAY_BUFFER, vboSize, data, vboUsage );
		}
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
}

GXMesh* GXCALL GXMesh::Find ( const GXWChar* fileName )
{
	for ( GXMesh* mesh = top; mesh; mesh = mesh->next )
		if ( GXWcscmp ( mesh->meshFile, fileName ) == 0 ) return mesh;

	return nullptr;
}

GXUInt GXCALL GXMesh::GetTotalLoadedMeshes ( const GXWChar** lastMesh )
{
	GXUInt total = 0;

	for ( GXMesh* mesh = top; mesh; mesh = mesh->next )
		total++;

	if ( total > 0 )
		*lastMesh = top->GetMeshFileName ();
	else
		*lastMesh = nullptr;

	return total;
}

GXMesh::~GXMesh ()
{
	if ( meshVBO == 0 ) return;

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
	glDeleteBuffers ( 1, &meshVBO );

	if ( !meshFile ) return;

	free ( meshFile );

	if ( top == this )
		top = top->next;
	else
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

	GXWChar* cacheFileName = (GXWChar*)malloc ( size );
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
	totalVertices = (GLsizei)GXLoadOBJ ( fileName, &points );

	GXAABB bounds;

	GXNativeStaticMeshDesc descriptor;
	descriptor.numVertices = (GXUInt)totalVertices;
	descriptor.numNormals = (GXUInt)totalVertices;
	descriptor.numTBPairs = (GXUInt)totalVertices;
	descriptor.numUVs = (GXUInt)totalVertices;
	descriptor.numElements = 0;

	GXUInt alpha = totalVertices * sizeof ( GXVec3 );
	GXUInt betta = totalVertices * sizeof ( GXVec2 );
	descriptor.vertices = (GXVec3*)malloc ( alpha );
	descriptor.uvs = (GXVec2*)malloc ( betta );
	descriptor.normals = (GXVec3*)malloc ( alpha );
	descriptor.tangents = (GXVec3*)malloc ( alpha );
	descriptor.bitangents = (GXVec3*)malloc ( alpha );
	descriptor.elements = nullptr;

	vboSize = (GLsizeiptr)( alpha + betta + alpha + alpha + alpha );
	GXUByte* cache = (GXUByte*)malloc ( (size_t)vboSize );
	GXPointer offset = 0;

	for ( GXUInt i = 0; i < descriptor.numVertices; i++ )
	{
		bounds.AddVertex ( points[ i ].vertex );

		descriptor.vertices[ i ] = points[ i ].vertex;
		descriptor.uvs[ i ] = points[ i ].uv;
		descriptor.normals[ i ] = points[ i ].normal;

		const GXUByte* vertices = (const GXUByte*)( &points[ 3 * ( i / 3 ) ].vertex );
		const GXUByte* uvs = (const GXUByte*)( &points[ 3 * ( i / 3 ) ].uv );

		GXGetTangentBitangent ( descriptor.tangents[ i ], descriptor.bitangents[ i ], i % 3, vertices, sizeof ( GXOBJPoint ), uvs, sizeof ( GXOBJPoint ) );

		GXVec3* gamma = (GXVec3*)( cache + offset );
		*gamma = descriptor.vertices[ i ];
		offset += sizeof ( GXVec3 );

		GXVec2* zeta = (GXVec2*)( cache + offset );
		*zeta = descriptor.uvs[ i ];
		offset += sizeof ( GXVec2 );

		gamma = (GXVec3*)( cache + offset );
		*gamma = descriptor.normals[ i ];
		offset += sizeof ( GXVec3 );

		gamma = (GXVec3*)( cache + offset );
		*gamma = descriptor.tangents[ i ];
		offset += sizeof ( GXVec3 );

		gamma = (GXVec3*)( cache + offset );
		*gamma = descriptor.bitangents[ i ];
		offset += sizeof ( GXVec3 );
	}

	free ( points );

	size = GXWcslen ( path ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		// L'\0' symbol

	GXWChar* cacheDirectory = (GXWChar*)malloc ( size );
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

	totalVertices = (GLsizei)info.numVertices;
	vboSize = (GLsizeiptr)( totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) ) );
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

	totalVertices = (GLsizei)skeletalMeshData.totalVertices;
	const GXUByte* source = (const GXUByte*)skeletalMeshData.vboData;
	static const GXUPointer meshVBOStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	static const GXUPointer skeletalMeshVBOStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );

	vboSize = (GLsizeiptr)( totalVertices * meshVBOStride );
	GXUByte* destination = (GXUByte*)malloc ( (size_t)vboSize );
	GXUPointer offset = 0;

	for ( GLsizei i = 0; i < totalVertices; i++ )
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
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	free ( destination );

	return GX_TRUE;
}

GXBool GXMesh::LoadFromMESH ( const GXWChar* fileName )
{
	GXMeshInfo meshInfo;
	GXLoadNativeMesh ( meshInfo, fileName );

	totalVertices = (GLsizei)meshInfo.totalVertices;
	vboSize = (GLsizeiptr)( totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) ) );
	vboUsage = GL_STATIC_DRAW;

	glGenBuffers ( 1, &meshVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	// {
		glBufferData ( GL_ARRAY_BUFFER, vboSize, meshInfo.vboData, vboUsage );
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	meshInfo.Cleanup ();

	return GX_TRUE;
}

//----------------------------------------------------------------------------------------

GXSkin* GXSkin::top = nullptr;

GXSkin::GXSkin ( const GXWChar* fileName )
{
	previous = nullptr;

	if ( top )
		top->previous = this;

	next = top;
	top = this;

	referenceCount = 1;

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

GXVoid GXSkin::AddReference ()
{
	referenceCount++;
}

GXVoid GXSkin::Release ()
{
	referenceCount--;

	if ( referenceCount > 0 ) return;

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
	GXUInt total = 0;

	for ( GXSkin* skin = top; skin; skin = skin->next )
		total++;

	if ( total > 0 )
		*lastSkin = top->skinFile;
	else
		*lastSkin = nullptr;

	return total;
}

GXSkin::~GXSkin ()
{
	free ( skinFile );

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
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

	totalVertices = (GLsizei)skeletalMeshData.totalVertices;
	const GXUByte* source = (const GXUByte*)skeletalMeshData.vboData + sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	static const GXUPointer skinVBOStride = sizeof ( GXVec4 ) + sizeof ( GXVec4 );
	static const GXUPointer skeletalMeshVBOStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );

	GXUPointer skinVBOSize = totalVertices * skinVBOStride;
	GXUByte* destination = (GXUByte*)malloc ( skinVBOSize );
	GXUPointer offset = 0;

	for ( GLsizei i = 0; i < totalVertices; i++ )
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
		glBufferData ( GL_ARRAY_BUFFER, (GLsizeiptr)skinVBOSize, destination, GL_STATIC_DRAW );
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	free ( destination );

	return GX_TRUE;
}

GXBool GXSkin::LoadFromSKIN ( const GXWChar* fileName )
{
	GXSkinInfo skinInfo;
	GXLoadNativeSkin ( skinInfo, fileName );

	totalVertices = (GLsizei)skinInfo.totalVertices;

	glGenBuffers ( 1, &skinVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, skinVBO );
	// {
		glBufferData ( GL_ARRAY_BUFFER, (GLsizeiptr)( skinInfo.totalVertices * ( sizeof ( GXVec4 ) + sizeof ( GXVec4 ) ) ), skinInfo.vboData, GL_STATIC_DRAW );
	// }
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	skinInfo.Cleanup ();

	return GX_TRUE;
}

//----------------------------------------------------------------------------------------

GXMeshGeometry::GXMeshGeometry ()
{
	mesh = nullptr;
	meshVAO = 0;
	topology = GL_TRIANGLES;

	skin = nullptr;
	pose[ 0 ] = pose[ 1 ] = nullptr;
	poseVAO[ 0 ] = poseVAO[ 1 ] = 0;

	skinningSwitchIndex = 0;
	skinningMaterial = nullptr;
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

	pose[ 0 ]->Release ();
	pose[ 1 ]->Release ();
}

GXVoid GXMeshGeometry::Render ()
{
	if ( !mesh || mesh->totalVertices == 0 ) return;

	if ( skinningMaterial )
	{
		glBindVertexArray ( poseVAO[ skinningSwitchIndex ] );
		glDrawArrays ( topology, 0, skin->totalVertices );
		glBindVertexArray ( 0 );

		if ( skinningSwitchIndex == 0 )
			skinningSwitchIndex = 1;
		else
			skinningSwitchIndex = 0;

		return;
	}

	glBindVertexArray ( meshVAO );
	glDrawArrays ( topology, 0, mesh->totalVertices );
	glBindVertexArray ( 0 );
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
	if ( meshVAO == 0 )
		glGenVertexArrays ( 1, &meshVAO );

	if ( !mesh || !mesh->GetMeshFileName () )
	{
		// Procedure generated mesh.

		if ( !mesh )
			mesh = new GXMesh ();

		glBindVertexArray ( meshVAO );
		// {
			if ( mesh->meshVBO == 0 )
				glGenBuffers ( 1, &mesh->meshVBO );

			glBindBuffer ( GL_ARRAY_BUFFER, mesh->meshVBO );
			glEnableVertexAttribArray ( (GLuint)streamIndex );
			glVertexAttribPointer ( (GLuint)streamIndex, numElements, elementType, GL_FALSE, stride, offset );
		// }
		glBindVertexArray ( 0 );

		return;
	}

	if ( mesh )
		mesh->Release ();

	mesh = new GXMesh ();

	glBindVertexArray ( meshVAO );
	// {
		glBindBuffer ( GL_ARRAY_BUFFER, mesh->meshVBO );
		glEnableVertexAttribArray ( (GLuint)streamIndex );
		glVertexAttribPointer ( (GLuint)streamIndex, numElements, elementType, GL_FALSE, stride, offset );
	// }
	glBindVertexArray ( 0 );
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
	glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0, pose[ skinningSwitchIndex ]->meshVBO );
	glBeginTransformFeedback ( GL_POINTS );

	glBindVertexArray ( meshVAO );

	glDrawArrays ( GL_POINTS, 0, skin->totalVertices );

	glBindVertexArray ( 0 );

	glEndTransformFeedback ();
	glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0 );
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

GXVoid GXMeshGeometry::UpdateGraphicResources ()
{
	if ( !mesh || !mesh->GetMeshFileName () || !skin || mesh->totalVertices != skin->totalVertices )
	{
		if ( mesh && meshVAO == 0 )
		{
			glGenVertexArrays ( 1, &meshVAO );
			glBindVertexArray ( meshVAO );
			// {
				GLsizei meshStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
				GXUPointer offset = 0;

				glBindBuffer ( GL_ARRAY_BUFFER, mesh->meshVBO );

				glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::CurrenVertex );
				glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
				offset += sizeof ( GXVec3 );

				glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::UV );
				glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::UV, 2, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
				offset += sizeof ( GXVec2 );

				glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Normal );
				glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Normal, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
				offset += sizeof ( GXVec3 );

				glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Tangent );
				glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
				offset += sizeof ( GXVec3 );

				glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Bitangent );
				glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );

				glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::LastFrameVertex );
				glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::LastFrameVertex, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)0 );
			// }
			glBindVertexArray ( 0 );
		}

		if ( !skinningMaterial ) return;

		delete skinningMaterial;

		glBindVertexArray ( 0 );
		glDeleteVertexArrays ( 2, poseVAO );

		pose[ 0 ]->Release ();
		pose[ 1 ]->Release ();

		pose[ 0 ] = pose[ 1 ] = nullptr;

		return;
	}

	skinningMaterial = new GXSkinningMaterial ();
	GLsizei meshStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	GLsizeiptr poseVBOSize = (GLsizeiptr)( meshStride * skin->totalVertices );

	glGenVertexArrays ( 2, poseVAO );
	
	pose[ 0 ] = new GXMesh ();
	pose[ 0 ]->FillVBO ( nullptr, poseVBOSize, GL_DYNAMIC_DRAW );

	pose[ 1 ] = new GXMesh ();
	pose[ 1 ]->FillVBO ( nullptr, poseVBOSize, GL_DYNAMIC_DRAW );

	GXUPointer offset = 0;

	glBindVertexArray ( poseVAO[ 0 ] );
	// {
		glBindBuffer ( GL_ARRAY_BUFFER, pose[ 0 ]->meshVBO );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::CurrenVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::UV );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::UV, 2, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec2 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Normal );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Normal, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Tangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Bitangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );

		glBindBuffer ( GL_ARRAY_BUFFER, pose[ 1 ]->meshVBO );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::LastFrameVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::LastFrameVertex, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)0 );
	// }
	glBindVertexArray ( 0 );

	offset = 0;

	glBindVertexArray ( poseVAO[ 1 ] );
	// {
		glBindBuffer ( GL_ARRAY_BUFFER, pose[ 1 ]->meshVBO );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::CurrenVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::UV );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::UV, 2, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec2 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Normal );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Normal, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Tangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Bitangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );

		glBindBuffer ( GL_ARRAY_BUFFER, pose[ 0 ]->meshVBO );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::LastFrameVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::LastFrameVertex, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)0 );
	// }
	glBindVertexArray ( 0 );

	offset = 0;
	GLsizei skinStride = sizeof ( GXVec4 ) + sizeof ( GXVec4 );

	glBindVertexArray ( meshVAO );
	// {
		glBindBuffer ( GL_ARRAY_BUFFER, mesh->meshVBO );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::CurrenVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::UV );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::UV, 2, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec2 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Normal );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Normal, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Tangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Bitangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, GL_FALSE, meshStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glBindBuffer ( GL_ARRAY_BUFFER, skin->skinVBO );

		offset = 0;

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::SkinningIndices );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::SkinningIndices, 4, GL_FLOAT, GL_FALSE, skinStride, (const GLvoid*)offset );
		offset += sizeof ( GXVec4 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::SkinnngWeights );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::SkinnngWeights, 4, GL_FLOAT, GL_FALSE, skinStride, (const GLvoid*)offset );
	// }
	glBindVertexArray ( 0 );
}
