//version 1.11

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

		GXMat4					currentModelMatrix;
		GXMat4					currentViewMatrix;
		GXMat4					currentProjectionMatrix;
		GXMat4					currentInverseProjectionMatrix;
		GXMat4					currentViewProjectionMatrix;
		GXMat4					lastFrameViewMatrix;

		GXProjectionClipPlanes	clipPlanesWorld;

	public:
		GXCamera ();
		virtual ~GXCamera ();

		const GXMat4& GetCurrentViewProjectionMatrix () const;
		const GXMat4& GetCurrentProjectionMatrix () const;
		const GXMat4& GetCurrentInverseProjectionMatrix () const;
		const GXMat4& GetCurrentModelMatrix () const;
		const GXMat4& GetCurrentViewMatrix () const;
		const GXMat4& GetLastFrameViewMatrix () const;

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetLocation ( const GXVec3 &location );

		GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		GXVoid SetRotation ( const GXMat4 &rotation );
		GXVoid SetRotation ( const GXQuat &rotation );

		GXVoid SetCurrentModelMatrix ( const GXMat4 &matrix );

		GXVoid GetLocation ( GXVec3& out ) const;

		GXVoid GetRotation ( GXMat4 &out ) const;
		GXVoid GetRotation ( GXQuat &out ) const;

		const GXProjectionClipPlanes& GetClipPlanesWorld ();
		GXBool IsObjectVisible ( const GXAABB objectBoundsWorld );

		virtual GXVoid SetZnear ( GXFloat znear ) = 0;
		virtual GXVoid SetZfar ( GXFloat zfar ) = 0;

		GXFloat GetZnear () const;
		GXFloat GetZfar () const;

		GXVoid UpdateLastFrameViewMatrix ();

		static GXCamera* GXCALL GetActiveCamera ();
		static GXVoid GXCALL SetActiveCamera ( GXCamera* camera );

	protected:
		GXVoid UpdateClipPlanes ();
};


#endif //GX_CAMERA
