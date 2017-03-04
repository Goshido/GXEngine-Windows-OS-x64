#include <GXEngine_Editor_Mobile/EMUIButton.h>
#include <GXEngine/GXCamera.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXFontStorage.h>
#include <GXEngine/GXTextureStorage.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXSamplerUtils.h>
#include <GXEngine/GXRenderer.h>
#include <GXCommon/GXCommon.h>
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


class EMUIButtonRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXFont*				font;
		GXWChar*			caption;
		GXTexture			background;

		GLuint				maskSampler;
		GLint				mod_view_proj_matLocation;
		GXShaderProgram		maskProgram;

	public:
		EMUIButtonRenderer ( GXUIButton* buttonWidget );
		~EMUIButtonRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

		GXVoid OnDrawMask ();

		GXVoid SetCaption ( const GXWChar* caption );

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIButtonRenderer::EMUIButtonRenderer ( GXUIButton* buttonWidget ):
GXWidgetRenderer ( buttonWidget )
{
	font = GXGetFont ( EM_DEFAULT_FONT, (GXUShort)( EM_DEFAULT_FONT_SIZE * gx_ui_Scale ) );
	GXWcsclone ( &caption, EM_DEFAULT_CAPTION );
	GXLoadTexture ( L"Textures/System/Default_Diffuse.tga", background );

	const GXAABB& boundsLocal = widget->GetBoundsLocal ();
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ), GX_FALSE );

	GXGLSamplerStruct ss;
	ss.anisotropy = 16.0f;
	ss.resampling = GX_SAMPLER_RESAMPLING_NONE;
	ss.wrap = GL_CLAMP_TO_EDGE;
	maskSampler = GXCreateSampler ( ss );

	GXGetShaderProgram ( maskProgram, L"Shaders/Editor Mobile/MaskVertexAndUV_vs.txt", 0, L"Shaders/Editor Mobile/MaskAlphaTest_fs.txt" );

	if ( !maskProgram.isSamplersTuned )
	{
		const GLuint samplerIndexes[ 1 ] = { 0 };
		const GLchar* samplerNames[ 1 ] = { "alphaSampler" };

		GXTuneShaderSamplers ( maskProgram, samplerIndexes, samplerNames, 1 );
	}

	mod_view_proj_matLocation = GXGetUniformLocation ( maskProgram.program, "mod_view_proj_mat" );
}

EMUIButtonRenderer::~EMUIButtonRenderer ()
{
	GXSafeFree ( caption );
	GXRemoveTexture ( background );
	GXRemoveFont ( font );
	GXRemoveShaderProgram ( maskProgram );

	glDeleteSamplers ( 1, &maskSampler );

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

	ii.texture = background;

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
	ii.overlayType = GX_SIMPLE_REPLACE;

	surface->AddImage ( ii );

	if ( !caption ) return;

	GXInt len = (GXInt)font->GetTextLength ( 0, caption );
	pi.font = font;
	pi.insertX = ( ii.insertWidth - len ) * 0.5f;
	pi.insertY = ( ii.insertHeight - font->GetSize () * 0.61f ) * 0.5f;
	pi.overlayType = GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA;

	surface->AddText ( pi, 0, caption );

	GXLineInfo li;
	GXColorToVec4 ( li.color, 128, 128, 128, 255 );
	li.thickness = 1.0f;
	li.startPoint = GXCreateVec3 ( 1.5f, 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( w - 0.5f, 0.5f, 0.0f );
	li.overlayType = GX_SIMPLE_REPLACE;

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( w - 0.5f, 1.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( w - 0.5f, h - 0.5f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( w - 1.5f, h - 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( 0.5f, h - 0.5f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( 0.5f, h - 1.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( 0.5f, 0.5f, 0.0f );

	surface->AddLine ( li );
}

GXVoid EMUIButtonRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Draw ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIButtonRenderer::OnDrawMask ()
{
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, surface->GetModelMatrix (), GXCamera::GetActiveCamera ()->GetViewMatrix () );

	const GXVAOInfo& surfaceVAOInfo = surface->GetMeshVAOInfo ();
	
	glUseProgram ( maskProgram.program );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.arr );
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, surface->GetTexture () );
	glBindSampler ( 0, maskSampler );

	glBindVertexArray ( surfaceVAOInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, surfaceVAOInfo.numVertices );
	glBindVertexArray ( 0 );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindSampler ( 0, 0 );

	glUseProgram ( 0 );
}

GXVoid EMUIButtonRenderer::SetCaption ( const GXWChar* caption )
{
	GXSafeFree ( this->caption );

	if ( caption )
		GXWcsclone ( &this->caption, caption );
	else
		caption = 0;
}

GXVoid EMUIButtonRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height, GX_FALSE );
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

GXVoid EMUIButton::OnDrawMask ()
{
	EMUIButtonRenderer* renderer = (EMUIButtonRenderer*)widget->GetRenderer ();
	renderer->OnDrawMask ();
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
