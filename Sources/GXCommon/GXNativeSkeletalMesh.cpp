// version 1.9

#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXCommon/GXFile.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>


#define UNKNOWN_BONE_INDEX      0xFFFE
#define DEFAULT_FPS             60.0f

//---------------------------------------------------------------------------------------------------------------------

class GXNativeSkeletalMeshLoaderMemoryInspector final : public GXMemoryInspector
{
public:
    GXNativeSkeletalMeshLoaderMemoryInspector ();
    ~GXNativeSkeletalMeshLoaderMemoryInspector () override;

private:
    GXNativeSkeletalMeshLoaderMemoryInspector ( const GXNativeSkeletalMeshLoaderMemoryInspector &other );
    GXNativeSkeletalMeshLoaderMemoryInspector& operator = ( const GXNativeSkeletalMeshLoaderMemoryInspector &other );
};

GXNativeSkeletalMeshLoaderMemoryInspector::GXNativeSkeletalMeshLoaderMemoryInspector ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXNativeSkeletalMeshLoaderMemoryInspector" )
{
    // NOTHING
}

GXNativeSkeletalMeshLoaderMemoryInspector::~GXNativeSkeletalMeshLoaderMemoryInspector ()
{
    // NOTHING
}

static GXNativeSkeletalMeshLoaderMemoryInspector gx_NativeSkeletalMeshLoaderMemoryInspector;

//---------------------------------------------------------------------------------------------------------------------

GXSkeletalMeshData::GXSkeletalMeshData ():
    _totalVertices ( 0u ),
    _vboData ( nullptr ),
    _totalBones ( 0u ),
    _boneNames ( nullptr ),
    _parentBoneIndices ( nullptr ),
    _referencePose ( nullptr ),
    _inverseBindTransform ( nullptr )
{
    // NOTHING
}

GXVoid GXSkeletalMeshData::Cleanup ()
{
    _totalVertices = 0u;
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_vboData ) );

    _totalBones = 0u;
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_boneNames ) );
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_parentBoneIndices ) );
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_referencePose ) );
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_inverseBindTransform ) );
}

//-------------------------------------------------------------------------------------------------------------

GXAnimationInfo::GXAnimationInfo ():
    _totalBones ( 0u ),
    _boneNames ( nullptr ),
    _fps ( DEFAULT_FPS ),
    _totalFrames ( 0u ),
    _keys ( nullptr )
{
    // NOTHING
}

GXVoid GXAnimationInfo::Cleanup ()
{
    _totalBones = 0u;
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_boneNames ) );

    _fps = DEFAULT_FPS;
    _totalFrames = 0u;
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &_keys ) );
}

//-------------------------------------------------------------------------------------------------------------

GXVoid GXCALL GXLoadNativeSkeletalMesh ( GXSkeletalMeshData &info, const GXWChar* fileName )
{
    GXUByte* data;
    GXUPointer fileSize;

    GXFile file ( fileName );

    if ( !file.LoadContent ( data, fileSize, eGXFileContentOwner::GXFile, GX_TRUE ) ) return;

    GXUPointer size = 0u;
    const GXNativeSkeletalMeshHeader* h = reinterpret_cast<const GXNativeSkeletalMeshHeader*> ( data );

    info._totalVertices = h->_totalVertices;

    size = info._totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
    info._vboData = static_cast<GXFloat*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._vboData, data + h->_vboOffset, size );

    info._totalBones = h->_totalBones;

    size = info._totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
    info._boneNames = static_cast<GXUTF8*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._boneNames, data + h->_boneNamesOffset, size );

    size = info._totalBones * sizeof ( GXShort );
    info._parentBoneIndices = static_cast<GXUShort*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._parentBoneIndices, data + h->_parentBoneIndicesOffset, size );

    size = info._totalBones * sizeof ( GXBoneJoint );
    info._referencePose = static_cast<GXBoneJoint*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._referencePose, data + h->_referensePoseOffset, size );

    info._inverseBindTransform = static_cast<GXBoneJoint*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._inverseBindTransform, data + h->_inverseBindTransformOffset, size );
}

GXVoid GXCALL GXLoadNativeAnimation ( GXAnimationInfo &info, const GXWChar* fileName )
{
    GXUByte* data;
    GXUPointer size;

    GXFile file ( fileName );

    if ( !file.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_TRUE ) ) return;

    const GXNativeAnimationHeader* h = reinterpret_cast<const GXNativeAnimationHeader*> ( data );

    info._fps = h->_fps;
    info._totalBones = h->_totalBones;
    info._totalFrames = h->_totalFrames;
    
    size = h->_totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
    info._boneNames = static_cast<GXUTF8*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._boneNames, data + h->_boneNamesOffset, size );

    size = h->_totalBones * h->_totalFrames * sizeof ( GXBoneJoint );
    info._keys = static_cast<GXBoneJoint*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info._keys, data + h->_keysOffset, size );
}
