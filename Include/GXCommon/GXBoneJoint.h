//version 1.0

#ifndef GX_BONE_JOINT
#define GX_BONE_JOINT


#include "GXMath.h"


#define GX_BONE_NAME_SIZE				64
#define GX_FLOATS_PER_BONE				7
#define GX_MAXIMUM_BONES_PER_MESH		80

#define GX_UNKNOWN_BONE_INDEX			0xFFFE
#define GX_ROOT_BONE_PARENT_INDEX		0xFFFF


struct GXBoneJoint
{
	GXQuat	_rotation;
	GXVec3	_location;
};


#endif //GX_BONE_JOINT
