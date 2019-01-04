#include <GXBuddhabrot/BBGame.h>
#include <GXBuddhabrot/BBGUIMeta.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXSplashScreen.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXStrings.h>


#define BB_WINDOW_TITLE     L"GXBuddhabrot"

//---------------------------------------------------------------------------------------------------------------------

BBGame::BBGame ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "BBGame" )
    needToInitGUI ( GX_TRUE ),
    testLabel ( nullptr )
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

    GXCamera::SetActiveCamera ( &guiCamera );

    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_STENCIL_TEST );

    const GXColorRGB backgroundColor ( static_cast<GXUByte> ( BB_GUI_COLOR_BACKGROUND_RED ), static_cast<GXUByte> ( BB_GUI_COLOR_BACKGROUND_GREEN ), static_cast<GXUByte> ( BB_GUI_COLOR_BACKGROUND_BLUE ), static_cast<GXUByte> ( BB_GUI_COLOR_BACKGROUND_ALPHA ) );
    glClearColor ( backgroundColor.GetRed (), backgroundColor.GetRed (), backgroundColor.GetRed (), backgroundColor.GetRed () );

    ShowCursor ( TRUE );
    SetCursor ( LoadCursor ( nullptr, IDC_ARROW ) );
}

GXVoid BBGame::OnResize ( GXInt width, GXInt height )
{
    guiCamera.SetProjection ( static_cast<GXFloat> ( width ), static_cast<GXFloat> ( height ), BB_GUI_NEAR_Z, BB_GUI_FAR_Z );
    needToInitGUI = GX_TRUE;
}

GXVoid BBGame::OnFrame ( GXFloat /*deltaTime*/ )
{
    glClear ( GL_COLOR_BUFFER_BIT );

    UpdateGUI ();

    GXRenderer& coreRenderer = GXRenderer::GetInstance ();

    if ( coreRenderer.IsVisible () ) return;

    coreRenderer.Show ();
    GXSplashScreen::GetInstance ().Hide ();
}

GXVoid BBGame::OnDestroy ()
{
    GXSafeDelete ( testLabel );
}

GXVoid BBGame::InitGUI ()
{
    if ( !needToInitGUI ) return;

    if ( !testLabel )
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "BBLabel" );
        testLabel = new BBLabel ( nullptr );
    }

    testLabel->Resize ( 100.0f, 100.0f, 100.0f, 30.0f );
    testLabel->SetAlignment ( eGXUITextAlignment::Left );
    testLabel->SetText ( L"Total progress" );

    needToInitGUI = GX_FALSE;
}

GXVoid BBGame::UpdateGUI ()
{
    InitGUI ();

    GXTouchSurface& touchSurface = GXTouchSurface::GetInstance ();
    touchSurface.ExecuteMessages ();
    touchSurface.DrawWidgets ();
}
