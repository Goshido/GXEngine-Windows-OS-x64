//version 1.0

#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXNativeStaticMeshSaver.h>


#define CACHE_DIRECTORY_NAME	L"Cache"
#define CACHE_FILE_EXTENSION	L"cache"


static GXMeshGeometryEntry* gx_MeshGeometryHead = nullptr;

class GXMeshGeometryEntry
{
	public:
		GXMeshGeometryEntry*	next;
		GXMeshGeometryEntry*	prev;

	private:
		GXWChar*				fileName;
		GXMeshGeometry*			meshGeometry;
		GXInt					refs;

	public:
		explicit GXMeshGeometryEntry ( GXMeshGeometry &meshGeometry, const GXWChar* fileName );

		const GXWChar* GetFileName () const;
		GXMeshGeometry& GetMeshGeometry () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXMeshGeometryEntry ();
};

GXMeshGeometryEntry::GXMeshGeometryEntry ( GXMeshGeometry &meshGeometry, const GXWChar* fileName )
{
	GXWcsclone ( &this->fileName, fileName );
	this->meshGeometry = &meshGeometry;
	refs = 1;

	prev = nullptr;

	if ( gx_MeshGeometryHead )
		gx_MeshGeometryHead->prev = this;

	next = gx_MeshGeometryHead;
	gx_MeshGeometryHead = this;
}

const GXWChar* GXMeshGeometryEntry::GetFileName () const
{
	return fileName;
}

GXMeshGeometry& GXMeshGeometryEntry::GetMeshGeometry () const
{
	return *meshGeometry;
}

GXVoid GXMeshGeometryEntry::AddRef ()
{
	refs++;
}

GXVoid GXMeshGeometryEntry::Release ()
{
	refs--;

	if ( refs <= 0 )
		delete this;
}

GXMeshGeometryEntry::~GXMeshGeometryEntry ()
{
	GXSafeFree ( fileName );
	delete meshGeometry;

	if ( gx_MeshGeometryHead == this )
		gx_MeshGeometryHead = gx_MeshGeometryHead->next;
	else
		prev->next = next;

	if ( next )
		next->prev = prev;
}

//----------------------------------------------------------------------------------------

GXMeshGeometry::GXMeshGeometry ()
{
	totalStaticVertices = 0;

	staticVAO = 0;
	staticVBO = 0;

	staticTopology = GL_TRIANGLES;

	totalSkeletalVertices = 0;

	for ( GXUByte i = 0; i < 2; i++ )
	{
		skeletalVAO[ i ] = 0;
		poseVBO[ i ] = 0;
	}

	skinningVAO = 0;
	skeletalVBO = 0;

	skinningSwitchIndex = 0;
	skinningMaterial = nullptr;
	skeletalTopology = GL_TRIANGLES;
}

GXMeshGeometry::~GXMeshGeometry ()
{
	if ( staticVAO != 0 )
	{
		glDeleteVertexArrays ( 1, &staticVAO );
		glDeleteBuffers ( 1, &staticVBO );
	}

	if ( skinningVAO != 0 )
	{
		glDeleteVertexArrays ( 2, skeletalVAO );
		glDeleteVertexArrays ( 1, &skinningVAO );
		glDeleteBuffers ( 2, poseVBO );
		glDeleteBuffers ( 1, &skeletalVBO );

		delete skinningMaterial;
	}
}

GXVoid GXMeshGeometry::Render ()
{
	if ( totalSkeletalVertices != 0 && skinningVAO != 0 )
	{
		glBindVertexArray ( skeletalVAO[ skinningSwitchIndex ] );
		glDrawArrays ( skeletalTopology, 0, totalSkeletalVertices );
		glBindVertexArray ( 0 );

		if ( skinningSwitchIndex == 0 )
			skinningSwitchIndex = 1;
		else
			skinningSwitchIndex = 0;
	}

	if ( totalStaticVertices != 0 && staticVAO != 0 )
	{
		glBindVertexArray ( staticVAO );
		glDrawArrays ( staticTopology, 0, totalStaticVertices );
		glBindVertexArray ( 0 );
	}
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
	this->totalStaticVertices = totalVertices;
}

GXVoid GXMeshGeometry::FillVertexBuffer ( const GXVoid* data, GLsizeiptr size, GLenum usage )
{
	if ( staticVAO == 0 )
		InitStaticResources ();

	glBindBuffer ( GL_ARRAY_BUFFER, staticVBO );
	//{
		glBufferData ( GL_ARRAY_BUFFER, size, data, usage );
	//}
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
}

GXVoid GXMeshGeometry::SetBufferStream ( eGXMeshStreamIndex streamIndex, GLint numElements, GLenum elementType, GLsizei stride, const GLvoid* offset )
{
	if ( staticVAO == 0 )
		InitStaticResources ();

	glBindVertexArray ( staticVAO );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, staticVBO );
		glEnableVertexAttribArray ( (GLuint)streamIndex );
		glVertexAttribPointer ( (GLuint)streamIndex, numElements, elementType, GL_FALSE, stride, offset );
	//}
	glBindVertexArray ( 0 );
}

GXVoid GXMeshGeometry::SetTopology ( GLenum topology )
{
	this->staticTopology = topology;
}

GXVoid GXMeshGeometry::UpdatePose ( const GXSkeleton &skeleton )
{
	if ( !IsSkeletalMesh () || skinningVAO == 0 ) return;

	skinningMaterial->SetSkeleton ( skeleton );
	skinningMaterial->Bind ( GXTransform::GetNullTransform () );

	glBindVertexArray ( skinningVAO );
	glEnable ( GL_RASTERIZER_DISCARD );
	glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0, poseVBO[ skinningSwitchIndex ] );
	glBeginTransformFeedback ( GL_POINTS );

	glDrawArrays ( GL_POINTS, 0, totalSkeletalVertices );

	glEndTransformFeedback ();
	glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0 );
	glDisable ( GL_RASTERIZER_DISCARD );
	glBindVertexArray ( 0 );

	skinningMaterial->Unbind ();
}

GXMeshGeometry& GXCALL GXMeshGeometry::LoadFromObj ( const GXWChar* fileName )
{
	for ( GXMeshGeometryEntry* p = gx_MeshGeometryHead; p; p = p->next )
	{
		if ( GXWcscmp ( p->GetFileName (), fileName ) == 0 )
		{
			p->AddRef ();
			return p->GetMeshGeometry ();
		}
	}

	GXWChar* path = nullptr;
	GXGetFileDirectoryPath ( &path, fileName );
	GXUInt size = GXWcslen ( path ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar );		//L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		//L'/' symbol

	GXWChar* baseFileName = nullptr;
	GXGetBaseFileName ( &baseFileName, fileName );
	size += GXWcslen ( baseFileName ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar );		//L'.' symbol
	size += GXWcslen ( CACHE_FILE_EXTENSION ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		//L'\0' symbol

	GXWChar* cacheFileName = (GXWChar*)malloc ( size );
	wsprintfW ( cacheFileName, L"%s/%s/%s.%s", path, CACHE_DIRECTORY_NAME, baseFileName, CACHE_FILE_EXTENSION );

	if ( GXDoesFileExist ( cacheFileName ) )
	{
		GXMeshGeometry& geometry = GetGeometryFromStm ( cacheFileName );

		free ( path );
		free ( baseFileName );
		free ( cacheFileName );

		new GXMeshGeometryEntry ( geometry, fileName );
		
		return geometry;
	}

	GXOBJPoint* points = nullptr;
	GXInt numVerticies = GXLoadOBJ ( fileName, &points );

	GXAABB bounds;
	GXSetAABBEmpty ( bounds );

	GXNativeStaticMeshDesc descriptor;
	descriptor.numVertices = (GXUInt)numVerticies;
	descriptor.numNormals = (GXUInt)numVerticies;
	descriptor.numTBPairs = (GXUInt)numVerticies;
	descriptor.numUVs = (GXUInt)numVerticies;
	descriptor.numElements = 0;

	GXUInt alpha = numVerticies * sizeof( GXVec3 );
	GXUInt betta = numVerticies * sizeof ( GXVec2 );
	descriptor.vertices = (GXVec3*)malloc ( alpha );
	descriptor.uvs = (GXVec2*)malloc( betta );
	descriptor.normals = (GXVec3*)malloc ( alpha );
	descriptor.tangents = (GXVec3*)malloc ( alpha );
	descriptor.bitangents = (GXVec3*)malloc ( alpha );
	descriptor.elements = nullptr;

	GXUInt cacheSize = alpha + betta + alpha + alpha + alpha;
	GXUByte* cache = (GXUByte*)malloc ( cacheSize );
	GXPointer offset = 0;

	for ( GXUInt i = 0; i < descriptor.numVertices; i++ )
	{
		GXAddVertexToAABB ( bounds, points[ i ].vertex );

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
	size += sizeof ( GXWChar );		//L'/' symbol
	size += GXWcslen ( CACHE_DIRECTORY_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar );		//L'\0' symbol

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

	GXMeshGeometry* geometry = new GXMeshGeometry ();
	geometry->SetTotalVertices ( (GLsizei)numVerticies );
	geometry->FillVertexBuffer ( cache, (GLsizeiptr)cacheSize, GL_STATIC_DRAW );
	geometry->SetTopology ( GL_TRIANGLES );

	GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	offset = 0;

	geometry->SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, stride, (const GLvoid*)offset );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::UV, 2, GL_FLOAT, stride, (const GLvoid*)offset );
	offset += sizeof ( GXVec2 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Normal, 3, GL_FLOAT, stride, (const GLvoid*)offset );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, stride, (const GLvoid*)offset );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, stride, (const GLvoid*)offset );

	geometry->SetBoundsLocal ( bounds );
	
	new GXMeshGeometryEntry ( *geometry, fileName );

	return *geometry;
}

GXMeshGeometry& GXCALL GXMeshGeometry::LoadFromStm ( const GXWChar* fileName )
{
	for ( GXMeshGeometryEntry* p = gx_MeshGeometryHead; p; p = p->next )
	{
		if ( GXWcscmp ( p->GetFileName (), fileName ) == 0 )
		{
			p->AddRef ();
			return p->GetMeshGeometry ();
		}
	}

	GXMeshGeometry& geometry = GetGeometryFromStm ( fileName );
	new GXMeshGeometryEntry ( geometry, fileName );

	return geometry;
}

GXMeshGeometry& GXCALL GXMeshGeometry::LoadFromSkm ( const GXWChar* fileName )
{
	GXSkeletalMeshData skeletalMeshData;
	GXLoadNativeSkeletalMesh ( fileName, skeletalMeshData );

	GLsizeiptr originalVBOSize = skeletalMeshData.numVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
	GLsizeiptr poseVBOSize = skeletalMeshData.numVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) );

	GXMeshGeometry* meshGeometry = new GXMeshGeometry ();
	meshGeometry->InitSkeletalResources ();
	meshGeometry->totalSkeletalVertices = skeletalMeshData.numVertices;
	meshGeometry->skeletalTopology = GL_TRIANGLES;
	meshGeometry->skinningMaterial = new GXSkinningMaterial ();

	glBindBuffer ( GL_ARRAY_BUFFER, meshGeometry->skeletalVBO );
	//{
		glBufferData ( GL_ARRAY_BUFFER, originalVBOSize, skeletalMeshData.vboData, GL_STATIC_DRAW );
	//}
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	for ( GXUByte i = 0; i < 2; i++ )
	{
		glBindBuffer ( GL_ARRAY_BUFFER, meshGeometry->poseVBO[ i ] );
		//{
			glBufferData ( GL_ARRAY_BUFFER, poseVBOSize, nullptr, GL_STATIC_DRAW );
		//}
		glBindBuffer ( GL_ARRAY_BUFFER, 0 );
	}

	new GXMeshGeometryEntry ( *meshGeometry, fileName );

	skeletalMeshData.Cleanup ();

	return *meshGeometry;
}

GXVoid GXCALL GXMeshGeometry::RemoveMeshGeometry ( GXMeshGeometry& mesh )
{
	for ( GXMeshGeometryEntry* p = gx_MeshGeometryHead; p; p = p->next )
	{
		if ( mesh == *p )
		{
			p->Release ();
			mesh = GXMeshGeometry ();
			return;
		}
	}
}

GXUInt GXCALL GXMeshGeometry::GetTotalLoadedMeshGeometries ( const GXWChar** lastMeshGeometry )
{
	GXUInt total = 0;
	for ( GXMeshGeometryEntry* p = gx_MeshGeometryHead; p; p = p->next )
		total++;

	if ( total > 0 )
		*lastMeshGeometry = gx_MeshGeometryHead->GetFileName ();
	else
		*lastMeshGeometry = nullptr;

	return total;
}

GXBool GXMeshGeometry::operator == ( const GXMeshGeometryEntry &entry ) const
{
	const GXMeshGeometry& other = entry.GetMeshGeometry ();
	GXBool isSkeletalMesh = IsSkeletalMesh ();

	if ( isSkeletalMesh != other.IsSkeletalMesh () ) return GX_FALSE;

	if ( isSkeletalMesh )
		return skinningVAO == other.skinningVAO;
	else
		return staticVAO == other.staticVAO;
}

GXVoid GXMeshGeometry::operator = ( const GXMeshGeometry &meshGeometry )
{
	memcpy ( this, &meshGeometry, sizeof ( GXMeshGeometry ) );
}

GXVoid GXMeshGeometry::InitStaticResources ()
{
	glGenVertexArrays ( 1, &staticVAO );
	glGenBuffers ( 1, &staticVBO );

	glBindVertexArray ( staticVAO );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, staticVBO );
	//}
	glBindVertexArray ( 0 );
}

GXVoid GXMeshGeometry::InitSkeletalResources ()
{
	glGenVertexArrays ( 2, skeletalVAO );
	glGenVertexArrays ( 1, &skinningVAO );
	glGenBuffers ( 2, poseVBO );
	glGenBuffers ( 1, &skeletalVBO );

	GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	GXPointer offset = 0;

	glBindVertexArray ( skeletalVAO[ 0 ] );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, poseVBO[ 0 ] );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::CurrenVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::UV );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::UV, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec2 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Normal );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Normal, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Tangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Bitangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );

		glBindBuffer ( GL_ARRAY_BUFFER, poseVBO[ 1 ] );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::LastFrameVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::LastFrameVertex, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)0 );
	//}
	glBindVertexArray ( 0 );

	offset = 0;

	glBindVertexArray ( skeletalVAO[ 1 ] );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, poseVBO[ 1 ] );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::CurrenVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::UV );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::UV, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec2 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Normal );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Normal, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Tangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Bitangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );

		glBindBuffer ( GL_ARRAY_BUFFER, poseVBO[ 0 ] );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::LastFrameVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::LastFrameVertex, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)0 );
	//}
	glBindVertexArray ( 0 );

	offset = 0;

	glBindVertexArray ( skinningVAO );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, skeletalVBO );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::CurrenVertex );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::UV );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::UV, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec2 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Normal );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Normal, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Tangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::Bitangent );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::SkinningIndices );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::SkinningIndices, 4, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec4 );

		glEnableVertexAttribArray ( (GLuint)eGXMeshStreamIndex::SkinnngWeights );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::SkinnngWeights, 4, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
	//}
	glBindVertexArray ( 0 );
}

GXMeshGeometry& GXCALL GXMeshGeometry::GetGeometryFromStm ( const GXWChar* fileName )
{
	GXNativeStaticMeshInfo info;
	GXLoadNativeStaticMesh ( fileName, info );

	GXPointer offset = 0;
	GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );

	GXMeshGeometry* geometry = new GXMeshGeometry ();
	geometry->SetTotalVertices ( info.numVertices );
	geometry->FillVertexBuffer ( info.vboData, (GLsizeiptr)( info.numVertices * stride ), GL_STATIC_DRAW );
	geometry->SetTopology ( GL_TRIANGLES );

	geometry->SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, stride, (const GLvoid*)offset );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::UV, 2, GL_FLOAT, stride, (const GLvoid*)offset );
	offset += sizeof ( GXVec2 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Normal, 3, GL_FLOAT, stride, (const GLvoid*)offset );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, stride, (const GLvoid*)offset );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, stride, (const GLvoid*)offset );

	geometry->SetBoundsLocal ( info.bounds );

	info.Cleanup ();

	return *geometry;
}

GXBool GXMeshGeometry::IsSkeletalMesh () const
{
	return skinningVAO != 0;
}
