// version 1.18

#include <GXEngine/GXCore.h>
#include <GXEngine/GXEngineSettings.h>
#include <GXEngine/GXFont.h>
#include <GXEngine/GXInput.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXEngine/GXNetwork.h>
#include <GXEngine/GXPhysXFrontend.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXSampler.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXSoundMixer.h>
#include <GXEngine/GXSoundStorage.h>
#include <GXEngine/GXSplashScreen.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXCommon/GXCFGLoader.h>


#define INPUT_SLEEP             30u

#define SYSTEM_LANGUAGE_RU      L"../../Locale/System/RU.lng"
#define SYSTEM_LANGUAGE_EN      L"../../Locale/System/EN.lng"

//---------------------------------------------------------------------------------------------------------------------

GXBool      GXCore::_loopFlag = GX_TRUE;
GXCore*     GXCore::_instance = nullptr;

GXCore& GXCALL GXCore::GetInstance ()
{
    if ( !_instance )
        _instance = new GXCore ();

    return *_instance;
}

GXCore::~GXCore ()
{
    delete &( GXLocale::GetInstance () );
    delete &( GXTouchSurface::GetInstance () );
    delete &( GXNetServer::GetInstance () );
    delete &( GXNetClient::GetInstance () );
    delete &( GXInput::GetInstance () );

    GXDestroyPhysX ();

    delete &( GXRenderer::GetInstance () );
    delete &( GXSoundMixer::GetInstance () );

    GXSoundDestroy ();

    GXFont::DestroyFreeTypeLibrary ();

    delete &( GXSplashScreen::GetInstance () );

    CheckMemoryLeak ();

    GXLogDestroy ();
}

GXVoid GXCore::Start ( GXGame &game )
{
    GXSoundMixer& soundMixer = GXSoundMixer::GetInstance ();
    soundMixer.Start ();

    GXRenderer& renderer = GXRenderer::GetInstance ();
    renderer.Start ( game );

    GXInput& input = GXInput::GetInstance ();
    input.Start ();

    while ( _loopFlag )
        Sleep ( INPUT_SLEEP );

    input.Shutdown ();
    renderer.Shutdown ();
    soundMixer.Shutdown ();

    delete &game;
}

GXVoid GXCore::Exit ()
{
    _loopFlag = GX_FALSE;
}

GXCore::GXCore ()
{
    GXLogInit ();

    GXSplashScreen::GetInstance ().Show ();

    GXEngineConfiguration config;
    GXLoadCFG ( config );

    gx_EngineSettings._rendererWidth = config.usRendererWidthResoluton;
    gx_EngineSettings._rendererHeight = config.usRendererHeightResoluton;

    gx_EngineSettings._potWidth = 1u;

    for ( ; gx_EngineSettings._potWidth < gx_EngineSettings._rendererWidth; gx_EngineSettings._potWidth <<= 1 );

    gx_EngineSettings._potHeight = 1u;

    for ( ; gx_EngineSettings._potHeight < gx_EngineSettings._rendererHeight; gx_EngineSettings._potHeight <<=  1 );

    gx_EngineSettings._windowed = config.bIsWindowedMode;
    gx_EngineSettings._vSync = GX_FALSE;
    gx_EngineSettings._resampling = config.chResampling;
    gx_EngineSettings._anisotropy = config.chAnisotropy;
    gx_EngineSettings._dof = config.bDoF;
    gx_EngineSettings._motionBlur = config.bMotionBlur;

    GXInput::GetInstance ();

    if ( !GXGetPhysXInstance () )
    {
        GXLogA ( "GXCore::GXCore::Error - ������������� ������ ������ ���������\n" );
        GXWarningBox ( "������������� ������ ������ ���������" );
    }

    GXLocale& locale = GXLocale::GetInstance ();
    locale.LoadLanguage ( SYSTEM_LANGUAGE_EN, eGXLanguage::English );
    locale.LoadLanguage ( SYSTEM_LANGUAGE_RU, eGXLanguage::Russian );

    GXRenderer::GetInstance ();

    if ( !GXSoundInit () )
    {
        GXLogA ( "GXCore::GXCore::Error - ������������� ��������� ������ ���������\n" );
        GXWarningBox ( "������������� ��������� ������ ���������" );
    }

    GXSoundMixer::GetInstance ();

    if ( !GXFont::InitFreeTypeLibrary () )
    {
        GXLogA ( "GXCore::GXCore::Error - ������������� ������ ������� ���������\n" );
        GXWarningBox ( "������������� ������ ������� ���������" );
    }

    GXNetServer::GetInstance ();
    GXNetClient::GetInstance ();

    GXTouchSurface::GetInstance ();

    SetCurrentDirectoryW ( L"../.." );

    _loopFlag = GX_TRUE;
}

GXVoid GXCore::CheckMemoryLeak ()
{
    GXBool haveHeapLeaks = !GXMemoryInspector::CheckMemoryLeaks ();

    const GXWChar* lastFont = nullptr;
    GXUShort lastFontSize = 0u;
    GXUInt fonts = GXFont::GetTotalLoadedFonts ( &lastFont, lastFontSize );

    const GXWChar* lastVS = nullptr;
    const GXWChar* lastGS = nullptr;
    const GXWChar* lastFS = nullptr;
    GXUInt shaders = GXShaderProgram::GetTotalLoadedShaderPrograms ( &lastVS, &lastGS, &lastFS );

    const GXWChar* lastSound = nullptr;
    GXUInt sounds = GXGetTotalSoundStorageObjects ( &lastSound );

    const GXWChar* lastTexture2D = nullptr;
    GXUInt texture2Ds = GXTexture2D::GetTotalLoadedTextures ( &lastTexture2D );

    const GXWChar* lastTextureCubeMap = nullptr;
    GXUInt textureCubeMaps = GXTextureCubeMap::GetTotalLoadedTextures ( &lastTextureCubeMap );

    GLint lastWrapMode;
    eGXResampling lastResampling;
    GXFloat lastAnisotropy;
    GXUInt samplers = GXSampler::GetTotalSamplers ( lastWrapMode, lastResampling, lastAnisotropy );

    const GXWChar* lastMesh = nullptr;
    GXUInt meshes = GXMeshGeometry::GetTotalLoadedMeshes ( &lastMesh );

    const GXWChar* lastSkin = nullptr;
    GXUInt skins = GXMeshGeometry::GetTotalLoadedSkins ( &lastSkin );

    if ( ( fonts + shaders + sounds + texture2Ds + textureCubeMaps + samplers + meshes + skins ) < 1u )
    {
        if ( haveHeapLeaks )
            system ( "pause" );

        return;
    }

    GXLogA ( "GXCore::CheckMemoryLeak::Warning - ���������� ������ ������\n" );

    if ( fonts > 0u )
        GXLogA ( "������ - %u [%S] [������ %hu]\n", fonts, lastFont, lastFontSize );

    if ( shaders > 0u )
        GXLogA ( "��������� ��������� - %u [%S] [%S] [%S]\n", shaders, lastVS, lastGS, lastFS );

    if ( sounds > 0u )
        GXLogA ( "�������� ����� - %u [%S]\n", sounds, lastSound );

    if ( texture2Ds > 0u )
        GXLogA ( "���������� ������� (2D ��������) - %u [%S]\n", texture2Ds, lastTexture2D );

    if ( textureCubeMaps > 0u )
        GXLogA ( "���������� ������� (Cube map ��������) - %u [%S]\n", textureCubeMaps, lastTextureCubeMap );

    if ( samplers > 0u )
    {
        GXLogA ( "�������� - %u [", samplers );

        switch ( lastWrapMode )
        {
            case GL_REPEAT:
                GXLogA ( "GL_REPEAT, " );
            break;

            case GL_MIRRORED_REPEAT:
                GXLogA ( "GL_MIRRORED_REPEAT, " );
            break;

            case GL_CLAMP_TO_EDGE:
                GXLogA ( "GL_CLAMP_TO_EDGE, " );
            break;

            default:
                // NOTHING
            break;
        }

        switch ( lastResampling )
        {
            case eGXResampling::None:
                GXLogA ( "eGXResampling::None, " );
            break;

            case eGXResampling::Linear:
                GXLogA ( "eGXResampling::Linear, " );
            break;

            case eGXResampling::Bilinear:
                GXLogA ( "eGXResampling::Bilinear, " );
            break;

            case eGXResampling::Trilinear:
                GXLogA ( "eGXResampling::Trilinear, " );
            break;
        }

        GXLogA ( "x%g anisotropy]\n", lastAnisotropy );
    }

    if ( meshes > 0u )
        GXLogA ( "���� - %u [%S]\n", meshes, lastMesh );

    if ( skins > 0u )
        GXLogA ( "����� - %u [%S]\n", skins, lastSkin );

    system ( "pause" );
}
