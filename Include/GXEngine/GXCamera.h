//version 1.12

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

		GXMat4					currentFrameModelMatrix;
		GXMat4					currentFrameViewMatrix;
		GXMat4					currentFrameProjectionMatrix;
		GXMat4					currentFrameInverseProjectionMatrix;
		GXMat4					currentFrameViewProjectionMatrix;
		GXMat4					lastFrameModelMatrix;
		GXMat4					lastFrameViewMatrix;
		GXMat4					lastFrameViewProjectionMatrix;

		GXProjectionClipPlanes	clipPlanesWorld;

	public:
		GXCamera ();
		virtual ~GXCamera ();

		const GXMat4& GetCurrentFrameViewProjectionMatrix () const;
		const GXMat4& GetCurrentFrameProjectionMatrix () const;
		const GXMat4& GetCurrentFrameInverseProjectionMatrix () const;
		const GXMat4& GetCurrentFrameModelMatrix () const;
		const GXMat4& GetCurrentFrameViewMatrix () const;
		const GXMat4& GetLastFrameModelMatrix () const;
		const GXMat4& GetLastFrameViewMatrix () const;
		const GXMat4& GetLastFrameViewProjectionMatrix () const;

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetLocation ( const GXVec3 &location );

		GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		GXVoid SetRotation ( const GXMat4 &rotation );
		GXVoid SetRotation ( const GXQuat &rotation );

		GXVoid SetCurrentFrameModelMatrix ( const GXMat4 &matrix );

		GXVoid GetLocation ( GXVec3& out ) const;

		GXVoid GetRotation ( GXMat4 &out ) const;
		GXVoid GetRotation ( GXQuat &out ) const;

		const GXProjectionClipPlanes& GetClipPlanesWorld ();
		GXBool IsObjectVisible ( const GXAABB objectBoundsWorld );

		virtual GXVoid SetZnear ( GXFloat znear ) = 0;
		virtual GXVoid SetZfar ( GXFloat zfar ) = 0;

		GXFloat GetZnear () const;
		GXFloat GetZfar () const;

		GXVoid UpdateLastFrameMatrices ();

		static GXCamera* GXCALL GetActiveCamera ();
		static GXVoid GXCALL SetActiveCamera ( GXCamera* camera );

	protected:
		GXVoid UpdateClipPlanes ();
};


#endif //GX_CAMERA
