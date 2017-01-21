#include <GXEngine_Editor_Mobile/EMUIEditBox.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXTextureStorage.h>
#include <GXEngine/GXUICommon.h>
#include <GXCommon/GXCommon.h>


#define EM_DEFAULT_BACKGROUND			L"Textures/System/Default_Diffuse.tga"

#define EM_BACKGROUND_COLOR_R			49
#define EM_BACKGROUND_COLOR_G			48
#define EM_BACKGROUND_COLOR_B			48
#define EM_BACKGROUND_COLOR_A			255

#define EM_BORDER_COLOR_R				128
#define EM_BORDER_COLOR_G				128
#define EM_BORDER_COLOR_B				128
#define EM_BORDER_COLOR_A				255

#define EM_FONT_COLOR_R					115
#define EM_FONT_COLOR_G					185
#define EM_FONT_COLOR_B					0
#define EM_FONT_COLOR_A					255

#define EM_SELECTION_COLOR_R			17
#define EM_SELECTION_COLOR_G			24
#define EM_SELECTION_COLOR_B			2
#define EM_SELECTION_COLOR_A			255

#define EM_CURSOR_COLOR_R				255
#define EM_CURSOR_COLOR_G				255
#define EM_CURSOR_COLOR_B				255
#define EM_CURSOR_COLOR_A				255

#define EM_DEFAULT_WIDTH				4.5f
#define EM_DEFAULT_HEIGHT				0.6f
#define EM_DEFAULT_BOTTOM_LEFT_X		0.1f
#define EM_DEFAULT_BOTTOM_LEFT_Y		0.1f

#define EM_DEFAULT_FONT					L"Fonts/trebuc.ttf"
#define EM_DEFAULT_FONT_SIZE			0.33f
#define EM_DEFAULT_ALIGNMENT			GX_UI_TEXT_ALIGNMENT_LEFT

#define EM_DEFAULT_TEXT_LEFT_OFFSET		0.1f
#define EM_DEFAULT_TEXT_RIGHT_OFFSET	0.1f


class EMUIEditBoxRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXTexture			background;

	public:
		explicit EMUIEditBoxRenderer ( GXUIEditBox* widget );
		~EMUIEditBoxRenderer () override;

	protected:
		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIEditBoxRenderer::EMUIEditBoxRenderer ( GXUIEditBox* widget ):
GXWidgetRenderer ( widget )
{
	GXLoadTexture ( EM_DEFAULT_BACKGROUND, background );
	const GXAABB& boundsLocal = widget->GetBoundsWorld ();
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ), GX_FALSE );
}

EMUIEditBoxRenderer::~EMUIEditBoxRenderer ()
{
	delete surface;
	GXRemoveTexture ( background );
}

GXVoid EMUIEditBoxRenderer::OnRefresh ()
{
	GXUIEditBox* editBoxWidget = (GXUIEditBox*)widget;
	GXFont* font = editBoxWidget->GetFont ();
	GXFloat width = (GXFloat)surface->GetWidth ();
	GXFloat height = (GXFloat)surface->GetHeight ();
	const GXWChar* text = editBoxWidget->GetText ();

	surface->Reset ();

	GXImageInfo ii;

	ii.texture = background;
	GXColorToVec4 ( ii.color, EM_BACKGROUND_COLOR_R, EM_BACKGROUND_COLOR_G, EM_BACKGROUND_COLOR_B, EM_BACKGROUND_COLOR_A );
	ii.insertX = ii.insertY = 1.5f;
	ii.insertWidth = width - 2.0f;
	ii.insertHeight = height - 2.0f;
	ii.overlayType = GX_SIMPLE_REPLACE;

	surface->AddImage ( ii );

	if ( text )
	{
		GXFloat beginOffset = editBoxWidget->GetSelectionBeginOffset ();
		GXFloat endOffset = editBoxWidget->GetSelectionEndOffset ();

		if ( beginOffset != endOffset )
		{
			GXColorToVec4 ( ii.color, EM_SELECTION_COLOR_R, EM_SELECTION_COLOR_G, EM_SELECTION_COLOR_B, EM_SELECTION_COLOR_A );
			ii.insertX = beginOffset;
			ii.insertY = 0.0f;
			ii.insertWidth = endOffset - beginOffset;
			ii.insertHeight = height;

			surface->AddImage ( ii );
		}

		GXPenInfo pi;
		pi.font = font;
		pi.insertY = ( height - font->GetSize () * 0.6f ) * 0.5f;
		pi.overlayType = GX_ALPHA_TRANSPARENCY_PRESERVE_ALPHA;
		GXColorToVec4 ( pi.color, EM_FONT_COLOR_R, EM_FONT_COLOR_G, EM_FONT_COLOR_B, EM_FONT_COLOR_A );

		switch ( editBoxWidget->GetAlignment () )
		{
			case GX_UI_TEXT_ALIGNMENT_LEFT:
				pi.insertX = editBoxWidget->GetTextLeftOffset ();
			break;

			case GX_UI_TEXT_ALIGNMENT_RIGHT:
			{
				GXFloat w = (GXFloat)surface->GetWidth ();
				GXFloat len = (GXFloat)font->GetTextLength ( 0, text );
				pi.insertX = w - len - editBoxWidget->GetTextRightOffset ();
			}
			break;

			case GX_UI_TEXT_ALIGNMENT_CENTER:
			{
				GXFloat w = (GXFloat)surface->GetWidth ();
				GXFloat len = (GXFloat)font->GetTextLength ( 0, text );
				pi.insertX = ( w - len ) * 0.5f;
			}
			break;
		}

		surface->AddText ( pi, 0, text );
	}

	GXLineInfo li;
	li.thickness = 1.0f;
	li.overlayType = GX_SIMPLE_REPLACE;

	if ( editBoxWidget->IsActive () )
	{
		GXColorToVec4 ( li.color, EM_CURSOR_COLOR_R, EM_CURSOR_COLOR_G, EM_CURSOR_COLOR_B, EM_CURSOR_COLOR_A );
		li.startPoint = GXCreateVec3 ( editBoxWidget->GetCursorOffset () + 0.5f, 0.5f, 0.0f );
		li.endPoint = GXCreateVec3 ( li.startPoint.x, height, 0.0f );
		surface->AddLine ( li );
	}

	GXColorToVec4 ( li.color, EM_BORDER_COLOR_R , EM_BORDER_COLOR_G , EM_BORDER_COLOR_B, EM_BORDER_COLOR_A );
	
	li.startPoint = GXCreateVec3 ( 0.5f, 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( width - 0.5f, 0.5f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( width - 0.5f, 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( width - 0.5f, height - 0.5f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( width - 0.5f, height - 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( 0.5f, height - 0.5f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( 0.5f, height - 0.5f, 0.0f );
	li.endPoint = GXCreateVec3 ( 0.5f, 0.5f, 0.0f );

	surface->AddLine ( li );
}

GXVoid EMUIEditBoxRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Draw ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIEditBoxRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height, GX_FALSE );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIEditBoxRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
}

//-------------------------------------------------------------------------

EMUIEditBox::EMUIEditBox ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXUIEditBox ( parent ? parent->GetWidget () : nullptr );
	widget->SetRenderer ( new EMUIEditBoxRenderer ( widget ) );
	widget->Resize ( EM_DEFAULT_BOTTOM_LEFT_X * gx_ui_Scale, EM_DEFAULT_BOTTOM_LEFT_Y * gx_ui_Scale, EM_DEFAULT_WIDTH * gx_ui_Scale, EM_DEFAULT_HEIGHT * gx_ui_Scale );
	widget->SetFont ( EM_DEFAULT_FONT, (GXUShort)( EM_DEFAULT_FONT_SIZE * gx_ui_Scale ) );
	widget->SetAlignment ( EM_DEFAULT_ALIGNMENT );
	widget->SetTextLeftOffset ( EM_DEFAULT_TEXT_LEFT_OFFSET * gx_ui_Scale );
	widget->SetTextRightOffset ( EM_DEFAULT_TEXT_RIGHT_OFFSET * gx_ui_Scale );
}

EMUIEditBox::~EMUIEditBox ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUIEditBox::GetWidget () const
{
	return widget;
}

GXVoid EMUIEditBox::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
	widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}

GXVoid EMUIEditBox::SetText ( const GXWChar* text )
{
	widget->SetText ( text );
}

const GXWChar* EMUIEditBox::GetText () const
{
	return widget->GetText ();
}

GXVoid EMUIEditBox::SetAlignment ( eGXUITextAlignment alignment )
{
	widget->SetAlignment ( alignment );
}

eGXUITextAlignment EMUIEditBox::GetAlignment () const
{
	return widget->GetAlignment ();
}

GXVoid EMUIEditBox::SetFont ( const GXWChar* fontFile, GXUShort fontSize )
{
	widget->SetFont ( fontFile, fontSize );
}

GXFont* EMUIEditBox::GetFont () const
{
	return widget->GetFont ();
}
