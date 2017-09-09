#include <GXEngine_Editor_Mobile/EMUIStaticText.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXFont.h>


#define FONT								L"Fonts/trebuc.ttf"
#define FONT_SIZE							0.33f

#define PIXEL_PERFECT_LOCATION_OFFSET_X		0.25f
#define PIXEL_PERFECT_LOCATION_OFFSET_Y		0.25f


class EMUIStaticTextRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXFont				font;

	public:
		EMUIStaticTextRenderer ( GXUIStaticText* staticTextWidget );
		~EMUIStaticTextRenderer () override;

		GXVoid OnRefresh () override;
		GXVoid OnDraw () override;

	protected:
		GXVoid OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height ) override;
		GXVoid OnMoved ( GXFloat x, GXFloat y ) override;
};

EMUIStaticTextRenderer::EMUIStaticTextRenderer ( GXUIStaticText* staticTextWidget ):
GXWidgetRenderer ( staticTextWidget )
{
	const GXAABB& boundsLocal = widget->GetBoundsWorld ();
	surface = new GXHudSurface ( (GXUShort)boundsLocal.GetWidth (), (GXUShort)boundsLocal.GetHeight () );
	font = GXFont::GetFont ( FONT, (GXUShort)( FONT_SIZE * gx_ui_Scale ) );
}

EMUIStaticTextRenderer::~EMUIStaticTextRenderer ()
{
	delete surface;
	GXFont::RemoveFont ( font );
}

GXVoid EMUIStaticTextRenderer::OnRefresh ()
{
	GXUIStaticText* staticText = (GXUIStaticText*)widget;

	surface->Reset ();
	GXFloat h = (GXFloat)surface->GetHeight ();
	const GXWChar* text = staticText->GetText ();

	if ( !text ) return;

	GXPenInfo pi;
	pi.font = &font;
	pi.insertY = ( h - font.GetSize () * 0.6f ) * 0.5f;
	pi.overlayType = eGXImageOverlayType::SimpleReplace;
	pi.color = staticText->GetTextColor ();

	switch ( staticText->GetAlignment () )
	{
	case eGXUITextAlignment::Left:
			pi.insertX = 0.0f;
		break;

		case eGXUITextAlignment::Right:
		{
			GXFloat w = (GXFloat)surface->GetWidth ();
			GXFloat len = (GXFloat)font.GetTextLength ( 0, staticText->GetText () );
			pi.insertX = w - len;
		}
		break;

		case eGXUITextAlignment::Center:
		{
			GXFloat w = (GXFloat)surface->GetWidth ();
			GXFloat len = (GXFloat)font.GetTextLength ( 0, staticText->GetText () );
			pi.insertX = ( w - len ) * 0.5f;
		}
		break;
	}

	surface->AddText ( pi, 0, staticText->GetText () );
}

GXVoid EMUIStaticTextRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Render ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIStaticTextRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	delete surface;
	surface = new GXHudSurface ( width, height );
	surface->SetLocation ( x, y, location.GetZ () );
}

GXVoid EMUIStaticTextRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	x = truncf ( x ) + PIXEL_PERFECT_LOCATION_OFFSET_X;
	y = truncf ( y ) + PIXEL_PERFECT_LOCATION_OFFSET_Y;

	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.GetZ () );
}

//------------------------------------------------------------------------------

EMUIStaticText::EMUIStaticText ( EMUI* parent ) :
EMUI ( parent )
{
	widget = new GXUIStaticText ( parent ? parent->GetWidget () : nullptr );
	widget->SetRenderer ( new EMUIStaticTextRenderer ( widget ) );
}

EMUIStaticText::~EMUIStaticText ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXWidget* EMUIStaticText::GetWidget () const
{
	return widget;
}

GXVoid EMUIStaticText::SetText ( const GXWChar* text )
{
	widget->SetText ( text );
}

const GXWChar* EMUIStaticText::GetText () const
{
	return widget->GetText ();
}

GXVoid EMUIStaticText::SetTextColor ( GXUByte red, GXUByte green, GXUByte blue, GXUByte alpha )
{
	widget->SetTextColor ( red, green, blue, alpha );
}

const GXColorRGB& EMUIStaticText::GetTextColor () const
{
	return widget->GetTextColor ();
}

GXVoid EMUIStaticText::SetAlingment ( eGXUITextAlignment alignment )
{
	widget->SetAlignment ( alignment );
}

eGXUITextAlignment EMUIStaticText::GetAlignment () const
{
	return widget->GetAlignment ();
}

GXVoid EMUIStaticText::Resize ( GXFloat bottomLeftX, GXFloat bottomLeftY, GXFloat width, GXFloat height )
{
	widget->Resize ( bottomLeftX, bottomLeftY, width, height );
}
