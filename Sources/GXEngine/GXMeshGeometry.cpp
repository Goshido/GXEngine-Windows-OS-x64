// version 1.8

#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXOBJLoader.h>
#include <GXCommon/GXNativeMesh.h>
#include <GXCommon/GXNativeSkin.h>
#include <GXCommon/GXNativeStaticMeshLoader.h>
#include <GXCommon/GXNativeStaticMeshSaver.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXUPointerAtomic.h>


#define CACHE_DIRECTORY_NAME    "Cache"
#define CACHE_FILE_EXTENSION    "cache"

//---------------------------------------------------------------------------------------------------------------------

class GXMesh final : public GXMemoryInspector
{
    friend class GXMeshGeometry;

    private:
        GXUPointerAtomic    _referenceCount;
        GXMesh*             _previous;
        GLsizeiptr          _vboSize;
        GXString            _meshFile;

        static GXMesh*      _top;
        GXMesh*             _next;

        GLenum              _vboUsage;

    public:
        GLsizei             _totalVertices;
        GLuint              _meshVBO;

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
        // Creates procedural mesh.
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

GXMesh* GXMesh::_top = nullptr;

GXVoid GXMesh::AddReference ()
{
    ++_referenceCount;
}

GXVoid GXMesh::Release ()
{
    --_referenceCount;

    if ( _referenceCount > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

const GXWChar* GXMesh::GetMeshFileName () const
{
    return _meshFile;
}

GXVoid GXMesh::FillVBO ( const GXVoid* data, GLsizeiptr size, GLenum usage )
{
    _meshFile.Clear ();

    if ( _meshVBO == 0u )
        glGenBuffers ( 1, &_meshVBO );

    glBindBuffer ( GL_ARRAY_BUFFER, _meshVBO );
    // {
        if ( size <= _vboSize && usage == _vboUsage )
        {
            glBufferSubData ( GL_ARRAY_BUFFER, 0, size, data );
        }
        else
        {
            _vboUsage = usage;
            _vboSize = size;
            glBufferData ( GL_ARRAY_BUFFER, _vboSize, data, _vboUsage );
        }
    // }
    glBindBuffer ( GL_ARRAY_BUFFER, 0u );
}

GXMesh* GXCALL GXMesh::Find ( const GXWChar* fileName )
{
    for ( GXMesh* mesh = _top; mesh; mesh = mesh->_next )
    {
        if ( GXWcscmp ( mesh->_meshFile, fileName ) != 0 ) continue;

        return mesh;
    }

    return nullptr;
}

GXUInt GXCALL GXMesh::GetTotalLoadedMeshes ( const GXWChar** lastMesh )
{
    GXUInt total = 0u;

    for ( GXMesh* mesh = _top; mesh; mesh = mesh->_next )
        ++total;

    if ( total > 0u )
        *lastMesh = _top->GetMeshFileName ();
    else
        *lastMesh = nullptr;

    return total;
}

GXMesh::GXMesh ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXMesh" )
    _referenceCount ( 1u ),
    _previous ( nullptr ), 
    _vboSize ( 0 )
{
    _next = nullptr;
    _vboUsage = GL_INVALID_ENUM;
    _totalVertices = 0;
    _meshVBO = 0u;
}

GXMesh::GXMesh ( const GXWChar* fileName )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXMesh" )
    _referenceCount ( 1u ),
    _previous ( nullptr ),
    _vboSize ( 0 ),
    _meshFile ( fileName )
{
    if ( _top )
        _top->_previous = this;

    _next = _top;
    _top = this;

    const GXString extension ( GXGetFileExtension ( fileName ) );
    GXBool result = GX_FALSE;

    if ( extension == "stm" || extension == "STM" )
        result = LoadFromSTM ( fileName );
    else if ( extension == "skm" || extension == "SKM" )
        result = LoadFromSKM ( fileName );
    else if ( extension == "obj" || extension == "OBJ" )
        result = LoadFromOBJ ( fileName );
    else if ( extension == "mesh" || extension == "MESH" )
        result = LoadFromMESH ( fileName );

    if ( result ) return;

    GXLogA ( "GXMesh::GXMesh::Error - Не могу загрузить меш %S.", fileName );
}

GXMesh::~GXMesh ()
{
    if ( _meshVBO == 0u ) return;

    glBindBuffer ( GL_ARRAY_BUFFER, 0u );
    glDeleteBuffers ( 1, &_meshVBO );

    if ( _meshFile.IsEmpty () ) return;

    if ( _top == this )
        _top = _top->_next;

    if ( _previous )
        _previous->_next = _next;

    if ( !_next ) return;

    _next->_previous = _previous;
}

GXBool GXMesh::LoadFromOBJ ( const GXWChar* fileName )
{
    const GXString path ( GXGetFileDirectoryPath ( fileName ) );
    const GXString baseFileName ( GXGetBaseFileName ( fileName ) );

    GXString cacheFileName;
    cacheFileName.Format ( "%s/%s/%s.%s", static_cast<const GXMBChar*> ( path ), CACHE_DIRECTORY_NAME, static_cast<const GXMBChar*> ( baseFileName ), CACHE_FILE_EXTENSION );

    if ( GXDoesFileExist ( cacheFileName ) )
        return LoadFromSTM ( cacheFileName );

    GXOBJPoint* points = nullptr;
    _totalVertices = static_cast<GLsizei> ( GXLoadOBJ ( fileName, points ) );

    GXAABB bounds;

    GXNativeStaticMeshDesc descriptor;
    descriptor._numVertices = static_cast<GXUInt> ( _totalVertices );
    descriptor._numNormals = static_cast<GXUInt> ( _totalVertices );
    descriptor._numTBPairs = static_cast<GXUInt> ( _totalVertices );
    descriptor._numUVs = static_cast<GXUInt> ( _totalVertices );
    descriptor._numElements = 0u;

    GXUInt alpha = _totalVertices * sizeof ( GXVec3 );
    GXUInt betta = _totalVertices * sizeof ( GXVec2 );
    descriptor._vertices = static_cast<GXVec3*> ( Malloc ( alpha ) );
    descriptor._uvs = static_cast<GXVec2*> ( Malloc ( betta ) );
    descriptor._normals = static_cast<GXVec3*> ( Malloc ( alpha ) );
    descriptor._tangents = static_cast<GXVec3*> ( Malloc ( alpha ) );
    descriptor._bitangents = static_cast<GXVec3*> ( Malloc ( alpha ) );
    descriptor._elements = nullptr;

    _vboSize = static_cast<GLsizeiptr> ( alpha + betta + alpha + alpha + alpha );
    GXUByte* cache = static_cast<GXUByte*> ( Malloc ( static_cast<GXUPointer> ( _vboSize ) ) );
    GXUPointer offset = 0u;

    for ( GXUInt i = 0u; i < descriptor._numVertices; ++i )
    {
        bounds.AddVertex ( points[ i ]._vertex );

        descriptor._vertices[ i ] = points[ i ]._vertex;
        descriptor._uvs[ i ] = points[ i ]._uv;
        descriptor._normals[ i ] = points[ i ]._normal;

        const GXUByte* vertices = reinterpret_cast<const GXUByte*> ( &points[ 3 * ( i / 3 ) ]._vertex );
        const GXUByte* uvs = reinterpret_cast<const GXUByte*> ( &points[ 3 * ( i / 3 ) ]._uv );

        GXGetTangentBitangent ( descriptor._tangents[ i ], descriptor._bitangents[ i ], i % 3, vertices, sizeof ( GXOBJPoint ), uvs, sizeof ( GXOBJPoint ) );

        GXVec3* gamma = reinterpret_cast<GXVec3*> ( cache + offset );
        *gamma = descriptor._vertices[ i ];
        offset += sizeof ( GXVec3 );

        GXVec2* zeta = reinterpret_cast<GXVec2*> ( cache + offset );
        *zeta = descriptor._uvs[ i ];
        offset += sizeof ( GXVec2 );

        gamma = reinterpret_cast<GXVec3*> ( cache + offset );
        *gamma = descriptor._normals[ i ];
        offset += sizeof ( GXVec3 );

        gamma = reinterpret_cast<GXVec3*> ( cache + offset );
        *gamma = descriptor._tangents[ i ];
        offset += sizeof ( GXVec3 );

        gamma = reinterpret_cast<GXVec3*> ( cache + offset );
        *gamma = descriptor._bitangents[ i ];
        offset += sizeof ( GXVec3 );
    }

    Free ( points );

    GXString cacheDirectory;
    cacheDirectory.Format ( "%s/%s", static_cast<const GXMBChar*> ( path ), CACHE_DIRECTORY_NAME );

    if ( !GXDoesDirectoryExist ( cacheDirectory ) )
        GXCreateDirectory ( cacheDirectory );

    GXExportNativeStaticMesh ( cacheFileName, descriptor );

    Free ( descriptor._vertices );
    Free ( descriptor._uvs );
    Free ( descriptor._normals );
    Free ( descriptor._tangents );
    Free ( descriptor._bitangents );

    _vboUsage = GL_STATIC_DRAW;

    glGenBuffers ( 1, &_meshVBO );
    glBindBuffer ( GL_ARRAY_BUFFER, _meshVBO );
    // {
        glBufferData ( GL_ARRAY_BUFFER, _vboSize, cache, _vboUsage );
    // }
    glBindBuffer ( GL_ARRAY_BUFFER, 0 );

    Free ( cache );

    return GX_TRUE;
}

GXBool GXMesh::LoadFromSTM ( const GXWChar* fileName )
{
    GXNativeStaticMeshInfo info;
    GXLoadNativeStaticMesh ( fileName, info );

    _totalVertices = static_cast<GLsizei> ( info._numVertices );
    _vboSize = static_cast<GLsizeiptr> ( _totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) ) );
    _vboUsage = GL_STATIC_DRAW;

    glGenBuffers ( 1, &_meshVBO );
    glBindBuffer ( GL_ARRAY_BUFFER, _meshVBO );
    // {
        glBufferData ( GL_ARRAY_BUFFER, _vboSize, info._vboData, _vboUsage );
    // }
    glBindBuffer ( GL_ARRAY_BUFFER, 0 );

    info.Cleanup ();

    return GX_TRUE;
}

GXBool GXMesh::LoadFromSKM ( const GXWChar* fileName )
{
    GXSkeletalMeshData skeletalMeshData;
    GXLoadNativeSkeletalMesh ( skeletalMeshData, fileName );

    _totalVertices = static_cast<GLsizei> ( skeletalMeshData._totalVertices );
    const GXUByte* source = reinterpret_cast<const GXUByte*> ( skeletalMeshData._vboData );
    static const GXUPointer meshVBOStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
    static const GXUPointer skeletalMeshVBOStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );

    _vboSize = static_cast<GLsizeiptr> ( _totalVertices * meshVBOStride );
    GXUByte* destination = static_cast<GXUByte*> ( Malloc ( static_cast<GXUPointer> ( _vboSize ) ) );
    GXUPointer offset = 0u;

    for ( GLsizei i = 0u; i < _totalVertices; ++i )
    {
        // Vertex, UV, normal, tangent, bitangent.
        memcpy ( destination + offset, source, meshVBOStride );

        offset += meshVBOStride;
        source += skeletalMeshVBOStride;
    }

    skeletalMeshData.Cleanup ();

    _vboUsage = GL_STATIC_DRAW;

    glGenBuffers ( 1, &_meshVBO );
    glBindBuffer ( GL_ARRAY_BUFFER, _meshVBO );
    // {
        glBufferData ( GL_ARRAY_BUFFER, _vboSize, destination, _vboUsage );
    // }
    glBindBuffer ( GL_ARRAY_BUFFER, 0u );

    Free ( destination );

    return GX_TRUE;
}

GXBool GXMesh::LoadFromMESH ( const GXWChar* fileName )
{
    GXMeshInfo meshInfo;
    GXLoadNativeMesh ( meshInfo, fileName );

    _totalVertices = static_cast<GLsizei> ( meshInfo.totalVertices );
    _vboSize = static_cast<GLsizeiptr> ( _totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) ) );
    _vboUsage = GL_STATIC_DRAW;

    glGenBuffers ( 1, &_meshVBO );
    glBindBuffer ( GL_ARRAY_BUFFER, _meshVBO );
    // {
        glBufferData ( GL_ARRAY_BUFFER, _vboSize, meshInfo.vboData, _vboUsage );
    // }
    glBindBuffer ( GL_ARRAY_BUFFER, 0u );

    meshInfo.Cleanup ();

    return GX_TRUE;
}

//---------------------------------------------------------------------------------------------------------------------

class GXSkin final : public GXMemoryInspector
{
    friend class GXMeshGeometry;

    private:
        GXUPointerAtomic    _referenceCount;
        GXSkin*             _previous;

        static GXSkin*      _top;
        GXSkin*             _next;

        GXWChar*            _skinFile;

    public:
        GLsizei             _totalVertices;
        GLuint              _skinVBO;

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

GXSkin* GXSkin::_top = nullptr;

GXVoid GXSkin::AddReference ()
{
    ++_referenceCount;
}

GXVoid GXSkin::Release ()
{
    --_referenceCount;

    if ( _referenceCount > static_cast<GXUPointer> ( 0u ) ) return;

    delete this;
}

GXSkin* GXCALL GXSkin::Find ( const GXWChar* fileName )
{
    for ( GXSkin* skin = _top; skin; skin = skin->_next )
    {
        if ( GXWcscmp ( skin->_skinFile, fileName ) != 0 ) continue;

        return skin;
    }

    return nullptr;
}

GXUInt GXCALL GXSkin::GetTotalLoadedSkins ( const GXWChar** lastSkin )
{
    GXUInt total = 0u;

    for ( GXSkin* skin = _top; skin; skin = skin->_next )
        ++total;

    if ( total > 0u )
        *lastSkin = _top->_skinFile;
    else
        *lastSkin = nullptr;

    return total;
}

GXSkin::GXSkin ( const GXWChar* fileName )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXSkin" )
    _referenceCount ( 1u ),
    _previous ( nullptr )
{
    if ( _top )
        _top->_previous = this;

    _next = _top;
    _top = this;

    GXWcsclone ( &_skinFile, fileName );

    const GXString extension ( GXGetFileExtension ( fileName ) );
    GXBool result = GX_FALSE;

    if ( extension == "skm" || extension == "SKM" )
        result = LoadFromSKM ( fileName );
    else if ( extension == "skin" || extension == "SKIN" )
        result = LoadFromSKIN ( fileName );

    if ( result ) return;

    GXLogA ( "GXMesh::GXMesh::Error - Не могу загрузить скин %S.", fileName );
}

GXSkin::~GXSkin ()
{
    free ( _skinFile );

    glBindBuffer ( GL_ARRAY_BUFFER, 0u );
    glDeleteBuffers ( 1, &_skinVBO );

    if ( _top == this )
        _top = _top->_next;
    else
        _previous->_next = _next;

    if ( _next )
        _next->_previous = _previous;
}

GXBool GXSkin::LoadFromSKM ( const GXWChar* fileName )
{
    GXSkeletalMeshData skeletalMeshData;
    GXLoadNativeSkeletalMesh ( skeletalMeshData, fileName );

    _totalVertices = static_cast<GLsizei> ( skeletalMeshData._totalVertices );
    const GXUByte* source = reinterpret_cast<const GXUByte*> ( skeletalMeshData._vboData ) + sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
    static const GXUPointer skinVBOStride = sizeof ( GXVec4 ) + sizeof ( GXVec4 );
    static const GXUPointer skeletalMeshVBOStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 );

    GXUPointer skinVBOSize = _totalVertices * skinVBOStride;
    GXUByte* destination = static_cast<GXUByte*> ( Malloc ( skinVBOSize ) );
    GXUPointer offset = 0u;

    for ( GLsizei i = 0; i < _totalVertices; ++i )
    {
        // Bone index, bone weight
        memcpy ( destination + offset, source, skinVBOStride );

        offset += skinVBOStride;
        source += skeletalMeshVBOStride;
    }

    skeletalMeshData.Cleanup ();

    glGenBuffers ( 1, &_skinVBO );
    glBindBuffer ( GL_ARRAY_BUFFER, _skinVBO );
    // {
        glBufferData ( GL_ARRAY_BUFFER, static_cast<GLsizeiptr> ( skinVBOSize ), destination, GL_STATIC_DRAW );
    // }
    glBindBuffer ( GL_ARRAY_BUFFER, 0u );

    Free ( destination );

    return GX_TRUE;
}

GXBool GXSkin::LoadFromSKIN ( const GXWChar* fileName )
{
    GXSkinInfo skinInfo;
    GXLoadNativeSkin ( skinInfo, fileName );

    _totalVertices = static_cast<GLsizei> ( skinInfo._totalVertices );

    glGenBuffers ( 1, &_skinVBO );
    glBindBuffer ( GL_ARRAY_BUFFER, _skinVBO );
    // {
        glBufferData ( GL_ARRAY_BUFFER, static_cast<GLsizeiptr> ( skinInfo._totalVertices * ( sizeof ( GXVec4 ) + sizeof ( GXVec4 ) ) ), skinInfo._vboData, GL_STATIC_DRAW );
    // }
    glBindBuffer ( GL_ARRAY_BUFFER, 0u );

    skinInfo.Cleanup ();

    return GX_TRUE;
}

//---------------------------------------------------------------------------------------------------------------------

GXMeshGeometry::GXMeshGeometry ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXMeshGeometry" )
    _mesh ( nullptr ),
    _meshVAO ( 0u ),
    _topology ( GL_TRIANGLES ),
    _skin ( nullptr ),
    _skinningSwitchIndex ( 0u ),
    _skinningMaterial ( nullptr )
{
    _pose[ 0u ] = _pose[ 1u ] = nullptr;
    _poseVAO[ 0u ] = _poseVAO[ 1u ] = 0u;
}

GXMeshGeometry::~GXMeshGeometry ()
{
    if ( _mesh )
    {
        glDeleteVertexArrays ( 1, &_meshVAO );
        _mesh->Release ();
    }

    if ( _skin )
        _skin->Release ();

    if ( !_skinningMaterial ) return;

    delete _skinningMaterial;

    glDeleteVertexArrays ( 2, _poseVAO );

    _pose[ 0u ]->Release ();
    _pose[ 1u ]->Release ();
}

GXVoid GXMeshGeometry::Render ()
{
    if ( !_mesh || _mesh->_totalVertices == 0 ) return;

    if ( !_skinningMaterial )
    {
        glBindVertexArray ( _meshVAO );
        glDrawArrays ( _topology, 0, _mesh->_totalVertices );
        glBindVertexArray ( 0u );

        return;
    }

    glBindVertexArray ( _poseVAO[ _skinningSwitchIndex ] );
    glDrawArrays ( _topology, 0, _skin->_totalVertices );
    glBindVertexArray ( 0u );

    _skinningSwitchIndex = _skinningSwitchIndex == 0u ? 1u : 0u;
}

GXVoid GXMeshGeometry::SetBoundsLocal ( const GXAABB& bounds )
{
    this->_boundsLocal = bounds;
}

const GXAABB& GXMeshGeometry::GetBoundsLocal () const
{
    return _boundsLocal;
}

GXVoid GXMeshGeometry::SetTotalVertices ( GLsizei totalVertices )
{
    if ( !_mesh || !_mesh->GetMeshFileName () )
    {
        // Procedure generated mesh.

        if ( !_mesh )
        {
            GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMesh" );
            _mesh = new GXMesh ();
        }

        _mesh->_totalVertices = totalVertices;
        return;
    }

    if ( _mesh )
        _mesh->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMesh" );
    _mesh = new GXMesh ();
    _mesh->_totalVertices = totalVertices;
}

GXVoid GXMeshGeometry::FillVertexBuffer ( const GXVoid* data, GLsizeiptr size, GLenum usage )
{
    if ( !_mesh || !_mesh->GetMeshFileName () )
    {
        // Procedure generated mesh.

        if ( !_mesh )
        {
            GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMesh" );
            _mesh = new GXMesh ();
        }

        _mesh->FillVBO ( data, size, usage );
        return;
    }

    if ( _mesh )
        _mesh->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMesh" );
    _mesh = new GXMesh ();
    _mesh->FillVBO ( data, size, usage );
}

GXVoid GXMeshGeometry::SetBufferStream ( eGXMeshStreamIndex streamIndex, GLint numElements, GLenum elementType, GLsizei stride, const GLvoid* offset )
{
    if ( _meshVAO == 0u )
        glGenVertexArrays ( 1, &_meshVAO );

    if ( !_mesh || !_mesh->GetMeshFileName () )
    {
        // Procedure generated mesh.

        if ( !_mesh )
        {
            GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMesh" );
            _mesh = new GXMesh ();
        }

        glBindVertexArray ( _meshVAO );
        // {
            if ( _mesh->_meshVBO == 0u )
                glGenBuffers ( 1, &_mesh->_meshVBO );

            glBindBuffer ( GL_ARRAY_BUFFER, _mesh->_meshVBO );
            glEnableVertexAttribArray ( static_cast<GLuint> ( streamIndex ) );
            glVertexAttribPointer ( static_cast<GLuint> ( streamIndex ), numElements, elementType, GL_FALSE, stride, offset );
        // }
        glBindVertexArray ( 0u );

        return;
    }

    if ( _mesh )
        _mesh->Release ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMesh" );
    _mesh = new GXMesh ();

    glBindVertexArray ( _meshVAO );
    // {
        glBindBuffer ( GL_ARRAY_BUFFER, _mesh->_meshVBO );
        glEnableVertexAttribArray ( static_cast<GLuint> ( streamIndex ) );
        glVertexAttribPointer ( static_cast<GLuint> ( streamIndex ), numElements, elementType, GL_FALSE, stride, offset );
    // }
    glBindVertexArray ( 0u );
}

GXVoid GXMeshGeometry::SetTopology ( GLenum newTopology )
{
    _topology = newTopology;
}

GXVoid GXMeshGeometry::UpdatePose ( const GXSkeleton &skeleton )
{
    if ( !_skinningMaterial ) return;

    _skinningMaterial->SetSkeleton ( skeleton );
    _skinningMaterial->Bind ( GXTransform::GetNullTransform () );

    glDisable ( GL_DEPTH_TEST );
    glEnable ( GL_RASTERIZER_DISCARD );
    glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0u, _pose[ _skinningSwitchIndex ]->_meshVBO );
    glBeginTransformFeedback ( GL_POINTS );

    glBindVertexArray ( _meshVAO );

    glDrawArrays ( GL_POINTS, 0, _skin->_totalVertices );

    glBindVertexArray ( 0u );

    glEndTransformFeedback ();
    glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0u, 0u );
    glDisable ( GL_RASTERIZER_DISCARD );
    glEnable ( GL_DEPTH_TEST );

    _skinningMaterial->Unbind ();
}

GXBool GXMeshGeometry::LoadMesh ( const GXWChar* fileName )
{
    if ( _mesh )
        _mesh->Release ();

    _mesh = GXMesh::Find ( fileName );

    if ( _mesh )
    {
        _mesh->AddReference ();
    }
    else
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMesh" );
        _mesh = new GXMesh ( fileName );
    }

    UpdateGraphicResources ();

    return GX_TRUE;
}

GXBool GXMeshGeometry::LoadSkin ( const GXWChar* fileName )
{
    if ( _skin )
        _skin->Release ();

    _skin = GXSkin::Find ( fileName );

    if ( _skin )
    {
        _skin->AddReference ();
    }
    else
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXSkin" );
        _skin = new GXSkin ( fileName );
    }

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
    if ( !_mesh || !_mesh->GetMeshFileName () || !_skin || _mesh->_totalVertices != _skin->_totalVertices )
    {
        if ( _mesh && _meshVAO == 0u )
        {
            glGenVertexArrays ( 1, &_meshVAO );
            glBindVertexArray ( _meshVAO );
            // {
                GLsizei meshStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
                GXUPointer offset = 0u;

                glBindBuffer ( GL_ARRAY_BUFFER, _mesh->_meshVBO );

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

        if ( !_skinningMaterial ) return;

        delete _skinningMaterial;

        glBindVertexArray ( 0u );
        glDeleteVertexArrays ( 2, _poseVAO );

        _pose[ 0u ]->Release ();
        _pose[ 1u ]->Release ();

        _pose[ 0u ] = _pose[ 1u ] = nullptr;

        return;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXSkinningMaterial" );
    _skinningMaterial = new GXSkinningMaterial ();
    GLsizei meshStride = sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 );
    GLsizeiptr poseVBOSize = static_cast<GLsizeiptr> ( meshStride * _skin->_totalVertices );

    glGenVertexArrays ( 2, _poseVAO );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMesh" );
    _pose[ 0u ] = new GXMesh ();
    _pose[ 0u ]->FillVBO ( nullptr, poseVBOSize, GL_DYNAMIC_DRAW );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXMesh" );
    _pose[ 1u ] = new GXMesh ();
    _pose[ 1u ]->FillVBO ( nullptr, poseVBOSize, GL_DYNAMIC_DRAW );

    GXUPointer offset = 0u;

    glBindVertexArray ( _poseVAO[ 0 ] );
    // {
        glBindBuffer ( GL_ARRAY_BUFFER, _pose[ 0 ]->_meshVBO );

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

        glBindBuffer ( GL_ARRAY_BUFFER, _pose[ 1u ]->_meshVBO );

        glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ) );
        glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ), 3, GL_FLOAT, GL_FALSE, meshStride, static_cast<const GLvoid*> ( 0u ) );
    // }
    glBindVertexArray ( 0u );

    offset = 0u;

    glBindVertexArray ( _poseVAO[ 1u ] );
    // {
        glBindBuffer ( GL_ARRAY_BUFFER, _pose[ 1u ]->_meshVBO );

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

        glBindBuffer ( GL_ARRAY_BUFFER, _pose[ 0u ]->_meshVBO );

        glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ) );
        glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::LastFrameVertex ), 3, GL_FLOAT, GL_FALSE, meshStride, static_cast<const GLvoid*> ( 0u ) );
    // }
    glBindVertexArray ( 0u );

    offset = 0u;
    GLsizei skinStride = sizeof ( GXVec4 ) + sizeof ( GXVec4 );

    glBindVertexArray ( _meshVAO );
    // {
        glBindBuffer ( GL_ARRAY_BUFFER, _mesh->_meshVBO );

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

        glBindBuffer ( GL_ARRAY_BUFFER, _skin->_skinVBO );

        offset = 0u;

        glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::SkinningIndices ) );
        glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::SkinningIndices ), 4, GL_FLOAT, GL_FALSE, skinStride, reinterpret_cast<const GLvoid*> ( offset ) );
        offset += sizeof ( GXVec4 );

        glEnableVertexAttribArray ( static_cast<GLuint> ( eGXMeshStreamIndex::SkinningWeights ) );
        glVertexAttribPointer ( static_cast<GLuint> ( eGXMeshStreamIndex::SkinningWeights ), 4, GL_FLOAT, GL_FALSE, skinStride, reinterpret_cast<const GLvoid*> ( offset ) );
    // }
    glBindVertexArray ( 0u );
}
