//version 1.0

#ifndef GX_TRANSFORM
#define GX_TRANSFORM


#include <GXCommon/GXMath.h>


class GXTransform
{
	protected:
		GXVec3				location;
		GXVec3				scale;
		GXMat4				rot_mat;
		GXMat4				mod_mat;

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

		const GXMat4& GetModelMatrix () const;

	protected:
		virtual GXVoid TransformUpdated () = 0;
};


#endif //GX_TRANSFORM
