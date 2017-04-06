//version 1.0

#ifndef GX_RENDERABLE
#define GX_RENDERABLE


#include <GXCommon/GXMath.h>


class GXRenderable
{
	protected:
		GXVec3				location;
		GXVec3				scale;
		GXMat4				rot_mat;
		GXMat4				mod_mat;

	public:
		GXRenderable ();
		virtual ~GXRenderable ();

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

		virtual GXVoid Render () = 0;

	protected:
		virtual GXVoid InitGraphicResources () = 0;
		virtual GXVoid UpdateBounds () = 0;
};

#endif //GX_RENDERABLE
