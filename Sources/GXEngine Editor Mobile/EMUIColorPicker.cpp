#include <GXEngine_Editor_Mobile/EMUIColorPicker.h>
#include <GXEngine_Editor_Mobile/EMCheckerGeneratorMaterial.h>
#include <GXEngine_Editor_Mobile/EMHueCircleGeneratorMaterial.h>
#include <GXEngine_Editor_Mobile/EMMesh.h>
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

#define DEFAULT_MAIN_PANEL_WIDTH					6.71292f
#define DEFAULT_MAIN_PANEL_HEIGHT					16.19156f

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

#define HSV_FACTOR									0.016666f
#define HSV_TO_RGB_FACTOR							2.55f
#define ALPHA_FACTOR								2.55f

#define MAX_BUFFER_SYMBOLS							128

#define PIXEL_PERFECT_LOCATION_OFFSET_X				0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y				0.25f

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


class EMColorRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*				surface;
		GXTexture					texture;
		GXTexture					checkerTexture;
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
	texture = GXTexture::LoadTexture ( TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );
	
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
	GXTexture::RemoveTexture ( texture );
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
		GXTexture						hueTexture;
		GXOpenGLState					openGLState;
		GXVec4							colorRGBA;

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

	hueCircleGeneratorMaterial.SetInnerRadius ( HUE_CIRCLE_INNER_RADIUS * gx_ui_Scale );
	hueCircleGeneratorMaterial.SetOuterRadius ( HUE_CIRCLE_OUTER_RADIUS * gx_ui_Scale );

	UpdateHueCircleTexture ();

	GXVec4 colorHSVA ( DEFAULT_SAVED_COLOR_H, DEFAULT_SAVED_COLOR_S, DEFAULT_SAVED_COLOR_V, DEFAULT_SAVED_COLOR_A );
	SetColorHSVA ( colorHSVA );
}

EMColorSelectorRenderer::~EMColorSelectorRenderer ()
{
	hueTexture.FreeResources ();
	glDeleteFramebuffers ( 1, &fbo );
	delete surface;
}

GXVoid EMColorSelectorRenderer::SetColorHSVA ( const GXVec4 &color )
{
	GXConvertHSVAToRGBA ( colorRGBA, color );

	static GXVec3 bufferData[ 6 ];
	bufferData[ 0 ].x = -0.500181f;
	bufferData[ 0 ].y = 0.864750f;
	bufferData[ 0 ].z = 0.0f;

	bufferData[ 1 ].r = 0.0f;
	bufferData[ 1 ].g = 0.0f;
	bufferData[ 1 ].b = 0.0f;

	bufferData[ 2 ].x = 1.000591f;
	bufferData[ 2 ].y = 0.0f;
	bufferData[ 2 ].z = 0.0f;

	bufferData[ 3 ].r = colorRGBA.r;
	bufferData[ 3 ].g = colorRGBA.g;
	bufferData[ 3 ].b = colorRGBA.b;

	bufferData[ 4 ].x = -0.500181f;
	bufferData[ 4 ].y = -0.864750f;
	bufferData[ 4 ].z = 0.0f;

	bufferData[ 5 ].r = 1.0f;
	bufferData[ 5 ].g = 1.0f;
	bufferData[ 5 ].b = 1.0f;

	triangle.FillVertexBuffer ( bufferData, 6 * sizeof ( GXVec3 ), GL_DYNAMIC_DRAW );
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
	ii.texture = &hueTexture;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;

	GXFloat hueTextureWidth = (GXFloat)hueTexture.GetWidth ();
	GXFloat hueTextureHeight = (GXFloat)hueTexture.GetHeight ();

	if ( hueTextureWidth == w )
	{
		ii.insertX = 0.0f;
		ii.insertY = ( h - hueTextureHeight ) * 0.5f;
	}
	else
	{
		ii.insertX = ( w - hueTextureWidth ) * 0.5f;
		ii.insertY = 0.0f;
	}

	ii.insertWidth = hueTextureWidth;
	ii.insertHeight = hueTextureHeight;

	surface->AddImage ( ii );
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

	UpdateHueCircleTexture ();
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

	openGLState.Save ();

	GXUShort w = surface->GetWidth ();
	GXUShort h = surface->GetHeight ();

	if ( w >= h )
	{
		hueTexture.InitResources ( h, h, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
		hueCircleGeneratorMaterial.SetResolution ( h, h );
		glViewport ( 0, 0, (GLsizei)h, (GLsizei)h );
	}
	else
	{
		hueTexture.InitResources ( w, w, GL_RGBA8, GX_FALSE, GL_CLAMP_TO_EDGE );
		hueCircleGeneratorMaterial.SetResolution ( w, w );
		glViewport ( 0, 0, (GLsizei)w, (GLsizei)w );
	}

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
	delete transparency;
	delete transparencyLabel;
	delete b;
	delete bLabel;
	delete v;
	delete vLabel;
	delete g;
	delete gLabel;
	delete s;
	delete sLabel;
	delete r;
	delete rLabel;
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

GXVoid EMUIColorPicker::PickHSVColor ( GXVoid* handler, PFNEMONHSVCOLORPROC callback )
{
	this->handler = handler;
	OnHSVColor = callback;
	OnRGBColor = nullptr;
	mainPanel->Show ();
}

GXVoid EMUIColorPicker::PickRGBColor ( GXVoid* handler, PFNEMONRGBCOLORPROC callback )
{
	this->handler = handler;
	OnRGBColor = callback;
	OnHSVColor = nullptr;
	mainPanel->Hide ();
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

	rLabel = new EMUIStaticText ( mainPanel );
	r = new EMUIEditBox ( mainPanel );

	sLabel = new EMUIStaticText ( mainPanel );
	s = new EMUIEditBox ( mainPanel );

	gLabel = new EMUIStaticText ( mainPanel );
	g = new EMUIEditBox ( mainPanel );

	vLabel = new EMUIStaticText ( mainPanel );
	v = new EMUIEditBox ( mainPanel );

	bLabel = new EMUIStaticText ( mainPanel );
	b = new EMUIEditBox ( mainPanel );

	transparencyLabel = new EMUIStaticText ( mainPanel );
	transparency = new EMUIEditBox ( mainPanel );

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

	buffer = (GXWChar*)malloc ( MAX_BUFFER_SYMBOLS * sizeof ( GXWChar ) );

	UpdateCurrentColor ( DEFAULT_CURRENT_COLOR_H, DEFAULT_CURRENT_COLOR_S, DEFAULT_CURRENT_COLOR_V, DEFAULT_CURRENT_COLOR_A );
	oldColorHSVA = currentColorHSVA;

	GXFloat height = DEFAULT_MAIN_PANEL_HEIGHT * gx_ui_Scale;
	mainPanel->Resize ( START_MAIN_PANEL_LEFT_X_OFFSET * gx_ui_Scale, (GXFloat)( GXRenderer::GetInstance ().GetHeight () ) - height - START_MAIN_PANEL_TOP_Y_OFFSET * gx_ui_Scale, DEFAULT_MAIN_PANEL_WIDTH * gx_ui_Scale, height );
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

	EMColorRenderer* renderer = (EMColorRenderer*)currentColor->GetRenderer ();
	renderer->SetColorHSVA ( currentColorHSVA );
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

	GXFloat h;
	GXFloat s;
	GXFloat v;
	GXFloat a;
	GXConvertRGBAToHSVA ( h, s, v, a, rgbColor );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", h );
	this->h->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", s );
	this->s->SetText ( buffer );

	swprintf_s ( buffer, MAX_BUFFER_SYMBOLS, L"%.6g", v );
	this->v->SetText ( buffer );
}

GXVoid GXCALL EMUIColorPicker::OnButton ( GXVoid* handler, GXUIButton* button, GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	//TODO
	EMUIColorPicker* colorPicker = (EMUIColorPicker*)handler;
	colorPicker->mainPanel->Hide ();
}

GXVoid GXCALL EMUIColorPicker::OnLeftMouseButton ( GXVoid* handler, GXUIInput* input, GXFloat x, GXFloat y )
{
	//TODO
	GXLogW ( L"0x%016llX, %f, %f\n", (GXUPointer)input, x, y );
}

GXVoid GXCALL EMUIColorPicker::OnResize ( GXVoid* handler, GXUIDragableArea* area, GXFloat width, GXFloat height )
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
