//version 1.5

#include <GXEngine/GXVAOStorage.h>
#include <GXCommon/GXSTGStructs.h>
#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXNativeStaticMeshSaver.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


struct GXVAOUnit
{
	GXVAOUnit*		next;
	GXVAOUnit*		prev;

	GXInt			ref;
	GXWChar*		name;

	GXVAOInfo		info;

	GLuint			vbo;
	GLuint			ebo;

	GXVAOUnit ();
	~GXVAOUnit ();

	GXVoid AddRef ();
	GXVoid Release ();
};

GXVAOUnit::GXVAOUnit ()
{
	next = prev = 0;

	ref = 0;
	name = 0;

	GXSetAABBEmpty ( info.bounds );

	info.vao = vbo = ebo = 0;
	info.numVertices = 0;
	info.numElements = 0;
}

GXVAOUnit::~GXVAOUnit ()
{
	GXSafeDelete ( name );
}

GXVoid GXVAOUnit::AddRef ()
{
	ref++;
}

GXVoid GXVAOUnit::Release ()
{
	ref--;
}

//------------------------------------------------------

GXVAOUnit* gx_strg_VAOs = 0;


GXVoid GXCALL GXCreateVAOFromObj ( GXVAOUnit &unit, const GXUByte* data, GXUInt size )
{
	glGenBuffers ( 1, &unit.vbo );
	glBindBuffer ( GL_ARRAY_BUFFER, unit.vbo );
		glBufferData ( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW );

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	glGenVertexArrays ( 1, &unit.info.vao );
	glBindVertexArray ( unit.info.vao );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, unit.vbo );

		const GXPointer vertexOffset = 0;
		const GXPointer uvOffset = sizeof ( GXVec3 );
		const GXPointer normalOffset = sizeof ( GXVec3 ) + sizeof ( GXVec2 );
		const GXPointer tangentOffset = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 );
		const GXPointer bitangentOffset = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
		
		const GXPointer stride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );

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
	//}
	glBindVertexArray ( 0 );
}

GXVoid GXCALL GXCreateVAOFromNativeStaticMesh ( GXVAOUnit &out, const GXNativeStaticMeshInfo &info )
{
	out.info.bounds = info.bounds;

	out.info.numVertices = info.numVertices;
	GXPointer vertexSize = info.numVertices * sizeof ( GXVec3 );
	GXPointer uvSize = info.numUVs * sizeof ( GXVec2 );
	GXPointer normalSize = info.numNormals * sizeof ( GXVec3 );
	GXPointer tangentBitangentSize = 2 * info.numTBPairs * sizeof ( GXVec3 );

	GXPointer size = vertexSize + uvSize + normalSize + tangentBitangentSize;

	glGenBuffers ( 1, &out.vbo );
	glBindBuffer ( GL_ARRAY_BUFFER, out.vbo );
		glBufferData ( GL_ARRAY_BUFFER, size, info.vboData, GL_STATIC_DRAW );

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	if ( info.numElements > 0 )
	{
		out.info.numElements = info.numElements;

		glGenBuffers ( 1, &out.ebo );
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, out.ebo );
			glBufferData ( GL_ELEMENT_ARRAY_BUFFER, info.numElements * sizeof ( GXUInt ), info.eboData, GL_STATIC_DRAW );
		
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}

	glGenVertexArrays ( 1, &out.info.vao );
	glBindVertexArray ( out.info.vao );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, out.vbo );

		const GXPointer vertexOffset = 0;
		const GXPointer uvOffset = ( uvSize > 0 ) ? vertexOffset + sizeof ( GXVec3 ) : vertexOffset;
		const GXPointer normalOffset = ( normalSize > 0 ) ? uvOffset + sizeof ( GXVec2 ) : uvOffset;
		const GXPointer tangentOffset = ( tangentBitangentSize > 0 ) ? normalOffset + sizeof ( GXVec3 ) : 0;
		const GXPointer bitangentOffset = ( tangentBitangentSize > 0 ) ? tangentOffset + sizeof ( GXVec3 ) : 0;
		
		GXInt stride = (GXInt)sizeof ( GXVec3 );
		stride += ( uvSize > 0 ) ? sizeof ( GXVec2 ) : 0;
		stride += ( normalSize > 0 ) ? sizeof ( GXVec3 ) : 0;
		stride += ( tangentOffset > 0 ) ? 2 * sizeof ( GXVec3 ) : 0;

		glVertexAttribPointer ( GX_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)vertexOffset );
		glEnableVertexAttribArray ( GX_VERTEX_INDEX );

		if ( uvSize )
		{
			glVertexAttribPointer ( GX_UV_COORD_INDEX, 2, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)uvOffset );
			glEnableVertexAttribArray ( GX_UV_COORD_INDEX );
		}

		if ( normalSize )
		{
			glVertexAttribPointer ( GX_NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)normalOffset );
			glEnableVertexAttribArray ( GX_NORMAL_INDEX );
		}

		if ( tangentBitangentSize )
		{
			glVertexAttribPointer ( GX_TANGENT_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)tangentOffset );
			glEnableVertexAttribArray ( GX_TANGENT_INDEX );

			glVertexAttribPointer ( GX_BITANGENT_INDEX, 3, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)bitangentOffset );
			glEnableVertexAttribArray ( GX_BITANGENT_INDEX );
		}

		if ( info.numElements > 0 )
			glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, out.ebo );
	//}
	glBindVertexArray ( 0 );
}

GXVoid GXCALL GXGetVAOFromOBJ ( GXVAOInfo &out, const GXWChar* fileName, const GXWChar* cacheFileName )
{
	GXVAOUnit* temp = new GXVAOUnit;

	GXWcsclone ( &temp->name, fileName );

	GXVAOUnit* t = gx_strg_VAOs;
	while ( t )
	{
		if ( GXWcscmp ( t->name, temp->name ) == 0 )
			break;

		t = t->next;
	}

	if ( t )
	{
		t->AddRef ();
		memcpy ( &out, &t->info, sizeof ( GXVAOInfo ) );

		delete temp;
		return;
	}

	GXUInt size;
	GXUByte* cache_source = 0;
	if ( GXDoesFileExist ( cacheFileName ) )
	{
		GXNativeStaticMeshInfo info;
		GXLoadNativeStaticMesh ( cacheFileName, info );
		GXCreateVAOFromNativeStaticMesh ( *temp, info );

		info.Cleanup ();

		memcpy ( &out, &temp->info, sizeof ( GXVAOInfo ) );
		
		temp->AddRef ();

		temp->next = gx_strg_VAOs;
		if ( gx_strg_VAOs )
			gx_strg_VAOs->prev = temp;
		gx_strg_VAOs = temp;

		free ( cache_source );

		return;
	}
	
	GXOBJPoint* points;
	temp->info.numVertices = GXLoadOBJ ( fileName, &points );
	
	GXNativeStaticMeshDesc desc;
	
	desc.numVertices = temp->info.numVertices;
	desc.numNormals = temp->info.numVertices;
	desc.numTBPairs = temp->info.numVertices;
	desc.numUVs = temp->info.numVertices;
	desc.numElements = temp->info.numElements = 0;

	size = desc.numVertices * sizeof ( GXVec3 );
	desc.vertices = (GXVec3*)malloc ( size );
	desc.uvs = (GXVec2*)malloc ( desc.numUVs * sizeof ( GXVec2 ) );
	desc.normals = (GXVec3*)malloc ( size );
	desc.tangents = (GXVec3*)malloc ( size );
	desc.bitangents = (GXVec3*)malloc ( size );

	desc.elements = 0;

	GXUInt cachedDataSize = 4 * size + temp->info.numVertices * sizeof ( GXVec2 );
	GXUByte* content = (GXUByte*)malloc ( cachedDataSize );
	GXUInt offset = 0;

	for ( GXUInt i = 0; i < desc.numVertices; i++ )
	{
		GXAddVertexToAABB ( temp->info.bounds, points[ i ].vertex );

		desc.vertices[ i ] = points[ i ].vertex;
		desc.uvs[ i ] = points[ i ].uv;
		desc.normals[ i ] = points[ i ].normal;

		const GXUByte* vertices = (const GXUByte*)( &points[ 3 * ( i / 3 ) ].vertex );
		const GXUByte* uvs = (const GXUByte*)( &points[ 3 * ( i / 3 ) ].uv );

		GXGetTangentBitangent ( desc.tangents[ i ], desc.bitangents[ i ], i % 3, vertices, sizeof ( GXOBJPoint ), uvs, sizeof ( GXOBJPoint ) );

		GXVec3* v = (GXVec3*)( content + offset );
		*v = desc.vertices[ i ];
		offset += sizeof ( GXVec3 );

		GXVec2* u = (GXVec2*)( content + offset );
		*u = desc.uvs[ i ];
		offset += sizeof ( GXVec2 );

		v = (GXVec3*)( content + offset );
		*v = desc.normals[ i ];
		offset += sizeof ( GXVec3 );

		v = (GXVec3*)( content + offset );
		*v = desc.tangents[ i ];
		offset += sizeof ( GXVec3 );

		v = (GXVec3*)( content + offset );
		*v = desc.bitangents[ i ];
		offset += sizeof ( GXVec3 );
	}

	GXExportNativeStaticMesh ( cacheFileName, desc );

	free ( desc.vertices );
	free ( desc.uvs );
	free ( desc.normals );
	free ( desc.tangents );
	free ( desc.bitangents );
	
	GXCreateVAOFromObj ( *temp, content, cachedDataSize );
	free ( content );
	memcpy ( &out, &temp->info, sizeof ( GXVAOInfo ) );

	free ( points );

	temp->AddRef ();

	temp->next = gx_strg_VAOs;
	if ( gx_strg_VAOs )
		gx_strg_VAOs->prev = temp;
	gx_strg_VAOs = temp;
}

GXVoid GXCALL GXGetVAOFromNativeStaticMesh ( GXVAOInfo &out, const GXWChar* fileName )
{
	GXVAOUnit* temp = new GXVAOUnit;
	GXWcsclone ( &temp->name, fileName );

	GXVAOUnit* t = gx_strg_VAOs;
	while ( t )
	{
		if ( GXWcscmp ( t->name, temp->name ) == 0 )
			break;

		t = t->next;
	}

	if ( t )
	{
		t->AddRef ();
		memcpy ( &out, &t->info, sizeof ( GXVAOInfo ) );

		delete temp;
		return;
	}
	
	GXNativeStaticMeshInfo info;
	GXLoadNativeStaticMesh ( fileName, info );
	GXCreateVAOFromNativeStaticMesh ( *temp, info );

	info.Cleanup ();

	memcpy ( &out, &temp->info, sizeof ( GXVAOInfo ) );

	temp->AddRef ();

	temp->next = gx_strg_VAOs;
	if ( gx_strg_VAOs )
		gx_strg_VAOs->prev = temp;
	gx_strg_VAOs = temp;
}

GXVoid GXCALL GXRemoveVAO ( const GXVAOInfo &info )
{
	GXVAOUnit* t = gx_strg_VAOs;
	while ( t )
	{
		if ( t->info.vao == info.vao )
			break;

		t = t->next;
	}

	if ( !t )
	{
		GXLogW ( L"GXRemoveVAO::Error - Произошла ошибка удаления\n" );
		return;
	}

	t->Release ();

	if ( t->ref > 0 )
		return;

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
	glDeleteBuffers ( 1, &t->vbo );

	if ( t->info.numElements > 0 )
	{
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
		glDeleteBuffers ( 1, &t->ebo );
	}

	glBindVertexArray ( 0 );
	glDeleteVertexArrays ( 1, &t->info.vao );

	if ( t == gx_strg_VAOs )
	{
		gx_strg_VAOs = t->next;
		if ( gx_strg_VAOs ) gx_strg_VAOs->prev = 0;
	}
	else if ( t->next )
	{
		t->next->prev = t->prev;
		t->prev->next = t->next;
	}
	else
		t->prev->next = 0;

	delete t;
}

GXUInt GXCALL GXGetTotalVAOStorageObjects ( const GXWChar** lastVAO )
{
	GXUInt objects = 0;
	*lastVAO = 0;

	for ( GXVAOUnit* p = gx_strg_VAOs; p; p = p->next )
	{
		*lastVAO = p->name;
		objects++;
	}

	return objects;
}
