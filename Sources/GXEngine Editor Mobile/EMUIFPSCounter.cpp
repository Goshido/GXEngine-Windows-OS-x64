#include <GXEngine_Editor_Mobile/EMUIFPSCounter.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXRenderer.h>


#define FONT						L"Fonts/trebuc.ttf"
#define FONT_SIZE					0.6f
#define FONT_COLOR_R				115
#define FONT_COLOR_G				185
#define FONT_COLOR_B				0
#define FONT_COLOR_A				255
#define CONVERT_BUFFER_SYMBOLS		10
#define LEFT_OFFSET					0.1f
#define TOP_OFFSET					0.01f
#define DEFAULT_LAST_FPS			0xFFFFFFFF


EMUIFPSCounter::EMUIFPSCounter ()
{
	GXUShort fontSize = (GXUShort)( gx_ui_Scale * FONT_SIZE );

	font = GXFont::GetFont ( FONT, fontSize );
	GXUInt requeredSize = font.GetTextLength ( 0, L"99999999999" );

	surface = new GXHudSurface ( (GXUShort)requeredSize, fontSize );

	GXRenderer& renderer = GXRenderer::GetInstance ();
	GXVec3 location;
	surface->GetLocation ( location );
	location.x = renderer.GetWidth () * 0.5f - ( requeredSize * 0.5f + LEFT_OFFSET * gx_ui_Scale );
	location.y = renderer.GetHeight () * 0.5f - ( FONT_SIZE * 0.5f + TOP_OFFSET ) * gx_ui_Scale;
	surface->SetLocation ( location );

	lastFPS = DEFAULT_LAST_FPS;
}

EMUIFPSCounter::~EMUIFPSCounter ()
{
	delete surface;
	GXFont::RemoveFont ( font );
}

void EMUIFPSCounter::Render ()
{
	GXUInt currentFPS = GXRenderer::GetInstance ().GetCurrentFPS ();

	if ( currentFPS != lastFPS )
	{
		surface->Reset ();
		wsprintfW ( fpsBuffer, L"%i", currentFPS );

		GXPenInfo pi;
		GXColorToVec4 ( pi.color, FONT_COLOR_R, FONT_COLOR_G, FONT_COLOR_B, FONT_COLOR_A );
		pi.font = &font;
		pi.overlayType = eGXImageOverlayType::SimpleReplace;
		pi.insertX = (GXFloat)( (GXUInt)surface->GetWidth () - font.GetTextLength ( 0, fpsBuffer ) );
		pi.insertY = 0.0f;
		surface->AddText ( pi, 0, fpsBuffer );

		lastFPS = currentFPS;
	}

	surface->Render ();
}
