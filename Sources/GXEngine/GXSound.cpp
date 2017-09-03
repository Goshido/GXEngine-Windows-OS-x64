//version 1.0

#include <GXEngine/GXSound.h>


extern HMODULE				gx_GXEngineDLLModuleHandle;

//--------------------------------------------------------------------------

PFNALLISTENERFV					GXAlListenerfv				= 0;
PFNALLISTENER3F					GXAlListener3f				= 0;
PFNALLISTENERF					GXAlListenerf				= 0;

PFNALGENBUFFERS					GXAlGenBuffers				= 0;
PFNALDELETEBUFFERS				GXAlDeleteBuffers			= 0;

PFNALBUFFERI					GXAlBufferi					= 0;
PFNALBUFFERDATA					GXAlBufferData				= 0;

PFNALGENSOURCES					GXAlGenSources				= 0;
PFNALGETSOURCEI					GXAlGetSourcei				= 0;
PFNALDELETESOURCES				GXAlDeleteSources			= 0;

PFNALSOURCEI					GXAlSourcei					= 0;
PFNALSOURCEFV					GXAlSourcefv				= 0;
PFNALSOURCE3F					GXAlSource3f				= 0;
PFNALSOURCEF					GXAlSourcef					= 0;
PFNALSOURCEQUEUEBUFFERS			GXAlSourceQueueBuffers		= 0;
PFNALSOURCEUNQUEUEBUFFERS		GXAlSourceUnqueueBuffers	= 0;

PFNALSOURCEPLAY					GXAlSourcePlay				= 0;
PFNALSOURCESTOP					GXAlSourceStop				= 0;
PFNALSOURCEPAUSE				GXAlSourcePause				= 0;
PFNALSOURCEREWIND				GXAlSourceRewind			= 0;

PFNGXOPENALCHECKERROR			GXOpenALCheckError			= 0;
PFNGXOPENALCHECKCONTEXTERROR	GXOpenALCheckContextError	= 0;
PFNGXOPENALDESTROY				GXOpenALDestroy				= 0;	

//--------------------------------------------------------------------------

PFNOVOPENCALLBACKS				GXOvOpenCallbacks			= 0;
PFNOVREAD						GXOvRead					= 0;
PFNOVPCMSEEK					GXOvPcmSeek					= 0;
PFNOVPCMTOTAL					GXOvPcmTotal				= 0;
PFNOVCLEAR						GXOvClear					= 0;

GXBool GXCALL GXSoundInit ()
{
	gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXPhysXUnit::Error - Не удалось загрузить GXEngine.dll\n" );
		return GX_FALSE;
	}

	PFNGXOGGVORBISINIT GXOGGVorbisInit;

	GXOGGVorbisInit = reinterpret_cast <PFNGXOGGVORBISINIT> ( reinterpret_cast <GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXOGGVorbisInit" ) ) );
	if ( !GXOGGVorbisInit )
	{
		GXLogW ( L"GXSoundInit::Error - Не удалось найти функцию GXOGGVorbisInit\n" );
		return GX_FALSE;
	}

	GXOGGVorbisFunctions oggVorbisOut;
	oggVorbisOut.ov_clear = &GXOvClear;
	oggVorbisOut.ov_open_callbacks = &GXOvOpenCallbacks;
	oggVorbisOut.ov_pcm_seek = &GXOvPcmSeek;
	oggVorbisOut.ov_pcm_total = &GXOvPcmTotal;
	oggVorbisOut.ov_read = &GXOvRead;

	GXOGGVorbisInit ( oggVorbisOut );

	PFNGXOPENALINIT GXOpenALInit;
	GXOpenALInit = reinterpret_cast <PFNGXOPENALINIT> ( reinterpret_cast <GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXOpenALInit" ) ) );
	if ( !GXOpenALInit )
	{
		GXLogW ( L"GXSoundInit::Error - Не удалось найти функцию GXOpenALInit\n" );
		return GX_FALSE;
	}

	GXOpenALFunctions alOut;
	alOut.alBufferData = &GXAlBufferData;
	alOut.alBufferi = &GXAlBufferi;
	alOut.alDeleteBuffers = &GXAlDeleteBuffers;
	alOut.alDeleteSources = &GXAlDeleteSources;
	alOut.alGenBuffers = &GXAlGenBuffers;
	alOut.alGenSources = &GXAlGenSources;
	alOut.alGetSourcei = &GXAlGetSourcei;
	alOut.alListener3f = &GXAlListener3f;
	alOut.alListenerf = &GXAlListenerf;
	alOut.alListenerfv = &GXAlListenerfv;
	alOut.alSource3f = &GXAlSource3f;
	alOut.alSourcef = &GXAlSourcef;
	alOut.alSourcefv = &GXAlSourcefv;
	alOut.alSourcei = &GXAlSourcei;
	alOut.alSourcePause = &GXAlSourcePause;
	alOut.alSourcePlay = &GXAlSourcePlay;
	alOut.alSourceQueueBuffers = &GXAlSourceQueueBuffers;
	alOut.alSourceRewind = &GXAlSourceRewind;
	alOut.alSourceStop = &GXAlSourceStop;
	alOut.alSourceUnqueueBuffers = &GXAlSourceUnqueueBuffers;
	alOut.GXOpenALCheckContextError = &GXOpenALCheckContextError;
	alOut.GXOpenALCheckError = &GXOpenALCheckError;
	alOut.GXOpenALDestroy = &GXOpenALDestroy;

	if ( !GXOpenALInit ( alOut ) )
	{
		GXLogW ( L"GXSoundInit::Error - GXOpenALInit провалена\n" );
		return GX_FALSE;
	}

	return GX_TRUE;
}

GXBool GXCALL GXSoundDestroy ()
{
	if ( !gx_GXEngineDLLModuleHandle )
	{
		GXLogW ( L"GXSoundDestroy::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll\n" );
		return GX_FALSE;
	}

	if ( !GXOpenALDestroy )
	{
		GXLogW ( L"GXSoundInit::Error - Не удалось найти функцию GXOGGVorbisInit\n" );
		return GX_FALSE;
	}

	GXOpenALDestroy ();

	if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
	{
		GXLogW ( L"GXSoundDestroy::Error - Не удалось выгрузить библиотеку GXEngine.dll\n" );
		return GX_FALSE;
	}

	return GX_TRUE;
}