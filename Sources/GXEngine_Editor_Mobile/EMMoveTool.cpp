#include <GXEngine_Editor_Mobile/EMMoveTool.h>
#include <GXEngine_Editor_Mobile/EMRenderer.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXLogger.h>


#define DEFAULT_START_LOCATION_X            0.0f
#define DEFAULT_START_LOCATION_Y            0.0f
#define DEFAULT_START_LOCATION_Z            0.0f

#define DEFAULT_DELTA_X                     0.0f
#define DEFAULT_DELTA_Y                     0.0f
#define DEFAULT_DELTA_Z                     0.0f

#define MOVE_TOOL_LOCAL_MODE                0u
#define MOVE_TOOL_WORLD_MODE                1u

#define MOVE_TOOL_ACTIVE_AXIS_UNKNOWN       0xFFu
#define MOVE_TOOL_ACTIVE_AXIS_X             0u
#define MOVE_TOOL_ACTIVE_AXIS_Y             1u
#define MOVE_TOOL_ACTIVE_AXIS_Z             2u
#define MOVE_TOOL_GISMO_SIZE_FACTOR         0.1f

#define X_AXIS_COLOR_R                      255u
#define X_AXIS_COLOR_G                      0u
#define X_AXIS_COLOR_B                      0u
#define X_AXIS_COLOR_A                      255u

#define Y_AXIS_COLOR_R                      0u
#define Y_AXIS_COLOR_G                      255u
#define Y_AXIS_COLOR_B                      0u
#define Y_AXIS_COLOR_A                      255u

#define Z_AXIS_COLOR_R                      0u
#define Z_AXIS_COLOR_G                      0u
#define Z_AXIS_COLOR_B                      255u
#define Z_AXIS_COLOR_A                      255u

#define CENTER_COLOR_R                      255u
#define CENTER_COLOR_G                      255u
#define CENTER_COLOR_B                      255u
#define CENTER_COLOR_A                      255u

#define SELECTED_AXIS_COLOR_R               255u
#define SELECTED_AXIS_COLOR_G               255u
#define SELECTED_AXIS_COLOR_B               255u
#define SELECTED_AXIS_COLOR_A               255u

#define CROSS_LINE_EPSILON                  1.0e-5f

//---------------------------------------------------------------------------------------------------------------------

EMMoveTool::EMMoveTool ():
    _mode ( MOVE_TOOL_LOCAL_MODE ),
    _activeAxis ( MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ),
    _startLocationWorld ( DEFAULT_START_LOCATION_X, DEFAULT_START_LOCATION_Y, DEFAULT_START_LOCATION_Z ),
    _deltaWorld ( DEFAULT_DELTA_X, DEFAULT_DELTA_Y, DEFAULT_DELTA_Z ),
    _isLMBPressed ( GX_FALSE ),
    _mouseX ( 0xFFFFu ),
    _mouseY ( 0xFFFFu ),
    _gismoScaleCorrector ( 1.0f ),
    _xAxis ( L"Meshes/Editor Mobile/Move gismo X axis.stm" ),
    _xAxisMask ( L"Meshes/Editor Mobile/Move gismo X axis mask.stm" ),
    _yAxis ( L"Meshes/Editor Mobile/Move gismo Y axis.stm" ),
    _yAxisMask ( L"Meshes/Editor Mobile/Move gismo Y axis mask.stm" ),
    _zAxis ( L"Meshes/Editor Mobile/Move gismo Z axis.stm" ),
    _zAxisMask ( L"Meshes/Editor Mobile/Move gismo z axis mask.stm" ),
    _center ( L"Meshes/Editor Mobile/Move gismo center.stm" )
{
    _gismoRotation.Identity ();
}

EMMoveTool::~EMMoveTool ()
{
    // NOTHING
}

GXVoid EMMoveTool::Bind ()
{
    _actor = nullptr;
    _activeAxis = MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;
    _isLMBPressed = GX_FALSE;
}

GXVoid EMMoveTool::SetActor ( EMActor* currentActor )
{
    _actor = currentActor;

    if ( !_actor ) return;

    GXCamera* activeCamera = GXCamera::GetActiveCamera ();

    if ( !activeCamera ) return;

    switch ( _mode )
    {
        case MOVE_TOOL_LOCAL_MODE:
            _actor->GetTransform ().GetRotation ( _gismoRotation );
        break;

        case MOVE_TOOL_WORLD_MODE:
            _gismoRotation.Identity ();
        break;

        default:
            // NOTHING
        break;
    }

    GXVec3 gismoLocationView;
    activeCamera->GetCurrentFrameViewMatrix ().MultiplyAsPoint ( gismoLocationView, _actor->GetTransform ().GetLocation () );

    constexpr GXVec3 deltaView ( 0.0f, 0.0f, 0.0f );
    _gismoScaleCorrector = GetScaleCorrector ( gismoLocationView, deltaView );
}

GXVoid EMMoveTool::UnBind ()
{
    _actor = nullptr;
}

GXVoid EMMoveTool::OnViewerTransformChanged ()
{
    constexpr GXVec3 deltaView ( 0.0f, 0.0f, 0.0f );
    GXVec3 axisLocationView;
    GXCamera::GetActiveCamera ()->GetCurrentFrameViewMatrix ().MultiplyAsPoint ( axisLocationView, _startLocationWorld );

    _gismoScaleCorrector = GetScaleCorrector ( axisLocationView, deltaView );
}

GXVoid EMMoveTool::OnDrawHudColorPass ()
{
    if ( !_actor ) return;

    glDisable ( GL_BLEND );

    UpdateMeshTransform ( _xAxis );

    if ( _activeAxis == MOVE_TOOL_ACTIVE_AXIS_X )
        _unlitColorMaterial.SetColor ( SELECTED_AXIS_COLOR_R, SELECTED_AXIS_COLOR_G, SELECTED_AXIS_COLOR_B, SELECTED_AXIS_COLOR_A );
    else
        _unlitColorMaterial.SetColor ( X_AXIS_COLOR_R, X_AXIS_COLOR_G, X_AXIS_COLOR_B, X_AXIS_COLOR_A );

    _unlitColorMaterial.Bind ( _xAxis );
    _xAxis.Render ();
    _unlitColorMaterial.Unbind ();

    UpdateMeshTransform ( _yAxis );

    if ( _activeAxis == MOVE_TOOL_ACTIVE_AXIS_Y )
        _unlitColorMaterial.SetColor ( SELECTED_AXIS_COLOR_R, SELECTED_AXIS_COLOR_G, SELECTED_AXIS_COLOR_B, SELECTED_AXIS_COLOR_A );
    else
        _unlitColorMaterial.SetColor ( Y_AXIS_COLOR_R, Y_AXIS_COLOR_G, Y_AXIS_COLOR_B, Y_AXIS_COLOR_A );

    _unlitColorMaterial.Bind ( _yAxis );
    _yAxis.Render ();
    _unlitColorMaterial.Unbind ();

    UpdateMeshTransform ( _zAxis );

    if ( _activeAxis == MOVE_TOOL_ACTIVE_AXIS_Z )
        _unlitColorMaterial.SetColor ( SELECTED_AXIS_COLOR_R, SELECTED_AXIS_COLOR_G, SELECTED_AXIS_COLOR_B, SELECTED_AXIS_COLOR_A );
    else
        _unlitColorMaterial.SetColor ( Z_AXIS_COLOR_R, Z_AXIS_COLOR_G, Z_AXIS_COLOR_B, Z_AXIS_COLOR_A );

    _unlitColorMaterial.Bind ( _zAxis );
    _zAxis.Render ();
    _unlitColorMaterial.Unbind ();

    UpdateMeshTransform ( _center );
    _unlitColorMaterial.SetColor ( CENTER_COLOR_R, CENTER_COLOR_G, CENTER_COLOR_B, CENTER_COLOR_A );
    _unlitColorMaterial.Bind ( _center );
    _center.Render ();
    _unlitColorMaterial.Unbind ();

    glEnable ( GL_BLEND );
}

GXVoid EMMoveTool::OnDrawHudMaskPass ()
{
    if ( !_actor ) return;
    
    EMRenderer& renderer = EMRenderer::GetInstance ();

    renderer.SetObjectMask ( &_xAxisMask );
    UpdateMeshTransform ( _xAxisMask );
    _objectMaskMaterial.Bind ( _xAxisMask );
    _xAxisMask.Render ();
    _objectMaskMaterial.Unbind ();

    renderer.SetObjectMask ( &_yAxisMask );
    UpdateMeshTransform ( _yAxisMask );
    _objectMaskMaterial.Bind ( _yAxisMask );
    _yAxisMask.Render ();
    _objectMaskMaterial.Unbind ();

    renderer.SetObjectMask ( &_zAxisMask );
    UpdateMeshTransform ( _zAxisMask );
    _objectMaskMaterial.Bind ( _zAxisMask );
    _zAxisMask.Render ();
    _objectMaskMaterial.Unbind ();

    renderer.SetObjectMask ( nullptr );
}

GXBool EMMoveTool::OnMouseMove ( GXFloat x, GXFloat y )
{
    _mouseX = static_cast<GXUShort> ( x );
    _mouseY = static_cast<GXUShort> ( y );

    if ( _isLMBPressed && _activeAxis != MOVE_TOOL_ACTIVE_AXIS_UNKNOWN )
    {
        OnMoveActor ();
        return GX_TRUE;
    }

    return GX_FALSE;
}

GXBool EMMoveTool::OnLeftMouseButtonDown ( GXFloat x, GXFloat y )
{
    _isLMBPressed = GX_TRUE;
    EMRenderer::GetInstance ().GetObject ( (GXUShort)x, (GXUShort)y );
    return GX_FALSE;
}

GXBool EMMoveTool::OnLeftMouseButtonUp ( GXFloat /*x*/, GXFloat /*y*/ )
{
    if ( _activeAxis == MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return GX_FALSE;

    _startLocationWorld.Sum ( _startLocationWorld, _deltaWorld );

    GXTransform newTransform = _actor->GetTransform ();
    newTransform.SetLocation ( _startLocationWorld );
    _actor->SetTransform ( newTransform );

    _isLMBPressed = GX_FALSE;

    return GX_FALSE;
}

GXBool EMMoveTool::OnObject ( GXVoid* object )
{
    if ( !_actor ) return GX_FALSE;

    if ( object == &_xAxisMask )
    {
        _activeAxis = MOVE_TOOL_ACTIVE_AXIS_X;
    }
    else if ( object == &_yAxisMask )
    {
        _activeAxis = MOVE_TOOL_ACTIVE_AXIS_Y;
    }
    else if ( object == &_zAxisMask )
    {
        _activeAxis = MOVE_TOOL_ACTIVE_AXIS_Z;
    }
    else if ( !object )
    {
        _activeAxis = MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;
        UnBind ();
        return GX_TRUE;
    }
    else
    {
        _activeAxis = MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;
        UnBind ();
        return GX_FALSE;
    }

    switch ( _mode )
    {
        case MOVE_TOOL_WORLD_MODE:
            SetWorldMode ();
        break;

        case MOVE_TOOL_LOCAL_MODE:
            SetLocalMode ();
        break;

        default:
            // NOTHING
        break;
    }

    UpdateModeMode ();

    return GX_TRUE;
}

GXVoid EMMoveTool::SetLocalMode ()
{
    _mode = MOVE_TOOL_LOCAL_MODE;
}

GXVoid EMMoveTool::SetWorldMode ()
{
    _mode = MOVE_TOOL_WORLD_MODE;
}

GXVoid EMMoveTool::UpdateModeMode ()
{
    GXCamera* activeCamera = GXCamera::GetActiveCamera ();
    
    if ( !activeCamera || !_actor ) return;

    const GXTransform& actorTransform = _actor->GetTransform ();
    actorTransform.GetLocation ( _startLocationWorld );
    memset ( &_deltaWorld, 0, sizeof ( GXVec3 ) );

    switch ( _mode )
    {
        case MOVE_TOOL_LOCAL_MODE:
            actorTransform.GetRotation ( _gismoRotation );
        break;

        case MOVE_TOOL_WORLD_MODE:
            _gismoRotation.Identity ();
        break;

        default:
            // NOTHING
        break;
    }

    GXVec3 axisLocationView;
    activeCamera->GetCurrentFrameViewMatrix ().MultiplyAsPoint ( axisLocationView, _startLocationWorld );

    GXVec3 axisDirectionView;
    GetAxis ( axisDirectionView );

    GXVec3 rayView;
    GetRayPerspective ( rayView );

    _axisStartParameter = GetAxisParameter ( axisLocationView, axisDirectionView, rayView );
}

GXVoid EMMoveTool::OnMoveActor ()
{
    if ( _activeAxis == MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return;

    GXCamera* activeCamera = GXCamera::GetActiveCamera ();

    GXVec3 axisLocationView;
    activeCamera->GetCurrentFrameViewMatrix ().MultiplyAsPoint ( axisLocationView, _startLocationWorld );

    GXVec3 axisDirectionView;
    GetAxis ( axisDirectionView );

    GXVec3 rayView;
    GetRayPerspective ( rayView );

    GXFloat axisParameterDelta = GetAxisParameter ( axisLocationView, axisDirectionView, rayView ) - _axisStartParameter;

    if ( !isfinite ( axisParameterDelta ) ) return;

    GXVec3 deltaView;
    deltaView.Multiply ( axisDirectionView, axisParameterDelta );

    _gismoScaleCorrector = GetScaleCorrector ( axisLocationView, deltaView );

    activeCamera->GetCurrentFrameModelMatrix ().MultiplyAsNormal ( _deltaWorld, deltaView );
    GXTransform newTransform = _actor->GetTransform ();
    GXVec3 w;
    newTransform.GetLocation ( w );
    w.Sum ( _startLocationWorld, _deltaWorld );
    newTransform.SetLocation ( w );

    _actor->SetTransform ( newTransform );
}

GXVoid EMMoveTool::GetAxis ( GXVec3& axisView )
{
    GXVec3 axisWorld;

    switch ( _activeAxis )
    {
        case MOVE_TOOL_ACTIVE_AXIS_X:
            if ( _mode == MOVE_TOOL_WORLD_MODE )
            {
                axisWorld.Init ( 1.0f, 0.0f, 0.0f );
            }
            else
            {
                _gismoRotation.GetX ( axisWorld );
            }
        break;

        case MOVE_TOOL_ACTIVE_AXIS_Y:
            if ( _mode == MOVE_TOOL_WORLD_MODE )
            {
                axisWorld.Init ( 0.0f, 1.0f, 0.0f );
            }
            else
            {
                _gismoRotation.GetY ( axisWorld );
            }
        break;

        case MOVE_TOOL_ACTIVE_AXIS_Z:
            if ( _mode == MOVE_TOOL_WORLD_MODE )
            {
                axisWorld.Init ( 0.0f, 0.0f, 1.0f );
            }
            else
            {
                _gismoRotation.GetZ ( axisWorld );
            }
        break;

        default:
            // NOTHING
        return;
    }

    GXCamera::GetActiveCamera ()->GetCurrentFrameViewMatrix ().MultiplyAsNormal ( axisView, axisWorld );
}

GXVoid EMMoveTool::GetRayPerspective ( GXVec3 &rayView )
{
    GXRenderer& renderer = GXRenderer::GetInstance ();
    const GXMat4& projectionMatrix = GXCamera::GetActiveCamera ()->GetCurrentFrameProjectionMatrix ();

    GXVec2 mouseCVV;
    mouseCVV.SetX ( ( _mouseX / static_cast<GXFloat> ( renderer.GetWidth () ) ) * 2.0f - 1.0f );
    mouseCVV.SetY ( ( _mouseY / static_cast<GXFloat> ( renderer.GetHeight () ) * 2.0f - 1.0f ) );

    projectionMatrix.GetRayPerspective ( rayView, mouseCVV );
}

GXFloat EMMoveTool::GetAxisParameter ( const GXVec3 &axisLocationView, const GXVec3 &axisDirectionView, const GXVec3 &rayView )
{
    const GXFloat b = axisLocationView.GetX () * rayView.GetX () + axisLocationView.GetY () * rayView.GetY () + axisLocationView.GetZ () * rayView.GetZ ();
    const GXFloat c = axisDirectionView.GetX () * rayView.GetX () + axisDirectionView.GetY () * rayView.GetY () + axisDirectionView.GetZ () * rayView.GetZ ();
    const GXFloat d = axisDirectionView.GetX () * axisDirectionView.GetX () + axisDirectionView.GetY () * axisDirectionView.GetY () + axisDirectionView.GetZ () * axisDirectionView.GetZ ();
    const GXFloat e = axisLocationView.GetX () * axisDirectionView.GetX () + axisLocationView.GetY () * axisDirectionView.GetY () + axisLocationView.GetZ () * axisDirectionView.GetZ ();
    const GXFloat zeta = ( rayView.GetX () * rayView.GetX () + rayView.GetY () * rayView.GetY () + rayView.GetZ () * rayView.GetZ () ) * c;
    const GXFloat omega = c * c;

    return ( zeta * e - b * omega ) / ( c * omega - zeta * d + CROSS_LINE_EPSILON );
}

GXFloat EMMoveTool::GetScaleCorrector ( const GXVec3 &axisLocationView, const GXVec3 &deltaView )
{
    return ( axisLocationView.GetZ () + deltaView.GetZ () ) * MOVE_TOOL_GISMO_SIZE_FACTOR;
}

GXVoid EMMoveTool::UpdateMeshTransform ( EMMesh &mesh )
{
    const GXTransform& actorTransform = _actor->GetTransform ();
    mesh.SetScale ( _gismoScaleCorrector, _gismoScaleCorrector, _gismoScaleCorrector );
    mesh.SetRotation ( _gismoRotation );
    GXVec3 tmp;
    actorTransform.GetLocation ( tmp );
    mesh.SetLocation ( tmp );
}
