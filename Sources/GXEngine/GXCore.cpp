//version 1.10

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


GXInput*				GXCore::input = nullptr;
GXRenderer*				GXCore::renderer = nullptr;
GXPhysics*				GXCore::physics = nullptr;
GXSoundMixer*			GXCore::soundMixer = nullptr;
GXNetServer*			GXCore::server = nullptr;
GXNetClient*			GXCore::client = nullptr;
GXTouchSurface*			GXCore::touchSurface = nullptr;
GXLocale*				GXCore::locale = nullptr;

GXBool					GXCore::loopFlag = GX_TRUE;

PFNGXONGAMEINITPROC		GXCore::OnGameInit = nullptr;
PFNGXONGAMECLOSEPROC	GXCore::OnGameClose = nullptr;

GXCore*					GXCore::instance = nullptr;


GXCore::GXCore ( PFNGXONGAMEINITPROC onGameInit, PFNGXONGAMECLOSEPROC onGameClose, const GXWChar* gameName )
{
	instance = this;

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

GXRenderer* GXCore::GetRenderer () const
{
	return renderer;
}

GXPhysics* GXCore::GetPhysics () const
{
	return physics;
}

GXInput* GXCore::GetInput () const
{
	return input;
}

GXSoundMixer* GXCore::GetSoundMixer () const
{
	return soundMixer;
}

GXNetServer* GXCore::GetNetServer () const
{
	return server;
}

GXNetClient* GXCore::GetNetClient () const
{
	return client;
}

GXTouchSurface* GXCore::GetTouchSurface () const
{
	return touchSurface;
}

GXLocale* GXCore::GetLocale () const
{
	return locale;
}

GXCore* GXCALL GXCore::GetInstance ()
{
	return instance;
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