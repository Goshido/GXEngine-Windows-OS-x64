//version 1.2

#ifndef GX_SCN_STRUCTS
#define GX_SCN_STRUCTS


#include "GXTypes.h"
#include "GXMath.h"


#define	GX_SCENE_OBJ_TYPE_MESH				0
#define GX_SCENE_OBJ_TYPE_DIR_LIGHT			1
#define GX_SCENE_OBJ_TYPE_BULP				2
#define GX_SCENE_OBJ_TYPE_SPOT_LIGHT		3
#define GX_SCENE_OBJ_TYPE_PHYSICAL_BOX		4
#define GX_SCENE_OBJ_TYPE_PHYSICAL_SPHERE	5
#define GX_SCENE_OBJ_TYPE_PHYSICAL_CAPSULE	6
#define GX_SCENE_OBJ_TYPE_PHYSICAL_MESH		7


struct GXSceneObjectHeader
{
	GXUShort	objectType;
	GXUInt		objectName;
};

struct GXSceneMeshHeader
{
	GXBool	castShadows;
	GXBool	twoSided;

	GXInt	meshClassID;

	GXUInt	meshFile;
	GXUInt	meshCache;
	GXUInt	materialFile;

	GXMat4	rotation;
	GXVec3	scale;
	GXVec3	location;
};

struct GXSceneDirectedLightHeader
{
	GXVec3		color;
	GXFloat		intensity;
	GXBool		genShadows;

	GXMat4		rotation;
};

struct GXSceneBulpHeader
{
	GXVec3		color;
	GXFloat		intensity;

	GXFloat		influence;

	GXVec3		location;
};

struct GXSceneSpotLightHeader
{
	GXVec3		color;
	GXFloat		intensity;
	GXBool		genShadows;

	GXFloat		influence;
	GXFloat		cutoff_rad;	

	GXMat4		rotation;
	GXVec3		location;
};

struct GXScenePhysicalBoxHeader
{
	GXFloat		width;
	GXFloat		height;
	GXFloat		length;

	GXMat4		rotation;
	GXVec3		location;
};

struct GXScenePhysicalSphereHeader
{
	GXFloat		radius;
	GXVec3		location;
};

struct GXScenePhysicalCapsuleHeader
{
	GXFloat		radius;
	GXFloat		height;

	GXMat4		rotation;
	GXVec3		location;
};

struct GXScenePhysicalMeshHeader
{
	GXMat4		rotation;
	GXVec3		scale;
	GXVec3		location;

	GXUInt		bakeFile;
};


#endif //GX_SCN_STRUCTS