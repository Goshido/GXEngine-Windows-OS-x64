//version 1.0

#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXMemory.h>


struct GXMeshGeomentryEntry;
static GXMeshGeomentryEntry* gx_MeshGeometryHead = nullptr;

class GXMeshGeomentryEntry
{
	private:
		GXWChar*				fileName;
		GXMeshGeometry*			meshGeometry;
		GXInt					refs;

	public:
		GXMeshGeomentryEntry*	next;
		GXMeshGeomentryEntry*	prev;

	public:
		GXMeshGeomentryEntry ( GXMeshGeometry& meshGeometry, const GXWChar* fileName );

		const GXWChar* GetFileName () const;
		GXMeshGeometry& GetMeshGeometry () const;

		GXVoid AddRef ();
		GXVoid Release ();

	private:
		~GXMeshGeomentryEntry ();
};

GXMeshGeomentryEntry::GXMeshGeomentryEntry ( GXMeshGeometry& meshGeometry, const GXWChar* fileName )
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

const GXWChar* GXMeshGeomentryEntry::GetFileName () const
{
	return fileName;
}

GXMeshGeometry& GXMeshGeomentryEntry::GetMeshGeometry () const
{
	return *meshGeometry;
}

GXVoid GXMeshGeomentryEntry::AddRef ()
{
	refs++;
}

GXVoid GXMeshGeomentryEntry::Release ()
{
	refs--;

	if ( refs <= 0 )
		delete this;
}

GXMeshGeomentryEntry::~GXMeshGeomentryEntry ()
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
	glGenVertexArrays ( 1, &vao );
	glGenBuffers ( 1, &vbo );

	glBindVertexArray ( vao );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, vbo );
	//}
	glBindVertexArray ( 0 );

	totalElements = 0;
}

GXMeshGeometry::~GXMeshGeometry ()
{
	glBindVertexArray ( 0 );
	glDeleteVertexArrays ( 1, &vao );
	glBindBuffer ( GL_ARRAY_BUFFER, vbo );
	glDeleteBuffers ( 1, &vbo );
}

GXVoid GXMeshGeometry::Render () const
{
	if ( totalElements == 0 ) return;
	
	glBindVertexArray ( vao );
	glDrawArrays ( GL_TRIANGLES, 0, totalElements );
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

GXVoid GXMeshGeometry::UpdateBoundsWorld ( const GXMat4& transform )
{
	GXSetAABBTransformed ( boundsWorld, boundsLocal, transform );
}

const GXAABB& GXMeshGeometry::GetBoundsWorld () const
{
	return boundsWorld;
}

GXVoid GXMeshGeometry::SetTotalElements ( GLsizei elements )
{
	totalElements = elements;
}

GXVoid GXMeshGeometry::FillVertexBuffer ( const GXVoid* data, GLsizeiptr size, GLenum usage )
{
	glBindVertexArray ( vao );
	glBindBuffer ( GL_ARRAY_BUFFER, vbo );
	//{
		glBufferData ( GL_ARRAY_BUFFER, size, data, usage );
	//}
	glBindVertexArray ( 0 );
}

GXVoid GXMeshGeometry::SetBufferStream ( eGXMeshStreamIndex streamIndex, GLint numElements, GLenum elementType, GLsizei stride, const GLvoid* offset )
{
	glBindVertexArray ( vao );
	//{	
		glEnableVertexAttribArray ( (GLuint)streamIndex );
		glVertexAttribPointer ( (GLuint)streamIndex, numElements, elementType, GL_FALSE, stride, offset );
	//}
	glBindVertexArray(0);
}

GXMeshGeometry& GXCALL GXMeshGeometry::LoadFromObj ( const GXWChar* fileName )
{
	QFileInfo fileInfo(fileName);

    if (!fileInfo.exists()) {
        gx_common::gxLogA("MeshGeometry::loadFromObj::Error - File %s not found\n", fileName);
        return QSharedPointer<MeshGeometry>();
    }

    QDir dir;
    QString cacheFileDir = fileInfo.path() + "/" + CACHE_FOLDER_NAME;

    if (dir.mkdir(cacheFileDir)) {
        gx_common::gxLogA("MeshGeometry::loadFromObj::Info - Directory %s created\n", cacheFileDir.toLatin1().data());
    }

    QSharedPointer<MeshGeometry> meshGeometry;
    QHash<QString, QWeakPointer<MeshGeometry>>::iterator it = _storage.find(fileName);

    if (it == _storage.end()) {
        meshGeometry = QSharedPointer<MeshGeometry>(new MeshGeometry(fileName), &MeshGeometry::remove);
        _storage.insert(fileName, meshGeometry.toWeakRef());

        QString cacheFileName = cacheFileDir + "/" + fileInfo.completeBaseName() + CACHE_FILE_SUFFIX;
        QFile cacheFile(cacheFileName);
        if (cacheFile.exists()) {
            // Load from cache
            gx_common::GXNativeStaticMeshInfo info;
            gx_common::gxLoadNativeStaticMesh(cacheFileName, info);

            meshGeometry->setBoundsLocal(info.bounds);
            gx_common::GXMat4 transform;
            gx_common::gxSetMat4Identity(transform);
            meshGeometry->updateBoundsWorld(transform);

            meshGeometry->setTotalElements(info.numVertices);
            meshGeometry->fillVertexBuffer(info.vboData, info.getVBOSize(), GL_STATIC_DRAW);

            size_t offset = 0;
            meshGeometry->setBufferStream(VERTEX_STREAM, 3, GL_FLOAT, info.vboStride, (const void*)offset);
            offset += sizeof(gx_common::GXVec3);
            meshGeometry->setBufferStream(UV_COORD_STREAM, 2, GL_FLOAT, info.vboStride, (const void*)offset);
            offset += sizeof(gx_common::GXVec2);
            meshGeometry->setBufferStream(NORMAL_STREAM, 3, GL_FLOAT, info.vboStride, (const void*)offset);
            offset += sizeof(gx_common::GXVec3);
            meshGeometry->setBufferStream(TANGENT_STREAM, 3, GL_FLOAT, info.vboStride, (const void*)offset);
            offset += sizeof(gx_common::GXVec3);
            meshGeometry->setBufferStream(BITANGENT_STREAM, 3, GL_FLOAT, info.vboStride, (const void*)offset);

            info.cleanup ();
        } else {
            // Load from original file and save cache
            gx_common::GXOBJPoint* objPoints = nullptr;
            int numVertices = gx_common::gxLoadOBJ(fileName, &objPoints);

            gx_common::GXNativeStaticMeshDesc desc;

            desc.numVertices = numVertices;
            desc.numNormals = numVertices;
            desc.numTBPairs = numVertices;
            desc.numUVs = numVertices;
            desc.numElements = 0;

            size_t size = desc.numVertices * sizeof(gx_common::GXVec3);

            desc.vertices = (gx_common::GXVec3*)malloc(size);
            desc.uvs = (gx_common::GXVec2*)malloc(desc.numUVs * sizeof(gx_common::GXVec2));
            desc.normals = (gx_common::GXVec3*)malloc(size);
            desc.tangents = (gx_common::GXVec3*)malloc(size);
            desc.bitangents = (gx_common::GXVec3*)malloc(size);
            desc.elements = 0;

            size_t cachedDataSize = 4 * size + desc.numUVs * sizeof(gx_common::GXVec2);
            unsigned char* content = (unsigned char*)malloc(cachedDataSize);
            size_t offset = 0;

            gx_common::GXAABB localBounds;

            for (unsigned int i = 0; i < desc.numVertices; i++) {
                gx_common::gxAddVertexToAABB(localBounds, objPoints[i].vertex);

                desc.vertices[i] = objPoints[i].vertex;
                desc.uvs[i] = objPoints[i].uv;
                desc.normals[i] = objPoints[i].normal;

                const unsigned char* vertices = (const unsigned char*)(&objPoints[3 * (i / 3)].vertex);
                const unsigned char* uvs = (const unsigned char*)(&objPoints[3 * (i / 3)].uv);

                gx_common::gxGetTangentBitangent(desc.tangents[i], desc.bitangents[i], i % 3, vertices,
                                                 sizeof(gx_common::GXOBJPoint), uvs, sizeof(gx_common::GXOBJPoint));

                gx_common::GXVec3* v = (gx_common::GXVec3*)(content + offset);
                *v = desc.vertices[i];
                offset += sizeof(gx_common::GXVec3);

                gx_common::GXVec2* u = (gx_common::GXVec2*)(content + offset);
                *u = desc.uvs[i];
                offset += sizeof(gx_common::GXVec2);

                v = (gx_common::GXVec3*)(content + offset);
                *v = desc.normals[i];
                offset += sizeof(gx_common::GXVec3);

                v = (gx_common::GXVec3*)(content + offset);
                *v = desc.tangents[i];
                offset += sizeof(gx_common::GXVec3);

                v = (gx_common::GXVec3*)(content + offset);
                *v = desc.bitangents[i];
                offset += sizeof(gx_common::GXVec3);
            }

            meshGeometry->setBoundsLocal(localBounds);
            gx_common::GXMat4 transform;
            gx_common::gxSetMat4Identity(transform);
            meshGeometry->updateBoundsWorld(transform);

            meshGeometry->setTotalElements(desc.numVertices);
            meshGeometry->fillVertexBuffer(content, cachedDataSize, GL_STATIC_DRAW);

            static const size_t vboStride = sizeof(gx_common::GXVec3) + sizeof(gx_common::GXVec2)
                    + sizeof(gx_common::GXVec3) + 2 * sizeof(gx_common::GXVec3);

            offset = 0;
            meshGeometry->setBufferStream(VERTEX_STREAM, 3, GL_FLOAT, vboStride, (const void*)offset);
            offset += sizeof(gx_common::GXVec3);
            meshGeometry->setBufferStream(UV_COORD_STREAM, 2, GL_FLOAT, vboStride, (const void*)offset);
            offset += sizeof(gx_common::GXVec2);
            meshGeometry->setBufferStream(NORMAL_STREAM, 3, GL_FLOAT, vboStride, (const void*)offset);
            offset += sizeof(gx_common::GXVec3);
            meshGeometry->setBufferStream(TANGENT_STREAM, 3, GL_FLOAT, vboStride, (const void*)offset);
            offset += sizeof(gx_common::GXVec3);
            meshGeometry->setBufferStream(BITANGENT_STREAM, 3, GL_FLOAT, vboStride, (const void*)offset);

            free(content);

            gx_common::gxExportNativeStaticMesh(cacheFileName, desc);

            free(desc.vertices);
            free(desc.uvs);
            free(desc.normals);
            free(desc.tangents);
            free(desc.bitangents);
        }
    } else {
        meshGeometry = it.value().lock();
    }

    return meshGeometry;
}

GXVoid GXCALL GXMeshGeometry::Remove ( GXMeshGeometry& mesh )
{
	for ( GXMeshGeomentryEntry* p = gx_MeshGeometryHead; p; p->next )
	{
		if ( mesh == *p )
			p->Release ();
	}
}

GXBool GXMeshGeometry::operator == ( const GXMeshGeomentryEntry &entry )
{
	return this == &entry.GetMeshGeometry ();
}
