#include <GXEngine_Editor_Mobile/EMUIButton.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXStrings.h>


#define EM_DEFAULT_WIDTH					4.0f
#define EM_DEFAULT_HEIGHT					0.5f
#define EM_DEFAULT_LEFT_BOTTOM_X			0.1f
#define EM_DEFAULT_LEFT_BOTTOM_Y			0.1f

#define EM_DEFAULT_CAPTION					L"Êíîïêà"
#define EM_DEFAULT_FONT						L"Fonts/trebuc.ttf"
#define EM_DEFAULT_FONT_SIZE				0.33f

#define EM_DISABLE_BACKGROUND_COLOR_R		29
#define EM_DISABLE_BACKGROUND_COLOR_G		29
#define EM_DISABLE_BACKGROUND_COLOR_B		29
#define EM_DISABLE_BACKGROUND_COLOR_A		255

#define EM_DISABLE_FONT_COLOR_R				60
#define EM_DISABLE_FONT_COLOR_G				100
#define EM_DISABLE_FONT_COLOR_B				16
#define EM_DISABLE_FONT_COLOR_A				255

#define EM_HIGHLIGHTED_BACKGROUND_COLOR_R	49
#define EM_HIGHLIGHTED_BACKGROUND_COLOR_G	48
#define EM_HIGHLIGHTED_BACKGROUND_COLOR_B	48
#define EM_HIGHLIGHTED_BACKGROUND_COLOR_A	255

#define EM_HIGHLIGHTED_FONT_COLOR_R			115
#define EM_HIGHLIGHTED_FONT_COLOR_G			185
#define EM_HIGHLIGHTED_FONT_COLOR_B			0
#define EM_HIGHLIGHTED_FONT_COLOR_A			255

#define EM_PRESSED_BACKGROUND_COLOR_R		83
#define EM_PRESSED_BACKGROUND_COLOR_G		116
#define EM_PRESSED_BACKGROUND_COLOR_B		20
#define EM_PRESSED_BACKGROUND_COLOR_A		255

#define EM_PRESSED_FONT_COLOR_R				142
#define EM_PRESSED_FONT_COLOR_G				255
#define EM_PRESSED_FONT_COLOR_B				5
#define EM_PRESSED_FONT_COLOR_A				255

#define EM_NORMAL_BACKGROUND_COLOR_R		46
#define EM_NORMAL_BACKGROUND_COLOR_G		64
#define EM_NORMAL_BACKGROUND_COLOR_B		11
#define EM_NORMAL_BACKGROUND_COLOR_A		255

#define EM_NORMAL_FONT_COLOR_R				115
#define EM_NORMAL_FONT_COLOR_G				185
#define EM_NORMAL_FONT_COLOR_B				0
#define EM_NORMAL_FONT_COLOR_A				255

#define EM_BORDER_COLOR_R					128
#define EM_BORDER_COLOR_G					128
#define EM_BORDER_COLOR_B					128
#define EM_BORDER_COLOR_A					255

#define EM_BACKGROUND_TEXTURE				L"Textures/System/Default_Diffuse.tga"


class EMUIButtonRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXFont				font;
		GXWChar*			caption;
		GXTexture			background;

	public:
		explicit EMUIButtonRenderer ( GXUIButton* buttonWidget );
		~EMUIButtonRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

		GXVoid SetCaption ( const GXWChar* caption );

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIButtonRenderer::EMUIButtonRenderer ( GXUIButton* buttonWidget ):
GXWidgetRenderer ( buttonWidget )
{
	font = GXFont::GetFont ( EM_DEFAULT_FONT, (GXUShort)( EM_DEFAULT_FONT_SIZE * gx_ui_Scale ) );
	GXWcsclone ( &caption, EM_DEFAULT_CAPTION );
	background = GXTexture::LoadTexture ( EM_BACKGROUND_TEXTURE, GX_FALSE, GL_CLAMP_TO_EDGE );

	const GXAABB& boundsLocal = widget->GetBoundsLocal ();
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ) );
}

EMUIButtonRenderer::~EMUIButtonRenderer ()
{
	GXSafeFree ( caption );

	GXTexture::RemoveTexture ( background );
	GXFont::RemoveFont ( font );

	delete surface;
}

GXVoid EMUIButtonRenderer::OnRefresh ()
{
	GXUIButton* button = (GXUIButton*)widget;
	const GXAABB& bounds = button->GetBoundsWorld ();

	surface->Reset ();
	GXFloat w = (GXFloat)surface->GetWidth ();
	GXFloat h = (GXFloat)surface->GetHeight ();

	GXImageInfo ii;
	GXPenInfo pi;

	ii.texture = &background;

	if ( button->IsDisabled () )
	{
		GXColorToVec4 ( ii.color, EM_DISABLE_BACKGROUND_COLOR_R, EM_DISABLE_BACKGROUND_COLOR_G, EM_DISABLE_BACKGROUND_COLOR_B, EM_DISABLE_BACKGROUND_COLOR_A );
		GXColorToVec4 ( pi.color, EM_DISABLE_FONT_COLOR_R, EM_DISABLE_FONT_COLOR_G, EM_DISABLE_FONT_COLOR_B, EM_DISABLE_FONT_COLOR_A );
	}
	else if ( !button->IsHighlighted () )
	{
		GXColorToVec4 ( ii.color, EM_HIGHLIGHTED_BACKGROUND_COLOR_R, EM_HIGHLIGHTED_BACKGROUND_COLOR_G, EM_HIGHLIGHTED_BACKGROUND_COLOR_B, EM_HIGHLIGHTED_BACKGROUND_COLOR_A );
		GXColorToVec4 ( pi.color, EM_HIGHLIGHTED_FONT_COLOR_R, EM_HIGHLIGHTED_FONT_COLOR_G, EM_HIGHLIGHTED_FONT_COLOR_B, EM_HIGHLIGHTED_FONT_COLOR_A );
	}
	else if ( button->IsPressed () )
	{
		GXColorToVec4 ( ii.color, EM_PRESSED_BACKGROUND_COLOR_R, EM_PRESSED_BACKGROUND_COLOR_G, EM_PRESSED_BACKGROUND_COLOR_B, EM_PRESSED_BACKGROUND_COLOR_A );
		GXColorToVec4 ( pi.color, EM_PRESSED_FONT_COLOR_R, EM_PRESSED_FONT_COLOR_G, EM_PRESSED_FONT_COLOR_B, EM_PRESSED_FONT_COLOR_A );
	}
	else
	{
		GXColorToVec4 ( ii.color, EM_NORMAL_BACKGROUND_COLOR_R, EM_NORMAL_BACKGROUND_COLOR_G, EM_NORMAL_BACKGROUND_COLOR_B, EM_NORMAL_BACKGROUND_COLOR_A );
		GXColorToVec4 ( pi.color, EM_NORMAL_FONT_COLOR_R, EM_NORMAL_FONT_COLOR_G, EM_NORMAL_FONT_COLOR_B, EM_NORMAL_FONT_COLOR_A );
	}

	ii.insertX = ii.insertY = 1.5f;
	ii.insertWidth = w - 2.0f;
	ii.insertHeight = h - 2.0f;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;

	surface->AddImage ( ii );

	if ( !caption ) return;

	GXInt len = (GXInt)font.GetTextLength ( 0, caption );
	pi.font = &font;
	pi.insertX = ( ii.insertWidth - len ) * 0.5f;
	pi.insertY = ( ii.insertHeight - font.GetSize () * 0.61f ) * 0.5f;
	pi.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;

	surface->AddText ( pi, 0, caption );

	GXLineInfo li;
	GXColorToVec4 ( li.color, EM_BORDER_COLOR_R, EM_BORDER_COLOR_G, EM_BORDER_COLOR_B, EM_BORDER_COLOR_A );
	li.thickness = 1.0f;
	li.startPoint = GXCreateVec2 ( 1.5f, 0.5f );
	li.endPoint = GXCreateVec2 ( w - 0.5f, 0.5f );
	li.overlayType = eGXImageOverlayType::SimpleReplace;

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( w - 0.5f, 1.5f );
	li.endPoint = GXCreateVec2 ( w - 0.5f, h - 0.5f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( w - 1.5f, h - 0.5f );
	li.endPoint = GXCreateVec2 ( 0.5f, h - 0.5f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec2 ( 0.5f, h - 1.5f );
	li.endPoint = GXCreateVec2 ( 0.5f, 0.5f );

	surface->AddLine ( li );
}

GXVoid EMUIButtonRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Render ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIButtonRenderer::SetCaption ( const GXWChar* caption )
{
	GXSafeFree ( this->caption );

	if ( caption )
		GXWcsclone ( &this->caption, caption );
	else
		caption = nullptr;
}

GXVoid EMUIButtonRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIButtonRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

//----------------------------------------------------------------------------------

EMUIButton::EMUIButton ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXUIButton ( parent ? parent->GetWidget () : 0 );
	widget->Resize ( EM_DEFAULT_LEFT_BOTTOM_X * gx_ui_Scale, EM_DEFAULT_LEFT_BOTTOM_Y * gx_ui_Scale, EM_DEFAULT_WIDTH * gx_ui_Scale, EM_DEFAULT_HEIGHT * gx_ui_Scale );
	widget->SetRenderer ( new EMUIButtonRenderer ( widget ) );
}

EMUIButton::~EMUIButton ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUIButton::GetWidget () const
{
	return widget;
}

GXVoid EMUIButton::Enable ()
{
	widget->Enable ();
}

GXVoid EMUIButton::Disable ()
{
	widget->Disable ();
}

GXVoid EMUIButton::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
	widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}

GXVoid EMUIButton::SetCaption ( const GXWChar* caption )
{
	EMUIButtonRenderer* renderer = (EMUIButtonRenderer*)widget->GetRenderer ();
	renderer->SetCaption ( caption );
	widget->Redraw ();
}

GXVoid EMUIButton::Show ()
{
	widget->Show ();
}

GXVoid EMUIButton::Hide ()
{
	widget->Hide ();
}

GXVoid EMUIButton::SetOnLeftMouseButtonCallback ( GXVoid* handler, PFNGXONMOUSEBUTTONPROC callback )
{
	widget->SetOnLeftMouseButtonCallback ( handler, callback );
}
