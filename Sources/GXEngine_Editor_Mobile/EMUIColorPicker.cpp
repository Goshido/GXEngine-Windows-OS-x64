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


#define CAPTION_LABEL_COLOR_R						115
#define CAPTION_LABEL_COLOR_G						185
#define CAPTION_LABEL_COLOR_B						0
#define CAPTION_LABEL_COLOR_A						255

#define PROPERTY_LABEL_COLOR_R						255
#define PROPERTY_LABEL_COLOR_G						255
#define PROPERTY_LABEL_COLOR_B						255
#define PROPERTY_LABEL_COLOR_A						255

#define DEFAULT_MAIN_PANEL_WIDTH					6.72f
#define DEFAULT_MAIN_PANEL_HEIGHT					16.12f

#define MAIN_PANEL_MINIMUM_WIDTH					6.71292f
#define MAIN_PANEL_MINIMUM_HEIGHT					16.19156f

#define START_MAIN_PANEL_LEFT_X_OFFSET				1.5f
#define START_MAIN_PANEL_TOP_Y_OFFSET				1.5f

#define MARGIN										0.24166f

#define CAPTION_TOP_Y_OFFSET						0.64444f
#define CAPTION_HEIGHT								0.5f

#define TOP_SEPARATOR_TOP_Y_OFFSET					0.85925f
#define TOP_SEPARATOR_HEIGHT						0.2f

#define HSV_COLOR_WIDGET_TOP_Y_OFFSET				7.41106f
#define HSV_COLOR_WIDGET_HEIGHT						6.25644f

#define COLOR_SEPARATOR_TOP_Y_OFFSET				8.24346f
#define COLOR_SEPARATOR_HEIGHT						0.53703f

#define ADD_COLOR_BUTTON_WIDTH						0.59074f
#define ADD_COLOR_BUTTON_HEIGHT						0.59074f
#define ADD_COLOR_BUTTON_TOP_Y_OFFSET				9.29068f

#define SAVED_COLOR_WIDTH							0.53703f
#define SAVED_COLOR_HEIGHT							0.53703f
#define SAVED_COLOR_HORIZONTAL_SPACING				0.16111f
#define SAVED_COLOR_VERTICAL_SPACING				0.16111f
#define SAVED_COLOR_TOP_ROW_TOP_Y_OFFSET			9.26383f

#define MIDDLE_SEPARATOR_HEIGHT						0.2f
#define MIDDLE_SEPARATOR_TOP_Y_OFFSET				10.39160f

#define EDIT_BOX_WIDTH								1.71851f
#define EDIT_BOX_HEIGHT								0.59074f

#define PROPERTY_LABEL_WIDTH						0.64444f
#define PROPERTY_LABEL_HEIGHT						0.59074f
#define PROPERTY_VERTICAL_STEP						1.10092f
#define H_PROPERTY_LABEL_TOP_Y_OFFSET				11.25085f
#define TRANSPARENCY_LABEL_WIDTH					2.36295f

#define BUTTON_WIDTH								1.90647f
#define BUTTON_HEIGHT								0.51018f
#define CANCEL_BUTTON_X_OFFSET						4.02775f

#define BOTTOM_SEPARATOR_HEIGHT						0.2f
#define BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET			0.64444f

#define DEFAULT_CURRENT_COLOR_H						83.0f
#define DEFAULT_CURRENT_COLOR_S						100.0f
#define DEFAULT_CURRENT_COLOR_V						73.0f
#define DEFAULT_CURRENT_COLOR_A						100.0f

#define DEFAULT_FLOAT_VALIDATOR_TEXT				L"7"
#define DEFAULT_INTEGER_VALIDATOR_TEXT				L"3"

#define MINIMUM_HUE_VALUE							0.0f
#define MAXIMUM_HUE_VALUE							360.0f

#define MINIMUM_SATURATION_VALUE					0.0f
#define MAXIMUM_SATURATION_VALUE					100.0f

#define MINIMUM_VALUE								0.0f
#define MAXIMUM_VALUE								100.0f

#define MINIMUM_RED_VALUE							0
#define MAXIMUM_RED_VALUE							255

#define MINIMUM_GREEN_VALUE							0
#define MAXIMUM_GREEN_VALUE							255

#define MINIMUM_BLUE_VALUE							0
#define MAXIMUM_BLUE_VALUE							255

#define MINIMUM_ALPHA_VALUE							0
#define MAXIMUM_ALPHA_VALUE							255

#define MAX_BUFFER_SYMBOLS							128

#define PIXEL_PERFECT_LOCATION_OFFSET_X				0.1f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y				0.1f

#define TEXTURE										L"Textures/System/Default_Diffuse.tga"

#define DEFAULT_SAVED_COLOR_H						0.0f
#define DEFAULT_SAVED_COLOR_S						0.0f
#define DEFAULT_SAVED_COLOR_V						100.0f
#define DEFAULT_SAVED_COLOR_A						0.0f

#define CHECKER_ELEMENT_WIDTH						0.11f
#define CHECKER_ELEMENT_HEIGHT						0.11f

#define CHECKER_COLOR_ONE_R							0
#define CHECKER_COLOR_ONE_G							0
#define CHECKER_COLOR_ONE_B							0
#define CHECKER_COLOR_ONE_A							255

#define CHECKER_COLOR_TWO_R							255
#define CHECKER_COLOR_TWO_G							255
#define CHECKER_COLOR_TWO_B							255
#define CHECKER_COLOR_TWO_A							255

#define HUE_CIRCLE_INNER_RADIUS						2.49720f
#define HUE_CIRCLE_OUTER_RADIUS						3.11479f

#define COLOR_SELECTOR_PROJECTION_NEAR				0.0f
#define COLOR_SELECTOR_PROJECTION_FAR				7.0f
#define COLOR_SELECTOR_VISIBLE_Z					3.0f

#define COLOR_SELECTOR_UNLIT_COLOR_R				255
#define COLOR_SELECTOR_UNLIT_COLOR_G				255
#define COLOR_SELECTOR_UNLIT_COLOR_B				255
#define COLOR_SELECTOR_UNLIT_COLOR_A				255

#define COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_R		0.0f
#define COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_G		0.0f
#define COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_B		0.0f
#define COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_A		0.0f

#define COLOR_SELECTOR_HUE_MARKER_COLOR_R			255
#define COLOR_SELECTOR_HUE_MARKER_COLOR_G			255
#define COLOR_SELECTOR_HUE_MARKER_COLOR_B			255
#define COLOR_SELECTOR_HUE_MARKER_COLOR_A			255

#define COLOR_SELECTOR_MSAA_FACTOR					4.0f
#define COLOR_SELECTOR_SAMPLE_BORDER_SIDE			0.13426f

#define EQUILATERAL_TRIANGLE_A_X					-0.500181f
#define EQUILATERAL_TRIANGLE_A_Y					0.864750f
#define EQUILATERAL_TRIANGLE_A_Z					0.0f

#define EQUILATERAL_TRIANGLE_B_X					1.000591f
#define EQUILATERAL_TRIANGLE_B_Y					0.0f
#define EQUILATERAL_TRIANGLE_B_Z					0.0f

#define EQUILATERAL_TRIANGLE_C_X					-0.500181f
#define EQUILATERAL_TRIANGLE_C_Y					-0.864750f
#define EQUILATERAL_TRIANGLE_C_Z					0.0f

#define ALPHA_RGBA_TO_ALPHA_HSVA					0.392156f


class EMColorRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*				surface;
		GXTexture2D					texture;
		GXTexture2D					checkerTexture;
		GXVec4						colorRGBA;
		EMCheckerGeneratorMaterial	checkerGeneratorMaterial;
		GLuint						fbo;
		EMMesh						screenQuad;
		GXOpenGLState				openGLState;

	public:
		explicit EMColorRenderer ( GXUIInput* widget );
		~EMColorRenderer () override;

		GXVoid SetColorHSVA ( const GXVec4 &color );

	protected:
		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

	private:
		GXVoid UpdateCheckerTexture ();
};

EMColorRenderer::EMColorRenderer ( GXUIInput* widget ) :
GXWidgetRenderer ( widget ), screenQuad ( L"3D Models/System/ScreenQuad.stm" )
{
	const GXAABB& boundsLocal = widget->GetBoundsLocal ();
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ) );
	texture = GXTexture2D::LoadTexture ( TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );
	
	GXVec4 colorHSVA ( DEFAULT_SAVED_COLOR_H, DEFAULT_SAVED_COLOR_S, DEFAULT_SAVED_COLOR_V, DEFAULT_SAVED_COLOR_A );
	SetColorHSVA ( colorHSVA );

	checkerGeneratorMaterial.SetColorOne ( CHECKER_COLOR_ONE_R, CHECKER_COLOR_ONE_G, CHECKER_COLOR_ONE_B, CHECKER_COLOR_ONE_A );
	checkerGeneratorMaterial.SetColorTwo ( CHECKER_COLOR_TWO_R, CHECKER_COLOR_TWO_G, CHECKER_COLOR_TWO_B, CHECKER_COLOR_TWO_A );
	checkerGeneratorMaterial.SetElementSize ( (GXUShort)( CHECKER_ELEMENT_WIDTH * gx_ui_Scale), (GXUShort)( CHECKER_ELEMENT_HEIGHT * gx_ui_Scale ) );

	glGenFramebuffers ( 1, &fbo );

	UpdateCheckerTexture ();
}

EMColorRenderer::~EMColorRenderer ()
{
	checkerTexture.FreeResources ();
	glDeleteFramebuffers ( 1, &fbo );
	GXTexture2D::RemoveTexture ( texture );
	delete surface;
}

GXVoid EMColorRenderer::SetColorHSVA ( const GXVec4 &color )
{
	GXConvertHSVAToRGBA ( colorRGBA, color );
}

GXVoid EMColorRenderer::OnRefresh ()
{
	GXUIButton* button = (GXUIButton*)widget;
	const GXAABB& bounds = button->GetBoundsWorld ();

	GXFloat w = (GXFloat)surface->GetWidth ();
	GXFloat h = (GXFloat)surface->GetHeight ();

	GXImageInfo ii;

	GXColorToVec4 ( ii.color, 255, 255, 255, 255 );
	ii.insertX = 0.0f;
	ii.insertY = 0.0f;
	ii.insertWidth = w;
	ii.insertHeight = h;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;
	ii.texture = &checkerTexture;

	surface->AddImage ( ii );

	ii.color = colorRGBA;
	ii.texture = &texture;
	ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

	surface->AddImage ( ii );
}

GXVoid EMColorRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Render ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMColorRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );

	UpdateCheckerTexture ();
}

GXVoid EMColorRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMColorRenderer::UpdateCheckerTexture ()
{
	checkerTexture.FreeResources ();
	checkerTexture.InitResources ( surface->GetWidth (), surface->GetHeight (), GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );

	openGLState.Save ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, checkerTexture.GetTextureObject (), 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GL_FALSE );

	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_CULL_FACE );

	glViewport ( 0, 0, (GLsizei)checkerTexture.GetWidth (), (GLsizei)checkerTexture.GetHeight () );

	GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMColorRenderer::UpdateCheckerTexture::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	checkerGeneratorMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	checkerGeneratorMaterial.Unbind ();

	openGLState.Restore ();
}

//----------------------------------------------------------------------------------

class EMColorSelectorRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*					surface;
		GLuint							fbo;
		EMMesh							screenQuad;
		GXMeshGeometry					triangle;
		EMHueCircleGeneratorMaterial	hueCircleGeneratorMaterial;
		EMVertexColorMaterial			vertexColorMaterial;
		GXUnlitTexture2DMaterial		unlitTexture2DMaterial;
		GXTexture2D						hueTexture;
		GXTexture2D						compositeTexture;
		GXOpenGLState					openGLState;
		GXCameraOrthographic			projectionCamera;
		GXVec4							colorHSVA;
		GXVec4							colorRGBA;
		GXVec3							equilateralTriangleGeometryLocal[ 6 ];

	public:
		explicit EMColorSelectorRenderer ( GXUIInput* widget );
		~EMColorSelectorRenderer () override;

		GXVoid SetColorHSVA ( const GXVec4 &color );

	protected:
		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;

	private:
		GXVoid UpdateHueCircleTexture ();
		GXVoid UpdateCompositeTexture ();
};

EMColorSelectorRenderer::EMColorSelectorRenderer ( GXUIInput* widget ) :
GXWidgetRenderer ( widget ), screenQuad ( L"3D Models/System/ScreenQuad.stm" )
{
	const GXAABB& boundsLocal = widget->GetBoundsLocal ();
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ) );

	glGenFramebuffers ( 1, &fbo );

	triangle.SetTotalVertices ( 3 );
	triangle.SetTopology ( GL_TRIANGLES );
	GLsizei stride = sizeof ( GXVec3 ) + sizeof ( GXVec3 );
	triangle.SetBufferStream ( eGXMeshStreamIndex::CurrenVertex, 3, GL_FLOAT, stride, (const GLvoid*)0 );
	triangle.SetBufferStream ( eGXMeshStreamIndex::Color, 3, GL_FLOAT, stride, (const GLvoid*)sizeof ( GXVec3 ) );

	equilateralTriangleGeometryLocal[ 0 ] = GXCreateVec3 ( EQUILATERAL_TRIANGLE_A_X, EQUILATERAL_TRIANGLE_A_Y, EQUILATERAL_TRIANGLE_A_Z );
	equilateralTriangleGeometryLocal[ 2 ] = GXCreateVec3 ( EQUILATERAL_TRIANGLE_B_X, EQUILATERAL_TRIANGLE_B_Y, EQUILATERAL_TRIANGLE_B_Z );
	equilateralTriangleGeometryLocal[ 4 ] = GXCreateVec3 ( EQUILATERAL_TRIANGLE_C_X, EQUILATERAL_TRIANGLE_C_Y, EQUILATERAL_TRIANGLE_C_Z );

	equilateralTriangleGeometryLocal[ 1 ] = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	equilateralTriangleGeometryLocal[ 5 ] = GXCreateVec3 ( 1.0f, 1.0f, 1.0f );

	hueCircleGeneratorMaterial.SetInnerRadius ( COLOR_SELECTOR_MSAA_FACTOR * HUE_CIRCLE_INNER_RADIUS * gx_ui_Scale );
	hueCircleGeneratorMaterial.SetOuterRadius ( COLOR_SELECTOR_MSAA_FACTOR * HUE_CIRCLE_OUTER_RADIUS * gx_ui_Scale );

	unlitTexture2DMaterial.SetTexture ( hueTexture );
	unlitTexture2DMaterial.SetColor ( COLOR_SELECTOR_UNLIT_COLOR_R, COLOR_SELECTOR_UNLIT_COLOR_G, COLOR_SELECTOR_UNLIT_COLOR_B, COLOR_SELECTOR_UNLIT_COLOR_A );

	UpdateHueCircleTexture ();

	GXVec4 colorHSVA ( DEFAULT_CURRENT_COLOR_H, DEFAULT_CURRENT_COLOR_S, DEFAULT_CURRENT_COLOR_V, DEFAULT_CURRENT_COLOR_A );
	SetColorHSVA ( colorHSVA );

	GXCheckOpenGLError ();
}

EMColorSelectorRenderer::~EMColorSelectorRenderer ()
{
	hueTexture.FreeResources ();
	compositeTexture.FreeResources ();
	glDeleteFramebuffers ( 1, &fbo );
	delete surface;
}

GXVoid EMColorSelectorRenderer::SetColorHSVA ( const GXVec4 &color )
{
	colorHSVA = color;
	GXConvertHSVAToRGBA ( colorRGBA, colorHSVA );
	UpdateCompositeTexture ();
}

GXVoid EMColorSelectorRenderer::OnRefresh ()
{
	GXUIButton* button = (GXUIButton*)widget;
	const GXAABB& bounds = button->GetBoundsWorld ();

	GXFloat w = (GXFloat)surface->GetWidth ();
	GXFloat h = (GXFloat)surface->GetHeight ();

	surface->Reset ();

	GXImageInfo ii;
	GXColorToVec4 ( ii.color, 255, 255, 255, 255 );
	ii.texture = &compositeTexture;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;

	GXFloat smoothCompositeTextureWidth = compositeTexture.GetWidth () / COLOR_SELECTOR_MSAA_FACTOR;
	GXFloat smoothCompositeTextureHeight = compositeTexture.GetHeight () / COLOR_SELECTOR_MSAA_FACTOR;

	if ( smoothCompositeTextureWidth == w )
	{
		ii.insertX = 0.0f;
		ii.insertY = ( h - smoothCompositeTextureHeight ) * 0.5f;
	}
	else
	{
		ii.insertX = ( w - smoothCompositeTextureWidth ) * 0.5f;
		ii.insertY = 0.0f;
	}

	ii.insertWidth = smoothCompositeTextureWidth;
	ii.insertHeight = smoothCompositeTextureHeight;

	surface->AddImage ( ii );

	GXFloat hRadians = GXDegToRad ( colorHSVA.h );
	GXVec2 markerDirection ( gx_ui_Scale * cosf ( hRadians ), gx_ui_Scale * sinf ( hRadians ) );
	GXVec2 center ( w * 0.5f, h * 0.5f );

	GXLineInfo li;
	GXColorToVec4 ( li.color, COLOR_SELECTOR_HUE_MARKER_COLOR_R, COLOR_SELECTOR_HUE_MARKER_COLOR_G, COLOR_SELECTOR_HUE_MARKER_COLOR_B, COLOR_SELECTOR_HUE_MARKER_COLOR_A );
	li.thickness = 1.0f;
	ii.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

	GXVec2 tmp;
	GXMulVec2Scalar ( tmp, markerDirection, HUE_CIRCLE_INNER_RADIUS );
	GXSumVec2Vec2 ( tmp, tmp, center );
	li.startPoint = GXCreateVec2 ( floorf ( tmp.x + 0.5f ) + 0.1f, floorf ( tmp.y + 0.5f ) + 0.1f );

	GXMulVec2Scalar ( tmp, markerDirection, HUE_CIRCLE_OUTER_RADIUS );
	GXSumVec2Vec2 ( tmp, tmp, center );
	li.endPoint = GXCreateVec2 ( floorf ( tmp.x + 0.5f ) + 0.1f, floorf ( tmp.y + 0.5f ) + 0.1f );

	surface->AddLine ( li );

	GXFloat triangleScale = HUE_CIRCLE_INNER_RADIUS * gx_ui_Scale;
	GXTransform transform;
	transform.SetRotation ( 0.0f, 0.0f, GXDegToRad ( colorHSVA.h ) );
	transform.SetScale ( triangleScale, triangleScale, 1.0f );
	transform.SetLocation ( center.x, center.y, 0.0f );

	GXVec4 currentHueRGBA;
	GXVec4 currentHueHSVA ( colorHSVA.h, 100.0f, 100.0f, 100.0f );
	GXConvertHSVAToRGBA ( currentHueRGBA, currentHueHSVA );

	GXVec3 colorA ( 0.0f, 0.0f, 0.0f );
	GXVec3 colorB ( currentHueRGBA.r, currentHueRGBA.g, currentHueRGBA.b );
	GXVec3 colorC ( 1.0f, 1.0f, 1.0f );
	GXVec3 currentRGB ( colorRGBA.r, colorRGBA.g, colorRGBA.b );

	GXVec3 barycentricCoords;
	GXGetBarycentricCoords ( barycentricCoords, currentRGB, colorA, colorB, colorC );

	GXVec3 colorCoordsLocal;
	colorCoordsLocal.x = barycentricCoords.x * equilateralTriangleGeometryLocal[ 0 ].x + barycentricCoords.y * equilateralTriangleGeometryLocal[ 2 ].x + barycentricCoords.z * equilateralTriangleGeometryLocal[ 4 ].x;
	colorCoordsLocal.y = barycentricCoords.x * equilateralTriangleGeometryLocal[ 0 ].y + barycentricCoords.y * equilateralTriangleGeometryLocal[ 2 ].y + barycentricCoords.z * equilateralTriangleGeometryLocal[ 4 ].y;
	colorCoordsLocal.z = barycentricCoords.x * equilateralTriangleGeometryLocal[ 0 ].z + barycentricCoords.y * equilateralTriangleGeometryLocal[ 2 ].z + barycentricCoords.z * equilateralTriangleGeometryLocal[ 4 ].z;

	GXVec3 colorCoordsSurface;
	GXMulVec3Mat4AsPoint ( colorCoordsSurface, colorCoordsLocal, transform.GetCurrentFrameModelMatrix () );

	GXVec4 borderColorHSVA ( colorHSVA.h + 180.0f, 100.0f, 100.0f, 100.0f );
	if ( borderColorHSVA.h > 360.0f )
		borderColorHSVA.h -= 360.0f;

	GXFloat borderSideFactor = floorf ( COLOR_SELECTOR_SAMPLE_BORDER_SIDE * gx_ui_Scale * 0.5f + 0.5f );
	colorCoordsSurface.x = floorf ( colorCoordsSurface.x + 0.5f );
	colorCoordsSurface.y = floorf ( colorCoordsSurface.y + 0.5f );

	GXConvertHSVAToRGBA ( li.color, borderColorHSVA );

	li.startPoint = GXCreateVec2 ( colorCoordsSurface.x - borderSideFactor + 1.0f + 0.1f, colorCoordsSurface.y - borderSideFactor + 0.1f );
	li.endPoint = GXCreateVec2 ( colorCoordsSurface.x + borderSideFactor - 1.0f + 0.9f, colorCoordsSurface.y - borderSideFactor + 0.1f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( colorCoordsSurface.x + borderSideFactor + 0.1f, colorCoordsSurface.y - borderSideFactor + 1.0f + 0.1f );
	li.endPoint = GXCreateVec2 ( colorCoordsSurface.x + borderSideFactor + 0.1f, colorCoordsSurface.y + borderSideFactor - 1.0f + 0.9f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( colorCoordsSurface.x + borderSideFactor - 1.0f + 0.9f, colorCoordsSurface.y + borderSideFactor + 0.1f );
	li.endPoint = GXCreateVec2 ( colorCoordsSurface.x - borderSideFactor + 1.0f + 0.1f, colorCoordsSurface.y + borderSideFactor + 0.1f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( colorCoordsSurface.x - borderSideFactor + 0.1f, colorCoordsSurface.y + borderSideFactor - 1.0f + 0.9f );
	li.endPoint = GXCreateVec2 ( colorCoordsSurface.x - borderSideFactor + 0.1f, colorCoordsSurface.y - borderSideFactor + 1.0f + 0.1f );

	surface->AddLine ( li );
}

GXVoid EMColorSelectorRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Render ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMColorSelectorRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );

	GXUShort w = surface->GetWidth ();
	GXUShort h = surface->GetHeight ();
	GXUShort msaaSide;

	if ( w >= h )
		msaaSide = (GXUShort)( h * COLOR_SELECTOR_MSAA_FACTOR );
	else
		msaaSide = (GXUShort)( w * COLOR_SELECTOR_MSAA_FACTOR );

	if ( compositeTexture.GetWidth () == msaaSide ) return;

	UpdateHueCircleTexture ();
	UpdateCompositeTexture ();
}

GXVoid EMColorSelectorRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMColorSelectorRenderer::UpdateHueCircleTexture ()
{
	hueTexture.FreeResources ();
	compositeTexture.FreeResources ();

	openGLState.Save ();

	GXUShort w = surface->GetWidth ();
	GXUShort h = surface->GetHeight ();
	GXUShort msaaSide;

	if ( w >= h )
		msaaSide = (GXUShort)( h * COLOR_SELECTOR_MSAA_FACTOR );
	else
		msaaSide = (GXUShort)( w * COLOR_SELECTOR_MSAA_FACTOR );

	hueTexture.InitResources ( msaaSide, msaaSide, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );

	compositeTexture.InitResources ( msaaSide, msaaSide, GL_RGBA8, GX_TRUE, GL_CLAMP_TO_EDGE );
	hueCircleGeneratorMaterial.SetResolution ( msaaSide, msaaSide );
	glViewport ( 0, 0, (GLsizei)msaaSide, (GLsizei)msaaSide );
	projectionCamera.SetProjection ( (GXFloat)msaaSide, (GXFloat)msaaSide, COLOR_SELECTOR_PROJECTION_NEAR, COLOR_SELECTOR_PROJECTION_FAR );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, hueTexture.GetTextureObject (), 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GL_FALSE );
	glDisable ( GL_CULL_FACE );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_BLEND );

	const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMColorSelectorRenderer::UpdateHueCircleTexture::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	hueCircleGeneratorMaterial.Bind ( GXTransform::GetNullTransform () );
	screenQuad.Render ();
	hueCircleGeneratorMaterial.Unbind ();

	openGLState.Restore ();
}

GXVoid EMColorSelectorRenderer::UpdateCompositeTexture ()
{
	GXVec4 hueHSVA ( colorHSVA.h, 100.0f, 100.0f, 100.0f );
	GXVec4 hueRGBA;
	GXConvertHSVAToRGBA ( hueRGBA, hueHSVA );

	equilateralTriangleGeometryLocal[ 3 ] = GXCreateVec3 ( hueRGBA.r, hueRGBA.g, hueRGBA.b );
	triangle.FillVertexBuffer ( equilateralTriangleGeometryLocal, 6 * sizeof ( GXVec3 ), GL_DYNAMIC_DRAW );

	openGLState.Save ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, compositeTexture.GetTextureObject (), 0 );

	glColorMask ( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask ( GL_FALSE );

	glDisable ( GL_CULL_FACE );
	glDisable ( GL_DEPTH_TEST );
	glDisable ( GL_BLEND );

	static const GLenum buffers[ 1 ] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GXUShort w = compositeTexture.GetWidth ();
	GXUShort h = compositeTexture.GetHeight ();

	glViewport ( 0, 0, (GLsizei)w, (GLsizei)h );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
		GXLogW ( L"EMColorSelectorRenderer::UpdateCompositeTexture::Error - Что-то не так с FBO (ошибка 0x%08x)\n", status );

	static const GXFloat clearValue[ 4 ] = { COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_R, COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_G, COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_B, COLOR_SELECTOR_COMPOSITE_CLEAR_COLOR_A };
	glClearBufferfv ( GL_COLOR, 0, clearValue );

	GXFloat triangleScale = COLOR_SELECTOR_MSAA_FACTOR * HUE_CIRCLE_INNER_RADIUS * gx_ui_Scale;

	GXTransform transform;
	transform.SetRotation ( 0.0f, 0.0f, GXDegToRad ( colorHSVA.h ) );
	transform.SetScale ( triangleScale, triangleScale, 1.0f );
	transform.SetLocation ( 0.0f, 0.0f, COLOR_SELECTOR_VISIBLE_Z );

	GXCamera* oldCamera = GXCamera::GetActiveCamera ();
	GXCamera::SetActiveCamera ( &projectionCamera );

	vertexColorMaterial.Bind ( transform );
	triangle.Render ();
	vertexColorMaterial.Unbind ();

	glEnable ( GL_BLEND );

	transform.SetRotation ( 0.0f, 0.0f, 0.0f );
	transform.SetScale ( w * 0.5f, h * 0.5f, 1.0f );

	unlitTexture2DMaterial.Bind ( transform );
	screenQuad.Render ();
	unlitTexture2DMaterial.Unbind ();

	GXCamera::SetActiveCamera ( oldCamera );
	openGLState.Restore ();

	compositeTexture.UpdateMipmaps ();
}

//----------------------------------------------------------------------------------

EMUIColorPicker* EMUIColorPicker::instance = nullptr;

EMUIColorPicker& EMUIColorPicker::GetInstance ()
{
	if ( !instance )
		instance = new EMUIColorPicker ();

	return *instance;
}

EMUIColorPicker::~EMUIColorPicker ()
{
	free ( buffer );

	delete pick;
	delete cancel;
	delete bottomSeparator;

	delete transparency->GetValidator ();
	delete transparency;
	delete transparencyLabel;

	delete b->GetValidator ();
	delete b;
	delete bLabel;

	delete v->GetValidator ();
	delete v;
	delete vLabel;

	delete g->GetValidator ();
	delete g;
	delete gLabel;

	delete s->GetValidator ();
	delete s;
	delete sLabel;

	delete r->GetValidator ();
	delete r;
	delete rLabel;

	delete h->GetValidator ();
	delete h;
	delete hLabel;

	delete middleSeparator;

	for ( GXUByte i = 0; i < 16; i++ )
	{
		delete savedColors[ i ]->GetRenderer ();
		delete savedColors[ i ];
	}

	delete addColor;

	delete oldColor->GetRenderer ();
	delete oldColor;

	delete currentColor->GetRenderer ();
	delete currentColor;

	delete hsvColorWidget->GetRenderer ();
	delete hsvColorWidget;

	delete topSeparator;
	delete caption;
	delete mainPanel;

	instance = nullptr;
}

GXWidget* EMUIColorPicker::GetWidget () const
{
	return mainPanel->GetWidget ();
}

GXVoid EMUIColorPicker::PickHSVAColor ( GXVoid* handler, PFNEMONHSVACOLORPROC callback, const GXVec4 oldColorHSVA )
{
	EMColorRenderer* renderer = (EMColorRenderer*)oldColor->GetRenderer ();
	renderer->SetColorHSVA ( oldColorHSVA );
	oldColor->Refresh ();

	this->handler = handler;
	OnHSVAColor = callback;
	OnRGBAColor = nullptr;
	mainPanel->Show ();
}

GXVoid EMUIColorPicker::PickRGBAColor ( GXVoid* handler, PFNEMONRGBACOLORPROC callback, GXUByte oldRed, GXUByte oldGreen, GXUByte oldBlue, GXUByte oldAlpha )
{
	GXVec4 oldHSVA;
	GXVec4 oldRGBA;
	GXColorToVec4 ( oldRGBA, oldRed, oldGreen, oldBlue, oldAlpha );
	GXConvertRGBAToHSVA ( oldHSVA, oldRGBA );
	EMColorRenderer* renderer = (EMColorRenderer*)oldColor->GetRenderer ();
	renderer->SetColorHSVA ( oldHSVA );
	oldColor->Refresh ();

	this->handler = handler;
	OnRGBAColor = callback;
	OnHSVAColor = nullptr;
	mainPanel->Show ();
}

EMUIColorPicker::EMUIColorPicker () :
EMUI ( nullptr )
{
	mainPanel = new EMUIDraggableArea ( nullptr );
	caption = new EMUIStaticText ( mainPanel );
	topSeparator = new EMUISeparator ( mainPanel );

	GXWidget* mainPanelWidget = mainPanel->GetWidget ();
	hsvColorWidget = new GXUIInput ( mainPanelWidget, GX_TRUE );
	hsvColorWidget->SetRenderer ( new EMColorSelectorRenderer ( hsvColorWidget ) );

	currentColor = new GXUIInput ( mainPanelWidget, GX_TRUE );
	currentColor->SetRenderer ( new EMColorRenderer ( currentColor ) );

	oldColor = new GXUIInput ( mainPanelWidget, GX_TRUE );
	oldColor->SetRenderer ( new EMColorRenderer ( oldColor ) );

	addColor = new EMUIButton ( mainPanel );

	for ( GXUByte i = 0; i < 16; i++ )
	{
		GXUIInput* savedColor = new GXUIInput ( mainPanelWidget, GX_TRUE );
		savedColor->SetRenderer ( new EMColorRenderer ( savedColor ) );
		savedColors[ i ] = savedColor;
	}

	middleSeparator = new EMUISeparator ( mainPanel );

	hLabel = new EMUIStaticText ( mainPanel );
	h = new EMUIEditBox ( mainPanel );
	GXUIEditBoxFloatValidator* floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_TEXT, *( (GXUIEditBox*)h->GetWidget () ), MINIMUM_HUE_VALUE, MAXIMUM_HUE_VALUE );
	h->SetValidator ( *floatValidator );
	h->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

	rLabel = new EMUIStaticText ( mainPanel );
	r = new EMUIEditBox ( mainPanel );
	GXUIEditBoxIntegerValidator* integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_TEXT, *( (GXUIEditBox*)r->GetWidget () ), MINIMUM_RED_VALUE, MAXIMUM_RED_VALUE );
	r->SetValidator ( *integerValidator );
	r->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

	sLabel = new EMUIStaticText ( mainPanel );
	s = new EMUIEditBox ( mainPanel );
	floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_TEXT, *( (GXUIEditBox*)s->GetWidget () ), MINIMUM_SATURATION_VALUE, MAXIMUM_SATURATION_VALUE );
	s->SetValidator ( *floatValidator );
	s->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

	gLabel = new EMUIStaticText ( mainPanel );
	g = new EMUIEditBox ( mainPanel );
	integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_TEXT, *( (GXUIEditBox*)g->GetWidget () ), MINIMUM_GREEN_VALUE, MAXIMUM_GREEN_VALUE );
	g->SetValidator ( *integerValidator );
	g->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

	vLabel = new EMUIStaticText ( mainPanel );
	v = new EMUIEditBox ( mainPanel );
	floatValidator = new GXUIEditBoxFloatValidator ( DEFAULT_FLOAT_VALIDATOR_TEXT, *( (GXUIEditBox*)v->GetWidget () ), MINIMUM_VALUE, MAXIMUM_VALUE );
	v->SetValidator ( *floatValidator );
	v->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

	bLabel = new EMUIStaticText ( mainPanel );
	b = new EMUIEditBox ( mainPanel );
	integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_TEXT, *( (GXUIEditBox*)b->GetWidget () ), MINIMUM_BLUE_VALUE, MAXIMUM_BLUE_VALUE );
	b->SetValidator ( *integerValidator );
	b->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

	transparencyLabel = new EMUIStaticText ( mainPanel );
	transparency = new EMUIEditBox ( mainPanel );
	integerValidator = new GXUIEditBoxIntegerValidator ( DEFAULT_INTEGER_VALIDATOR_TEXT, *( (GXUIEditBox*)transparency->GetWidget () ), MINIMUM_ALPHA_VALUE, MAXIMUM_ALPHA_VALUE );
	transparency->SetValidator ( *integerValidator );
	transparency->SetOnFinishEditingCallback ( this, &EMUIColorPicker::OnFinishEditing );

	bottomSeparator = new EMUISeparator ( mainPanel );

	cancel = new EMUIButton ( mainPanel );
	pick = new EMUIButton ( mainPanel );

	GXLocale& locale = GXLocale::GetInstance ();

	hsvColorWidget->SetHandler ( this );
	hsvColorWidget->SetOnLeftMouseButtonDownCallback ( &EMUIColorPicker::OnLeftMouseButton );

	oldColor->SetHandler ( this );
	oldColor->SetOnLeftMouseButtonDownCallback ( &EMUIColorPicker::OnLeftMouseButton );

	for ( GXUByte i = 0; i < 16; i++ )
	{
		GXUIInput* input = savedColors[ i ];
		input->SetHandler ( this );
		input->SetOnLeftMouseButtonDownCallback ( &EMUIColorPicker::OnLeftMouseButton );
	}

	caption->SetText ( locale.GetString ( L"Color picker->Color picker" ) );
	caption->SetTextColor ( CAPTION_LABEL_COLOR_R, CAPTION_LABEL_COLOR_G, CAPTION_LABEL_COLOR_B, CAPTION_LABEL_COLOR_A );
	caption->SetAlingment ( eGXUITextAlignment::Center );

	hLabel->SetText ( locale.GetString ( L"Color picker->H" ) );
	hLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	hLabel->SetAlingment ( eGXUITextAlignment::Left );

	rLabel->SetText ( locale.GetString ( L"Color picker->R" ) );
	rLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	rLabel->SetAlingment ( eGXUITextAlignment::Left );

	sLabel->SetText ( locale.GetString ( L"Color picker->S" ) );
	sLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	sLabel->SetAlingment ( eGXUITextAlignment::Left );

	gLabel->SetText ( locale.GetString ( L"Color picker->G" ) );
	gLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	gLabel->SetAlingment ( eGXUITextAlignment::Left );

	vLabel->SetText ( locale.GetString ( L"Color picker->V" ) );
	vLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	vLabel->SetAlingment ( eGXUITextAlignment::Left );

	bLabel->SetText ( locale.GetString ( L"Color picker->B" ) );
	bLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	bLabel->SetAlingment ( eGXUITextAlignment::Left );

	transparencyLabel->SetText ( locale.GetString ( L"Color picker->Transparency" ) );
	transparencyLabel->SetTextColor ( PROPERTY_LABEL_COLOR_R, PROPERTY_LABEL_COLOR_G, PROPERTY_LABEL_COLOR_B, PROPERTY_LABEL_COLOR_A );
	transparencyLabel->SetAlingment ( eGXUITextAlignment::Left );

	h->SetAlignment ( eGXUITextAlignment::Center );
	r->SetAlignment ( eGXUITextAlignment::Center );
	s->SetAlignment ( eGXUITextAlignment::Center );
	g->SetAlignment ( eGXUITextAlignment::Center );
	v->SetAlignment ( eGXUITextAlignment::Center );
	b->SetAlignment ( eGXUITextAlignment::Center );
	transparency->SetAlignment ( eGXUITextAlignment::Center );

	addColor->SetCaption ( L"+" );
	cancel->SetCaption ( locale.GetString ( L"Color picker->Cancel" ) );
	pick->SetCaption ( locale.GetString ( L"Color picker->Pick" ) );

	addColor->SetOnLeftMouseButtonCallback ( this, &EMUIColorPicker::OnButton );
	cancel->SetOnLeftMouseButtonCallback ( this, &EMUIColorPicker::OnButton );
	pick->SetOnLeftMouseButtonCallback ( this, &EMUIColorPicker::OnButton );

	mainPanel->SetOnResizeCallback ( this, &EMUIColorPicker::OnResize );

	OnHSVAColor = nullptr;
	OnRGBAColor = nullptr;
	handler = nullptr;

	buffer = (GXWChar*)malloc ( MAX_BUFFER_SYMBOLS * sizeof ( GXWChar ) );

	UpdateCurrentColor ( DEFAULT_CURRENT_COLOR_H, DEFAULT_CURRENT_COLOR_S, DEFAULT_CURRENT_COLOR_V, DEFAULT_CURRENT_COLOR_A );
	oldColorHSVA = currentColorHSVA;

	static const GXVec4 defaultSavedColorHSVA ( DEFAULT_SAVED_COLOR_H, DEFAULT_SAVED_COLOR_S, DEFAULT_SAVED_COLOR_V, DEFAULT_SAVED_COLOR_A );
	for ( GXUByte i = 0; i < 16; i++ )
		savedColorHSVAs[ i ] = defaultSavedColorHSVA;

	GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
	mainPanel->Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, (GXFloat)( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
	mainPanel->SetMinimumWidth ( MAIN_PANEL_MINIMUM_WIDTH * gx_ui_Scale );
	mainPanel->SetMinimumHeight ( MAIN_PANEL_MINIMUM_HEIGHT * gx_ui_Scale );
	mainPanel->Hide ();
}

GXVoid EMUIColorPicker::UpdateCurrentColor ( GXFloat hue, GXFloat saturation, GXFloat value, GXFloat alpha )
{
	currentColorHSVA = GXCreateVec4 ( hue, saturation, value, alpha );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", hue );
	h->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", saturation );
	s->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", value );
	v->SetText ( buffer );

	GXUByte r;
	GXUByte g;
	GXUByte b;
	GXUByte a;
	GXConvertHSVAToRGBA ( r, g, b, a, currentColorHSVA );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", r );
	this->r->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", g );
	this->g->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", b );
	this->b->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", a );
	transparency->SetText ( buffer );

	EMColorRenderer* currentColorRenderer = (EMColorRenderer*)currentColor->GetRenderer ();
	currentColorRenderer->SetColorHSVA ( currentColorHSVA );
	currentColor->Refresh ();

	EMColorSelectorRenderer* colorSelectorRenderer = (EMColorSelectorRenderer*)hsvColorWidget->GetRenderer ();
	colorSelectorRenderer->SetColorHSVA ( currentColorHSVA );
	hsvColorWidget->Refresh ();
}

GXVoid EMUIColorPicker::UpdateCurrentColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", red );
	this->r->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", green );
	this->g->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", blue );
	this->b->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", alpha );
	transparency->SetText ( buffer );

	GXVec4 rgbColor;
	GXColorToVec4 ( rgbColor, red, green, blue, alpha );
	GXConvertRGBAToHSVA ( currentColorHSVA, rgbColor );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", currentColorHSVA.h );
	this->h->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", currentColorHSVA.s );
	this->s->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", currentColorHSVA.v );
	this->v->SetText ( buffer );

	EMColorRenderer* currentColorRenderer = (EMColorRenderer*)currentColor->GetRenderer ();
	currentColorRenderer->SetColorHSVA ( currentColorHSVA );
	currentColor->Refresh ();

	EMColorSelectorRenderer* colorSelectorRenderer = (EMColorSelectorRenderer*)hsvColorWidget->GetRenderer ();
	colorSelectorRenderer->SetColorHSVA ( currentColorHSVA );
	hsvColorWidget->Refresh ();
}

GXVoid EMUIColorPicker::UpdateCurrentColorWithCorrection ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	GXVec4 rgbColor;
	GXColorToVec4 ( rgbColor, red, green, blue, alpha );

	GXFloat oldHue = currentColorHSVA.h;
	GXConvertRGBAToHSVA ( currentColorHSVA, rgbColor );

	GXUByte correctedRed;
	GXUByte correctedGreen;
	GXUByte correctedBlue;
	GXUByte correctedAlpha;

	currentColorHSVA.h = oldHue;
	GXConvertHSVAToRGBA ( correctedRed, correctedGreen, correctedBlue, correctedAlpha, currentColorHSVA );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", correctedRed );
	this->r->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", correctedGreen );
	this->g->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", correctedBlue );
	this->b->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%hhu", correctedAlpha );
	transparency->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", currentColorHSVA.h );
	this->h->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", currentColorHSVA.s );
	this->s->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", currentColorHSVA.v );
	this->v->SetText ( buffer );

	EMColorRenderer* currentColorRenderer = (EMColorRenderer*)currentColor->GetRenderer ();
	currentColorRenderer->SetColorHSVA ( currentColorHSVA );
	currentColor->Refresh ();

	EMColorSelectorRenderer* colorSelectorRenderer = (EMColorSelectorRenderer*)hsvColorWidget->GetRenderer ();
	colorSelectorRenderer->SetColorHSVA ( currentColorHSVA );
	hsvColorWidget->Refresh ();
}

GXVoid GXCALL EMUIColorPicker::OnButton ( GXVoid* handler, GXUIButton& button, GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state == eGXMouseButtonState::Down ) return;

	EMUIColorPicker* colorPicker = (EMUIColorPicker*)handler;
	
	if ( &button == colorPicker->addColor->GetWidget () )
	{
		memmove ( colorPicker->savedColorHSVAs + 1, colorPicker->savedColorHSVAs, 15 * sizeof ( GXVec4 ) );
		colorPicker->savedColorHSVAs[ 0 ] = colorPicker->currentColorHSVA;
		
		for ( GXUByte i = 0; i < 16; i++ )
		{
			EMColorRenderer* renderer = (EMColorRenderer*)colorPicker->savedColors[ i ]->GetRenderer ();
			renderer->SetColorHSVA ( colorPicker->savedColorHSVAs[ i ] );
			colorPicker->savedColors[ i ]->Refresh ();
		}

		return;
	}
	else if ( &button == colorPicker->pick->GetWidget () )
	{
		if ( colorPicker->OnHSVAColor )
			colorPicker->OnHSVAColor ( colorPicker->handler, colorPicker->currentColorHSVA.h, colorPicker->currentColorHSVA.s, colorPicker->currentColorHSVA.v, colorPicker->currentColorHSVA.a );
		else
		{
			GXUByte red;
			GXUByte green;
			GXUByte blue;
			GXUByte alpha;
			swscanf_s ( colorPicker->r->GetText (), L"%hhu", &red );
			swscanf_s ( colorPicker->g->GetText (), L"%hhu", &green );
			swscanf_s ( colorPicker->b->GetText (), L"%hhu", &blue );
			swscanf_s ( colorPicker->transparency->GetText (), L"%hhu", &alpha );
			colorPicker->OnRGBAColor ( handler, red, green, blue, alpha );
		}

		colorPicker->mainPanel->Hide ();
		return;
	}

	colorPicker->mainPanel->Hide ();
}

GXVoid GXCALL EMUIColorPicker::OnLeftMouseButton ( GXVoid* handler, GXUIInput& input, GXFloat x, GXFloat y )
{
	EMUIColorPicker* colorPicker = (EMUIColorPicker*)handler;

	if ( &input == colorPicker->hsvColorWidget )
	{
		GXVec3 center;
		GXGetAABBCenter ( center, colorPicker->hsvColorWidget->GetBoundsWorld () );
		GXVec3 cursorWorld ( x, y, 0.0f );

		GXVec3 direction;
		GXSubVec3Vec3 ( direction, cursorWorld, center );

		GXFloat innerRadius = HUE_CIRCLE_INNER_RADIUS * gx_ui_Scale;
		GXFloat outerRadius = HUE_CIRCLE_OUTER_RADIUS * gx_ui_Scale;
		GXFloat radius = GXLengthVec3 ( direction );

		static const GXVec3 xAxis ( 1.0f, 0.0f, 0.0f );
		GXNormalizeVec3 ( direction );
		GXFloat cosAngle = GXDotVec3Fast ( direction, xAxis );
		GXFloat angleRadians = acosf ( cosAngle );

		if ( y < center.y )
			angleRadians = GX_MATH_TWO_PI - angleRadians;

		if ( radius >= innerRadius && radius <= outerRadius )
		{
			GXFloat angleDegrees = GXRadToDeg ( angleRadians );
			colorPicker->UpdateCurrentColor ( angleDegrees, colorPicker->currentColorHSVA.s, colorPicker->currentColorHSVA.v, colorPicker->currentColorHSVA.a );
			return;
		}

		GXTransform transform;
		transform.SetScale ( innerRadius, innerRadius, 1.0f );
		transform.SetRotation ( 0.0f, 0.0f, GXDegToRad ( colorPicker->currentColorHSVA.h ) );
		transform.SetLocation ( center );

		static const GXVec3 equilateralTriangleLocal[ 3 ] = 
		{
			GXVec3 ( EQUILATERAL_TRIANGLE_A_X, EQUILATERAL_TRIANGLE_A_Y, EQUILATERAL_TRIANGLE_A_Z),
			GXVec3 ( EQUILATERAL_TRIANGLE_B_X, EQUILATERAL_TRIANGLE_B_Y, EQUILATERAL_TRIANGLE_B_Z ),
			GXVec3 ( EQUILATERAL_TRIANGLE_C_X, EQUILATERAL_TRIANGLE_C_Y, EQUILATERAL_TRIANGLE_C_Z )
		};

		const GXMat4& modelMatrix = transform.GetCurrentFrameModelMatrix ();

		static GXVec3 equitaleralTriangleWorld[ 3 ];

		for ( GXUByte i = 0; i < 3; i++ )
			GXMulVec3Mat4AsPoint ( equitaleralTriangleWorld[ i ], equilateralTriangleLocal[ i ], modelMatrix );

		GXVec3 barycentricCoordinates;
		GXGetBarycentricCoords ( barycentricCoordinates, cursorWorld, equitaleralTriangleWorld[ 0 ], equitaleralTriangleWorld[ 1 ], equitaleralTriangleWorld[ 2 ] );

		if ( barycentricCoordinates.x < 0.0f || barycentricCoordinates.x > 1.0f || barycentricCoordinates.y < 0.0f || barycentricCoordinates.y > 1.0f || barycentricCoordinates.z < 0.0f || barycentricCoordinates.z > 1.0f ) return;

		GXVec4 currentHueRGBA;
		GXVec4 currentHueHSVA ( colorPicker->currentColorHSVA.h, 100.0f, 100.0f, 100.0f );
		GXConvertHSVAToRGBA ( currentHueRGBA, currentHueHSVA );

		GXUByte selectedRed = (GXUByte)( 255.0f * ( barycentricCoordinates.y * currentHueRGBA.r + barycentricCoordinates.z ) );
		GXUByte selectedGreen = (GXUByte)( 255.0f * ( barycentricCoordinates.y * currentHueRGBA.g + barycentricCoordinates.z ) );
		GXUByte selectedBlue = (GXUByte)( 255.0f * ( barycentricCoordinates.y * currentHueRGBA.b + barycentricCoordinates.z ) );

		colorPicker->UpdateCurrentColorWithCorrection ( selectedRed, selectedGreen, selectedBlue, (GXUByte)( colorPicker->currentColorHSVA.a * 2.55f ) );

		return;
	}
	else if ( &input == colorPicker->oldColor )
	{
		colorPicker->UpdateCurrentColor ( colorPicker->oldColorHSVA.h, colorPicker->oldColorHSVA.s, colorPicker->oldColorHSVA.v, colorPicker->oldColorHSVA.a );
		return;
	}

	for ( GXUByte i = 0; i < 16; i++ )
	{
		if ( &input != colorPicker->savedColors[ i ] ) continue;

		colorPicker->UpdateCurrentColor ( colorPicker->savedColorHSVAs[ i ].h, colorPicker->savedColorHSVAs[ i ].s, colorPicker->savedColorHSVAs[ i ].v, colorPicker->savedColorHSVAs[ i ].a );
		return;
	}
}

GXVoid GXCALL EMUIColorPicker::OnResize ( GXVoid* handler, GXUIDragableArea& /*area*/, GXFloat width, GXFloat height )
{
	EMUIColorPicker* colorPicker = (EMUIColorPicker*)handler;

	GXFloat margin = MARGIN * gx_ui_Scale;

	colorPicker->caption->Resize ( margin, height - ( MARGIN + CAPTION_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, CAPTION_HEIGHT * gx_ui_Scale );
	colorPicker->topSeparator->Resize ( margin, height - ( MARGIN + TOP_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, TOP_SEPARATOR_HEIGHT * gx_ui_Scale );
	colorPicker->hsvColorWidget->Resize ( margin, height - ( MARGIN + HSV_COLOR_WIDGET_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, HSV_COLOR_WIDGET_HEIGHT * gx_ui_Scale );

	GXFloat colorSeparatorY = height - ( MARGIN + COLOR_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale;
	GXFloat colorSeperatorWidth = ( width - 2.0f * margin ) * 0.5f;
	GXFloat colorSeparatorHeight = COLOR_SEPARATOR_HEIGHT * gx_ui_Scale;
	colorPicker->currentColor->Resize ( margin, colorSeparatorY, colorSeperatorWidth, colorSeparatorHeight );
	colorPicker->oldColor->Resize ( margin + colorSeperatorWidth, colorSeparatorY, colorSeperatorWidth, colorSeparatorHeight );

	colorPicker->addColor->Resize ( margin, height - ( MARGIN + ADD_COLOR_BUTTON_TOP_Y_OFFSET ) * gx_ui_Scale, ADD_COLOR_BUTTON_WIDTH * gx_ui_Scale, ADD_COLOR_BUTTON_HEIGHT * gx_ui_Scale );

	GXFloat savedColorX = width - margin - ( SAVED_COLOR_WIDTH * 8 + SAVED_COLOR_HORIZONTAL_SPACING * 7 ) * gx_ui_Scale;
	GXFloat savedColorTopRowY = height - ( MARGIN + SAVED_COLOR_TOP_ROW_TOP_Y_OFFSET ) * gx_ui_Scale;
	GXFloat savedColorBottomRowY = savedColorTopRowY - ( SAVED_COLOR_VERTICAL_SPACING + SAVED_COLOR_HEIGHT ) * gx_ui_Scale;
	GXFloat savedColorWidth = SAVED_COLOR_WIDTH * gx_ui_Scale;
	GXFloat savedColorHeight = SAVED_COLOR_HEIGHT * gx_ui_Scale;
	GXFloat savedColorHorizontalStep = ( SAVED_COLOR_WIDTH + SAVED_COLOR_HORIZONTAL_SPACING ) * gx_ui_Scale;
	for ( GXUByte i = 0; i < 8; i++ )
	{
		colorPicker->savedColors[ i ]->Resize ( savedColorX, savedColorTopRowY, savedColorWidth, savedColorHeight );
		colorPicker->savedColors[ i + 8 ]->Resize ( savedColorX, savedColorBottomRowY, savedColorWidth, savedColorHeight );

		savedColorX += savedColorHorizontalStep;
	}

	colorPicker->middleSeparator->Resize ( margin, height - ( MARGIN + MIDDLE_SEPARATOR_TOP_Y_OFFSET ) * gx_ui_Scale, width - 2.0f * margin, MIDDLE_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat propertyY = height - margin - H_PROPERTY_LABEL_TOP_Y_OFFSET * gx_ui_Scale;
	GXFloat propertyVerticalStep = PROPERTY_VERTICAL_STEP * gx_ui_Scale;
	GXFloat propertyLabelWidth = PROPERTY_LABEL_WIDTH * gx_ui_Scale;
	GXFloat propertyLabelHeight = PROPERTY_LABEL_HEIGHT * gx_ui_Scale;
	GXFloat editBoxWidth = EDIT_BOX_WIDTH * gx_ui_Scale;
	GXFloat editBoxHeight = EDIT_BOX_HEIGHT * gx_ui_Scale;
	GXFloat leftColumnEditBoxX = margin + propertyLabelWidth;
	GXFloat rightColumnPropertyLabelX = width - margin - editBoxWidth - propertyLabelWidth;
	GXFloat rightColumnEditBoxX = width - margin - editBoxWidth;

	colorPicker->hLabel->Resize ( margin, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->h->Resize ( leftColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	colorPicker->rLabel->Resize ( rightColumnPropertyLabelX, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->r->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	propertyY -= propertyVerticalStep;

	colorPicker->sLabel->Resize ( margin, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->s->Resize ( leftColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	colorPicker->gLabel->Resize ( rightColumnPropertyLabelX, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->g->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	propertyY -= propertyVerticalStep;

	colorPicker->vLabel->Resize ( margin, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->v->Resize ( leftColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	colorPicker->bLabel->Resize ( rightColumnPropertyLabelX, propertyY, propertyLabelWidth, propertyLabelHeight );
	colorPicker->b->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	propertyY -= propertyVerticalStep;

	colorPicker->transparencyLabel->Resize ( margin, propertyY, TRANSPARENCY_LABEL_WIDTH * gx_ui_Scale, propertyLabelHeight );
	colorPicker->transparency->Resize ( rightColumnEditBoxX, propertyY, editBoxWidth, editBoxHeight );

	colorPicker->bottomSeparator->Resize ( margin, margin + BOTTOM_SEPARATOR_BOTTOM_Y_OFFSET * gx_ui_Scale, width - 2.0f * margin, BOTTOM_SEPARATOR_HEIGHT * gx_ui_Scale );

	GXFloat buttonWidth = BUTTON_WIDTH * gx_ui_Scale;
	GXFloat buttonHeight = BUTTON_HEIGHT * gx_ui_Scale;

	colorPicker->cancel->Resize ( width - ( MARGIN + CANCEL_BUTTON_X_OFFSET ) * gx_ui_Scale, margin, buttonWidth, buttonHeight );
	colorPicker->pick->Resize ( width - margin - buttonWidth, margin, buttonWidth, buttonHeight );
}

GXVoid GXCALL EMUIColorPicker::OnFinishEditing ( GXVoid* handler, GXUIEditBox& editBox )
{
	EMUIColorPicker* colorPicker = (EMUIColorPicker*)handler;

	if ( &editBox == colorPicker->h->GetWidget () )
	{
		swscanf_s ( colorPicker->h->GetText (), L"%g", &( colorPicker->currentColorHSVA.h ) );
		colorPicker->UpdateCurrentColor ( colorPicker->currentColorHSVA.h, colorPicker->currentColorHSVA.s, colorPicker->currentColorHSVA.v, colorPicker->currentColorHSVA.a );
	}
	else if ( &editBox == colorPicker->s->GetWidget () )
	{
		swscanf_s ( colorPicker->s->GetText (), L"%g", &( colorPicker->currentColorHSVA.s ) );
		colorPicker->UpdateCurrentColor ( colorPicker->currentColorHSVA.h, colorPicker->currentColorHSVA.s, colorPicker->currentColorHSVA.v, colorPicker->currentColorHSVA.a );
	}
	else if ( &editBox == colorPicker->v->GetWidget () )
	{
		swscanf_s ( colorPicker->v->GetText (), L"%g", &( colorPicker->currentColorHSVA.v ) );
		colorPicker->UpdateCurrentColor ( colorPicker->currentColorHSVA.h, colorPicker->currentColorHSVA.s, colorPicker->currentColorHSVA.v, colorPicker->currentColorHSVA.a );
	}
	else if ( &editBox == colorPicker->r->GetWidget () || &editBox == colorPicker->g->GetWidget () || &editBox == colorPicker->b->GetWidget () || &editBox == colorPicker->transparency->GetWidget () )
	{
		GXUByte red;
		GXUByte green;
		GXUByte blue;
		GXUByte alpha;
		swscanf_s ( colorPicker->r->GetText (), L"%hhu", &red );
		swscanf_s ( colorPicker->g->GetText (), L"%hhu", &green );
		swscanf_s ( colorPicker->b->GetText (), L"%hhu", &blue );
		swscanf_s ( colorPicker->transparency->GetText (), L"%hhu", &alpha );

		colorPicker->UpdateCurrentColor ( red, green, blue, alpha );
	}
}
