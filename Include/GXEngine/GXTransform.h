// version 1.5

#ifndef GX_TRANSFORM
#define GX_TRANSFORM


#include <GXCommon/GXMath.h>


class GXTransform
{
    protected:
        GXVec3                  _currentFrameLocation;
        GXVec3                  _currentFrameScale;
        GXMat4                  _currentFrameRotationMatrix;
        GXMat4                  _currentFrameModelMatrix;
        GXMat4                  _lastFrameModelMatrix;

    private:
        static GXTransform      _nullTransform;

    public:
        GXTransform ();
        GXTransform ( const GXTransform &other );
        virtual ~GXTransform ();

        GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
        GXVoid SetLocation ( const GXVec3 &loc );

        GXVoid SetRotation ( const GXEuler &rot_rad );
        GXVoid SetRotation ( const GXMat4 &rot );
        GXVoid SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );
        GXVoid SetRotation ( const GXQuat &quaternion );

        GXVoid SetScale ( GXFloat x, GXFloat y, GXFloat z );
        GXVoid SetScale ( const GXVec3 &scale );

        GXVoid GetLocation ( GXVec3 &loc ) const;
        const GXVec3& GetLocation () const;

        GXVoid GetRotation ( GXMat4 &rot ) const;
        GXVoid GetRotation ( GXQuat &rot ) const;
        const GXMat4& GetRotation () const;

        GXVoid GetScale ( GXVec3 &scale ) const;
        const GXVec3& GetScale () const;

        const GXMat4& GetCurrentFrameModelMatrix () const;
        GXVoid SetCurrentFrameModelMatrix ( const GXMat4 &matrix );

        const GXMat4& GetLastFrameModelMatrix () const;
        GXVoid UpdateLastFrameModelMatrix ();

        static const GXTransform& GetNullTransform ();

        GXTransform& operator = ( const GXTransform &other );

    protected:
        virtual GXVoid TransformUpdated ();
};


#endif // GX_TRANSFORM
