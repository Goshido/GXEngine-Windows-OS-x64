//version 1.1

#ifndef GX_TRANSFORM
#define GX_TRANSFORM


#include <GXCommon/GXMath.h>


class GXTransform
{
	protected:
		GXVec3				currentFrameLocation;
		GXVec3				currentFrameScale;
		GXMat4				currentFrameRotationMatrix;
		GXMat4				currentFrameModelMatrix;
		GXMat4				lastFrameModelMatrix;

	private:
		static GXTransform	nullTransform;

	public:
		GXTransform ();
		virtual ~GXTransform ();

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetLocation ( const GXVec3 &loc );

		GXVoid SetRotation ( const GXVec3 &rot_rad );
		GXVoid SetRotation ( const GXMat4 &rot );
		GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		GXVoid SetRotation ( const GXQuat &quaternion );

		GXVoid SetScale ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetScale ( const GXVec3 &scale );

		GXVoid GetLocation ( GXVec3 &loc ) const;

		GXVoid GetRotation ( GXMat4 &rot ) const;
		GXVoid GetRotation ( GXQuat &rot ) const;

		GXVoid GetScale ( GXVec3 &scale ) const;

		const GXMat4& GetCurrentFrameModelMatrix () const;
		const GXMat4& GetLastFrameModelMatrix () const;
		GXVoid UpdateLastFrameModelMatrix ();

		static const GXTransform& GetNullTransform ();

	protected:
		virtual GXVoid TransformUpdated ();
};


#endif //GX_TRANSFORM
