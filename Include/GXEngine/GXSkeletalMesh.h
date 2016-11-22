//version 1.1

#ifndef GX_SKELETAL_MESH
#define GX_SKELETAL_MESH


#include "GXMesh.h"
#include "GXCamera.h"


#define SKIN_INDEX			5
#define SKIN_WEIGHT			6
#define VAO_SKELETON_BONES	0
#define VAO_SKELETON_JOINTS	1
#define MAX_BONES			80


union GXLightJoint
{
	struct
	{
		GXQuat	Orientation;
		GXVec3	Position;
	};
	GXFloat v[ 7 ];
};

union GXLightBone
{
	struct
	{
		GXChar			Name[ 64 ];
		GXUInt			ParentIndex;	// 0/NULL в случае корневой кости
		GXLightJoint	BonePos;		// Ссылка на положение в 3D пространстве
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + sizeof ( GXUInt ) + sizeof ( GXLightJoint ) ];
};

union GXVBOSkinStruct
{
	struct
	{
		GXVec4 index;
		GXVec4 weight;
	};
	GXUChar v[ 2 * sizeof ( GXVec4 ) ];
};

union GXSkeletalMeshInfo
{
	struct
	{
		GXUShort			totalVBOxyz;
		GXFloat*			VBOxyz;

		GXUShort			totalVBOskin;
		GXVBOSkinStruct*	VBOskin;

		GXUShort			totalVBOuv;
		GXFloat*			VBOuv;

		GXUShort			totalelements;
		GXUShort*			ElementArray;

		GXUShort			totalBones;
		GXLightBone*		Bones;
	};
	GXUChar v[ 5 * ( sizeof ( GXUShort ) + sizeof ( GXVoid* ) ) ];
};

//------------------------------------------------------------------------------------

struct GXBoneNames
{
	GXChar Name[ 64 ];
};

union GXAnimSeq
{
	struct
	{
		GXChar		Name[ 64 ];
		GXFloat		FPS;
		GXInt		FirstRawFrame;
		GXInt		NumRawFrames;
	};
	GXUChar v[ 64 * sizeof ( GXChar ) + sizeof ( GXFloat ) + 2 * sizeof ( GXInt ) ];
};

union VQuatAnimKey
{
	struct
	{
		GXVec3	Position;           // Относительно родителя
		GXQuat	Orientation;        // Относительно родителя
		GXFloat	Time;				// Продолжительность до следующего кадра ( последний ключ прыгает на первый...)
	};
	GXFloat v[ 3 + 4 + 1 ];
};

union GXSkeletalMeshAnimationInfo
{
	struct
	{
		GXUInt			totalrawkeys;
		VQuatAnimKey*	RawKeys;

		GXUInt			totalbones;
		GXBoneNames*	Bones;

		GXUInt			totalanimations;
		GXAnimSeq*		Animations;
	};
	GXUChar v[ 3 * ( sizeof ( GXUInt ) + sizeof ( GXVoid* ) ) ];
};

//------------------------------------------------------------------------------------

class GXSkeletalMesh : public GXMesh
{
	protected:
		GLuint							VAOskeleton[ 2 ];
		GLuint							VBOskeleton;
		GLuint							VBOxyz;
		GLuint							VBOuv;
		GLuint							VBOskin;
		GLuint							element_array_buffer;
		GLuint							skel_mod_view_proj_mat_Location;
		GXSkeletalMeshInfo				skminfo;
		GXSkeletalMeshAnimationInfo*	skmaniminfo;
		GXBool							bShowSkeleton;
		GXBool							bNeedUpdateSkeleton;
	
	public:
		GXSkeletalMesh ();
		GXVoid ShowSkeleton ( GXBool bShow );

		virtual	GXVoid GetBoneGlobalLocation ( GXVec3 &out, const GXChar* bone ) = 0;
		virtual	GXVoid GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXChar* bone ) = 0;

	protected:
		virtual GXVoid InitReferencePose () = 0;
		virtual GXVoid InitPose ( GXUInt anim_number, GXUInt frame_number ) = 0;
		virtual GXVoid InitDrawableSkeleton () = 0;
		virtual GXVoid DrawSkeleton ( GXCamera* cam ) = 0;
};


#endif //GX_SKELETAL_MESH
