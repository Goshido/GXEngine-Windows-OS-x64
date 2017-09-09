//version 1.13

#ifndef GX_CAMERA
#define GX_CAMERA


#include <GXCommon/GXMath.h>


class GXCamera
{
	private:
		static GXCamera*		activeCamera;

	protected:
		GXFloat					zNear;
		GXFloat					zFar;

		GXMat4					currentFrameModelMatrix;
		GXMat4					currentFrameViewMatrix;
		GXMat4					currentFrameProjectionMatrix;
		GXMat4					currentFrameInverseProjectionMatrix;
		GXMat4					currentFrameViewProjectionMatrix;
		GXMat4					currentFrameInverseViewProjectionMatrix;
		GXMat4					lastFrameModelMatrix;
		GXMat4					lastFrameViewMatrix;
		GXMat4					lastFrameViewProjectionMatrix;

		GXProjectionClipPlanes	clipPlanesWorld;

	public:
		GXCamera ();
		virtual ~GXCamera ();

		const GXMat4& GetCurrentFrameViewProjectionMatrix () const;
		const GXMat4& GetCurrentFrameInverseViewProjectionMatrix () const;
		const GXMat4& GetCurrentFrameProjectionMatrix () const;
		const GXMat4& GetCurrentFrameInverseProjectionMatrix () const;
		const GXMat4& GetCurrentFrameModelMatrix () const;
		const GXMat4& GetCurrentFrameViewMatrix () const;
		const GXMat4& GetLastFrameModelMatrix () const;
		const GXMat4& GetLastFrameViewMatrix () const;
		const GXMat4& GetLastFrameViewProjectionMatrix () const;

		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		GXVoid SetLocation ( const GXVec3 &location );

		// pitchRadians increasing -> down to top rotation.
		// yawRadians increasing -> left to right rotation.
		// rollRadians increasing -> clockwise rotation.
		GXVoid SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );

		GXVoid SetRotation ( const GXMat4 &rotation );
		GXVoid SetRotation ( const GXQuat &rotation );

		GXVoid SetCurrentFrameModelMatrix ( const GXMat4 &matrix );

		GXVoid GetLocation ( GXVec3& out ) const;

		GXVoid GetRotation ( GXMat4 &out ) const;
		GXVoid GetRotation ( GXQuat &out ) const;

		const GXProjectionClipPlanes& GetClipPlanesWorld ();
		GXBool IsObjectVisible ( const GXAABB objectBoundsWorld );

		virtual GXVoid SetZNear ( GXFloat newZNear ) = 0;
		virtual GXVoid SetZFar ( GXFloat newZFar ) = 0;

		GXFloat GetZNear () const;
		GXFloat GetZFar () const;

		GXVoid UpdateLastFrameMatrices ();

		static GXCamera* GXCALL GetActiveCamera ();
		static GXVoid GXCALL SetActiveCamera ( GXCamera* camera );

	protected:
		GXVoid UpdateClipPlanes ();
};


#endif //GX_CAMERA
