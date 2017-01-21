#include <GXEngine_Editor_Mobile/EMUIStaticText.h>
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXFontStorage.h>


#define EM_STATIC_TEXT_DEFAULT_WIDTH		4.0f
#define EM_STATIC_TEXT_DEFAULT_HEIGHT		0.5f
#define EM_STATIC_TEXT_DEFAULT_FONT			L"Fonts/trebuc.ttf"
#define EM_STATIC_TEXT_DEFAULT_TEXT_SIZE	0.33f
#define EM_STATIC_TEXT_DEFAULT_COLOR_R		115
#define EM_STATIC_TEXT_DEFAULT_COLOR_G		185
#define EM_STATIC_TEXT_DEFAULT_COLOR_B		0
#define EM_STATIC_TEXT_DEFAULT_COLOR_A		255


class EMUIStaticTextRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurface*		surface;
		GXFont*				font;

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
	surface = new GXHudSurface ( (GXUShort)GXGetAABBWidth ( boundsLocal ), (GXUShort)GXGetAABBHeight ( boundsLocal ), GX_FALSE );
	font = GXGetFont ( EM_STATIC_TEXT_DEFAULT_FONT, (GXUShort)( EM_STATIC_TEXT_DEFAULT_TEXT_SIZE * gx_ui_Scale ) );
}

EMUIStaticTextRenderer::~EMUIStaticTextRenderer ()
{
	delete surface;
	GXRemoveFont ( font );
}

GXVoid EMUIStaticTextRenderer::OnRefresh ()
{
	GXUIStaticText* staticText = (GXUIStaticText*)widget;

	const GXAABB& bounds = staticText->GetBoundsWorld ();

	surface->Reset ();
	GXFloat h = (GXFloat)surface->GetHeight ();
	const GXWChar* text = staticText->GetText ();

	if ( !text ) return;

	GXPenInfo pi;
	pi.font = font;
	pi.insertY = ( h - font->GetSize () * 0.6f ) * 0.5f;
	pi.overlayType = GX_SIMPLE_REPLACE;
	GXColorToVec4 ( pi.color, EM_STATIC_TEXT_DEFAULT_COLOR_R, EM_STATIC_TEXT_DEFAULT_COLOR_G, EM_STATIC_TEXT_DEFAULT_COLOR_B, EM_STATIC_TEXT_DEFAULT_COLOR_A );	

	switch ( staticText->GetAlignment () )
	{
		case GX_UI_TEXT_ALIGNMENT_LEFT:
			pi.insertX = 0.0f;
		break;

		case GX_UI_TEXT_ALIGNMENT_RIGHT:
		{
			GXFloat w = (GXFloat)surface->GetWidth ();
			GXFloat len = (GXFloat)font->GetTextLength ( 0, staticText->GetText () );
			pi.insertX = w - len;
		}
		break;

		case GX_UI_TEXT_ALIGNMENT_CENTER:
		{
			GXFloat w = (GXFloat)surface->GetWidth ();
			GXFloat len = (GXFloat)font->GetTextLength ( 0, staticText->GetText () );
			pi.insertX = ( w - len ) * 0.5f;
		}
		break;
	}

	surface->AddText ( pi, 0, staticText->GetText () );
}

GXVoid EMUIStaticTextRenderer::OnDraw ()
{
	glDisable ( GL_DEPTH_TEST );
	surface->Draw ();
	glEnable ( GL_DEPTH_TEST );
}

GXVoid EMUIStaticTextRenderer::OnResized ( GXFloat x, GXFloat y, GXUShort width, GXUShort height )
{
	GXVec3 location;
	surface->GetLocation ( location );
	delete surface;
	surface = new GXHudSurface ( width, height, GX_FALSE );
	surface->SetLocation ( x, y, location.z );
}

GXVoid EMUIStaticTextRenderer::OnMoved ( GXFloat x, GXFloat y )
{
	GXVec3 location;
	surface->GetLocation ( location );
	surface->SetLocation ( x, y, location.z );
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

GXVoid EMUIStaticText::OnDrawMask ()
{
	//TODO
}

GXVoid EMUIStaticText::SetText ( const GXWChar* text )
{
	widget->SetText ( text );
}

const GXWChar* EMUIStaticText::GetText () const
{
	return widget->GetText ();
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
