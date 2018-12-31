#include <GXBuddhabrot/BBGame.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXSplashScreen.h>


#define BB_WINDOW_TITLE     L"GXBuddhabrot"

//---------------------------------------------------------------------------------------------------------------------

BBGame::BBGame ()
{
    // TODO or NOTHING
}

BBGame::~BBGame ()
{
    // TODO or NOTHING
}

GXVoid BBGame::OnInit ()
{
    GXRenderer& coreRenderer = GXRenderer::GetInstance ();
    coreRenderer.SetWindowName ( BB_WINDOW_TITLE );

    ShowCursor ( TRUE );
    SetCursor ( LoadCursorW ( nullptr, IDC_ARROW ) );
}

GXVoid BBGame::OnResize ( GXInt /*width*/, GXInt /*height*/ )
{
    // TODO or NOTHING
}

GXVoid BBGame::OnFrame ( GXFloat /*deltaTime*/ )
{
    GXRenderer& coreRenderer = GXRenderer::GetInstance ();

    if ( coreRenderer.IsVisible () ) return;

    coreRenderer.Show ();
    GXSplashScreen::GetInstance ().Hide ();
}

GXVoid BBGame::OnDestroy ()
{
    // TODO or NOTHING
}
