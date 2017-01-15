//version 1.11

#include <GXEngine/GXCore.h>
#include <GXEngine/GXInput.h>
#include <GXEngine/GXRenderer.h>
#include <GXEngine/GXSoundMixer.h>
#include <GXEngine/GXNetwork.h>
#include <GXEngine/GXTouchSurface.h>
#include <GXEngine/GXLocale.h>
#include <GXEngine/GXEngineSettings.h>
#include <GXEngine/GXPhysXFrontend.h>
#include <GXEngine/GXFont.h>
#include <GXEngine/GXFontStorage.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXSoundStorage.h>
#include <GXEngine/GXTextureStorage.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXCommon/GXCFGLoader.h>


GXBool					GXCore::loopFlag = GX_TRUE;
GXCore*					GXCore::instance = nullptr;


GXCore::~GXCore ()
{
	delete GXLocale::GetInstance ();
	delete GXTouchSurface::GetInstance ();
	delete GXNetServer::GetInstance ();
	delete GXNetClient::GetInstance ();
	delete GXInput::GetInstance ();

	GXDestroyPhysX ();

	delete GXRenderer::GetInstance ();
	delete GXSoundMixer::GetInstance ();

	GXSoundDestroy ();

	GXFontDestroy ();
	GXLogDestroy ();
}

GXVoid GXCore::Start ( PFNGXONGAMEINITPROC onGameInit, PFNGXONGAMECLOSEPROC onGameClose )
{
	GXSoundMixer* soundMixer = GXSoundMixer::GetInstance ();
	soundMixer->Start ();

	GXRenderer* renderer = GXRenderer::GetInstance ();
	renderer->Start ();

	GXInput* input = GXInput::GetInstance ();
	input->Start ();

	if ( onGameInit )
		onGameInit ();

	while ( loopFlag )
		Sleep ( 30 );

	input->Suspend ();
	renderer->Suspend ();
	soundMixer->Suspend ();

	if ( onGameClose )
		onGameClose ();

	input->Shutdown ();
	renderer->Shutdown ();
	soundMixer->Shutdown ();

	CheckMemoryLeak ();
}

GXVoid GXCore::Exit ()
{
	loopFlag = GX_FALSE;
}

GXCore* GXCALL GXCore::GetInstance ()
{
	if ( !instance )
		instance = new GXCore ();

	return instance;
}

GXCore::GXCore ()
{
	GXLogInit ();

	GXEngineConfiguration config;
	GXLoadCFG ( config );

	gx_EngineSettings.rendererWidth = config.usRendererWidthResoluton;
	gx_EngineSettings.rendererHeight = config.usRendererHeightResoluton;

	gx_EngineSettings.potWidth = 1;
	for ( ; gx_EngineSettings.potWidth < gx_EngineSettings.rendererWidth; gx_EngineSettings.potWidth <<= 1 );
	gx_EngineSettings.potHeight = 1;
	for ( ; gx_EngineSettings.potHeight < gx_EngineSettings.rendererHeight; gx_EngineSettings.potHeight <<=  1 );

	gx_EngineSettings.windowed = config.bIsWindowedMode;
	gx_EngineSettings.vSync = config.bVSync;
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

	GXRenderer::GetInstance ();

	if ( !GXSoundInit () )
	{
		GXLogW ( L"GXCore::GXCore::Error - Инициализация звукового модуля провалена\n" );
		GXDebugBox ( L"Инициализация звукового модуля провалена" );
	}

	GXSoundMixer::GetInstance ();

	if ( !GXFontInit () )
	{
		GXLogW ( L"GXCore::GXCore::Error - Инициализация модуля шрифтов провалена\n" );
		GXDebugBox ( L"Инициализация модуля шрифтов провалена" );
	}

	GXNetServer::GetInstance ();
	GXNetClient::GetInstance ();

	GXTouchSurface::GetInstance ();
	GXLocale::GetInstance ();

	SetCurrentDirectoryW ( L"../.." );

	loopFlag = GX_TRUE;
}

GXVoid GXCore::CheckMemoryLeak ()
{
	const GXWChar* lastFont = nullptr;
	GXUInt fonts = GXGetTotalFontStorageObjects ( &lastFont );

	const GXWChar* lastVS = nullptr;
	const GXWChar* lastGS = nullptr;
	const GXWChar* lastFS = nullptr;
	GXUInt shaders = GXGetTotalShaderStorageObjects ( &lastVS, &lastGS, &lastFS );

	GXWChar* lastSound = nullptr;
	GXUInt sounds = GXGetTotalSoundStorageObjects ( &lastSound );

	const GXWChar* lastTexture = nullptr;
	GXUInt textures = GXGetTotalTextureStorageObjects ( &lastTexture );

	const GXWChar* lastVAO = nullptr;
	GXUInt vaos = GXGetTotalVAOStorageObjects ( &lastVAO );

	if ( ( fonts + shaders + sounds + textures + vaos ) != 0 )
	{
		GXLogW ( L"GXCore::CheckMemoryLeak::Warning - Обнаружена утечка памяти\n" );

		if ( fonts > 0 )
			GXLogW ( L"Шрифты - %i [%s]\n", fonts, lastFont );

		if ( shaders > 0 )
			GXLogW ( L"Шейдерные программы - %i [%s] [%s] [%s]\n", shaders, lastVS, lastGS, lastFS );

		if ( sounds > 0 )
			GXLogW ( L"Звуковые файлы - %i [%s]\n", sounds, lastSound );

		if ( textures > 0 )
			GXLogW ( L"Текстурные объёкты  - %i [%s]\n ", textures, lastTexture );

		if ( vaos > 0 )
			GXLogW ( L"Статичные меши - %i [%s]\n", vaos, lastVAO );

		system ( "pause" );
	}
}