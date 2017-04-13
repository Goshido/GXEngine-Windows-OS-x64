//version 1.0

#ifndef GX_TRANSFORM
#define GX_TRANSFORM


#include <GXCommon/GXMath.h>


class GXTransform
{
	protected:
		GXVec3				currentLocation;
		GXVec3				currentScale;
		GXMat4				currentRotationMatrix;
		GXMat4				currentModelMatrix;
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

		const GXMat4& GetCurrentModelMatrix () const;
		const GXMat4& GetLastFrameModelMatrix () const;
		GXVoid UpdateLastFrameModelMatrix ();

		static const GXTransform& GetNullTransform ();

	protected:
		virtual GXVoid TransformUpdated ();
};


#endif //GX_TRANSFORM
