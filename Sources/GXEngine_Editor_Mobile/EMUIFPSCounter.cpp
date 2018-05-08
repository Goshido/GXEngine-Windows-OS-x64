#include <GXEngine_Editor_Mobile/EMUIFPSCounter.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXRenderer.h>


#define DEFAULT_LAST_FPS			0xFFFFFFFFu

#define FONT						L"Fonts/trebuc.ttf"
#define FONT_SIZE					0.6f
#define FONT_COLOR_R				115u
#define FONT_COLOR_G				185u
#define FONT_COLOR_B				0u
#define FONT_COLOR_A				255u
#define CONVERT_BUFFER_SYMBOLS		10
#define LEFT_OFFSET					0.1f
#define TOP_OFFSET					0.01f

//---------------------------------------------------------------------------------------------------------------------

EMUIFPSCounter* EMUIFPSCounter::instance = nullptr;

EMUIFPSCounter& EMUIFPSCounter::GetInstance ()
{
	if ( !instance )
		instance = new EMUIFPSCounter ();

	return *instance;
}

EMUIFPSCounter::~EMUIFPSCounter ()
{
	delete surface;
	GXFont::RemoveFont ( font );

	instance = nullptr;
}

void EMUIFPSCounter::Render ()
{
	GXUInt currentFPS = GXRenderer::GetInstance ().GetCurrentFPS ();

	if ( currentFPS != lastFPS )
	{
		surface->Reset ();
		wsprintfW ( fpsBuffer, L"%i", currentFPS );

		GXPenInfo pi;
		pi.color.From ( FONT_COLOR_R, FONT_COLOR_G, FONT_COLOR_B, FONT_COLOR_A );
		pi.font = &font;
		pi.overlayType = eGXImageOverlayType::SimpleReplace;
		pi.insertX = static_cast<GXFloat> ( static_cast<GXUInt> ( surface->GetWidth () ) - font.GetTextLength ( 0u, fpsBuffer ) );
		pi.insertY = 0.0f;
		surface->AddText ( pi, 0u, fpsBuffer );

		lastFPS = currentFPS;
	}

	surface->Render ();
}

EMUIFPSCounter::EMUIFPSCounter ():
	lastFPS ( DEFAULT_LAST_FPS )
{
	GXUShort fontSize = static_cast<GXUShort> ( gx_ui_Scale * FONT_SIZE );

	font = GXFont::GetFont ( FONT, fontSize );
	GXUInt requeredSize = font.GetTextLength ( 0u, L"99999999999" );

	surface = new GXHudSurface ( static_cast<GXUShort> ( requeredSize ), fontSize );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	GXVec3 location;
	surface->GetLocation ( location );
	location.SetX ( renderer.GetWidth () * 0.5f - ( requeredSize * 0.5f + LEFT_OFFSET * gx_ui_Scale ) );
	location.SetY ( renderer.GetHeight () * 0.5f - ( FONT_SIZE * 0.5f + TOP_OFFSET ) * gx_ui_Scale );
	surface->SetLocation ( location );
}
