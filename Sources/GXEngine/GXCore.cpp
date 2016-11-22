//version 1.9

#include <GXEngine/GXCore.h>
#include <GXCommon/GXCFGLoader.h>
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXPhysics.h>
#include <GXEngine/GXFont.h>
#include <GXEngine/GXFontStorage.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXSoundStorage.h>
#include <GXEngine/GXTextureStorage.h>
#include <GXEngine/GXVAOStorage.h>


GXInput*				GXCore::input = 0;
GXRenderer*				GXCore::renderer = 0;
GXPhysics*				GXCore::physics = 0;
GXSoundMixer*			GXCore::soundMixer = 0;
GXNetServer*			GXCore::server = 0;
GXNetClient*			GXCore::client = 0;
GXTouchSurface*			GXCore::touchSurface = 0;
GXLocale*				GXCore::locale = 0;

GXBool					GXCore::loopFlag = GX_TRUE;

PFNGXONGAMEINITPROC		GXCore::OnGameInit = 0;
PFNGXONGAMECLOSEPROC	GXCore::OnGameClose = 0;


GXCore::GXCore ( PFNGXONGAMEINITPROC onGameInit, PFNGXONGAMECLOSEPROC onGameClose, const GXWChar* gameName )
{
	GXLogInit ();

	if ( !onGameInit ) GXDebugBox ( L"GXCore::GXCore::Error - Не верна стартовая функция игры" );
	if ( !onGameClose ) GXDebugBox ( L"GXCore::GXCore::Error - Не верна финишная функция игры" );

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

	input = new GXInput ( this );

	if ( !GXPhysXInit () )
	{
		GXLogW ( L"GXCore::GXCore::Error - Инициализация модуля физики провалена\n" );
		GXDebugBox ( L"Инициализация модуля физики провалена" );
	}

	physics = GXPhysicsCreate ();

	renderer = new GXRenderer ( this, gameName, 32, 8 );

	OnGameInit = onGameInit;
	OnGameClose = onGameClose;

	if ( !GXSoundInit () )
	{
		GXLogW ( L"GXCore::GXCore::Error - Инициализация звукового модуля провалена\n" );
		GXDebugBox ( L"Инициализация звукового модуля провалена" );
	}

	soundMixer = new GXSoundMixer ();

	if ( !GXFontInit () )
	{
		GXLogW ( L"GXCore::GXCore::Error - Инициализация модуля шрифтов провалена\n" );
		GXDebugBox ( L"Инициализация модуля шрифтов провалена" );
	}

	server = new GXNetServer ();
	client = new GXNetClient ();

	touchSurface = new GXTouchSurface ();
	locale = new GXLocale ();

	SetCurrentDirectoryW ( L"../.." );

	loopFlag = GX_TRUE;
}

GXCore::~GXCore ()
{
	GXSafeDelete ( locale );
	GXSafeDelete ( touchSurface );
	GXSafeDelete ( server );
	GXSafeDelete ( client );

	GXSafeDelete ( input );

	GXPhysicsDestroy ( physics );
	GXPhysXDestroy ();

	GXSafeDelete ( renderer );

	GXSafeDelete ( soundMixer );
	GXSoundDestroy ();

	GXFontDestroy ();
	GXLogDestroy ();
}

GXVoid GXCore::Start ()
{
	soundMixer->Start ();
	renderer->Start ();
	input->Start ();

	OnGameInit ();

	while ( loopFlag )
		Sleep ( 30 );

	input->Suspend ();
	renderer->Suspend ();
	soundMixer->Suspend ();

	OnGameClose ();

	input->Shutdown ();
	renderer->Shutdown ();
	soundMixer->Shutdown ();

	CheckMemoryLeak ();
}

GXVoid GXCore::Exit ()
{
	loopFlag = GX_FALSE;
}

WNDPROC GXCore::NotifyGetInputProc ()
{
	return &input->InputProc;
}

GXRenderer* GXCore::GetRenderer ()
{
	return renderer;
}

GXPhysics* GXCore::GetPhysics ()
{
	return physics;
}

GXInput* GXCore::GetInput ()
{
	return input;
}

GXSoundMixer* GXCore::GetSoundMixer ()
{
	return soundMixer;
}

GXNetServer* GXCore::GetNetServer ()
{
	return server;
}

GXNetClient* GXCore::GetNetClient ()
{
	return client;
}

GXTouchSurface* GXCore::GetTouchSurface ()
{
	return touchSurface;
}

GXLocale* GXCore::GetLocale ()
{
	return locale;
}

GXVoid GXCore::CheckMemoryLeak ()
{
	const GXWChar* lastFont;
	GXUInt fonts = GXGetTotalFontStorageObjects ( &lastFont );

	const GXWChar* lastVS;
	const GXWChar* lastGS;
	const GXWChar* lastFS;
	GXUInt shaders = GXGetTotalShaderStorageObjects ( &lastVS, &lastGS, &lastFS );

	GXWChar* lastSound;
	GXUInt sounds = GXGetTotalSoundStorageObjects ( &lastSound );

	const GXWChar* lastTexture;
	GXUInt textures = GXGetTotalTextureStorageObjects ( &lastTexture );

	const GXWChar* lastVAO;
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