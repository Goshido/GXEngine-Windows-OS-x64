// version 1.6

#ifndef GX_SKELETON
#define GX_SKELETON


#include <GXEngine/GXAnimationSolver.h>
#include <GXCommon/GXNativeSkeletalMesh.h>


class GXSkeleton final
{
    private:
        GXUShort        _totalBones;
        GXUTF8*         _boneNames;
        GXShort*        _parentBoneIndices;

        GXBoneJoint*    _tempPoseLocal;
        GXBoneJoint*    _tempPoseGlobal;

        GXBoneJoint*    _referencePose;
        GXBoneJoint*    _inverseBindTransform;
        GXBoneJoint*    _skinTransform;

    public:
        GXSkeleton ();
        ~GXSkeleton ();

        GXVoid LoadFromSkm ( const GXWChar* fileName );
        GXVoid UpdatePose ( GXAnimationSolver &solver, GXFloat deltaTime );
        const GXBoneJoint* GetSkinTransform () const;

        const GXBoneJoint& GetBoneTransformWorld ( GXUShort boneIndex ) const;
        const GXBoneJoint& GetParentBoneTransformWorld ( GXUShort boneIndex ) const;

        GXUShort GetTotalBones () const;

    private:
        GXVoid CalculatePose ();

        GXSkeleton ( const GXSkeleton &other ) = delete;
        GXSkeleton& operator = ( const GXSkeleton &other ) = delete;
};


#endif // GX_SKELETON
