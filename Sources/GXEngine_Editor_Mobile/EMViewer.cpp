#include <GXEngine_Editor_Mobile/EMViewer.h>
#include <GXEngine_Editor_Mobile/EMTool.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXUICommon.h>


#define VIEWER_ZOOM_MODE            0u
#define VIEWER_ROTATE_MODE          1u
#define VIEWER_PAN_MODE             2u
#define VIEWER_NOTHING_MODE         0xFFu

#define VIEWER_ZOOM_STEP            2.0f
#define VIEWER_MIN_DISTANCE         0.1f
#define VIEWER_MAX_DISTANCE         .0e+30f
#define VIEWER_ROTATION_SPEED       45.0f       // 45 degrees per centimeter
#define VIEWER_PAN_SPEED            0.2f        // 0.2 unit per centimeter
#define VIEWER_FOVY_DEGREES         60.0f

#define DEFAULT_DISTANCE            7.0f

#define DEFAULT_ORIGIN_X            2.0f
#define DEFAULT_ORIGIN_Y            -1.0f
#define DEFAULT_ORIGIN_Z            0.0f

#define DEFAULT_PITCH_RAD           0.0f
#define DEFAULT_YAW_RAD             0.0f

#define VIEWER_Z_NEAR               0.1f
#define VIEWER_Z_FAR                1000.0f

//---------------------------------------------------------------------------------------------------------------------

EMViewer* EMViewer::_instance = nullptr;

EMViewer::~EMViewer ()
{
    if ( !_inputWidget ) return;

    _inputWidget->SetOnLeftMouseButtonDownCallback ( nullptr );
    _inputWidget->SetOnLeftMouseButtonUpCallback ( nullptr );
    _inputWidget->SetOnMiddleMouseButtonDownCallback ( nullptr );
    _inputWidget->SetOnMiddleMouseButtonUpCallback ( nullptr );
    _inputWidget->SetOnRightMouseButtonDownCallback ( nullptr );
    _inputWidget->SetOnRightMouseButtonUpCallback ( nullptr );
    _inputWidget->SetOnDoubleClickCallback ( nullptr );
    _inputWidget->SetOnMouseMoveCallback ( nullptr );
    _inputWidget->SetOnMouseScrollCallback ( nullptr );
    _inputWidget->SetOnKeyDownCallback ( nullptr );
    _inputWidget->SetOnKeyUpCallback ( nullptr );

    _inputWidget->SetHandler ( nullptr );
    _instance = nullptr;
}

GXVoid EMViewer::SetInputWidget ( GXUIInput &inputWidget )
{
    _inputWidget = &inputWidget;

    _inputWidget->SetOnLeftMouseButtonDownCallback ( &EMViewer::OnLMBDownCallback );
    _inputWidget->SetOnLeftMouseButtonUpCallback ( &EMViewer::OnLMBUpCallback );
    _inputWidget->SetOnMiddleMouseButtonDownCallback ( &EMViewer::OnMMBDownCallback );
    _inputWidget->SetOnMiddleMouseButtonUpCallback ( &EMViewer::OnMMBUpCallback );
    _inputWidget->SetOnMouseMoveCallback ( &EMViewer::OnMouseMoveCallback );
    _inputWidget->SetOnMouseScrollCallback ( &EMViewer::OnMouseScrollCallback );
    _inputWidget->SetOnKeyDownCallback ( &EMViewer::OnKeyDownCallback );
    _inputWidget->SetOnKeyUpCallback ( &EMViewer::OnKeyUpCallback );

    _inputWidget->SetHandler ( this );
}

GXVoid EMViewer::SetTarget ( EMActor* actor )
{
    _target = actor;
}

GXVoid EMViewer::SetOnViewerTransformChangedCallback ( GXVoid* context, EMViewerOnTransformHandler callback )
{
    _transformChangedContext = context;
    _onViewerTransformChanged = callback;
}

GXCamera& EMViewer::GetCamera ()
{
    return _camera;
}

GXVoid EMViewer::UpdateMouse ( const GXVec2 &position )
{
    memcpy ( &_mousePosition, &position, sizeof ( GXVec2 ) );
}

EMViewer* EMViewer::GetInstance ()
{
    if ( !_instance )
        _instance = new EMViewer ();

    return _instance;
}

EMViewer::EMViewer ():
    _inputWidget ( nullptr ),
    _transformChangedContext ( nullptr ),
    _onViewerTransformChanged ( nullptr ),
    _pitch ( DEFAULT_PITCH_RAD ),
    _yaw ( DEFAULT_YAW_RAD ),
    _distance ( DEFAULT_DISTANCE ),
    _origin ( DEFAULT_ORIGIN_X, DEFAULT_ORIGIN_Y, DEFAULT_ORIGIN_Z ),
    _target ( nullptr ),
    _rotationSpeed ( GXDegToRad ( VIEWER_ROTATION_SPEED ) / gx_ui_Scale ),
    _panSpeed ( VIEWER_PAN_SPEED / gx_ui_Scale ),
    _zoomSpeed ( VIEWER_ZOOM_STEP ),
    _camera ( GXDegToRad ( VIEWER_FOVY_DEGREES ), 1.0f, VIEWER_Z_NEAR, VIEWER_Z_FAR ),
    _isAltPressed ( GX_FALSE ),
    _isMMBPressed ( GX_FALSE )
{
    GXRenderer& renderer = GXRenderer::GetInstance ();
    _camera.SetAspectRatio ( renderer.GetWidth () / static_cast<GXFloat> ( renderer.GetHeight () ) );

    UpdateCamera ();
}

GXVoid EMViewer::OnPan ( const GXVec2& mouseDelta )
{
    const GXMat4& cameraTransform = _camera.GetCurrentFrameModelMatrix ();

    GXVec3 deltaRight;
    GXVec3 tmp;
    cameraTransform.GetX ( tmp );
    deltaRight.Multiply ( tmp, -mouseDelta.GetX () * _panSpeed );

    GXVec3 deltaUp;
    cameraTransform.GetY ( tmp );
    deltaUp.Multiply ( tmp, -mouseDelta.GetY () * _panSpeed );

    _origin.Sum ( _origin, deltaRight );
    _origin.Sum ( _origin, deltaUp );

    UpdateCamera ();

    if ( !_onViewerTransformChanged ) return;

    _onViewerTransformChanged ( _transformChangedContext );
}

GXVoid EMViewer::OnRotate ( const GXVec2& mouseDelta )
{
    const GXFloat pitchDelta = -mouseDelta.GetY () * _rotationSpeed;
    const GXFloat yawDelta = mouseDelta.GetX () * _rotationSpeed;

    _pitch = FixPitch ( _pitch + pitchDelta );
    _yaw = FixYaw ( _yaw + yawDelta );

    GXVec3 targetLocation;

    if ( _target )
        _target->GetTransform ().GetLocation ( targetLocation );
    else
        targetLocation = _origin;

    GXVec3 targetOriginView;
    _camera.GetCurrentFrameViewMatrix ().MultiplyAsPoint ( targetOriginView, targetLocation );

    UpdateCamera ();
    GXVec3 tempTargetWorld;
    _camera.GetCurrentFrameModelMatrix ().MultiplyAsPoint ( tempTargetWorld, targetOriginView );

    GXVec3 deltaTargetWorld;
    deltaTargetWorld.Substract ( targetLocation, tempTargetWorld );

    _origin.Sum ( _origin, deltaTargetWorld );

    UpdateCamera ();
    
    if ( !_onViewerTransformChanged ) return;

    _onViewerTransformChanged ( _transformChangedContext );
}

GXVoid EMViewer::OnZoom ( GXFloat mouseWheelSteps )
{
    _distance = GXClampf ( _distance - VIEWER_ZOOM_STEP * mouseWheelSteps, VIEWER_MIN_DISTANCE, VIEWER_MAX_DISTANCE );
    UpdateCamera ();

    if ( !_onViewerTransformChanged ) return;

    _onViewerTransformChanged ( _transformChangedContext );
}

GXVoid EMViewer::UpdateCamera ()
{
    GXMat4 matrix;
    matrix.RotationXY ( _pitch, _yaw );

    GXVec3 tmp;
    GXVec3 stick;
    matrix.GetZ ( tmp );
    stick.Multiply ( tmp, _distance );

    tmp.Substract ( _origin, stick );
    matrix.SetW ( tmp );

    _camera.SetCurrentFrameModelMatrix ( matrix );
}

GXFloat EMViewer::FixPitch ( GXFloat currentPitch )
{
    return GXClampf ( currentPitch, -GX_MATH_HALF_PI, GX_MATH_HALF_PI );
}

GXFloat EMViewer::FixYaw ( GXFloat currentYaw )
{
    while ( currentYaw < -GX_MATH_PI )
        currentYaw += GX_MATH_DOUBLE_PI;

    while ( currentYaw > GX_MATH_PI )
        currentYaw -= GX_MATH_DOUBLE_PI;

    return currentYaw;
}

GXUByte EMViewer::ResolveMode ( GXBool isAltDown, GXBool isMMBDown, GXBool isWheel )
{
    if ( isWheel )
        return VIEWER_ZOOM_MODE;

    if ( isAltDown && isMMBDown )
        return VIEWER_ROTATE_MODE;

    if ( isMMBDown )
        return VIEWER_PAN_MODE;

    return VIEWER_NOTHING_MODE;
}

GXVoid GXCALL EMViewer::OnLMBDownCallback ( GXVoid* /*handler*/, GXUIInput& /*input*/, GXFloat x, GXFloat y )
{
    EMTool* tool = EMTool::GetActiveTool ();

    if ( !tool ) return;

    tool->OnLeftMouseButtonDown ( x, y );
}

GXVoid GXCALL EMViewer::OnLMBUpCallback ( GXVoid* /*handler*/, GXUIInput& /*input*/, GXFloat x, GXFloat y )
{
    EMTool* tool = EMTool::GetActiveTool ();

    if ( !tool ) return;

    tool->OnLeftMouseButtonUp ( x, y );
}

GXVoid GXCALL EMViewer::OnMMBDownCallback ( GXVoid* handler, GXUIInput& /*input*/, GXFloat /*x*/, GXFloat /*y*/ )
{
    EMViewer* viewer = static_cast<EMViewer*> ( handler );
    viewer->_isMMBPressed = GX_TRUE;
}

GXVoid GXCALL EMViewer::OnMMBUpCallback ( GXVoid* handler, GXUIInput& /*input*/, GXFloat /*x*/, GXFloat /*y*/ )
{
    EMViewer* viewer = static_cast<EMViewer*> ( handler );
    viewer->_isMMBPressed = GX_FALSE;
}

GXVoid GXCALL EMViewer::OnMouseScrollCallback ( GXVoid* handler, GXUIInput& /*input*/, GXFloat x, GXFloat y, GXFloat scroll )
{
    EMTool* tool = EMTool::GetActiveTool ();

    if ( tool && tool->OnMouseScroll ( x, y, scroll ) ) return;

    EMViewer* viewer = static_cast<EMViewer*> ( handler );

    if ( viewer->ResolveMode ( viewer->_isAltPressed, viewer->_isMMBPressed, GX_TRUE ) != VIEWER_ZOOM_MODE ) return;

    viewer->OnZoom ( scroll );
}

GXVoid GXCALL EMViewer::OnMouseMoveCallback ( GXVoid* handler, GXUIInput& /*input*/, GXFloat x, GXFloat y )
{
    EMTool* tool = EMTool::GetActiveTool ();

    if ( tool && tool->OnMouseMove ( x, y ) ) return;

    EMViewer* viewer = static_cast<EMViewer*> ( handler );
    const GXVec2 mousePosition ( x, y );

    switch ( viewer->ResolveMode ( viewer->_isAltPressed, viewer->_isMMBPressed, GX_FALSE ) )
    {
        case VIEWER_ROTATE_MODE:
        {
            GXVec2 delta;
            delta.Substract ( mousePosition, viewer->_mousePosition );
            viewer->OnRotate ( delta );
        }
        break;

        case VIEWER_PAN_MODE:
        {
            GXVec2 delta;
            delta.Substract ( mousePosition, viewer->_mousePosition );
            viewer->OnPan ( delta );
        }
        break;

        default:
            // NOTHING
        break;
    }

    viewer->UpdateMouse ( mousePosition );
}

GXVoid GXCALL EMViewer::OnKeyDownCallback ( GXVoid* handler, GXUIInput& /*input*/, GXInt keyCode )
{
    EMTool* tool = EMTool::GetActiveTool ();

    if ( tool && tool->OnKeyDown ( keyCode ) ) return;

    if ( keyCode != VK_MENU ) return;

    EMViewer* viewer = static_cast<EMViewer*> ( handler );
    viewer->_isAltPressed = GX_TRUE;
}

GXVoid GXCALL EMViewer::OnKeyUpCallback ( GXVoid* handler, GXUIInput& /*input*/, GXInt keyCode )
{
    EMTool* tool = EMTool::GetActiveTool ();

    if ( tool && tool->OnKeyUp ( keyCode ) ) return;

    if ( keyCode != VK_MENU ) return;

    EMViewer* viewer = static_cast<EMViewer*> ( handler );
    viewer->_isAltPressed = GX_FALSE;
}
