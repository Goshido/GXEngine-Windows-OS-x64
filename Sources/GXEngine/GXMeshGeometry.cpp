//version 1.0

#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXNativeStaticMeshSaver.h>


#define CACHE_FOLDER_NAME L"Cache"
#define CACHE_FILE_EXTENSION L"cache"


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
		explicit GXMeshGeometryEntry ( GXMeshGeometry& meshGeometry, const GXWChar* fileName );

		const GXWChar* GetFileName () const;
		GXMeshGeometry& GetMeshGeometry () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXMeshGeometryEntry ();
};

GXMeshGeometryEntry::GXMeshGeometryEntry ( GXMeshGeometry& meshGeometry, const GXWChar* fileName )
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
	totalVertices = 0;

	staticVAO = 0;
	staticVBO = 0;

	for ( GXUByte i = 0; i < 2; i++ )
	{
		skeletalVAO[ i ] = 0;
		skinningVAO[ i ] = 0;
		skinningVBO[ i ] = 0;
	}

	skeletalVBO = 0;

	skinningSwitchIndex = 0;

	topology = GL_TRIANGLES;
	skeletalMeshID = (GXUPointer)nullptr;
}

GXMeshGeometry::~GXMeshGeometry ()
{
	if ( staticVAO != 0 )
	{
		glDeleteVertexArrays ( 1, &staticVAO );
		glDeleteBuffers ( 1, &staticVBO );
	}
	else if ( skeletalVBO != 0 )
	{
		glDeleteVertexArrays ( 2, skeletalVAO );
		glDeleteVertexArrays ( 2, skinningVAO );
		glDeleteBuffers ( 2, skinningVBO );
		glDeleteBuffers ( 1, &skeletalVBO );
	}
}

GXVoid GXMeshGeometry::Render () const
{
	if ( totalVertices == 0 ) return;

	if ( staticVAO != 0 )
	{
		glBindVertexArray ( staticVAO );
		glDrawArrays ( topology, 0, totalVertices );
		glBindVertexArray ( 0 );
	}
	else if ( skeletalVBO != 0 )
	{
		glBindVertexArray ( skeletalVAO[ skinningSwitchIndex ] );
		glDrawArrays ( topology, 0, totalVertices );
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
	this->totalVertices = totalVertices;
}

GXVoid GXMeshGeometry::FillVertexBuffer ( const GXVoid* data, GLsizeiptr size, GLenum usage, eGXMeshGeometryType type )
{
	switch ( type )
	{
		case eGXMeshGeometryType::Static:
		{
			if ( staticVAO == 0 )
				InitStaticVAO ();

			glBindBuffer ( GL_ARRAY_BUFFER, staticVBO );
			//{
				glBufferData ( GL_ARRAY_BUFFER, size, data, usage );
			//}
			glBindVertexArray ( 0 );
		}
		break;

		case eGXMeshGeometryType::Skeletal:
		{
			if ( skeletalVBO == 0 )
				InitSkeletalVAO ();

			glBindBuffer ( GL_ARRAY_BUFFER, skeletalVBO );
			//{
				glBufferData ( GL_ARRAY_BUFFER, size, data, usage );
			//}
			glBindVertexArray ( 0 );
		}
		break;
	}
}

GXVoid GXMeshGeometry::EnableBufferStream ( eGXMeshStreamIndex streamIndex, eGXMeshGeometryType type )
{
	switch ( type )
	{
		case eGXMeshGeometryType::Static:
		{
			if ( staticVAO == 0 )
				InitStaticVAO ();

			glBindVertexArray ( staticVAO );
			//{
				glEnableVertexAttribArray ( (GLuint)streamIndex );
			//}
			glBindVertexArray ( 0 );
		}
		break;

		case eGXMeshGeometryType::Skeletal:
		{
			if ( skeletalVBO == 0 )
				InitSkeletalVAO ();

			for ( GXUByte i = 0; i < 2; i++ )
			{
				glBindVertexArray ( skinningVAO[ i ] );
				//{
					glEnableVertexAttribArray ( (GLuint)streamIndex );
				//}
				glBindVertexArray ( 0 );
			}
		}
		break;
	}
}

GXVoid GXMeshGeometry::DisableBufferStream ( eGXMeshStreamIndex streamIndex, eGXMeshGeometryType type )
{
	switch ( type )
	{
		case eGXMeshGeometryType::Static:
		{
			if ( staticVAO == 0 )
				InitStaticVAO ();

			glBindVertexArray ( staticVAO );
			//{
				glDisableVertexAttribArray ( (GLuint)streamIndex );
			//}
			glBindVertexArray ( 0 );
		}
		break;

		case eGXMeshGeometryType::Skeletal:
		{
			if ( skeletalVBO == 0 )
				InitSkeletalVAO ();

			for ( GXUByte i = 0; i < 2; i++ )
			{
				glBindVertexArray ( skinningVAO[ i ] );
				//{
					glDisableVertexAttribArray ( (GLuint)streamIndex );
				//}
				glBindVertexArray ( 0 );
			}
		}
		break;
	}
}

GXVoid GXMeshGeometry::SetBufferStream ( eGXMeshStreamIndex streamIndex, GLint numElements, GLenum elementType, GLsizei stride, const GLvoid* offset, eGXMeshGeometryType type )
{
	switch ( type )
	{
		case eGXMeshGeometryType::Static:
		{
			if ( staticVAO == 0 )
				InitStaticVAO ();

			glBindVertexArray ( staticVAO );
			//{
				glVertexAttribPointer ( (GLuint)streamIndex, numElements, elementType, GL_FALSE, stride, offset );
			//}
			glBindVertexArray ( 0 );
		}
		break;

		case eGXMeshGeometryType::Skeletal:
		{
			if ( skeletalVBO == 0 )
				InitSkeletalVAO ();

			for ( GXUByte i = 0; i < 2; i++ )
			{
				glBindVertexArray ( skinningVAO[ i ] );
				//{
					glVertexAttribPointer ( (GLuint)streamIndex, numElements, elementType, GL_FALSE, stride, offset );
				//}
				glBindVertexArray ( 0 );
			}
		}
		break;
	}
}

GXVoid GXMeshGeometry::SetTopology ( GLenum topology )
{
	this->topology = topology;
}

GXVoid GXMeshGeometry::UpdatePose ( const GXSkeleton &skeleton )
{
	//TODO
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

	size += sizeof ( GXWChar ); // "/" symbol
	size += GXWcslen ( CACHE_FOLDER_NAME ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar ); // "/" symbol

	GXWChar* baseFileName = nullptr;
	GXGetBaseFileName ( &baseFileName, fileName );
	size += GXWcslen ( baseFileName ) * sizeof ( GXWChar );

	size += sizeof ( GXWChar ); // "." symbol
	size += GXWcslen ( CACHE_FILE_EXTENSION ) * sizeof ( GXWChar );
	size += sizeof ( GXWChar ); // "\0" symbol

	GXWChar* cacheFileName = (GXWChar*)malloc ( size );
	wsprintfW ( cacheFileName, L"%s/%s/%s.%s", path, CACHE_FOLDER_NAME, baseFileName, CACHE_FILE_EXTENSION );

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
	geometry->FillVertexBuffer ( cache, (GLsizeiptr)cacheSize, GL_STATIC_DRAW, eGXMeshGeometryType::Static );
	geometry->SetTopology ( GL_TRIANGLES );

	GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	offset = 0;

	geometry->SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::UV, 2, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );
	offset += sizeof ( GXVec2 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Normal, 3, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );

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

GXMeshGeometry& GXCALL GXMeshGeometry::LoadFromSkm ( const GXWChar* fileName )
{
	GXSkeletalMeshData skeletalMeshData;
	GXLoadNativeSkeletalMesh ( fileName, skeletalMeshData );

	GLsizeiptr originalVBOSize = skeletalMeshData.numVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
	GLsizeiptr poseVBOSize = skeletalMeshData.numVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) );

	GXMeshGeometry* meshGeometry = new GXMeshGeometry ();
	meshGeometry->SetTotalVertices ( (GLsizei)skeletalMeshData.numVertices );
	meshGeometry->SetTopology ( GL_TRIANGLES );
	meshGeometry->FillVertexBuffer ( skeletalMeshData.vboData, originalVBOSize, GL_STATIC_DRAW );

	glGenBuffers ( 2, meshGeometry->vboPose );
	for ( GXUByte i = 0; i < 2; i++ )
	{
		glBindBuffer ( GL_TRANSFORM_FEEDBACK_BUFFER, meshGeometry->vboPose[ i ] );
		//{
			glBufferData ( GL_TRANSFORM_FEEDBACK_BUFFER, poseVBOSize, nullptr, GL_DYNAMIC_COPY );
		//}
		glBindBuffer ( GL_TRANSFORM_FEEDBACK_BUFFER, 0 );
	}

	meshGeometry->skeletalMeshID = (GXUPointer)meshGeometry;
	new GXMeshGeometryEntry ( *meshGeometry, fileName );

	skeletalMeshData.Cleanup ();

	return *meshGeometry;
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
		return skeletalVBO == other.skeletalVBO;
	else
		return staticVAO == other.staticVAO;
}

GXVoid GXMeshGeometry::operator = ( const GXMeshGeometry &meshGeometry )
{
	memcpy ( this, &meshGeometry, sizeof ( GXMeshGeometry ) );
}

GXVoid GXMeshGeometry::InitStaticVAO ()
{
	glGenVertexArrays ( 1, &staticVAO );
	glGenBuffers ( 1, &staticVBO );

	glBindVertexArray ( staticVBO );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, staticVBO );
	//}
	glBindVertexArray ( 0 );
}

GXVoid GXMeshGeometry::InitSkeletalVAO ()
{
	glGenVertexArrays ( 2, skeletalVAO );
	glGenVertexArrays ( 2, skinningVAO );
	glGenBuffers ( 2, skinningVBO );
	glGenBuffers ( 1, &skeletalVBO );

	GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	GXPointer offset = 0;

	glBindVertexArray ( skeletalVAO[ 0 ] );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, skinningVBO[ 0 ] );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::UV, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec2 );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Normal, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );

		glBindBuffer ( GL_ARRAY_BUFFER, skinningVBO[ 1 ] );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::LastFrameVertex, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)0 );
	//}
	glBindVertexArray ( 0 );

	offset = 0;

	glBindVertexArray ( skeletalVAO[ 1 ] );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, skinningVBO[ 1 ] );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::UV, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec2 );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Normal, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );
		offset += sizeof ( GXVec3 );

		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)offset );

		glBindBuffer ( GL_ARRAY_BUFFER, skinningVBO[ 0 ] );
		glVertexAttribPointer ( (GLuint)eGXMeshStreamIndex::LastFrameVertex, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)0 );
	//}
	glBindVertexArray ( 0 );

	glBindVertexArray ( skinningVAO[ 0 ] );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, skeletalVBO );
		glBindBuffer ( GL_TRANSFORM_FEEDBACK_BUFFER, skinningVBO[ 0 ] );
	//}
	glBindVertexArray ( 0 );
}

GXMeshGeometry& GXCALL  GXMeshGeometry::GetGeometryFromStm ( const GXWChar* fileName )
{
	GXNativeStaticMeshInfo info;
	GXLoadNativeStaticMesh ( fileName, info );

	GXPointer offset = 0;
	GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );

	GXMeshGeometry* geometry = new GXMeshGeometry ();
	geometry->SetTotalVertices ( info.numVertices );
	geometry->FillVertexBuffer ( info.vboData, (GLsizeiptr)( info.numVertices * stride ), GL_STATIC_DRAW, eGXMeshGeometryType::Static );
	geometry->SetTopology ( GL_TRIANGLES );

	geometry->SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::UV, 2, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );
	offset += sizeof ( GXVec2 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Normal, 3, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Tangent, 3, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );
	offset += sizeof ( GXVec3 );

	geometry->SetBufferStream ( eGXMeshStreamIndex::Bitangent, 3, GL_FLOAT, stride, (const GLvoid*)offset, eGXMeshGeometryType::Static );

	GXMat4 transform;
	GXSetMat4Identity ( transform );
	geometry->SetBoundsLocal ( info.bounds );

	info.Cleanup ();

	return *geometry;
}

GXBool GXMeshGeometry::IsSkeletalMesh () const
{
	return skeletalVBO != 0;
}
