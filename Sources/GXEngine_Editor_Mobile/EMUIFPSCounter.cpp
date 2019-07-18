#include <GXEngine_Editor_Mobile/EMUIFPSCounter.h>
#include <GXEngine/GXUICommon.h>
#include <GXEngine/GXRenderer.h>


#define DEFAULT_LAST_FPS            0xFFFFFFFFu

#define FONT                        L"Fonts/trebuc.ttf"
#define FONT_SIZE                   0.6f
#define FONT_COLOR_R                115u
#define FONT_COLOR_G                185u
#define FONT_COLOR_B                0u
#define FONT_COLOR_A                255u
#define CONVERT_BUFFER_SYMBOLS      10
#define LEFT_OFFSET                 0.1f
#define TOP_OFFSET                  0.01f

//---------------------------------------------------------------------------------------------------------------------

EMUIFPSCounter* EMUIFPSCounter::_instance = nullptr;

EMUIFPSCounter& EMUIFPSCounter::GetInstance ()
{
    if ( !_instance )
        _instance = new EMUIFPSCounter ();

    return *_instance;
}

EMUIFPSCounter::~EMUIFPSCounter ()
{
    delete _surface;
    _instance = nullptr;
}

void EMUIFPSCounter::Render ()
{
    GXUInt currentFPS = GXRenderer::GetInstance ().GetCurrentFPS ();

    if ( currentFPS != _lastFPS )
    {
        _surface->Reset ();
        wsprintfW ( _fpsBuffer, L"%i", currentFPS );

        GXPenInfo pi;
        pi._color.From ( FONT_COLOR_R, FONT_COLOR_G, FONT_COLOR_B, FONT_COLOR_A );
        pi._font = &_font;
        pi._overlayType = eGXImageOverlayType::SimpleReplace;
        pi._insertX = static_cast<GXFloat> ( static_cast<GXUInt> ( _surface->GetWidth () ) - _font.GetTextLength ( 0u, _fpsBuffer ) );
        pi._insertY = 0.0f;
        _surface->AddText ( pi, 0u, _fpsBuffer );

        _lastFPS = currentFPS;
    }

    _surface->Render ();
}

EMUIFPSCounter::EMUIFPSCounter ():
    _lastFPS ( DEFAULT_LAST_FPS ),
    _font ( FONT, static_cast<GXUShort> ( gx_ui_Scale * FONT_SIZE ) )
{
    GXUInt requeredSize = _font.GetTextLength ( 0u, L"99999999999" );

    GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXHudSurface" );
    _surface = new GXHudSurface ( static_cast<GXUShort> ( requeredSize ), _font.GetSize () );

    GXRenderer& renderer = GXRenderer::GetInstance ();
    GXVec3 location;
    _surface->GetLocation ( location );
    location.SetX ( renderer.GetWidth () * 0.5f - ( requeredSize * 0.5f + LEFT_OFFSET * gx_ui_Scale ) );
    location.SetY ( renderer.GetHeight () * 0.5f - ( FONT_SIZE * 0.5f + TOP_OFFSET ) * gx_ui_Scale );
    _surface->SetLocation ( location );
}
