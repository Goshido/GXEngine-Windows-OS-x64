// version 1.7

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
    totalVertices ( 0u ),
    vboData ( nullptr ),
    totalBones ( 0u ),
    boneNames ( nullptr ),
    parentBoneIndices ( nullptr ),
    referencePose ( nullptr ),
    inverseBindTransform ( nullptr )
{
    // NOTHING
}

GXVoid GXSkeletalMeshData::Cleanup ()
{
    totalVertices = 0u;
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &vboData ) );

    totalBones = 0u;
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &boneNames ) );
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &parentBoneIndices ) );
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &referencePose ) );
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &inverseBindTransform ) );
}

//-------------------------------------------------------------------------------------------------------------

GXAnimationInfo::GXAnimationInfo ():
    totalBones ( 0u ),
    boneNames ( nullptr ),
    fps ( DEFAULT_FPS ),
    totalFrames ( 0u ),
    keys ( nullptr )
{
    // NOTHING
}

GXVoid GXAnimationInfo::Cleanup ()
{
    totalBones = 0u;
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &boneNames ) );

    fps = DEFAULT_FPS;
    totalFrames = 0u;
    gx_NativeSkeletalMeshLoaderMemoryInspector.SafeFree ( reinterpret_cast<GXVoid**> ( &keys ) );
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

    info.totalVertices = h->totalVertices;

    size = info.totalVertices * ( sizeof ( GXVec3 ) + sizeof ( GXVec2 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec3 ) + sizeof ( GXVec4 ) + sizeof ( GXVec4 ) );
    info.vboData = static_cast<GXFloat*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.vboData, data + h->vboOffset, size );

    info.totalBones = h->totalBones;

    size = info.totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
    info.boneNames = static_cast<GXUTF8*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.boneNames, data + h->boneNamesOffset, size );

    size = info.totalBones * sizeof ( GXShort );
    info.parentBoneIndices = static_cast<GXUShort*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.parentBoneIndices, data + h->parentBoneIndicesOffset, size );

    size = info.totalBones * sizeof ( GXBoneJoint );
    info.referencePose = static_cast<GXBoneJoint*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.referencePose, data + h->referensePoseOffset, size );

    info.inverseBindTransform = static_cast<GXBoneJoint*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.inverseBindTransform, data + h->inverseBindTransformOffset, size );
}

GXVoid GXCALL GXLoadNativeAnimation ( GXAnimationInfo &info, const GXWChar* fileName )
{
    GXUByte* data;
    GXUPointer size;

    GXFile file ( fileName );

    if ( !file.LoadContent ( data, size, eGXFileContentOwner::GXFile, GX_TRUE ) ) return;

    const GXNativeAnimationHeader* h = reinterpret_cast<const GXNativeAnimationHeader*> ( data );

    info.fps = h->fps;
    info.totalBones = h->totalBones;
    info.totalFrames = h->totalFrames;
    
    size = h->totalBones * GX_BONE_NAME_SIZE * sizeof ( GXUTF8 );
    info.boneNames = static_cast<GXUTF8*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.boneNames, data + h->boneNamesOffset, size );

    size = h->totalBones * h->totalFrames * sizeof ( GXBoneJoint );
    info.keys = static_cast<GXBoneJoint*> ( gx_NativeSkeletalMeshLoaderMemoryInspector.Malloc ( size ) );
    memcpy ( info.keys, data + h->keysOffset, size );
}
