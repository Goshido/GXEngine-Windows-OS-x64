#include <GXEngine_Editor_Mobile/EMUIColorPicker.h>
#include <GXEngine_Editor_Mobile/EMCheckerGeneratorMaterial.h>
#include <GXEngine_Editor_Mobile/EMHueCircleGeneratorMaterial.h>
#include <GXEngine_Editor_Mobile/EMVertexColorMaterial.h>
#include <GXEngine_Editor_Mobile/EMMesh.h>
#include <GXEngine/GXUIEditBoxIntegerValidator.h>
#include <GXEngine/GXUIEditBoxFloatValidator.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXLogger.h>


#define SCREEN_QUAD_MESH                            L"Meshes/System/ScreenQuad.stm"

#define CAPTION_LABEL_COLOR_R                       115u
#define CAPTION_LABEL_COLOR_G                       185u
#define CAPTION_LABEL_COLOR_B                       0u
#define CAPTION_LABEL_COLOR_A                       255u

#define PROPERTY_LABEL_COLOR_R                      255u
#define PROPERTY_LABEL_COLOR_G                      255u
#define PROPERTY_LABEL_COLOR_B                      255u
#define PROPERTY_LABEL_COLOR_A                      255u

#define DEFAULT_MAIN_PANEL_WIDTH                    6.72f
#define DEFAULT_MAIN_PANEL_HEIGHT                   16.12f

#define MAIN_PANEL_MINIMUM_WIDTH                    6.71292f
#define MAIN_PANEL_MINIMUM_HEIGHT                   16.19156f

#define START_MAIN_PANEL_LEFT_X_OFFSET              1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET               1.5f

#define MARGIN                                      0.24166f

#define CAPTION_TOP_Y_OFFSET                        0.64444f
#define CAPTION_HEIGHT                              0.5f

#define TOP_SEPARATOR_TOP_Y_OFFSET                  0.85925f
#define TOP_SEPARATOR_HEIGHT                        0.2f

#define HSV_COLOR_WIDGET_TOP_Y_OFFSET               7.41106f
#define HSV_COLOR_WIDGET_HEIGHT                     6.25644f

#define COLOR_SEPARATOR_TOP_Y_OFFSET                8.24346f
#define COLOR_SEPARATOR_HEIGHT                      0.53703f

#define ADD_COLOR_BUTTON_WIDTH                      0.59074f
#define ADD_COLOR_BUTTON_HEIGHT                     0.59074f
#define ADD_COLOR_BUTTON_TOP_Y_OFFSET               9.29068f

#define SAVED_COLOR_WIDTH                           0.53703f
#define SAVED_COLOR_HEIGHT                          0.53703f
#define SAVED_COLOR_HORIZONTAL_SPACING              0.16111f
#define SAVED_COLOR_VERTICAL_SPACING                0.16111f
#define SAVED_COLOR_TOP_ROW_TOP_Y_OFFSET            9.26383f

#define MIDDLE_SEPARATOR_HEIGHT                     0.2f
#define MIDDLE_SEPARATOR_TOP_Y_OFFSET               10.39160f

#define EDIT_BOX_WIDTH                              1.71851f
#define EDIT_BOX_HEIGHT                             0.59074f

#define PROPERTY_LABEL_WIDTH                        0.64444f
#define PROPERTY_LABEL_HEIGHT                       0.59074f
#define PROPERTY_VERTICAL_STEP                      1.10092f
#define H_PROPERTY_LABEL_TOP_Y_OFFSET               11.25085f
#define TRANSPARENCY_LABEL_WIDTH                    2.36295f

#define BUTTON_WIDTH                                1.90647f
#define BUTTON_HEIGHT                               0.51018f
#define CANCEL_BUTTON_X_OFFSET                      4.02775f

#define BOTTOM_SEPARATOR_HEIGHT                     0.2f
#define BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET            0.64444f

#define DEFAULT_CURRENT_COLOR_H                     83.0f
#define DEFAULT_CURRENT_COLOR_S                     100.0f
#define DEFAULT_CURRENT_COLOR_V                     73.0f
#define DEFAULT_CURRENT_COLOR_A                     100.0f

#define DEFAULT_FLOAT_VALIDATOR_TEXT                L"7"
#define DEFAULT_INTEGER_VALIDATOR_TEXT              L"3"

#define MINIMUM_HUE_VALUE                           0.0f
#define MAXIMUM_HUE_VALUE                           360.0f

#define MINIMUM_SATURATION_VALUE                    0.0f
#define MAXIMUM_SATURATION_VALUE                    100.0f

#define MINIMUM_VALUE                               0.0f
#define MAXIMUM_VALUE                               100.0f

#define MINIMUM_RED_VALUE                           0
#define MAXIMUM_RED_VALUE                           255

#define MINIMUM_GREEN_VALUE                         0
#define MAXIMUM_GREEN_VALUE                         255

#define MINIMUM_BLUE_VALUE                          0
#define MAXIMUM_BLUE_VALUE                          255

#define MINIMUM_ALPHA_VALUE                         0
#define MAXIMUM_ALPHA_VALUE                         255

#define MAX_BUFFER_SYMBOLS                          128u

#define PIXEL_PERFECT_LOCATION_OFFSET_X             0.1f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y             0.1f

#define TEXTURE                                     L"Textures/System/Default_Diffuse.tga"

#define DEFAULT_SAVED_COLOR_H                       0.0f
#define DEFAULT_SAVED_COLOR_S                       0.0f
#define DEFAULT_SAVED_COLOR_V                       100.0f
#define DEFAULT_SAVED_COLOR_A                       0.0f

#define CHECKER_ELEMENT_WIDTH                       0.11f
#define CHECKER_ELEMENT_HEIGHT                      0.11f

#define CHECKER_COLOR_ONE_R                         0u
#define CHECKER_COLOR_ONE_G                         0u
#define CHECKER_COLOR_ONE_B                         0u
#define CHECKER_COLOR_ONE_A                         255u

#define CHECKER_COLOR_TWO_R                         255u
#define CHECKER_COLOR_TWO_G                         255u
#define CHECKER_COLOR_TWO_B                         255u
#define CHECKER_COLOR_TWO_A                         255u

#define HUE_CIRCLE_INNER_RADIUS                     2.49720f
#define HUE_CIRCLE_OUTER_RADIUS                     3.11479f

#define COLOR_SELECTOR_PROJECTION_NEAR              0.0f
#define COLOR_SELECTOR_PROJECTION_FAR               7.0f
#define COLOR_SELECTOR_VISIBLE_Z                    3.0f

#define COLOR_SELECTOR_UNLIT_COLOR_R                255u
#define COLOR_SELECTOR_UNLIT_COLOR_G                255u
#define COLOR_SELECTOR_UNLIT_COLOR_B                255u
#define COLOR_SELECTOR_UNLIT_COLOR_A                255u

#define COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_R      0.0f
#define COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_G      0.0f
#define COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_B      0.0f
#define COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_A      0.0f

#define COLOR_SELECTOR_HUE_MARKER_COLOR_R           255u
#define COLOR_SELECTOR_HUE_MARKER_COLOR_G           255u
#define COLOR_SELECTOR_HUE_MARKER_COLOR_B           255u
#define COLOR_SELECTOR_HUE_MARKER_COLOR_A           255u

#define COLOR_SELECTOR_MSAA_FACTOR                  4.0f
#define COLOR_SELECTOR_SAMPLE_BORDER_SIDE           0.13426f

#define EQUILATERAL_TRIANGLE_A_X                    -0.500181f
#define EQUILATERAL_TRIANGLE_A_Y                    0.864750f
#define EQUILATERAL_TRIANGLE_A_Z                    0.0f

#define EQUILATERAL_TRIANGLE_B_X                    1.000591f
#define EQUILATERAL_TRIANGLE_B_Y                    0.0f
#define EQUILATERAL_TRIANGLE_B_Z                    0.0f

#define EQUILATERAL_TRIANGLE_C_X                    -0.500181f
#define EQUILATERAL_TRIANGLE_C_Y                    -0.864750f
#define EQUILATERAL_TRIANGLE_C_Z                    0.0f

#define ALPHA_RGBA_TO_ALPHA_HSVA                    0.392156f

//---------------------------------------------------------------------------------------------------------------------

class EMColorRenderer final : public GXWidgetRenderer
{
    private:
        GXTexture2D                     _texture;
        EMMesh                          _screenQuad;

        GXHudSurface*                   _surface;
        GXTexture2D                     _checkerTexture;
        GXColorRGB                      _colorRGBA;
        GXColorHSV                      _colorHSV;
        EMCheckerGeneratorMaterial      _checkerGeneratorMaterial;
        GLuint                          _fbo;
        GXOpenGLState                   _openGLState;

    public:
        explicit EMColorRenderer ( GXUIInput* widget );
        ~EMColorRenderer () override;

        GXVoid SetColor ( const GXColorHSV &color );
        const GXColorHSV& GetColor () const;

    protected:
        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

    private:
        GXVoid UpdateCheckerTexture ();

        EMColorRenderer () = delete;
        EMColorRenderer ( const EMColorRenderer &other ) = delete;
        EMColorRenderer& operator = ( const EMColorRenderer &other ) = delete;
};

EMColorRenderer::EMColorRenderer ( GXUIInput* widget ):
    GXWidgetRenderer ( widget ),
    _texture ( TEXTURE, GX_FALSE, GX_FALSE ),
    _screenQuad ( SCREEN_QUAD_MESH )
{
    const GXAABB& boundsLocal = widget->GetBoundsLocal ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );
    
    SetColor ( GXColorHSV ( DEFAULT_SAVED_COLOR_H, DEFAULT_SAVED_COLOR_S, DEFAULT_SAVED_COLOR_V, DEFAULT_SAVED_COLOR_A ) );

    _checkerGeneratorMaterial.SetColorOne ( CHECKER_COLOR_ONE_R, CHECKER_COLOR_ONE_G, CHECKER_COLOR_ONE_B, CHECKER_COLOR_ONE_A );
    _checkerGeneratorMaterial.SetColorTwo ( CHECKER_COLOR_TWO_R, CHECKER_COLOR_TWO_G, CHECKER_COLOR_TWO_B, CHECKER_COLOR_TWO_A );
    _checkerGeneratorMaterial.SetElementSize ( static_cast<GXUShort> ( CHECKER_ELEMENT_WIDTH * gx_ui_Scale), static_cast<GXUShort> ( CHECKER_ELEMENT_HEIGHT * gx_ui_Scale ) );

    glGenFramebuffers ( 1, &_fbo );

    UpdateCheckerTexture ();
}

EMColorRenderer::~EMColorRenderer ()
{
    _checkerTexture.FreeResources ();
    glDeleteFramebuffers ( 1, &_fbo );
    delete _surface;
}

GXVoid EMColorRenderer::SetColor ( const GXColorHSV &color )
{
    _colorHSV = color;
    _colorRGBA.From ( color );
}

const GXColorHSV& EMColorRenderer::GetColor () const
{
    return _colorHSV;
}

GXVoid EMColorRenderer::OnRefresh ()
{
    const GXFloat w = static_cast<GXFloat> ( _surface->GetWidth () );
    const GXFloat h = static_cast<GXFloat> ( _surface->GetHeight () );

    GXImageInfo ii;

    ii._color.From ( 255u, 255u, 255u, 255u );
    ii._insertX = 0.0f;
    ii._insertY = 0.0f;
    ii._insertWidth = w;
    ii._insertHeight = h;
    ii._overlayType = eGXImageOverlayType::SimpleReplace;
    ii._texture = &_checkerTexture;

    _surface->AddImage ( ii );

    ii._color = _colorRGBA;
    ii._texture = &_texture;
    ii._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

    _surface->AddImage ( ii );
}

GXVoid EMColorRenderer::OnDraw ()
{
    glDisable ( GL_DEPTH_TEST );
    _surface->Render ();
    glEnable ( GL_DEPTH_TEST );
}

GXVoid EMColorRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXSafeDelete ( _surface );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( width, height );

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );

    UpdateCheckerTexture ();
}

GXVoid EMColorRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location._data[ 2u ] );
}

GXVoid EMColorRenderer::UpdateCheckerTexture ()
{
    _checkerTexture.FreeResources ();
    _checkerTexture.InitResources ( _surface->GetWidth (), _surface->GetHeight (), GL_RGBA8, GX_FALSE );

    _openGLState.Save ();

    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _checkerTexture.GetTextureObject (), 0 );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDepthMask ( GL_FALSE );

    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_CULL_FACE );

    glViewport ( 0, 0, static_cast<GLsizei> ( _checkerTexture.GetWidth () ), static_cast<GLsizei> ( _checkerTexture.GetHeight () ) );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    const GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMColorRenderer::UpdateCheckerTexture::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    _checkerGeneratorMaterial.Bind ( GXTransform::GetNullTransform () );
    _screenQuad.Render ();
    _checkerGeneratorMaterial.Unbind ();

    _openGLState.Restore ();
}

//---------------------------------------------------------------------------------------------------------------------

class EMColorSelectorRenderer final : public GXWidgetRenderer
{
    private:
        EMMesh                          _screenQuad;

        GXHudSurface*                   _surface;
        GLuint                          _fbo;
        GXMeshGeometry                  _triangle;

        EMHueCircleGeneratorMaterial    _hueCircleGeneratorMaterial;
        EMVertexColorMaterial           _vertexColorMaterial;
        GXUnlitTexture2DMaterial        _unlitTexture2DMaterial;

        GXTexture2D                     _hueTexture;
        GXTexture2D                     _compositeTexture;

        GXOpenGLState                   _openGLState;
        GXCameraOrthographic            _projectionCamera;

        GXColorHSV                      _colorHSVA;
        GXColorRGB                      _colorRGBA;

        GXVec3                          _equilateralTriangleGeometryLocal[ 6u ];

    public:
        explicit EMColorSelectorRenderer ( GXUIInput* widget );
        ~EMColorSelectorRenderer () override;

        GXVoid SetColor ( const GXColorHSV &color );

    protected:
        GXVoid OnRefresh () override;
        GXVoid OnDraw () override;
        GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
        GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

    private:
        GXVoid UpdateHueCircleTexture ();
        GXVoid UpdateCompositeTexture ();

        EMColorSelectorRenderer () = delete;
        EMColorSelectorRenderer ( const EMColorSelectorRenderer &other ) = delete;
        EMColorSelectorRenderer& operator = ( const EMColorSelectorRenderer &other ) = delete;
};

EMColorSelectorRenderer::EMColorSelectorRenderer ( GXUIInput* widget ):
    GXWidgetRenderer ( widget ),
    _screenQuad ( SCREEN_QUAD_MESH )
{
    const GXAABB& boundsLocal = widget->GetBoundsLocal ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( static_cast<GXUShort> ( boundsLocal.GetWidth () ), static_cast<GXUShort> ( boundsLocal.GetHeight () ) );

    glGenFramebuffers ( 1, &_fbo );

    _triangle.SetTotalVertices ( 3 );
    _triangle.SetTopology ( GL_TRIANGLES );

    constexpr GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec3 );
    _triangle.SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, stride, static_cast<const GLvoid*> ( 0u ) );
    _triangle.SetBufferStream ( eGXMeshStreamIndex::Color, 3, GL_FLOAT, stride, reinterpret_cast<const GLvoid*> ( sizeof ( GXVec3 ) ) );

    _equilateralTriangleGeometryLocal[ 0u ].Init ( EQUILATERAL_TRIANGLE_A_X, EQUILATERAL_TRIANGLE_A_Y, EQUILATERAL_TRIANGLE_A_Z );
    _equilateralTriangleGeometryLocal[ 2u ].Init ( EQUILATERAL_TRIANGLE_B_X, EQUILATERAL_TRIANGLE_B_Y, EQUILATERAL_TRIANGLE_B_Z );
    _equilateralTriangleGeometryLocal[ 4u ].Init ( EQUILATERAL_TRIANGLE_C_X, EQUILATERAL_TRIANGLE_C_Y, EQUILATERAL_TRIANGLE_C_Z );

    _equilateralTriangleGeometryLocal[ 1u ].Init ( 0.0f, 0.0f, 0.0f );
    _equilateralTriangleGeometryLocal[ 5u ].Init ( 1.0f, 1.0f, 1.0f );

    _hueCircleGeneratorMaterial.SetInnerRadius ( COLOR_SELECTOR_MSAA_FACTOR * HUE_CIRCLE_INNER_RADIUS * gx_ui_Scale );
    _hueCircleGeneratorMaterial.SetOuterRadius ( COLOR_SELECTOR_MSAA_FACTOR * HUE_CIRCLE_OUTER_RADIUS * gx_ui_Scale );

    _unlitTexture2DMaterial.SetTexture ( _hueTexture );
    _unlitTexture2DMaterial.SetColor ( COLOR_SELECTOR_UNLIT_COLOR_R, COLOR_SELECTOR_UNLIT_COLOR_G, COLOR_SELECTOR_UNLIT_COLOR_B, COLOR_SELECTOR_UNLIT_COLOR_A );

    UpdateHueCircleTexture ();

    SetColor ( GXColorHSV ( DEFAULT_CURRENT_COLOR_H, DEFAULT_CURRENT_COLOR_S, DEFAULT_CURRENT_COLOR_V, DEFAULT_CURRENT_COLOR_A ) );

    GXCheckOpenGLError ();
}

EMColorSelectorRenderer::~EMColorSelectorRenderer ()
{
    _hueTexture.FreeResources ();
    _compositeTexture.FreeResources ();
    glDeleteFramebuffers ( 1, &_fbo );
    delete _surface;
}

GXVoid EMColorSelectorRenderer::SetColor ( const GXColorHSV &color )
{
    _colorHSVA = color;
    _colorRGBA.From ( color );
    UpdateCompositeTexture ();
}

GXVoid EMColorSelectorRenderer::OnRefresh ()
{
    const GXFloat w = static_cast<GXFloat> ( _surface->GetWidth () );
    const GXFloat h = static_cast<GXFloat> ( _surface->GetHeight () );

    _surface->Reset ();

    GXImageInfo ii;
    ii._color.From ( 255u, 255u, 255u, 255u );
    ii._texture = &_compositeTexture;
    ii._overlayType = eGXImageOverlayType::SimpleReplace;

    const GXFloat smoothCompositeTextureWidth = _compositeTexture.GetWidth () / COLOR_SELECTOR_MSAA_FACTOR;
    const GXFloat smoothCompositeTextureHeight = _compositeTexture.GetHeight () / COLOR_SELECTOR_MSAA_FACTOR;

    if ( smoothCompositeTextureWidth == w )
    {
        ii._insertX = 0.0f;
        ii._insertY = ( h - smoothCompositeTextureHeight ) * 0.5f;
    }
    else
    {
        ii._insertX = ( w - smoothCompositeTextureWidth ) * 0.5f;
        ii._insertY = 0.0f;
    }

    ii._insertWidth = smoothCompositeTextureWidth;
    ii._insertHeight = smoothCompositeTextureHeight;

    _surface->AddImage ( ii );

    const GXFloat hRadians = GXDegToRad ( _colorHSVA.GetHue () );
    const GXVec2 markerDirection ( gx_ui_Scale * cosf ( hRadians ), gx_ui_Scale * sinf ( hRadians ) );
    const GXVec2 center ( w * 0.5f, h * 0.5f );

    GXLineInfo li;
    li._color.From ( GXColorHSV ( _colorHSVA.GetHue () + 180.0f, 85.0f, 100.0f, 100.0f ) );
    li._thickness = 1.0f;
    ii._overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

    GXVec2 tmp;
    tmp.Multiply ( markerDirection, HUE_CIRCLE_INNER_RADIUS );
    tmp.Sum ( tmp, center );
    li._startPoint.Init ( floorf ( tmp.GetX () + 0.5f ) + 0.1f, floorf ( tmp.GetY () + 0.5f ) + 0.1f );

    tmp.Multiply ( markerDirection, HUE_CIRCLE_OUTER_RADIUS );
    tmp.Sum ( tmp, center );
    li._endPoint.Init ( floorf ( tmp.GetX () + 0.5f ) + 0.1f, floorf ( tmp.GetY () + 0.5f ) + 0.1f );

    _surface->AddLine ( li );

    const GXFloat triangleScale = HUE_CIRCLE_INNER_RADIUS * gx_ui_Scale;
    GXTransform transform;
    transform.SetRotation ( 0.0f, 0.0f, GXDegToRad ( _colorHSVA.GetHue () ) );
    transform.SetScale ( triangleScale, triangleScale, 1.0f );
    transform.SetLocation ( center.GetX (), center.GetY (), 0.0f );

    GXColorRGB currentHueRGBA;
    GXColorHSV currentHueHSVA ( _colorHSVA.GetHue (), 100.0f, 100.0f, 100.0f );
    currentHueRGBA.From ( currentHueHSVA );

    constexpr GXVec3 colorA ( 0.0f, 0.0f, 0.0f );
    GXVec3 colorB ( currentHueRGBA.GetRed (), currentHueRGBA.GetGreen (), currentHueRGBA.GetBlue () );
    constexpr GXVec3 colorC ( 1.0f, 1.0f, 1.0f );
    GXVec3 currentRGB ( _colorRGBA.GetRed (), _colorRGBA.GetGreen (), _colorRGBA.GetBlue () );

    GXVec3 barycentricCoords;
    GXGetBarycentricCoords ( barycentricCoords, currentRGB, colorA, colorB, colorC );

    GXMat3 barycentricToLocalTransform;
    barycentricToLocalTransform.SetX ( _equilateralTriangleGeometryLocal[ 0u ] );
    barycentricToLocalTransform.SetY ( _equilateralTriangleGeometryLocal[ 2u ] );
    barycentricToLocalTransform.SetZ ( _equilateralTriangleGeometryLocal[ 4u ] );

    GXVec3 colorCoordsLocal;
    barycentricToLocalTransform.MultiplyVectorMatrix ( colorCoordsLocal, barycentricCoords );

    GXVec3 colorCoordsSurface;
    transform.GetCurrentFrameModelMatrix ().MultiplyAsPoint ( colorCoordsSurface, colorCoordsLocal );

    GXColorHSV borderColorHSVA ( _colorHSVA.GetHue () + 180.0f, 100.0f, 100.0f, 100.0f );

    if ( borderColorHSVA.GetHue () > 360.0f )
        borderColorHSVA._data[ 0u ] -= 360.0f;

    const GXFloat borderSideFactor = floorf ( COLOR_SELECTOR_SAMPLE_BORDER_SIDE * gx_ui_Scale * 0.5f + 0.5f );
    colorCoordsSurface.Init ( floorf ( colorCoordsSurface.GetX () + 0.5f ), floorf ( colorCoordsSurface.GetY () + 0.5f ), colorCoordsSurface.GetZ () );

    li._color.From ( borderColorHSVA );

    li._startPoint.Init ( colorCoordsSurface.GetX () - borderSideFactor + 1.0f + 0.1f, colorCoordsSurface.GetY () - borderSideFactor + 0.1f );
    li._endPoint.Init ( colorCoordsSurface.GetX () + borderSideFactor - 1.0f + 0.9f, colorCoordsSurface.GetY () - borderSideFactor + 0.1f );

    _surface->AddLine ( li );

    li._startPoint.Init ( colorCoordsSurface.GetX () + borderSideFactor + 0.1f, colorCoordsSurface.GetY () - borderSideFactor + 1.0f + 0.1f );
    li._endPoint.Init ( colorCoordsSurface.GetX () + borderSideFactor + 0.1f, colorCoordsSurface.GetY () + borderSideFactor - 1.0f + 0.9f );

    _surface->AddLine ( li );

    li._startPoint.Init ( colorCoordsSurface.GetX () + borderSideFactor - 1.0f + 0.9f, colorCoordsSurface.GetY () + borderSideFactor + 0.1f );
    li._endPoint.Init ( colorCoordsSurface.GetX () - borderSideFactor + 1.0f + 0.1f, colorCoordsSurface.GetY () + borderSideFactor + 0.1f );

    _surface->AddLine ( li );

    li._startPoint.Init ( colorCoordsSurface.GetX () - borderSideFactor + 0.1f, colorCoordsSurface.GetY () + borderSideFactor - 1.0f + 0.9f );
    li._endPoint.Init ( colorCoordsSurface.GetX () - borderSideFactor + 0.1f, colorCoordsSurface.GetY () - borderSideFactor + 1.0f + 0.1f );

    _surface->AddLine ( li );
}

GXVoid EMColorSelectorRenderer::OnDraw ()
{
    glDisable ( GL_DEPTH_TEST );
    _surface->Render ();
    glEnable ( GL_DEPTH_TEST );
}

GXVoid EMColorSelectorRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXSafeDelete ( _surface );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" )
    _surface = new GXHudSurface ( width, height );

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location.GetZ () );

    const GXUShort w = _surface->GetWidth ();
    const GXUShort h = _surface->GetHeight ();
    GXUShort msaaSide;

    if ( w >= h )
        msaaSide = static_cast<GXUShort> ( h * COLOR_SELECTOR_MSAA_FACTOR );
    else
        msaaSide = static_cast<GXUShort> ( w * COLOR_SELECTOR_MSAA_FACTOR );

    if ( _compositeTexture.GetWidth () == msaaSide ) return;

    UpdateHueCircleTexture ();
    UpdateCompositeTexture ();
}

GXVoid EMColorSelectorRenderer::OnMoved ( GXFloat x, GXFloat y )
{
    x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
    y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

    GXVec3 location;
    _surface->GetLocation ( location );
    _surface->SetLocation ( x, y, location.GetZ () );
}

GXVoid EMColorSelectorRenderer::UpdateHueCircleTexture ()
{
    _hueTexture.FreeResources ();
    _compositeTexture.FreeResources ();

    _openGLState.Save ();

    const GXUShort w = _surface->GetWidth ();
    const GXUShort h = _surface->GetHeight ();
    GXUShort msaaSide;

    if ( w >= h )
        msaaSide = static_cast<GXUShort> ( h * COLOR_SELECTOR_MSAA_FACTOR );
    else
        msaaSide = static_cast<GXUShort> ( w * COLOR_SELECTOR_MSAA_FACTOR );

    _hueTexture.InitResources ( msaaSide, msaaSide, GL_RGBA8, GX_FALSE );

    _compositeTexture.InitResources ( msaaSide, msaaSide, GL_RGBA8, GX_TRUE );
    _hueCircleGeneratorMaterial.SetResolution ( msaaSide, msaaSide );
    glViewport ( 0, 0, static_cast<GLsizei> ( msaaSide ), static_cast<GLsizei> ( msaaSide ) );
    _projectionCamera.SetProjection ( static_cast<GXFloat> ( msaaSide ), static_cast<GXFloat> ( msaaSide ), COLOR_SELECTOR_PROJECTION_NEAR, COLOR_SELECTOR_PROJECTION_FAR );

    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _hueTexture.GetTextureObject (), 0 );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDepthMask ( GL_FALSE );
    glDisable ( GL_CULL_FACE );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_BLEND );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    const GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMColorSelectorRenderer::UpdateHueCircleTexture::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    _hueCircleGeneratorMaterial.Bind ( GXTransform::GetNullTransform () );
    _screenQuad.Render ();
    _hueCircleGeneratorMaterial.Unbind ();

    _openGLState.Restore ();
}

GXVoid EMColorSelectorRenderer::UpdateCompositeTexture ()
{
    GXColorHSV hueHSVA ( _colorHSVA.GetHue (), 100.0f, 100.0f, 100.0f );
    GXColorRGB hueRGBA;
    hueRGBA.From ( hueHSVA );

    _equilateralTriangleGeometryLocal[ 3u ].Init ( hueRGBA.GetRed (), hueRGBA.GetGreen (), hueRGBA.GetBlue () );
    _triangle.FillVertexBuffer ( _equilateralTriangleGeometryLocal, 6 * sizeof ( GXVec3 ), GL_DYNAMIC_DRAW );

    _openGLState.Save ();

    glBindFramebuffer ( GL_FRAMEBUFFER, _fbo );
    glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _compositeTexture.GetTextureObject (), 0 );

    glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glDepthMask ( GL_FALSE );

    glDisable ( GL_CULL_FACE );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_BLEND );

    constexpr GLenum buffers[ 1u ] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers ( 1, buffers );

    GXUShort w = _compositeTexture.GetWidth ();
    GXUShort h = _compositeTexture.GetHeight ();

    glViewport ( 0, 0, static_cast<GLsizei> ( w ), static_cast<GLsizei> ( h ) );

    const GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );

    if ( status != GL_FRAMEBUFFER_COMPLETE )
        GXLogA ( "EMColorSelectorRenderer::UpdateCompositeTexture::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

    static const GXFloat clearValue[ 4u ] = { COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_R, COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_G, COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_B, COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_A };
    glClearBufferfv ( GL_COLOR, 0, clearValue );

    const GXFloat triangleScale = COLOR_SELECTOR_MSAA_FACTOR * HUE_CIRCLE_INNER_RADIUS * gx_ui_Scale;

    GXTransform transform;
    transform.SetRotation ( 0.0f, 0.0f, GXDegToRad ( _colorHSVA.GetHue () ) );
    transform.SetScale ( triangleScale, triangleScale, 1.0f );
    transform.SetLocation ( 0.0f, 0.0f, COLOR_SELECTOR_VISIBLE_Z );

    GXCamera* oldCamera = GXCamera::GetActiveCamera ();
    GXCamera::SetActiveCamera ( &_projectionCamera );

    _vertexColorMaterial.Bind ( transform );
    _triangle.Render ();
    _vertexColorMaterial.Unbind ();

    glEnable ( GL_BLEND );

    transform.SetRotation ( 0.0f, 0.0f, 0.0f );
    transform.SetScale ( w * 0.5f, h * 0.5f, 1.0f );

    _unlitTexture2DMaterial.Bind ( transform );
    _screenQuad.Render ();
    _unlitTexture2DMaterial.Unbind ();

    GXCamera::SetActiveCamera ( oldCamera );
    _openGLState.Restore ();

    _compositeTexture.UpdateMipmaps ();
}

//---------------------------------------------------------------------------------------------------------------------

EMUIColorPicker* EMUIColorPicker::_instance = nullptr;

EMUIColorPicker& EMUIColorPicker::GetInstance ()
{
    if ( !_instance )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIColorPicker" )
        _instance = new EMUIColorPicker ();
    }

    return *_instance;
}

EMUIColorPicker::~EMUIColorPicker ()
{
    free ( _buffer );

    delete _pick;
    delete _cancel;
    delete _bottomSeparator;

    delete _transparency->GetValidator ();
    delete _transparency;
    delete _transparencyLabel;

    delete _b->GetValidator ();
    delete _b;
    delete _bLabel;

    delete _v->GetValidator ();
    delete _v;
    delete _vLabel;

    delete _g->GetValidator ();
    delete _g;
    delete _gLabel;

    delete _s->GetValidator ();
    delete _s;
    delete _sLabel;

    delete _r->GetValidator ();
    delete _r;
    delete _rLabel;

    delete _h->GetValidator ();
    delete _h;
    delete _hLabel;

    delete _middleSeparator;

    for ( GXUByte i = 0u; i < 16u; ++i )
    {
        delete _savedColors[ i ]->GetRenderer ();
        delete _savedColors[ i ];
    }

    delete _addColor;

    delete _oldColor->GetRenderer ();
    delete _oldColor;

    delete _currentColor->GetRenderer ();
    delete _currentColor;

    delete _hsvColorWidget->GetRenderer ();
    delete _hsvColorWidget;

    delete _topSeparator;
    delete _caption;

    _instance = nullptr;
}

GXWidget* EMUIColorPicker::GetWidget ()
{
    return _mainPanel.GetWidget ();
}

GXVoid EMUIColorPicker::PickColor ( GXVoid* context, EMColorPickerOnHSVAColorHandler callback, const GXColorHSV &oldColorHSVAValue )
{
    EMColorRenderer* renderer = static_cast<EMColorRenderer*> ( _oldColor->GetRenderer () );
    renderer->SetColor ( oldColorHSVAValue );
    _oldColor->Refresh ();

    _context = context;
    _onHSVColor = callback;
    _onRGBColor = nullptr;
    _onRGBUByteColor = nullptr;

    _mainPanel.Show ();
}

GXVoid EMUIColorPicker::PickColor ( GXVoid* context, EMColorPickerOnRGBAColorHandler callback, const GXColorRGB &oldColorValue )
{
    GXColorHSV oldColorHSV ( oldColorValue );
    EMColorRenderer* renderer = static_cast<EMColorRenderer*> ( _oldColor->GetRenderer () );
    renderer->SetColor ( oldColorHSV );
    _oldColor->Refresh ();

    _context = context;
    _onRGBColor = callback;
    _onHSVColor = nullptr;
    _onRGBUByteColor = nullptr;

    _mainPanel.Show ();
}

GXVoid EMUIColorPicker::PickColor ( GXVoid* context, EMColorPickerOnHSVAColorUByteHandler callback, GXUByte oldRed, GXUByte oldGreen, GXUByte oldBlue, GXUByte oldAlpha )
{
    GXColorHSV oldHSVA;
    GXColorRGB oldRGBA ( oldRed, oldGreen, oldBlue, oldAlpha );
    oldHSVA.From ( oldRGBA );
    EMColorRenderer* renderer = static_cast<EMColorRenderer*> ( _oldColor->GetRenderer () );
    renderer->SetColor ( oldHSVA );
    _oldColor->Refresh ();

    _context = context;
    _onRGBUByteColor = callback;
    _onRGBColor = nullptr;
    _onHSVColor = nullptr;

    _mainPanel.Show ();
}

EMUIColorPicker::EMUIColorPicker ():
    EMUI ( nullptr ),
    _mainPanel ( nullptr ),
    _onHSVColor ( nullptr ),
    _onRGBColor ( nullptr ),
    _onRGBUByteColor ( nullptr ),
    _context ( nullptr ),
    _buffer ( static_cast<GXWChar*> ( malloc ( MAX_BUFFER_SYMBOLS * sizeof ( GXWChar ) ) ) )
{
    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _caption = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparator" )
    _topSeparator = new EMUISeparator ( &_mainPanel );

    GXWidget* mainPanelWidget = _mainPanel.GetWidget ();

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXUIInput" )
    _hsvColorWidget = new GXUIInput ( mainPanelWidget, GX_TRUE );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMColorSelectorRenderer" )
    _hsvColorWidget->SetRenderer ( new EMColorSelectorRenderer ( _hsvColorWidget ) );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXUIInput" )
    _currentColor = new GXUIInput ( mainPanelWidget, GX_TRUE );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMColorRenderer" )
    _currentColor->SetRenderer ( new EMColorRenderer ( _currentColor ) );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXUIInput" )
    _oldColor = new GXUIInput ( mainPanelWidget, GX_TRUE );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMColorRenderer" )
    _oldColor->SetRenderer ( new EMColorRenderer ( _oldColor ) );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButton" )
    _addColor = new EMUIButton ( &_mainPanel );

    for ( GXUByte i = 0u; i < 16u; ++i )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXUIInput" )
        GXUIInput* savedColor = new GXUIInput ( mainPanelWidget, GX_TRUE );

        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMColorRenderer" )
        savedColor->SetRenderer ( new EMColorRenderer ( savedColor ) );
        _savedColors[ i ] = savedColor;
    }

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparator" )
    _middleSeparator = new EMUISeparator ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _hLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _h = new EMUIEditBox ( &_mainPanel );
    GXUIEditBox* editBox = static_cast<GXUIEditBox*> ( _h->GetWidget () );
    GXUIEditBoxFloatValidator* floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_TEXT, *editBox, MINIMUM_HUE_VALUE, MAXIMUM_HUE_VALUE );
    _h->SetValidator ( *floatValidator );
    _h->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _rLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _r = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _r->GetWidget () );
    GXUIEditBoxIntegerValidator* integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_TEXT, *editBox, MINIMUM_RED_VALUE, MAXIMUM_RED_VALUE );
    _r->SetValidator ( *integerValidator );
    _r->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _sLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _s = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _s->GetWidget () );
    floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_TEXT, *editBox, MINIMUM_SATURATION_VALUE, MAXIMUM_SATURATION_VALUE );
    _s->SetValidator ( *floatValidator );
    _s->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _gLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _g = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _g->GetWidget () );
    integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_TEXT, *editBox, MINIMUM_GREEN_VALUE, MAXIMUM_GREEN_VALUE );
    _g->SetValidator ( *integerValidator );
    _g->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _vLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _v = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _v->GetWidget () );
    floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_TEXT, *editBox, MINIMUM_VALUE, MAXIMUM_VALUE );
    _v->SetValidator ( *floatValidator );
    _v->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _bLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _b = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _b->GetWidget () );
    integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_TEXT, *editBox, MINIMUM_BLUE_VALUE, MAXIMUM_BLUE_VALUE );
    _b->SetValidator ( *integerValidator );
    _b->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIStaticText" )
    _transparencyLabel = new EMUIStaticText ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIEditBox" )
    _transparency = new EMUIEditBox ( &_mainPanel );
    editBox = static_cast<GXUIEditBox*> ( _transparency->GetWidget () );
    integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_TEXT, *editBox, MINIMUM_ALPHA_VALUE, MAXIMUM_ALPHA_VALUE );
    _transparency->SetValidator ( *integerValidator );
    _transparency->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUISeparator" )
    _bottomSeparator = new EMUISeparator ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButton" )
    _cancel = new EMUIButton ( &_mainPanel );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "EMUIButton" )
    _pick = new EMUIButton ( &_mainPanel );

    GXLocale& locale = GXLocale::GetInstance ();

    _hsvColorWidget->SetHandler ( this );
    _hsvColorWidget->SetOnLeftMouseButtonDownCallback ( &EMUIColorPicker::OnLeftMouseButton );

    _oldColor->SetHandler ( this );
    _oldColor->SetOnLeftMouseButtonDownCallback ( &EMUIColorPicker::OnLeftMouseButton );

    for ( GXUByte i = 0u; i < 16u; ++i )
    {
        GXUIInput* input = _savedColors[ i ];
        input->SetHandler ( this );
        input->SetOnLeftMouseButtonDownCallback ( &EMUIColorPicker::OnLeftMouseButton );
    }

    _caption->SetText ( locale.GetString ( L"Color picker->Color picker" ) );
    _caption->SetTextColor ( static_cast<GXUByte> ( CAPTION_LABEL_COLOR_R ), static_cast<GXUByte> ( CAPTION_LABEL_COLOR_G ), static_cast<GXUByte> ( CAPTION_LABEL_COLOR_B ), static_cast<GXUByte> ( CAPTION_LABEL_COLOR_A ) );
    _caption->SetAlingment ( eGXUITextAlignment::Center );

    _hLabel->SetText ( locale.GetString ( L"Color picker->H" ) );
    _hLabel->SetTextColor ( static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_R ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_G ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_B ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_A ) );
    _hLabel->SetAlingment ( eGXUITextAlignment::Left );

    _rLabel->SetText ( locale.GetString ( L"Color picker->R" ) );
    _rLabel->SetTextColor ( static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_R ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_G ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_B ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_A ) );
    _rLabel->SetAlingment ( eGXUITextAlignment::Left );

    _sLabel->SetText ( locale.GetString ( L"Color picker->S" ) );
    _sLabel->SetTextColor ( static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_R ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_G ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_B ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_A ) );
    _sLabel->SetAlingment ( eGXUITextAlignment::Left );

    _gLabel->SetText ( locale.GetString ( L"Color picker->G" ) );
    _gLabel->SetTextColor ( static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_R ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_G ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_B ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_A ) );
    _gLabel->SetAlingment ( eGXUITextAlignment::Left );

    _vLabel->SetText ( locale.GetString ( L"Color picker->V" ) );
    _vLabel->SetTextColor ( static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_R ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_G ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_B ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_A  ) );
    _vLabel->SetAlingment ( eGXUITextAlignment::Left );

    _bLabel->SetText ( locale.GetString ( L"Color picker->B" ) );
    _bLabel->SetTextColor ( static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_R ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_G ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_B ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_A ) );
    _bLabel->SetAlingment ( eGXUITextAlignment::Left );

    _transparencyLabel->SetText ( locale.GetString ( L"Color picker->Transparency" ) );
    _transparencyLabel->SetTextColor ( static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_R ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_G ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_B ), static_cast<GXUByte> ( PROPERTY_LABEL_COLOR_A ) );
    _transparencyLabel->SetAlingment ( eGXUITextAlignment::Left );

    _h->SetAlignment ( eGXUITextAlignment::Center );
    _r->SetAlignment ( eGXUITextAlignment::Center );
    _s->SetAlignment ( eGXUITextAlignment::Center );
    _g->SetAlignment ( eGXUITextAlignment::Center );
    _v->SetAlignment ( eGXUITextAlignment::Center );
    _b->SetAlignment ( eGXUITextAlignment::Center );
    _transparency->SetAlignment ( eGXUITextAlignment::Center );

    _addColor->SetCaption ( L"+" );
    _cancel->SetCaption ( locale.GetString ( L"Color picker->Cancel" ) );
    _pick->SetCaption ( locale.GetString ( L"Color picker->Pick" ) );

    _addColor->SetOnLeftMouseButtonCallback ( this, &EMUIColorPicker::OnButton );
    _cancel->SetOnLeftMouseButtonCallback ( this, &EMUIColorPicker::OnButton );
    _pick->SetOnLeftMouseButtonCallback ( this, &EMUIColorPicker::OnButton );

    _mainPanel.SetOnResizeCallback ( this, &EMUIColorPicker::OnResize );

    UpdateCurrentColor ( DEFAULT_CURRENT_COLOR_H, DEFAULT_CURRENT_COLOR_S, DEFAULT_CURRENT_COLOR_V, DEFAULT_CURRENT_COLOR_A );
    EMColorRenderer* destinationRenderer = static_cast<EMColorRenderer*> ( _oldColor->GetRenderer () );
    EMColorRenderer* sourceRenderer = static_cast<EMColorRenderer*> ( _currentColor->GetRenderer () );
    destinationRenderer->SetColor ( sourceRenderer->GetColor () );

    static const GXColorHSV defaultSavedColor ( DEFAULT_SAVED_COLOR_H, DEFAULT_SAVED_COLOR_S, DEFAULT_SAVED_COLOR_V, DEFAULT_SAVED_COLOR_A );

    for ( GXUByte i = 0u; i < 16u; ++i )
    {
        EMColorRenderer* renderer = static_cast<EMColorRenderer*> ( _savedColors[ i ]->GetRenderer () );
        renderer->SetColor ( defaultSavedColor );
    }

    const GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
    _mainPanel.Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, static_cast<GXFloat> ( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
    _mainPanel.SetMinimumWidth ( MAIN_PANEL_MINIMUM_WIDTH * gx_ui_Scale );
    _mainPanel.SetMinimumHeight ( MAIN_PANEL_MINIMUM_HEIGHT * gx_ui_Scale );
    _mainPanel.Hide ();
}

GXVoid EMUIColorPicker::UpdateCurrentColor ( GXFloat hue, GXFloat saturation, GXFloat value, GXFloat alpha )
{
    GXColorHSV newColorHSV ( hue, saturation, value, alpha );

    EMColorRenderer* currentColorRenderer = static_cast<EMColorRenderer*> ( _currentColor->GetRenderer () );
    currentColorRenderer->SetColor ( newColorHSV );
    _currentColor->Refresh ();

    EMColorSelectorRenderer* colorSelectorRenderer = static_cast<EMColorSelectorRenderer*> ( _hsvColorWidget->GetRenderer () );
    colorSelectorRenderer->SetColor ( newColorHSV );
    _hsvColorWidget->Refresh ();

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%.6g", hue );
    _h->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%.6g", saturation );
    _s->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%.6g", value );
    _v->SetText ( _buffer );

    GXUByte newRed;
    GXUByte newGreen;
    GXUByte newBlue;
    GXUByte newAlpha;
    GXColorRGB newColorRGB;
    newColorRGB.From ( newColorHSV );
    newColorRGB.ConvertToUByte ( newRed, newGreen, newBlue, newAlpha );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", newRed );
    _r->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", newGreen );
    _g->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", newBlue );
    _b->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", newAlpha );
    _transparency->SetText ( _buffer );
}

GXVoid EMUIColorPicker::UpdateCurrentColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", red );
    this->_r->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", green );
    this->_g->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", blue );
    this->_b->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", alpha );
    _transparency->SetText ( _buffer );

    GXColorRGB newColorRGB ( red, green, blue, alpha );
    GXColorHSV newColorHSV;
    newColorHSV.From ( newColorRGB );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%.6g", newColorHSV.GetHue () );
    this->_h->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%.6g", newColorHSV.GetSaturation () );
    this->_s->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%.6g", newColorHSV.GetValue () );
    this->_v->SetText ( _buffer );

    EMColorRenderer* currentColorRenderer = static_cast<EMColorRenderer*> ( _currentColor->GetRenderer () );
    currentColorRenderer->SetColor ( newColorHSV );
    _currentColor->Refresh ();

    EMColorSelectorRenderer* colorSelectorRenderer = static_cast<EMColorSelectorRenderer*> ( _hsvColorWidget->GetRenderer () );
    colorSelectorRenderer->SetColor ( newColorHSV );
    _hsvColorWidget->Refresh ();
}

GXVoid EMUIColorPicker::UpdateCurrentColorWithCorrection ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
    GXColorRGB newColorRGB ( red, green, blue, alpha );

    EMColorRenderer* currentColorRenderer = static_cast<EMColorRenderer*> ( _currentColor->GetRenderer () );
    const GXFloat oldHue = currentColorRenderer->GetColor ().GetHue ();
    GXColorHSV newColorHSV;
    newColorHSV.From ( newColorRGB );

    GXUByte correctedRed;
    GXUByte correctedGreen;
    GXUByte correctedBlue;
    GXUByte correctedAlpha;

    newColorHSV.SetHue ( oldHue );
    newColorRGB.From ( newColorHSV );
    newColorRGB.ConvertToUByte ( correctedRed, correctedGreen, correctedBlue, correctedAlpha );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", correctedRed );
    this->_r->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", correctedGreen );
    this->_g->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", correctedBlue );
    this->_b->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%hhu", correctedAlpha );
    _transparency->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%.6g", newColorHSV.GetHue () );
    this->_h->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%.6g", newColorHSV.GetSaturation () );
    this->_s->SetText ( _buffer );

    swprintf_s ( _buffer, MAX_BUFFER_SYMBOLS, L"%.6g", newColorHSV.GetValue () );
    this->_v->SetText ( _buffer );

    currentColorRenderer->SetColor ( newColorHSV );
    _currentColor->Refresh ();

    EMColorSelectorRenderer* colorSelectorRenderer = static_cast<EMColorSelectorRenderer*> ( _hsvColorWidget->GetRenderer () );
    colorSelectorRenderer->SetColor ( newColorHSV );
    _hsvColorWidget->Refresh ();
}

GXVoid GXCALL EMUIColorPicker::OnButton ( GXVoid* handler, GXUIButton& button, GXFloat /*x*/, GXFloat /*y*/, eGXMouseButtonState state )
{
    if ( state == eGXMouseButtonState::Down ) return;

    EMUIColorPicker* colorPicker = static_cast<EMUIColorPicker*> ( handler );
    
    if ( &button == colorPicker->_addColor->GetWidget () )
    {
        for ( GXByte i = 15; i > 0; --i )
        {
            EMColorRenderer* destinationRenderer = static_cast<EMColorRenderer*> ( colorPicker->_savedColors[ i ]->GetRenderer () );
            const EMColorRenderer* sourceRenderer = static_cast<const EMColorRenderer*> ( colorPicker->_savedColors[ i - 1 ]->GetRenderer () );
            destinationRenderer->SetColor ( sourceRenderer->GetColor () );
            colorPicker->_savedColors[ i ]->Refresh ();
        }

        EMColorRenderer* newSavedColorRenderer = static_cast<EMColorRenderer*> ( colorPicker->_savedColors[ 0u ]->GetRenderer () );
        const EMColorRenderer* currentColorRenderer = static_cast<const EMColorRenderer*> ( colorPicker->_currentColor->GetRenderer () );
        newSavedColorRenderer->SetColor ( currentColorRenderer->GetColor () );
        colorPicker->_savedColors[ 0u ]->Refresh ();

        return;
    }

    if ( &button == colorPicker->_pick->GetWidget () )
    {
        if ( colorPicker->_onHSVColor )
        {
            EMColorRenderer* currentColorRenderer = static_cast<EMColorRenderer*> ( colorPicker->_currentColor->GetRenderer () );
            const GXColorHSV& currentColor = currentColorRenderer->GetColor ();
            colorPicker->_onHSVColor ( colorPicker->_context, currentColor );
        }
        else if ( colorPicker->_onRGBColor )
        {
            EMColorRenderer* currentColorRenderer = static_cast<EMColorRenderer*> ( colorPicker->_currentColor->GetRenderer () );
            const GXColorHSV& currentColor = currentColorRenderer->GetColor ();
            colorPicker->_onRGBColor ( colorPicker->_context, GXColorRGB ( currentColor ) );
        }
        else
        {
            GXUByte red;
            GXUByte green;
            GXUByte blue;
            GXUByte alpha;
            swscanf_s ( colorPicker->_r->GetText (), L"%hhu", &red );
            swscanf_s ( colorPicker->_g->GetText (), L"%hhu", &green );
            swscanf_s ( colorPicker->_b->GetText (), L"%hhu", &blue );
            swscanf_s ( colorPicker->_transparency->GetText (), L"%hhu", &alpha );
            colorPicker->_onRGBUByteColor ( handler, red, green, blue, alpha );
        }

        colorPicker->_mainPanel.Hide ();
        return;
    }

    colorPicker->_mainPanel.Hide ();
}

GXVoid GXCALL EMUIColorPicker::OnLeftMouseButton ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y )
{
    EMUIColorPicker* colorPicker = static_cast<EMUIColorPicker*> ( handler );

    if ( &input == colorPicker->_hsvColorWidget )
    {
        GXVec3 center;
        colorPicker->_hsvColorWidget->GetBoundsWorld ().GetCenter ( center );
        GXVec3 cursorWorld ( x, y, 0.0f );

        GXVec3 direction;
        direction.Substract ( cursorWorld, center );

        const GXFloat innerRadius = HUE_CIRCLE_INNER_RADIUS * gx_ui_Scale;
        const GXFloat outerRadius = HUE_CIRCLE_OUTER_RADIUS * gx_ui_Scale;
        const GXFloat radius = direction.Length ();

        constexpr GXVec3 xAxis ( 1.0f, 0.0f, 0.0f );
        direction.Normalize ();
        const GXFloat cosAngle = direction.DotProduct ( xAxis );
        GXFloat angleRadians = acosf ( cosAngle );

        if ( y < center.GetY () )
            angleRadians = GX_MATH_DOUBLE_PI - angleRadians;

        if ( radius >= innerRadius && radius <= outerRadius )
        {
            const GXFloat angleDegrees = GXRadToDeg ( angleRadians );
            EMColorRenderer* currentColorRenderer = static_cast<EMColorRenderer*> ( colorPicker->_currentColor->GetRenderer () );
            const GXColorHSV& color = currentColorRenderer->GetColor ();
            colorPicker->UpdateCurrentColor ( angleDegrees, color.GetSaturation (), color.GetValue (), color.GetAlpha () );

            return;
        }

        const EMColorRenderer* currentColorRenderer = static_cast<const EMColorRenderer*> ( colorPicker->_currentColor->GetRenderer () );
        const GXColorHSV& currentColor = currentColorRenderer->GetColor ();

        GXTransform transform;
        transform.SetScale ( innerRadius, innerRadius, 1.0f );
        transform.SetRotation ( 0.0f, 0.0f, GXDegToRad ( currentColor.GetHue () ) );
        transform.SetLocation ( center );

        constexpr GXVec3 equilateralTriangleLocal[ 3u ] = 
        {
            GXVec3 ( EQUILATERAL_TRIANGLE_A_X, EQUILATERAL_TRIANGLE_A_Y, EQUILATERAL_TRIANGLE_A_Z ),
            GXVec3 ( EQUILATERAL_TRIANGLE_B_X, EQUILATERAL_TRIANGLE_B_Y, EQUILATERAL_TRIANGLE_B_Z ),
            GXVec3 ( EQUILATERAL_TRIANGLE_C_X, EQUILATERAL_TRIANGLE_C_Y, EQUILATERAL_TRIANGLE_C_Z )
        };

        const GXMat4& modelMatrix = transform.GetCurrentFrameModelMatrix ();

        GXVec3 equitaleralTriangleWorld[ 3u ];

        for ( GXUByte i = 0u; i < 3u; ++i )
            modelMatrix.MultiplyAsPoint ( equitaleralTriangleWorld[ i ], equilateralTriangleLocal[ i ] );

        GXVec3 barycentricCoordinates;
        GXGetBarycentricCoords ( barycentricCoordinates, cursorWorld, equitaleralTriangleWorld[ 0u ], equitaleralTriangleWorld[ 1u ], equitaleralTriangleWorld[ 2u ] );

        if ( barycentricCoordinates.GetX () < 0.0f || barycentricCoordinates.GetX () > 1.0f || barycentricCoordinates.GetY () < 0.0f || barycentricCoordinates.GetY () > 1.0f || barycentricCoordinates.GetZ () < 0.0f || barycentricCoordinates.GetZ () > 1.0f ) return;

        GXColorHSV currentHueHSV ( currentColor.GetHue (), 100.0f, 100.0f, 100.0f );
        GXColorRGB currentHueRGB ( currentHueHSV );

        const GXUByte selectedRed = static_cast<GXUByte> ( 255.0f * ( barycentricCoordinates.GetY () * currentHueRGB.GetRed () + barycentricCoordinates.GetZ () ) );
        const GXUByte selectedGreen = static_cast<GXUByte> ( 255.0f * ( barycentricCoordinates.GetY () * currentHueRGB.GetGreen () + barycentricCoordinates.GetZ () ) );
        const GXUByte selectedBlue = static_cast<GXUByte> ( 255.0f * ( barycentricCoordinates.GetY () * currentHueRGB.GetBlue () + barycentricCoordinates.GetZ () ) );

        colorPicker->UpdateCurrentColorWithCorrection ( selectedRed, selectedGreen, selectedBlue, static_cast<GXUByte> ( currentColor.GetAlpha () * 2.55f ) );

        return;
    }

    if ( &input == colorPicker->_oldColor )
    {
        EMColorRenderer* oldColorRenderer = static_cast<EMColorRenderer*> ( colorPicker->_oldColor->GetRenderer () );
        const GXColorHSV& oldColor = oldColorRenderer->GetColor ();

        colorPicker->UpdateCurrentColor ( oldColor.GetHue (), oldColor.GetSaturation (), oldColor.GetValue (), oldColor.GetAlpha () );
        return;
    }

    for ( GXUByte i = 0u; i < 16u; ++i )
    {
        if ( &input != colorPicker->_savedColors[ i ] ) continue;

        const EMColorRenderer* savedColorRenderer = static_cast<const EMColorRenderer*> ( colorPicker->_savedColors[ i ]->GetRenderer () );
        const GXColorHSV& savedColor = savedColorRenderer->GetColor ();

        colorPicker->UpdateCurrentColor ( savedColor.GetHue (), savedColor.GetSaturation (), savedColor.GetValue (), savedColor.GetAlpha () );
        return;
    }
}

GXVoid GXCALL EMUIColorPicker::OnResize ( GXVoid* handler, GXUIDragableArea& /*area*/, GXFloat width, GXFloat height )
{
    EMUIColorPicker* colorPicker = static_cast<EMUIColorPicker*> ( handler );

    const GXFloat margin = MARGIN * gx_ui_Scale;

    colorPicker->_caption->Resize ( margin, height - ( MARGIN + CAPTION_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, CAPTION_HEIGHT * gx_ui_Scale );
    colorPicker->_topSeparator->Resize ( margin, height - ( MARGIN + TOP_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, TOP_SEPARATOR_HEIGHT * gx_ui_Scale );
    colorPicker->_hsvColorWidget->Resize ( margin, height - ( MARGIN + HSV_COLOR_WIDGET_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, HSV_COLOR_WIDGET_HEIGHT * gx_ui_Scale );

    const GXFloat colorSeparatorY = height - ( MARGIN + COLOR_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale;
    const GXFloat colorSeperatorWidth = ( width - 2.0f * margin ) * 0.5f;
    const GXFloat colorSeparatorHeight = COLOR_SEPARATOR_HEIGHT * gx_ui_Scale;
    colorPicker->_currentColor->Resize ( margin, colorSeparatorY, colorSeperatorWidth, colorSeparatorHeight );
    colorPicker->_oldColor->Resize ( margin + colorSeperatorWidth, colorSeparatorY, colorSeperatorWidth, colorSeparatorHeight );

    colorPicker->_addColor->Resize ( margin, height - ( MARGIN + ADD_COLOR_BUTTON_TOP_Y_OFFSET ) * gx_ui_Scale, ADD_COLOR_BUTTON_WIDTH * gx_ui_Scale, ADD_COLOR_BUTTON_HEIGHT * gx_ui_Scale );

    GXFloat savedColorX = width - margin - ( SAVED_COLOR_WIDTH * 8 + SAVED_COLOR_HORIZONTAL_SPACING * 7 ) * gx_ui_Scale;
    const GXFloat savedColorTopRowY = height - ( MARGIN + SAVED_COLOR_TOP_ROW_TOP_Y_OFFSET ) * gx_ui_Scale;
    const GXFloat savedColorBottomRowY = savedColorTopRowY - ( SAVED_COLOR_VERTICAL_SPACING + SAVED_COLOR_HEIGHT ) * gx_ui_Scale;
    const GXFloat savedColorWidth = SAVED_COLOR_WIDTH * gx_ui_Scale;
    const GXFloat savedColorHeight = SAVED_COLOR_HEIGHT * gx_ui_Scale;
    const GXFloat savedColorHorizontalStep = ( SAVED_COLOR_WIDTH + SAVED_COLOR_HORIZONTAL_SPACING ) * gx_ui_Scale;

    for ( GXUByte i = 0u; i < 8u; ++i )
    {
        colorPicker->_savedColors[ i ]->Resize ( savedColorX, savedColorTopRowY, savedColorWidth, savedColorHeight );
        colorPicker->_savedColors[ i + 8u ]->Resize ( savedColorX, savedColorBottomRowY, savedColorWidth, savedColorHeight );

        savedColorX += savedColorHorizontalStep;
    }

    colorPicker->_middleSeparator->Resize ( margin, height - ( MARGIN + MIDDLE_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, MIDDLE_SEPARATOR_HEIGHT * gx_ui_Scale );

    GXFloat propertyY = height - margin - H_PROPERTY_LABEL_TOP_Y_OFFSET * gx_ui_Scale;
    const GXFloat propertyVerticalStep = PROPERTY_VERTICAL_STEP * gx_ui_Scale;
    const GXFloat propertyLabelWidth = PROPERTY_LABEL_WIDTH * gx_ui_Scale;
    const GXFloat propertyLabelHeight = PROPERTY_LABEL_HEIGHT * gx_ui_Scale;
    const GXFloat editBoxWidth = EDIT_BOX_WIDTH * gx_ui_Scale;
    const GXFloat editBoxHeight = EDIT_BOX_HEIGHT * gx_ui_Scale;
    const GXFloat leftColumnEditBoxX = margin + propertyLabelWidth;
    const GXFloat rightColumnPropertyLabelX = width - margin - editBoxWidth - propertyLabelWidth;
    const GXFloat rightColumnEditBoxX = width - margin - editBoxWidth;

    colorPicker->_hLabel->Resize ( margin, propertyY, propertyLabelWidth, propertyLabelHeight );
    colorPicker->_h->Resize ( leftColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

    colorPicker->_rLabel->Resize ( rightColumnPropertyLabelX, propertyY, propertyLabelWidth, propertyLabelHeight );
    colorPicker->_r->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

    propertyY -= propertyVerticalStep;

    colorPicker->_sLabel->Resize ( margin, propertyY, propertyLabelWidth, propertyLabelHeight );
    colorPicker->_s->Resize ( leftColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

    colorPicker->_gLabel->Resize ( rightColumnPropertyLabelX, propertyY, propertyLabelWidth, propertyLabelHeight );
    colorPicker->_g->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

    propertyY -= propertyVerticalStep;

    colorPicker->_vLabel->Resize ( margin, propertyY, propertyLabelWidth, propertyLabelHeight );
    colorPicker->_v->Resize ( leftColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

    colorPicker->_bLabel->Resize ( rightColumnPropertyLabelX, propertyY, propertyLabelWidth, propertyLabelHeight );
    colorPicker->_b->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

    propertyY -= propertyVerticalStep;

    colorPicker->_transparencyLabel->Resize ( margin, propertyY, TRANSPARENCY_LABEL_WIDTH * gx_ui_Scale, propertyLabelHeight );
    colorPicker->_transparency->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

    colorPicker->_bottomSeparator->Resize ( margin, margin + BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET * gx_ui_Scale, width - 2.0f * margin, BOTTOM_SEPARATOR_HEIGHT * gx_ui_Scale );

    const GXFloat buttonWidth = BUTTON_WIDTH * gx_ui_Scale;
    const GXFloat buttonHeight = BUTTON_HEIGHT * gx_ui_Scale;

    colorPicker->_cancel->Resize ( width - ( MARGIN + CANCEL_BUTTON_X_OFFSET ) * gx_ui_Scale, margin, buttonWidth, buttonHeight );
    colorPicker->_pick->Resize ( width - margin - buttonWidth, margin, buttonWidth, buttonHeight );
}

GXVoid GXCALL EMUIColorPicker::OnFinishEditing ( GXVoid* handler, GXUIEditBox& editBox )
{
    EMUIColorPicker* colorPicker = static_cast<EMUIColorPicker*> ( handler );
    const EMColorRenderer* currentColorRenderer = static_cast<const EMColorRenderer*> ( colorPicker->_currentColor->GetRenderer () );
    GXColorHSV newCurrentColor ( currentColorRenderer->GetColor () );

    if ( &editBox == colorPicker->_h->GetWidget () )
    {
        swscanf_s ( colorPicker->_h->GetText (), L"%g", &( newCurrentColor._data[ 0u ] ) );
        colorPicker->UpdateCurrentColor ( newCurrentColor.GetHue (), newCurrentColor.GetSaturation (), newCurrentColor.GetValue (), newCurrentColor.GetAlpha () );
        return;
    }

    if ( &editBox == colorPicker->_s->GetWidget () )
    {
        swscanf_s ( colorPicker->_s->GetText (), L"%g", &( newCurrentColor._data[ 1u ] ) );
        colorPicker->UpdateCurrentColor ( newCurrentColor.GetHue (), newCurrentColor.GetSaturation (), newCurrentColor.GetValue (), newCurrentColor.GetAlpha () );
        return;
    }

    if ( &editBox == colorPicker->_v->GetWidget () )
    {
        swscanf_s ( colorPicker->_v->GetText (), L"%g", &( newCurrentColor._data[ 2u ] ) );
        colorPicker->UpdateCurrentColor ( newCurrentColor.GetHue (), newCurrentColor.GetSaturation (), newCurrentColor.GetValue (), newCurrentColor.GetAlpha () );
        return;
    }

    if ( &editBox != colorPicker->_r->GetWidget () && &editBox != colorPicker->_g->GetWidget () && &editBox != colorPicker->_b->GetWidget () && &editBox == colorPicker->_transparency->GetWidget () ) return;

    GXUByte red;
    GXUByte green;
    GXUByte blue;
    GXUByte alpha;
    swscanf_s ( colorPicker->_r->GetText (), L"%hhu", &red );
    swscanf_s ( colorPicker->_g->GetText (), L"%hhu", &green );
    swscanf_s ( colorPicker->_b->GetText (), L"%hhu", &blue );
    swscanf_s ( colorPicker->_transparency->GetText (), L"%hhu", &alpha );

    colorPicker->UpdateCurrentColor ( red, green, blue, alpha );
}
