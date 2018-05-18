// version 1.15

#include <GXEngine/GXCore.h>
#include <GXEngine/GXInput.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXSoundMixer.h>
#include <GXEngine/GXNetwork.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXSplashScreen.h>
#include <GXEngine/GXEngineSettings.h>
#include <GXEngine/GXPhysXFrontend.h>
#include <GXEngine/GXFont.h>
#include <GXEngine/GXSoundStorage.h>
#include <GXEngine/GXTexture2D.h>
#include <GXEngine/GXTextureCubeMap.h>
#include <GXEngine/GXSampler.h>
#include <GXEngine/GXShaderProgram.h>
#include <GXEngine/GXMeshGeometry.h>
#include <GXCommon/GXCFGLoader.h>


#define INPUT_SLEEP				30

#define SYSTEM_LANGUAGE_RU		L"../../Locale/System/RU.lng"
#define SYSTEM_LANGUAGE_EN		L"../../Locale/System/EN.lng"

//---------------------------------------------------------------------------------------------------------------------

GXBool		GXCore::loopFlag = GX_TRUE;
GXCore*		GXCore::instance = nullptr;

GXCore& GXCALL GXCore::GetInstance ()
{
	if ( !instance )
		instance = new GXCore ();

	return *instance;
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

	while ( loopFlag )
		Sleep ( INPUT_SLEEP );

	input.Shutdown ();
	renderer.Shutdown ();
	soundMixer.Shutdown ();

	CheckMemoryLeak ();
}

GXVoid GXCore::Exit ()
{
	loopFlag = GX_FALSE;
}

GXCore::GXCore ()
{
	GXLogInit ();

	GXSplashScreen::GetInstance ().Show ();

	GXEngineConfiguration config;
	GXLoadCFG ( config );

	gx_EngineSettings.rendererWidth = config.usRendererWidthResoluton;
	gx_EngineSettings.rendererHeight = config.usRendererHeightResoluton;

	gx_EngineSettings.potWidth = 1u;
	for ( ; gx_EngineSettings.potWidth < gx_EngineSettings.rendererWidth; gx_EngineSettings.potWidth <<= 1 );
	gx_EngineSettings.potHeight = 1u;
	for ( ; gx_EngineSettings.potHeight < gx_EngineSettings.rendererHeight; gx_EngineSettings.potHeight <<=  1 );

	gx_EngineSettings.windowed = config.bIsWindowedMode;
	gx_EngineSettings.vSync = GX_TRUE;
	gx_EngineSettings.resampling = config.chResampling;
	gx_EngineSettings.anisotropy = config.chAnisotropy;
	gx_EngineSettings.dof = config.bDoF;
	gx_EngineSettings.motionBlur = config.bMotionBlur;

	GXInput::GetInstance ();

	if ( !GXGetPhysXInstance () )
	{
		GXLogW ( L"GXCore::GXCore::Error - Инициализация модуля физики провалена\n" );
		GXDebugBox ( L"Инициализация модуля физики провалена" );
	}

	GXLocale& locale = GXLocale::GetInstance ();
	locale.LoadLanguage ( SYSTEM_LANGUAGE_EN, eGXLanguage::English );
	locale.LoadLanguage ( SYSTEM_LANGUAGE_RU, eGXLanguage::Russian );

	GXRenderer::GetInstance ();

	if ( !GXSoundInit () )
	{
		GXLogW ( L"GXCore::GXCore::Error - Инициализация звукового модуля провалена\n" );
		GXDebugBox ( L"Инициализация звукового модуля провалена" );
	}

	GXSoundMixer::GetInstance ();

	if ( !GXFont::InitFreeTypeLibrary () )
	{
		GXLogW ( L"GXCore::GXCore::Error - Инициализация модуля шрифтов провалена\n" );
		GXDebugBox ( L"Инициализация модуля шрифтов провалена" );
	}

	GXNetServer::GetInstance ();
	GXNetClient::GetInstance ();

	GXTouchSurface::GetInstance ();

	SetCurrentDirectoryW ( L"../.." );

	loopFlag = GX_TRUE;
}

GXVoid GXCore::CheckMemoryLeak ()
{
	const GXWChar* lastFont = nullptr;
	GXUShort lastFontSize = 0u;
	GXUInt fonts = GXFont::GetTotalLoadedFonts ( &lastFont, lastFontSize );

	const GXWChar* lastVS = nullptr;
	const GXWChar* lastGS = nullptr;
	const GXWChar* lastFS = nullptr;
	GXUInt shaders = GXShaderProgram::GetTotalLoadedShaderPrograms ( &lastVS, &lastGS, &lastFS );

	GXWChar* lastSound = nullptr;
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

	if ( ( fonts + shaders + sounds + texture2Ds + textureCubeMaps + samplers + meshes + skins ) < 1u ) return;

	GXLogW ( L"GXCore::CheckMemoryLeak::Warning - Обнаружена утечка памяти\n" );

	if ( fonts > 0u )
		GXLogW ( L"Шрифты - %i [%s] [размер %i]\n", fonts, lastFont, lastFontSize );

	if ( shaders > 0u )
		GXLogW ( L"Шейдерные программы - %i [%s] [%s] [%s]\n", shaders, lastVS, lastGS, lastFS );

	if ( sounds > 0u )
		GXLogW ( L"Звуковые файлы - %i [%s]\n", sounds, lastSound );

	if ( texture2Ds > 0u )
		GXLogW ( L"Текстурные объекты (2D текстуры)  - %i [%s]\n", texture2Ds, lastTexture2D );

	if ( textureCubeMaps > 0u )
		GXLogW ( L"Текстурные объекты (Cube map текстуры)- %i [%s]\n", textureCubeMaps, lastTextureCubeMap );

	if ( samplers > 0u )
	{
		GXLogW ( L"Семплеры - %i [", samplers );

		switch ( lastWrapMode )
		{
			case GL_REPEAT:
				GXLogW ( L"GL_REPEAT, " );
			break;

			case GL_MIRRORED_REPEAT:
				GXLogW ( L"GL_MIRRORED_REPEAT, " );
			break;

			case GL_CLAMP_TO_EDGE:
				GXLogW ( L"GL_CLAMP_TO_EDGE, " );
			break;

			default:
				// NOTHING
			break;
		}

		switch ( lastResampling )
		{
			case eGXResampling::None:
				GXLogW ( L"eGXResampling::None, " );
			break;

			case eGXResampling::Linear:
				GXLogW ( L"eGXResampling::Linear, " );
			break;

			case eGXResampling::Bilinear:
				GXLogW ( L"eGXResampling::Bilinear, " );
			break;

			case eGXResampling::Trilinear:
				GXLogW ( L"eGXResampling::Trilinear, " );
			break;
		}

		GXLogW ( L"x%g anisotropy]\n", lastAnisotropy );
	}

	if ( meshes > 0u )
		GXLogW ( L"Меши - %i [%s]\n", meshes, lastMesh );

	if ( meshes > 0u )
		GXLogW ( L"Скины - %i [%s]\n", skins, lastSkin );

	system ( "pause" );
}
