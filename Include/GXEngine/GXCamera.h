// version 1.16

#ifndef GX_CAMERA
#define GX_CAMERA


#include <GXCommon/GXMath.h>


class GXCamera
{
    private:
        static GXCamera*            _activeCamera;

    protected:
        GXFloat                     _zNear;
        GXFloat                     _zFar;

        GXMat4                      _currentFrameModelMatrix;
        GXMat4                      _currentFrameViewMatrix;
        GXMat4                      _currentFrameProjectionMatrix;
        GXMat4                      _currentFrameInverseProjectionMatrix;
        GXMat4                      _currentFrameViewProjectionMatrix;
        GXMat4                      _currentFrameInverseViewProjectionMatrix;
        GXMat4                      _lastFrameModelMatrix;
        GXMat4                      _lastFrameViewMatrix;
        GXMat4                      _lastFrameViewProjectionMatrix;

        GXProjectionClipPlanes      _clipPlanesWorld;

    public:
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

        GXVoid GetLocation ( GXVec3 &location ) const;

        GXVoid GetRotation ( GXMat4 &rotation ) const;
        GXVoid GetRotation ( GXQuat &rotation ) const;

        const GXProjectionClipPlanes& GetClipPlanesWorld ();
        GXBool IsObjectVisible ( const GXAABB objectBoundsWorld );

        virtual GXVoid SetZNear ( GXFloat zNear ) = 0;
        virtual GXVoid SetZFar ( GXFloat zFar ) = 0;

        GXFloat GetZNear () const;
        GXFloat GetZFar () const;

        GXVoid UpdateLastFrameMatrices ();

        static GXCamera* GXCALL GetActiveCamera ();
        static GXVoid GXCALL SetActiveCamera ( GXCamera* camera );

    protected:
        explicit GXCamera ( GXFloat zNear, GXFloat zFar );
        virtual ~GXCamera ();

        GXVoid UpdateClipPlanes ();

    private:
        GXCamera () = delete;
        GXCamera ( const GXCamera &other ) = delete;
        GXCamera& operator = ( const GXCamera &other ) = delete;
};


#endif // GX_CAMERA
