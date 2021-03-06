#ifndef EM_VIEWER
#define EM_VIEWER


#include "EMActor.h"
#include <GXEngine/GXCameraPerspective.h>
#include <GXEngine/GXUIInput.h>


typedef GXVoid ( GXCALL* EMViewerOnTransformHandler ) ( GXVoid* context );


class EMViewer final
{
    private:
        GXUIInput*                      _inputWidget;

        GXVoid*                         _transformChangedContext;
        EMViewerOnTransformHandler      _onViewerTransformChanged;

        GXFloat                         _pitch;
        GXFloat                         _yaw;
        GXFloat                         _distance;
        GXVec3                          _origin;

        EMActor*                        _target;

        GXFloat                         _rotationSpeed;
        GXFloat                         _panSpeed;
        GXFloat                         _zoomSpeed;

        GXCameraPerspective             _camera;

        GXBool                          _isAltPressed;
        GXBool                          _isMMBPressed;

        GXVec2                          _mousePosition;

        static EMViewer*                _instance;

    public:
        ~EMViewer ();

        GXVoid SetInputWidget ( GXUIInput &inputWidget );
        GXVoid SetTarget ( EMActor* actor );
        GXVoid SetOnViewerTransformChangedCallback ( GXVoid* context, EMViewerOnTransformHandler callback );
        GXCamera& GetCamera ();
        GXVoid UpdateMouse ( const GXVec2 &position );

        static EMViewer* GetInstance ();

    private:
        EMViewer ();

        GXVoid OnPan ( const GXVec2& mouseDelta );
        GXVoid OnRotate ( const GXVec2& mouseDelta );
        GXVoid OnZoom ( GXFloat mouseWheelSteps );

        GXVoid UpdateCamera ();
        GXFloat FixPitch ( GXFloat currentPitch );
        GXFloat FixYaw ( GXFloat currentYaw );

        GXUByte ResolveMode ( GXBool isAltDown, GXBool isMMBDown, GXBool isWheel );

        static GXVoid GXCALL OnLMBDownCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );
        static GXVoid GXCALL OnLMBUpCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );
        static GXVoid GXCALL OnMMBDownCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );
        static GXVoid GXCALL OnMMBUpCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );

        static GXVoid GXCALL OnMouseScrollCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y, GXFloat scroll );
        static GXVoid GXCALL OnMouseMoveCallback ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y );

        static GXVoid GXCALL OnKeyDownCallback ( GXVoid* handler, GXUIInput& input, GXInt keyCode );
        static GXVoid GXCALL OnKeyUpCallback ( GXVoid* handler, GXUIInput& input, GXInt keyCode );

        EMViewer ( const EMViewer &other ) = delete;
        EMViewer& operator = ( const EMViewer &other ) = delete;
};


#endif // EM_VIEWER
