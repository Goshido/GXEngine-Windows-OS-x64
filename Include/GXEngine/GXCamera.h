//version 1.9

#ifndef GX_CAMERA
#define GX_CAMERA


#include <GXCommon/GXMath.h>


class GXCamera
{
	private:
		static GXCamera*		activeCamera;

	protected:
		GXFloat					znear;
		GXFloat					zfar;

		GXMat4					trans_mat;
		GXMat4					rot_mat;
		GXMat4					mod_mat;
		GXMat4					view_mat;
		GXMat4					proj_mat;
		GXMat4					view_proj_mat;

		GXProjectionClipPlanes	clipPlanesWorld;

	public:
		GXCamera ();
		virtual ~GXCamera ();

		const GXMat4& GetViewProjectionMatrix () const;
		const GXMat4& GetProjectionMatrix () const;
		const GXMat4& GetModelMatrix () const;
		const GXMat4& GetViewMatrix () const;

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetLocation ( const GXVec3 &location );

		GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		GXVoid SetRotation ( const GXMat4 &rotation );
		GXVoid SetRotation ( const GXQuat &rotation );

		GXVoid SetModelMatrix ( const GXMat4 &matrix );

		GXVoid GetLocation ( GXVec3& out ) const;

		GXVoid GetRotation ( GXMat4 &out ) const;
		GXVoid GetRotation ( GXQuat &out ) const;

		const GXProjectionClipPlanes& GetClipPlanesWorld ();
		GXBool IsObjectVisible ( const GXAABB objectBoundsWorld );

		virtual GXVoid SetZnear ( GXFloat znear ) = 0;
		virtual GXVoid SetZfar	( GXFloat zfar ) = 0;

		virtual GXFloat GetZnear () const = 0;
		virtual GXFloat GetZfar () const = 0;

		static GXCamera* GXCALL GetActiveCamera ();
		static GXVoid GXCALL SetActiveCamera ( GXCamera* camera );

	protected:
		GXVoid UpdateClipPlanes ();
};


#endif //GX_CAMERA