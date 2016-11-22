//version 1.8

#ifndef GX_CAMERA
#define GX_CAMERA


#include <GXCommon/GXMath.h>


class GXCamera
{
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

		const GXMat4& GetViewProjectionMatrix ();
		const GXMat4& GetProjectionMatrix ();
		const GXMat4& GetModelMatrix ();
		const GXMat4& GetViewMatrix ();

		const GXMat4* GetViewProjectionMatrixPtr ();
		const GXMat4* GetProjectionMatrixPtr ();
		const GXMat4* GetModelMatrixPtr ();
		const GXMat4* GetViewMatrixPtr ();

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetLocation ( const GXVec3 &location );

		GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		GXVoid SetRotation ( const GXMat4 &rotation );
		GXVoid SetRotation ( const GXQuat &rotation );

		GXVoid SetModelMatrix ( const GXMat4 &matrix );

		GXVoid GetLocation ( GXVec3& out );
		GXVec3* GetLocation ();

		GXVoid GetRotation ( GXMat4 &out );
		GXVoid GetRotation ( GXQuat &out );

		const GXProjectionClipPlanes& GetClipPlanesWorld ();
		GXBool IsObjectVisible ( const GXAABB objectBoundsWorld );

		virtual GXVoid SetZnear ( GXFloat znear ) = 0;
		virtual GXVoid SetZfar	( GXFloat zfar ) = 0;

		virtual GXFloat GetZnear () = 0;
		virtual GXFloat GetZfar () = 0;

	protected:
		GXVoid UpdateClipPlanes ();
};


#endif //GX_CAMERA