// version 1.5

#ifndef GX_SCN_STRUCTS
#define GX_SCN_STRUCTS


#include "GXTypes.h"
#include "GXMath.h"


#define GX_SCENE_OBJ_TYPE_MESH                  0
#define GX_SCENE_OBJ_TYPE_DIR_LIGHT             1
#define GX_SCENE_OBJ_TYPE_BULP                  2
#define GX_SCENE_OBJ_TYPE_SPOT_LIGHT            3
#define GX_SCENE_OBJ_TYPE_PHYSICAL_BOX          4
#define GX_SCENE_OBJ_TYPE_PHYSICAL_SPHERE       5
#define GX_SCENE_OBJ_TYPE_PHYSICAL_CAPSULE      6
#define GX_SCENE_OBJ_TYPE_PHYSICAL_MESH         7


struct GXSceneObjectHeader final
{
    GXUShort    _objectType;
    GXUInt      _objectName;
};

struct GXSceneMeshHeader final
{
    GXBool      _castShadows;
    GXBool      _twoSided;

    GXInt       _meshClassID;

    GXUInt      _meshFile;
    GXUInt      _meshCache;
    GXUInt      _materialFile;

    GXMat4      _rotation;
    GXVec3      _scale;
    GXVec3      _location;
};

struct GXSceneDirectedLightHeader final
{
    GXVec3      _color;
    GXFloat     _intensity;
    GXBool      _genShadows;

    GXMat4      _rotation;
};

struct GXSceneBulpHeader final
{
    GXVec3      _color;
    GXFloat     _intensity;

    GXFloat     _influence;

    GXVec3      _location;
};

struct GXSceneSpotLightHeader final
{
    GXVec3      _color;
    GXFloat     _intensity;
    GXBool      _genShadows;

    GXFloat     _influence;
    GXFloat     _cutoff_rad;

    GXMat4      _rotation;
    GXVec3      _location;
};

struct GXScenePhysicalBoxHeader final
{
    GXFloat     _width;
    GXFloat     _height;
    GXFloat     _length;

    GXMat4      _rotation;
    GXVec3      _location;
};

struct GXScenePhysicalSphereHeader final
{
    GXFloat     _radius;
    GXVec3      _location;
};

struct GXScenePhysicalCapsuleHeader final
{
    GXFloat     _radius;
    GXFloat     _height;

    GXMat4      _rotation;
    GXVec3      _location;
};

struct GXScenePhysicalMeshHeader final
{
    GXMat4      _rotation;
    GXVec3      _scale;
    GXVec3      _location;

    GXUInt      _bakeFile;
};


#endif // GX_SCN_STRUCTS
