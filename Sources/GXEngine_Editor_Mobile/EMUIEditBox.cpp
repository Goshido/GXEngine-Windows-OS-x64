#include <GXEngine_Editor_Mobile/EMUIEditBox.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXUICommon.h>


#define DEFAULT_BACKGROUND					L"Textures/System/Default_Diffuse.tga"

#define BACKGROUND_COLOR_R					49
#define BACKGROUND_COLOR_G					48
#define BACKGROUND_COLOR_B					48
#define BACKGROUND_COLOR_A					255

#define BORDER_COLOR_R						128
#define BORDER_COLOR_G						128
#define BORDER_COLOR_B						128
#define BORDER_COLOR_A						255

#define FONT_COLOR_R						115
#define FONT_COLOR_G						185
#define FONT_COLOR_B						0
#define FONT_COLOR_A						255

#define SELECTION_COLOR_R					17
#define SELECTION_COLOR_G					24
#define SELECTION_COLOR_B					2
#define SELECTION_COLOR_A					255

#define CURSOR_COLOR_R						255
#define CURSOR_COLOR_G						255
#define CURSOR_COLOR_B						255
#define CURSOR_COLOR_A						255

#define DEFAULT_WIDTH						4.5f
#define DEFAULT_HEIGHT						0.6f
#define DEFAULT_BOTTOM_LEFT_X				0.1f
#define DEFAULT_BOTTOM_LEFT_Y				0.1f

#define DEFAULT_FONT						L"Fonts/trebuc.ttf"
#define DEFAULT_FONT_SIZE					0.33f
#define DEFAULT_ALIGNMENT					eGXUITextAlignment::Left

#define DEFAULT_TEXT_LEFT_OFFSET			0.1f
#define DEFAULT_TEXT_RIGHT_OFFSET			0.1f

#define PIXEL_PERFECT_LOCATION_OFFSET_X		0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y		0.25f


class EMUIEditBoxRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXTexture2D			background;

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
	background = GXTexture2D::LoadTexture ( DEFAULT_BACKGROUND, GX_FALSE, GL_CLAMP_TO_EDGE, GX_FALSE );
	const GXAABB& boundsLocal = widget->GetBoundsWorld ();
	surface = new GXHudSurface ( (GXUShort)boundsLocal.GetWidth (), (GXUShort)boundsLocal.GetHeight () );
}

EMUIEditBoxRenderer::~EMUIEditBoxRenderer ()
{
	delete surface;
	GXTexture2D::RemoveTexture ( background );
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

	ii.texture = &background;
	ii.color.From ( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, BACKGROUND_COLOR_A );
	ii.insertX = ii.insertY = 1.5f;
	ii.insertWidth = width - 2.0f;
	ii.insertHeight = height - 2.0f;
	ii.overlayType = eGXImageOverlayType::SimpleReplace;

	surface->AddImage ( ii );

	if ( text )
	{
		GXFloat beginOffset = editBoxWidget->GetSelectionBeginOffset ();
		GXFloat endOffset = editBoxWidget->GetSelectionEndOffset ();

		if ( beginOffset != endOffset )
		{
			ii.color.From ( SELECTION_COLOR_R, SELECTION_COLOR_G, SELECTION_COLOR_B, SELECTION_COLOR_A );
			ii.insertX = beginOffset;
			ii.insertY = 0.0f;
			ii.insertWidth = endOffset - beginOffset;
			ii.insertHeight = height;

			surface->AddImage ( ii );
		}

		GXPenInfo pi;
		pi.font = font;
		pi.insertY = ( height - font->GetSize () * 0.6f ) * 0.5f;
		pi.overlayType = eGXImageOverlayType::AlphaTransparencyPreserveAlpha;
		pi.color.From ( FONT_COLOR_R, FONT_COLOR_G, FONT_COLOR_B, FONT_COLOR_A );

		switch ( editBoxWidget->GetAlignment () )
		{
		case eGXUITextAlignment::Left:
				pi.insertX = editBoxWidget->GetTextLeftOffset ();
			break;

			case eGXUITextAlignment::Right:
			{
				GXFloat w = (GXFloat)surface->GetWidth ();
				GXFloat len = (GXFloat)font->GetTextLength ( 0, text );
				pi.insertX = w - len - editBoxWidget->GetTextRightOffset ();
			}
			break;

			case eGXUITextAlignment::Center:
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
	li.overlayType = eGXImageOverlayType::SimpleReplace;

	if ( editBoxWidget->IsActive () )
	{
		li.color.From ( CURSOR_COLOR_R, CURSOR_COLOR_G, CURSOR_COLOR_B, CURSOR_COLOR_A );
		li.startPoint.Init ( editBoxWidget->GetCursorOffset () + 0.5f, 0.5f );
		li.endPoint.Init ( li.startPoint.GetX (), height );
		surface->AddLine ( li );
	}

	li.color.From ( BORDER_COLOR_R , BORDER_COLOR_G , BORDER_COLOR_B, BORDER_COLOR_A );
	
	li.startPoint.Init ( 0.5f, 0.5f );
	li.endPoint.Init ( width - 0.5f, 0.5f );

	surface->AddLine ( li );

	li.startPoint.Init ( width - 0.5f, 0.5f );
	li.endPoint.Init ( width - 0.5f, height - 0.5f );

	surface->AddLine ( li );

	li.startPoint.Init ( width - 0.5f, height - 0.5f );
	li.endPoint.Init ( 0.5f, height - 0.5f );

	surface->AddLine ( li );

	li.startPoint.Init ( 0.5f, height - 0.5f );
	li.endPoint.Init ( 0.5f, 0.5f );

	surface->AddLine ( li );
}

GXVoid EMUIEditBoxRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Render ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIEditBoxRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXSafeDelete ( surface );
	surface = new GXHudSurface ( width, height );
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.GetZ () );
}

GXVoid EMUIEditBoxRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.GetZ () );
}

//-------------------------------------------------------------------------

EMUIEditBox::EMUIEditBox ( EMUI* parent ):
EMUI ( parent )
{
	widget = new GXUIEditBox ( parent ? parent->GetWidget () : nullptr );
	widget->SetRenderer ( new EMUIEditBoxRenderer ( widget ) );
	widget->Resize ( DEFAULT_BOTTOM_LEFT_X * gx_ui_Scale, DEFAULT_BOTTOM_LEFT_Y * gx_ui_Scale, DEFAULT_WIDTH * gx_ui_Scale, DEFAULT_HEIGHT * gx_ui_Scale );
	widget->SetFont ( DEFAULT_FONT, (GXUShort)( DEFAULT_FONT_SIZE * gx_ui_Scale ) );
	widget->SetAlignment ( DEFAULT_ALIGNMENT );
	widget->SetTextLeftOffset ( DEFAULT_TEXT_LEFT_OFFSET * gx_ui_Scale );
	widget->SetTextRightOffset ( DEFAULT_TEXT_RIGHT_OFFSET * gx_ui_Scale );
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

GXVoid EMUIEditBox::SetValidator ( GXTextValidator& validator )
{
	widget->SetValidator ( validator );
}

GXTextValidator* EMUIEditBox::GetValidator () const
{
	return widget->GetValidator ();
}

GXVoid EMUIEditBox::SetOnFinishEditingCallback ( GXVoid* handler, PFNGXUIEDITBOXONFINISHEDITINGPROC callback )
{
	widget->SetOnFinishEditingCallback ( handler, callback );
}
