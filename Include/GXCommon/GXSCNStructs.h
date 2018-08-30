// version 1.3

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


struct GXSceneObjectHeader final
{
	GXUShort	objectType;
	GXUInt		objectName;
};

struct GXSceneMeshHeader final
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

struct GXSceneDirectedLightHeader final
{
	GXVec3		color;
	GXFloat		intensity;
	GXBool		genShadows;

	GXMat4		rotation;
};

struct GXSceneBulpHeader final
{
	GXVec3		color;
	GXFloat		intensity;

	GXFloat		influence;

	GXVec3		location;
};

struct GXSceneSpotLightHeader final
{
	GXVec3		color;
	GXFloat		intensity;
	GXBool		genShadows;

	GXFloat		influence;
	GXFloat		cutoff_rad;	

	GXMat4		rotation;
	GXVec3		location;
};

struct GXScenePhysicalBoxHeader final
{
	GXFloat		width;
	GXFloat		height;
	GXFloat		length;

	GXMat4		rotation;
	GXVec3		location;
};

struct GXScenePhysicalSphereHeader final
{
	GXFloat		radius;
	GXVec3		location;
};

struct GXScenePhysicalCapsuleHeader final
{
	GXFloat		radius;
	GXFloat		height;

	GXMat4		rotation;
	GXVec3		location;
};

struct GXScenePhysicalMeshHeader final
{
	GXMat4		rotation;
	GXVec3		scale;
	GXVec3		location;

	GXUInt		bakeFile;
};


#endif // GX_SCN_STRUCTS
